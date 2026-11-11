# Tuning logbook

One row per test (3 laps each). Change ONE number at a time. Battery set = A or B. Settings live in the TUNING table at the top of `firmware/ZumoLineFollower/ZumoLineFollower.ino`.

| Date / time | Mode | Battery set | maxSpeed | Kp (num/den) | Kd | turnSlowdown | reverseLimit | Timeout (µs) | Lap times (s) | Line losses | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 11 Nov | Safe | — | 250 | 1/4 | 6 | 50 | 100 | 2000 | — | — | First run on a small, uneven tape circle: follows the line but wobbles a lot. Calibration: 256/2000 168/1556 168/1032 172/1248 292/1604 380/2000 (OK). Next test: Kd 6 → 12 |
| | | | | | | | | | | | |
