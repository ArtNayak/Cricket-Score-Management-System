# Cricket Score Management System

A console program in C for recording and managing the scores of a cricket match between two teams.

## Features
- Enter batting figures (runs, balls, fours, sixes) and bowling figures (overs, runs, wickets) for up to 11 batsmen and 11 bowlers per team, plus extras.
- Overs use cricket notation (`3.4` = 3 overs and 4 balls), so economy rates are exact.
- Scorecards with strike rate and economy, team totals and the match result.
- Update any batsman, bowler or the extras afterwards (player lookup ignores case).
- Every input is validated: ranges, non-empty names, overs format, and consistency (boundaries cannot exceed runs or balls faced). Bad input is re-prompted instead of crashing.

## Build and run

```
gcc -std=c99 -Wall -Wextra -o cricket cricket.c
./cricket
```

Any C99 compiler works (gcc, clang, MSVC, tcc). No dependencies.
