SELECT P2.teamRoadKills, AVG(P2.winPlacePerc) AS avgWinPlacePerc
FROM((SELECT P.matchId, SUM(P.roadKills) AS teamRoadKills, AVG(P.winPlacePerc) AS winPlacePerc
      FROM player_statistic P
      GROUP BY P.groupId, P.matchId) AS P2,
     (SELECT M.matchId
      FROM `match` M
      WHERE M.matchType='squad' OR M.matchType='squad-fpp') AS M2)
WHERE P2.matchId = M2.matchId
GROUP BY P2.teamRoadKills
ORDER BY P2.teamRoadKills DESC;