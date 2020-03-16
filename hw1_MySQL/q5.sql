SELECT  M.matchType, AVG(M.matchDuration) AS averageDuration
FROM `match` M
GROUP BY M.matchType
ORDER BY AVG(M.matchDuration) ASC;