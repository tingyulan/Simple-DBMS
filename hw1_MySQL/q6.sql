SELECT P.walkDistance+P.swimDistance+P.rideDistance AS totalDistance, P.walkDistance, P.swimDistance, P.rideDistance
FROM player_statistic P
WHERE P.walkDistance>0 AND P.swimDistance>0 AND P.rideDistance>0
ORDER BY totalDistance DESC
LIMIT 10;
