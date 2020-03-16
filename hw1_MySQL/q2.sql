SELECT P.Id, P.matchId, P.damageDealt
FROM player_statistic P
WHERE P.damageDealt BETWEEN 2000 AND 2010;
