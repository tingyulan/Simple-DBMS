SELECT COUNT(*) AS numberOfPlayers, MAX(TMP.heals) AS maxHeals
FROM(
    SELECT P.heals
    FROM `match` M,
            (SELECT P2.matchId, P2.heals FROM player_statistic P2 WHERE P2.winPlacePerc=1 AND P2.damageDealt=0) AS P,
            (SELECT AVG(M2.matchDuration) AS average FROM `match` M2 ) AS TMP2
    WHERE M.matchId=P.matchId AND M.matchDuration>TMP2.average
    )AS TMP;