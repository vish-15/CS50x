-- Keep a log of any SQL queries you execute as you solve the mystery.

--IMPORTANT INFO: SOME OF LOG QUERIES ARE MISSING. THIS LOG FILE WAS MADE AFTER SOLVING THE PROBLEM AND QUERIES HAVE BEEN COMBINED FOR TO MAKE IT LOOK SIMPLE.

.table
.schema

-- get crime scene report
SELECT id, description
FROM crime_scene_reports
WHERE street = "Humphrey Street" AND day = 28 AND month = 07 AND year = 2023;

-- get witnesses reports
SELECT name, transcript
FROM interviews
WHERE day = 28 AND month = 07 AND year = 2023;

-- find bank account details from ATM transactions
SELECT p.name, p.passport_number, b.account_number
FROM people p
JOIN bank_accounts b ON p.id = b.person_id
JOIN atm_transactions a ON b.account_number = a.account_number
WHERE a.day = 28 AND a.month = 07 AND a.year = 2023
AND a.atm_location = "Leggett Street"
AND a.transaction_type = "withdraw";

-- trace phone calls for suspicious coomunication
SELECT p.name AS caller_name, pc.receiver, pc.duration
FROM phone_calls pc
JOIN people p ON pc.caller = p.phone_number
WHERE pc.day = 28 AND pc.month = 07 AND pc.year = 2023
AND pc.duration < 60;

-- track flight recoreds departing from fiftyville the next day
SELECT a.city AS destination_city, a.full_name AS airport_name, p.name AS passenger_name, p.passport_number
FROM airports a
JOIN flights f ON a.id = f.destination_airport_id
JOIN passengers ps ON f.id = ps.flight_id
JOIN people p ON ps.passport_number = p.passport_number
WHERE f.day = 29 AND f.month = 07 AND f.year = 2023
AND f.origin_airport_id = (SELECT id FROM airports WHERE city = "Fiftyville")
ORDER BY f.hour, f.minute;
