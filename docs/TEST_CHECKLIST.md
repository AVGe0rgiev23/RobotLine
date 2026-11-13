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

| Mode | speed | kp | kd | slow | rev | edge | hold | Best lap time (s) | 3 clean laps? | ✓ |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 Safe | 250 | 1/4 | 6 | 50 | 100 | 0 | 0 | | | [ ] |
| 2 Medium | | | | | | | | | | [ ] |
| 3 Fast | | | | | | | | | | [ ] |
| 4 Max | 400 | | | | | | | | | [ ] |

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
| 5.1a | Crossing detection practice | Enable `CROSSING_PRACTICE`, upload, calibrate, and run in Safe mode on a track with two crossings and a normal bend | Stops once at each crossing and stays stopped until you press the button; continues straight after one press; does not stop on the bend. Restore `false`, upload again, and repeat 5.1. | [ ] |
| 5.2 | Tight hairpin | 3 cm-radius U-turn in the line | Follows it without leaving the line | [ ] |
| 5.2a | Sharp corner at full speed | `mode 4`, long straight into the sharpest corner, 5 runs | Takes the corner at full speed (pivot) and never leaves the line (see TUNING.md, "Full speed through curves") | [ ] |
| 5.3 | Lost line in a sharp turn | Run the fastest mode into the sharpest turn | If it overshoots onto white, it does NOT stop: it spins back towards the turn (the same side it was turning) and continues | [ ] |
| 5.4 | No stopping on white | Let it drive off the end of a line, or past a crossing | It never stops on white; it searches by spinning (stop it with the button) | [ ] |
| 5.4a | Solid black finish | Black area (e.g. 10 × 10 cm of black tape/paper) at the end of the line; also start ON a black start line | Stops on the black area; Serial says "solid black (finish)"; does NOT stop on crossings or on the start line | [ ] |
| 5.5 | Stop button | Press the button during a run | Stops; Serial says "button pressed" | [ ] |
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
