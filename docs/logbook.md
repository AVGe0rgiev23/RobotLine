# Tuning logbook

One row per test (3 laps each). Change ONE number at a time. Battery set = A or B. Settings live in the TUNING table at the top of `firmware/ZumoLineFollower/ZumoLineFollower.ino`.

| Date / time | Mode | Battery set | maxSpeed | Kp (num/den) | Kd | turnSlowdown | reverseLimit | Timeout (µs) | Lap times (s) | Line losses | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 11 Nov | Safe | — | 250 | 1/4 | 6 | 50 | 100 | 2000 | — | — | First run on a small, uneven tape circle: follows the line but wobbles a lot. Calibration: 256/2000 168/1556 168/1032 172/1248 292/1604 380/2000 (OK). Next test: Kd 6 → 12 |
| 11 Nov | Safe | — | 250 | 1/4 | 12 (test) | 50 | 100 | 2000 | — | — | Follows the line but goes "crazy"; left side seems slower than right (robot partly broken). At the end of a strip it spins and comes back (line-lost search, by design). Team is getting a replacement Zumo |
| 12 Nov | — | new alkaline | — | — | — | — | — | — | — | — | SetupCheck with brand-new AA batteries: all motor steps correct; straight-drive test (speed 200, 2 s) curves a little left = normal small motor difference. Robot OK for tuning |
| 12 Nov | Safe | new | 250 | 1/4 | 6 | 50 | 100 | 2000 | — | — | NEW Zumo on the real track: follows the line perfectly, but too slow. Next: Medium → Fast → Max |
| | | | | | | | | | | | |
