# Test checklist — what to test, what to record, what to save

Work through this top to bottom. Tick each box when it passes. Write the numbers in the "Record" columns (here or in [logbook.md](logbook.md)), and push this file when you are done so the whole team sees the results.

Race: **Saturday 14 Nov 2026**, Line following – ready kits.

## 0. Before every test session

- [ ] Latest code: `cd ~/RobotLine && git checkout -- . && git pull`, then upload `ZumoLineFollower` (about 13 900 bytes)
- [ ] Batteries: the same type you will race with (fresh alkaline = fastest). Battery set used: ______
- [ ] Tracks cleaned with sticky tape
- [ ] Serial Monitor open: 9600 baud, line ending **Newline**
- [ ] Type `show` and note the settings in use

## 1. Hardware (new Zumo)

| # | Test | How | Pass when | Record | ✓ |
| --- | --- | --- | --- | --- | --- |
| 1.1 | Jumpers | Sensor array LEDON on pin 2; shield "Bz." on 328P | Hello beep at switch-on | — | [ ] |
| 1.2 | Sensors | `SetupCheck`, tape under each sensor left → right | Each number jumps high in turn | White ___ / black ___ | [ ] |
| 1.3 | Motor steps | `SetupCheck`, short press, robot lifted | Every step matches its name | — | [ ] |
| 1.4 | Straight drive | `SetupCheck`, hold button 1 s, robot on the floor | Straight, or only a slight curve | Curve: none / slight left / slight right | [ ] |
| 1.5 | No restarts | Start a run at `mode 4` | No hello beep in the middle of a run | — | [ ] |

## 2. Calibration

| # | Test | How | Pass when | Record | ✓ |
| --- | --- | --- | --- | --- | --- |
| 2.1 | Calibrate | Robot on the line, facing along it, hold button 1 s | "Calibration OK", LED stays on | The 6 white/black pairs: ______ | [ ] |
| 2.2 | Safety check | Calibrate with **no** line under the robot | Low double tone, "Calibration FAILED", LED off | — | [ ] |

## 3. Tune each mode (USB tuning tool, see [TUNING.md](TUNING.md))

Goal: 3 clean laps in a row. Fill in the values that passed, then type `save`.

| Mode | speed | kp | kd | slow | rev | Best lap time (s) | 3 clean laps? | ✓ |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 Safe | 250 | 1/4 | 6 | 50 | 100 | | | [ ] |
| 2 Medium | | | | | | | | [ ] |
| 3 Fast | | | | | | | | [ ] |
| 4 Max | 400 | | | | | | | [ ] |

## 4. Sensor timeout

| # | Test | How | Pass when | Record | ✓ |
| --- | --- | --- | --- | --- | --- |
| 4.1 | Loop speed now | Run any mode, stop, read the Serial Monitor | — | Loops per second at 2000 µs: ____ | [ ] |
| 4.2 | Faster timeout | `timeout 1000`, calibrate again | "Calibration OK" | Loops per second at 1000 µs: ____ | [ ] |
| 4.3 | Is it better? | Best mode, 3 laps | Same or faster, no new line losses | Lap time: ____ | [ ] |

If 4.2 or 4.3 fails, type `timeout 2000` and calibrate again.

## 5. Special situations

| # | Test | How | Pass when | ✓ |
| --- | --- | --- | --- | --- |
| 5.1 | Crossing | Tape strip across the line at 90°, 10 cm straight after it | Drives straight over it, every time (try 5×) | [ ] |
| 5.2 | Tight hairpin | 3 cm-radius U-turn in the line | Follows it without leaving the line | [ ] |
| 5.3 | Lost line in a curve | Run the best mode through the sharpest curve | If it overshoots, it spins back and finds the line | [ ] |
| 5.4 | Recover a lost line | Try a short gap and a curve with a crinkle. If the line stays out of reach, press the user button. | Backs up and sweeps until it sees the line, then resumes following. The button stops the search. | [ ] |
| 5.4a | Filled finish circle | Drive across a regular crossing and then into the circle in Safe mode. Repeat slightly off centre. | Drives through the crossing and stops on the circle. Serial says "finish circle". | [ ] |
| 5.5 | Picked up | Lift the robot during a run | Stops; Serial says "end of the line" or "line lost" | [ ] |
| 5.6 | Instant start | Press and release | Starts the moment the button is released, no music | [ ] |

## 6. Mock competition (Friday)

New track layout, 5 attempts, real rules, phone timing, robot switched on from calibration to the last attempt.

| Attempt | Mode | Time (s) | Finished? | Notes |
| --- | --- | --- | --- | --- |
| 1 | Safe | | | |
| 2 | | | | |
| 3 | | | | |
| 4 | | | | |
| 5 | | | | |

- [ ] At least 4 of 5 finished
- [ ] Race mode chosen: ______

## 7. Save the results (Friday, before 20:00 code freeze)

- [ ] `save` typed on the robot after the final values
- [ ] Final values sent to Claude/Codex (or edited yourself) into `DEFAULT_MODES` and `SENSOR_TIMEOUT_US` in `ZumoLineFollower.ino`, uploaded, checked with `show`
- [ ] This checklist and `logbook.md` filled in and pushed to GitHub
- [ ] Code frozen: no new features after 20:00

## 8. Race-day bag

- [ ] Zumo + 2 sets of fresh batteries (race type) + spare set
- [ ] Laptop with Arduino IDE, ZumoShield library and the final code; USB cable + spare
- [ ] Sticky tape for the tracks, cloth, small screwdriver
- [ ] This checklist with the final values per mode
