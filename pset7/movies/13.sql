SELECT DISTINCT people.name
FROM people
JOIN stars ON people.id = stars.person_id
JOIN movies ON stars.movie_id = movies.id
JOIN stars AS s2 ON movies.id = s2.movie_id
JOIN people AS p2 ON s2.person_id = p2.id
WHERE p2.name = 'Kevin Bacon'
AND people.id <> p2.id;
