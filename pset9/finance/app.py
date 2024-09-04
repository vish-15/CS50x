import os

from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session
from flask_session import Session
from werkzeug.security import check_password_hash, generate_password_hash

from helpers import apology, login_required, lookup, usd

# Configure application
app = Flask(__name__)

# Custom filter
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")


@app.after_request
def after_request(response):
    """Ensure responses aren't cached"""
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""
    user_id = session["user_id"]

    # query user's cash balance
    user = db.execute("SELECT cash FROM users WHERE id = ?", user_id)
    cash = user[0]["cash"]

    # qyery user's stock holdings
    stocks = db.execute(
        "SELECT symbol, SUM(shares) AS total_shares FROM transactions "
        "WHERE user_id = ? "
        "GROUP BY symbol "
        "HAVING total_shares > 0", user_id
    )

    # calculate total value of each stock holding and total portfolio value
    total_portfolio_value = cash
    for stock in stocks:
        quote_data = lookup(stock["symbol"])
        stock["price"] = quote_data["price"]
        stock["total_value"] = stock["total_shares"] * stock["price"]
        total_portfolio_value += stock["total_value"]

    return render_template("index.html", cash=cash, stocks=stocks, total_portfolio_value=total_portfolio_value)


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""

    if request.method == "POST":
        # ensure symbol was submitted
        symbol = request.form.get("symbol")
        if not symbol:
            return apology("must provide stock symbol", 400)

        # ensure shares input was submitted and is a positive integer
        try:
            shares = int(request.form.get("shares"))
            if shares <= 0:
                return apology("must provide positive number of shares", 400)
        except ValueError:
            return apology("shares must be a positive integer", 400)

        # lookup current price of the stock
        quote = lookup(symbol)
        if not quote:
            return apology("symbol lookup failed", 400)

        price_per_share = quote["price"]
        total_cost = shares * price_per_share

        # query user's current cash balance
        user = db.execute("SELECT cash FROM users WHERE id = ?", session["user_id"])
        if not user or user[0]["cash"] < total_cost:
            return apology("not enough cash to buy", 400)

        # record the purchase in transactions table
        db.execute("""
            INSERT INTO transactions (user_id, symbol, shares, price)
            VALUES (?, ?, ?, ?)
        """, session["user_id"], symbol, shares, price_per_share)

        # deduct the total cost from user's cash balance
        db.execute("""
            UPDATE users
            SET cash = cash - ?
            WHERE id = ?
        """, total_cost, session["user_id"])

        flash("Bought successfully!")

        return redirect("/")

    else:
        return render_template("buy.html")


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""
    user_id = session["user_id"]

    # query all transactions for the user
    transactions = db.execute(
        "SELECT symbol, shares, price, transacted_at, CASE WHEN shares > 0 THEN 'BUY' ELSE 'SELL' END AS transaction_type "
        "FROM transactions "
        "WHERE user_id = ? "
        "ORDER BY transacted_at DESC", user_id
    )
    return render_template("history.html", transactions=transactions)


@app.route("/login", methods=["GET", "POST"])
def login():
    session.clear()

    if request.method == "POST":
        username = request.form.get("username")
        password = request.form.get("password")

        if not username:
            return apology("must provide username", 403)
        elif not password:
            return apology("must provide password", 403)

        user = db.execute("SELECT * FROM users WHERE username = ?", username)

        if len(user) != 1 or not check_password_hash(user[0]["hash"], password):
            return apology("invalid username and/or password", 403)

        session["user_id"] = user[0]["id"]
        flash("Logged in!")
        return redirect("/")

    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    session.clear()
    flash("Logged out!")
    return redirect(url_for("login"))


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""
    if request.method == "POST":
        symbol = request.form.get("symbol")
        if not symbol:
            return apology("must provide symbol", 400)

        quote_info = lookup(symbol)
        if not quote_info:
            return apology("invalid symbol", 400)

        return render_template("quoted.html", quote_info=quote_info)

    else:
        return render_template("quote.html")


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""
    if request.method == "POST":
        username = request.form.get("username")
        password = request.form.get("password")
        confirmation = request.form.get("confirmation")

        if not username:
            return apology("must provide username", 400)

        elif not password or not confirmation:
            return apology("must provide password", 400)

        elif password != confirmation:
            return apology("passwords must match", 400)

        # Hhash the password
        hashed_password = generate_password_hash(password)

        try:
            # insert new user into database
            db.execute("INSERT INTO users (username, hash) VALUES (?, ?)",
                       username, hashed_password)
        except ValueError:
            return apology("username already exists", 400)

        return redirect("/login")

    else:
        return render_template("register.html")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""

    if request.method == "POST":
        symbol = request.form.get("symbol")
        if not symbol:
            return apology("must select stock symbol", 400)

        # ensure shares input is provided and is a positive integer
        try:
            shares = int(request.form.get("shares"))
            if shares <= 0:
                return apology("must provide positive number of shares", 400)
        except ValueError:
            return apology("shares must be a positive integer", 400)

        # query user's current holdings of the selected stock
        rows = db.execute("""
            SELECT symbol, SUM(shares) AS total_shares
            FROM transactions
            WHERE user_id = ?
            AND symbol = ?
            GROUP BY symbol
            HAVING total_shares > 0
        """, session["user_id"], symbol)

        if not rows or rows[0]["total_shares"] < shares:
            return apology("not enough shares to sell", 400)

        # get current price of the stock
        quote = lookup(symbol)
        if not quote:
            return apology("symbol lookup failed", 400)

        price_per_share = quote["price"]
        total_sale_value = shares * price_per_share

        # deduct sold shares from user's portfolio
        db.execute("""
            INSERT INTO transactions (user_id, symbol, shares, price)
            VALUES (?, ?, ?, ?)
        """, session["user_id"], symbol, -shares, price_per_share)

        # update user's cash balance
        db.execute("""
            UPDATE users
            SET cash = cash + ?
            WHERE id = ?
        """, total_sale_value, session["user_id"])

        flash("Sold successfully!")

        return redirect("/")

    else:
        symbols = db.execute("""
            SELECT symbol, SUM(shares) AS total_shares
            FROM transactions
            WHERE user_id = ?
            GROUP BY symbol
            HAVING total_shares > 0
        """, session["user_id"])

        return render_template("sell.html", symbols=symbols)


@app.route("/add_cash", methods=["POST"])
@login_required
def add_cash():
    """Add cash to user's account"""
    user_id = session["user_id"]

    amount = request.form.get("amount")
    if not amount:
        return apology("must provide amount", 400)

    # ensure amount is a positive number
    try:
        amount = float(amount)
        if amount <= 0:
            raise ValueError("amount must be positive")
    except ValueError:
        return apology("amount must be a positive number", 400)

    # update user's cash balance
    db.execute("UPDATE users SET cash = cash + ? WHERE id = ?", amount, user_id)

    flash(f"${amount} added to your account successfully!", "success")

    return redirect("/")
