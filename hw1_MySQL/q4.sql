SELECT P.Id, AVG(P.kills) AS avgKills
FROM player_statistic P, `match` M
WHERE P.matchId=M.matchID AND M.numGroups<=10
GROUP BY P.Id
ORDER BY AVG(P.kills) DESC
LIMIT 20;