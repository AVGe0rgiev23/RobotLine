# Implementation Plan — the Zumo line-following software

As of 11 Nov 2026. Race: **Saturday 14 Nov 2026**. Companion to [PLAN.md](PLAN.md) and [BUILD_PLAN.md](BUILD_PLAN.md).

The race sketch now exists: `firmware/ZumoLineFollower/ZumoLineFollower.ino` (compiles for the Uno with ZumoShield 2.1.0).

**Status (12 Nov):**

| Feature | Code | Tested on the robot |
| --- | --- | --- |
| Mode selection, calibration with safety check, instant start, LED + buzzer signals | Done | Yes |
| PD steering, Safe mode | Done | Follows the line; wobbles; tuning pending on the new Zumo |
| Inside track reverses in tight turns, slow down in curves | Done | Not yet |
| Straight over crossings; lost line → spin back towards the side the line was last clearly on, never stop | Done | Not yet (tests 5.3, 5.4) |
| Pause at detected crossings in practice | Implemented | Enable `CROSSING_PRACTICE` and run test 5.1a; off by default |
| 4 speed modes (Safe / Medium / Fast / Max) | Done | Medium/Fast/Max untested |
| Soft start (0.15 s) | Done | Not yet |
| Battery warning + compensation | Done, off until the battery-level wire is soldered | Not yet |
| Full speed through curves (modes 2–4: no curve slowdown, pivot with outer track at full speed) | Done | Not yet: test 5.2a |
| Stop on solid black (finish), ignored on the start line | Done | Not yet: test 5.4a |
| Faster sensor timeout | Setting ready (2000 µs now) | Not yet |

What remains is testing and tuning (M4–M8 tests below), on the new Zumo.

This section started as the plan for the code. We build one Arduino sketch in small milestones. Each milestone has a test the robot must pass before the next one starts, and each passing version is committed to this repo.

## 1. Starting point: Pololu's LineFollower example

The ZumoShield library (v2.1.0) ships a `LineFollower` example made for our robot. What it does today, and what limits its speed:

| Part of the example | What it does | Problem for racing |
| --- | --- | --- |
| Setup | Welcome beep → wait for button → 1 s delay → spins left/right for 1.6 s while calling `calibrate()` → beep → wait for button → plays a tune and waits for it to finish → starts | The tune and delays at the start break the "no delay" rule. Calibration and start must be separate steps |
| Steering | `speedDifference = error / 4 + 6 * (error - lastError)` (Kp = 1/4, Kd = 6) | Fixed numbers, not tuned for our track or batteries |
| Speed | `MAX_SPEED = 400` (full power) on both motors, then one motor slows by `speedDifference` | Already full power: we cannot go faster by raising it |
| Motor limits | Both motor speeds clamped to 0 … 400 | A motor can never reverse, so in a 3 cm hairpin the robot only slows one track and slides wide. This is the biggest speed loss in tight turns |
| Line lost | Nothing special; `readLine()` returns 0 or 5000 (the side last seen) | It steers hard to that side, which is right, but without reverse it turns slowly |
| Crossings | Nothing special | All sensors black can jerk the steering |
| Sensor timeout | 2000 µs (library default) | Each reading can take up to 2 ms, which caps the loop at about 500 corrections per second |

Library facts we rely on (from the ZumoShield library source):

- `ZumoMotors::setSpeeds(left, right)`, each −400 … 400 (negative = reverse); 20 kHz PWM.
- `ZumoReflectanceSensorArray`: default pins {4, A3, 11, A0, A2, 5}, emitter pin 2 on an Uno. `init(pins, 6, timeout, 2)` sets a custom timeout; Pololu recommends 1000–3000 µs.
- `calibrate()` learns min/max per sensor; `readCalibrated()` gives 0–1000 per sensor; `readLine()` gives 0–5000 (2500 = centred) and, when no sensor sees the line, returns the end of the side where it was last seen.
- `Pushbutton button(ZUMO_BUTTON)` (pin 12); `ZumoBuzzer` with `play()` and `isPlaying()`; LED is plain pin 13.

## 2. How our program will work

One sketch: `firmware/ZumoLineFollower/ZumoLineFollower.ino`. Everything the team tunes sits in one table at the top.

**States, in order:**

1. **Choose mode.** Each short press of the button moves to the next mode (Safe → Fast → Safe …); the buzzer beeps the mode number. A long press (about 1 s) confirms.
2. **Calibrate.** Robot placed across the line, beside the start (not on the start line). It spins left and right for about 1.6 s while learning black and white, like the example. Then a beep and the LED turns on: ready.
3. **Ready.** Robot is placed on the start line. Nothing moves.
4. **Run.** Starts the instant the button is **released** (no music, no delay). Runs the control loop below.
5. **Stop (safety).** A button press during the run stops the motors, for testing at home.

The robot stays switched on from calibration to the start, so calibration is not lost.

**The control loop (Run state), every cycle:**

1. Read the 6 sensors with `readLine()` → position 0–5000.
2. Count how many sensors see black (calibrated value above a threshold, for example 700) and how many see white (below, for example, 200).
3. **Crossing:** 5 or 6 sensors black → drive straight at the current base speed and do not update the steering. The line after a crossing is straight for at least 10 cm, so this is safe.
4. **Line lost:** all 6 white → spin on the spot towards the side where the line was last seen (one track forward, the other backward). Never stop: the 30-second rule.
5. **Normal:** error = position − 2500; derivative = error − last error.
6. **Steering:** correction = Kp × error + Kd × derivative (calculated in `long` to avoid overflow).
7. **Speed in turns:** base = maximum speed − slowdown × |error|. Full speed when centred on a straight, less when the line is far off-centre (a curve).
8. **Motors:** left = base + correction, right = base − correction. Clamp each to **−reverse limit … maximum speed**, so the inner track may run backwards in hairpins.
9. Remember the error for the next cycle.

**Tuning table (one row per mode).** Starting numbers come from the example; real values come from testing.

| Mode | Max speed | Kp | Kd | Turn slowdown | Reverse limit |
| --- | --- | --- | --- | --- | --- |
| Safe | 250 | 1/4 | 6 | low | 100 |
| Fast | 400 | 1/4 | 6–10 | medium | 200 |
| (Round 2) Medium, Max | — | — | — | — | — |

## 3. Milestones

| # | Milestone | Day | Test to pass |
| --- | --- | --- | --- |
| M0 | Toolchain: IDE, ZumoShield library, upload `ZumoBuzzerExample` over USB | Wed | The buzzer plays |
| M1 | Sensors: upload `LineSensorTest` / `LineSensorCalibration` examples | Thu | All 6 sensors change clearly between white and black tape |
| M2 | Motors: upload `ZumoMotorExample` | Thu | Both tracks run forwards and backwards; robot drives straight. If a track runs the wrong way, use `flipLeftMotor`/`flipRightMotor` |
| M3 | Baseline: `LineFollower` unchanged, with `MAX_SPEED` lowered to 200 | Thu | One full lap of the practice track. Record the time |
| M4 | Our sketch v1: states 1–5, mode table, start on release, example's PD logic | Thu evening | Full lap in Safe mode; start has no delay; mode beeps are right |
| M5 | Reverse limit + speed in turns | Fri morning | Clean hairpin at 3 cm radius in Fast mode |
| M6 | Crossing + line-lost behaviour | Fri | Goes straight over the crossing 10/10 times; finds the line again when lifted off it |
| M7 | Loop speed: sensor timeout 2000 → 1500 → 1000 µs | Fri | Black still reads as black; faster laps without new line losses |
| M8 | Final tuning + mock competition + **code freeze 20:00** | Fri | 5 attempts on a new layout: at least 4 clean finishes in Safe, and Fast tuned as far as it stays clean |

Commit after every passing milestone with a message like `M5: reverse limit and turn slowdown, hairpin clean in Fast`. Tag the frozen version `round1-race`.

## 4. How to tune for speed (Thursday–Friday)

Change **one number at a time**, run **3 laps**, write it down.

1. **Safe mode first.** Max speed 250, Kd = 0. Raise Kp until the robot follows the line but wobbles. Then raise Kd until the wobble stops.
2. **Reverse limit.** Raise it from 0 until the 3 cm hairpin is clean. Too high makes the robot jerky on gentle curves.
3. **Fast mode.** Copy the Safe numbers, set max speed 400, and raise Kd until it stops overshooting the turns. If it still leaves the line in turns, raise the turn slowdown.
4. **Sensor timeout.** Lower it in steps (1500, then 1000 µs). Keep the lowest value where black still reads near 1000 after calibration.
5. **Battery check.** Repeat the best Fast settings on battery set B. If the times differ a lot, tune on half-charged batteries so race-day fresh batteries only make it better.

**Logbook** (keep it in a notebook or `docs/logbook.md`):

| Time | Mode | Battery set | Max | Kp | Kd | Slowdown | Reverse | Timeout | Lap time | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| | | | | | | | | | | |

## 5. Speed recommendations, ranked

Ranked by how much time each one should save for the effort, for a Zumo on a track with 3 cm turns.

| # | Idea | Why it is faster | When |
| --- | --- | --- | --- |
| 1 | **Let the inner track reverse in tight turns** (reverse limit) | The example only slows one track, so hairpins are slow and wide. Spinning on the spot keeps the line and exits the turn faster | This week |
| 2 | **Stay on the line at full power** (good Kp/Kd) | The motors already run at 400 on straights; every line loss or wobble costs time | This week |
| 3 | **Never stop on lost line**: spin towards the last-seen side at once | A lost line becomes a short pause instead of a +15 s rescue | This week |
| 4 | **Instant start on button release, no tune** | Required by the rules; also no wasted time before the gate | This week |
| 5 | **Faster loop** (sensor timeout 1000–1500 µs) | More corrections per second, so less overshoot at high speed | This week |
| 6 | **Speed in turns** (slow down when the error is big) | Lets Fast mode use 400 on straights without flying out of curves | This week |
| 7 | **Fresh batteries for the fast attempts; clean tracks** | Motor speed follows battery voltage; grip decides cornering | This week |
| 8 | **Battery compensation** (needs the battery-level wire soldered; code ready, off by default) | Same behaviour all day, so tuning holds | When the wire is soldered |
| 9 | **Faster motors** (lower gear ratio, "HP" micro metal gearmotors) | Higher top speed (75:1 → 50:1 = 1.5×); needs re-tuning and organizer approval | Round 2 |
| 10 | **Encoders + track memory** | Sprint straights, brake before known curves | Round 2 stretch |

What not to do this week: no integral (I) term (Pololu notes it is rarely useful for line following); no new features after the Friday 20:00 freeze; no Serial printing or `delay()` inside the race loop.

## 6. Repo layout

```
docs/
  PLAN.md                  master plan (copy of the team doc)
  BUILD_PLAN.md            hardware prep and practice track
  IMPLEMENTATION_PLAN.md   this file
  logbook.md               tuning log (created when testing starts)
firmware/
  ZumoLineFollower/
    ZumoLineFollower.ino   the race sketch (milestones M4–M8)
```

## Sources

- [Pololu Zumo Shield Arduino library](https://github.com/pololu/zumo-shield-arduino-library), including the `LineFollower` example and the `ZumoMotors` and `ZumoReflectanceSensorArray` headers (v2.1.0).
- [Pololu Zumo Shield user's guide, sensor array pinout](https://www.pololu.com/docs/0J57/2.c).
