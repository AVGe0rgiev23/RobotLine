# AGENTS.md — notes for AI coding agents (Codex, Claude) and teammates

## The project

A line-following robot for the Robo League 2026-2027 (Varna, Bulgaria), category **Line following – ready kits**. Race: **Saturday 14 Nov 2026**, then Round 2 on 17 Apr 2027. Two student beginners; keep everything simple and explained.

- **Robot:** Pololu Zumo chassis + Zumo Shield v1.2 + Arduino Uno R3 (ATmega328P) + Zumo reflectance sensor array (6 RC sensors). 4 × AA batteries.
- **Library:** Pololu `ZumoShield` 2.1.0 (Arduino Library Manager).
- **Track rules that matter:** white track, black 2 cm line, turns down to 3 cm radius, 90° crossings (go straight), lines ≥ 20 cm apart, 5 attempts (best time counts), start on the judge's signal with no delay, 3 min limit, hand rescue = +15 s, robot must not stand still > 30 s or skip turns.

## Repo layout

| Path | What |
| --- | --- |
| `firmware/ZumoLineFollower/ZumoLineFollower.ino` | The race program |
| `firmware/SetupCheck/SetupCheck.ino` | Hardware test (sensors, motor steps, straight-drive test) |
| `docs/PLAN.md` | Master plan |
| `docs/IMPLEMENTATION_PLAN.md` | Software plan, milestones, feature status table |
| `docs/TUNING.md` | USB tuning tool and what to change when |
| `docs/TEST_CHECKLIST.md` | Every test, what to record, pass criteria |
| `docs/logbook.md` | Results of every test run |
| `docs/SETUP.md`, `docs/BUILD_PLAN.md` | Laptop setup, hardware prep |
| `scripts/check.sh` | Compiles both sketches for the Uno and runs the computer tests |
| `tests/test_line_tracking.cpp` | Computer tests for `LineTracking.h` (crossing detection, line tracking) |

## Hardware facts (do not change)

- Sensor pins left → right: `4, A3, 11, A0, A2, 5`; emitter (LEDON) pin 2 on the Uno.
- Motors via the shield: pins 7, 8 (direction), 9, 10 (PWM); `motors.setSpeeds(left, right)`, each −400 … 400. 400 = 100% power; nothing faster exists in software.
- User button pin 12 (`Pushbutton button(ZUMO_BUTTON)`); buzzer pin 3 (jumper "Bz." on 328P); LED pin 13.
- `readLine()` returns 0–5000 (2500 = centred; negative error = line to the left); `readCalibrated()` 0–1000 per sensor. Default sensor timeout 2000 µs; Pololu recommends 1000–3000.
- Battery voltage on A1 only if the shield's "battery level" holes are joined with a soldered wire (divider 2/3; mV = analogRead(A1) × 5000 × 3 / 2 / 1023). Code has `BATTERY_WIRE_SOLDERED = false`.
- Motors run directly from battery voltage: alkaline ~6 V is faster than NiMH ~4.8 V. Never suggest more than 4 AA (motors ~6 V).

## How the race program works (keep this behaviour)

Button: short press = next mode (before calibrating); hold ~1 s = calibrate (spins ~4 s; refused with a low double tone if any sensor did not see both white and black); short press when calibrated = start on release. LED + buzzer show everything. 4 modes in `DEFAULT_MODES` (Safe tested; Medium/Fast/Max starting guesses). PD steering, slowdown in curves, inside track may reverse, straight over crossings, spin back to a line lost off one side, stop at the end of the line (line vanished while centred → 100 ms straight, then stop), soft start 150 ms, optional battery compensation, crossings/zebra/start line detected by `spansBothSides()` (dark at an outer sensor on both sides) or 5+ black → straight with no steering for `CROSSING_COMMIT_MS` (120 ms at speed 250, speed-scaled) after the last dark reading, lastError reset so it never spins back; steering uses `trackedPosition()` (dark patch nearest the previous position) from `firmware/ZumoLineFollower/LineTracking.h`, unit-tested in `tests/test_line_tracking.cpp` (run by `scripts/check.sh`), stop at the finish (solid black area, a black circle on the team's track: 5+ of 6 sensors above `FINISH_DARK_LEVEL` = 500 for `FINISH_BLACK_MS` = 120 ms at speed 250, scaled inversely with the current top speed; armed only after leaving black once, so a black start line is ignored; a "Finish check" line is printed after each run). USB tuning commands (`mode`, `speed`, `kp`, `kd`, `slow`, `rev`, `timeout`, `show`, `save`, `reset`) with EEPROM storage; code values win after an upload.

## Team decisions (respect these)

- A motor-balance trim and a "stop after 2 s without the line" version were **rejected** by the team. The current end-of-line stop (centred line vanishes → stop) was **requested** and is wanted.
- The Safe mode row `{250, 1, 4, 6, 50, 100}` is tested on the robot: do not change it without a test result in the logbook.
- Code freeze Friday 13 Nov, 20:00: after that only tuning numbers change.

## Rules for changes

1. **Compile before every commit:** `./scripts/check.sh` (needs `arduino-cli` with `arduino:avr` and `ZumoShield`; the script says how to install). Both sketches must compile for `arduino:avr:uno` with no warnings from our files.
2. **No `delay()` and no `Serial` printing inside the race loop** (`race()`); it slows steering.
3. **Keep it beginner-readable:** plain names, a short comment per block, all tuning numbers at the top.
4. **One feature per commit**, with a clear message. Update `docs/IMPLEMENTATION_PLAN.md` (status table) and the "HOW TO USE" header in the sketch when behaviour changes.
5. **Pull before starting, push when done.** Two people and two AI tools work on this repo: avoid editing the same file at the same time; tell each other what you are changing.

## Branches and how work gets merged

- `claude/adoring-pasteur-gcprs8` is the **shared branch** (also the repo's default branch). It always holds code that compiles and has been tested on the robot. **Race-day code comes from this branch.**
- **Codex / the second teammate works on their own branch**, created from the shared branch, named after the task, e.g. `codex/serial-report` or `codex/<short-task-name>`. Never commit directly to the shared branch from there.
- When a task is done on a branch:
  1. `./scripts/check.sh` passes (both sketches compile, no warnings from our files).
  2. Push the branch and open a pull request into `claude/adoring-pasteur-gcprs8`, describing what changed and how to test it on the robot.
  3. The team tests it on the robot together (`git fetch`, `git checkout <branch>`, upload, run the relevant rows of `docs/TEST_CHECKLIST.md`).
  4. Only after the robot test passes: merge the pull request. Everyone then runs `git checkout claude/adoring-pasteur-gcprs8 && git pull`.
- Before starting a branch, pull the latest shared branch, and keep branches short-lived (one task each) to avoid conflicts in `ZumoLineFollower.ino`.
- After the Friday 20:00 code freeze, only tuning-number changes are merged.
6. Memory: the Uno has 2 KB RAM; keep long strings in `F("...")`.

## Current status (12 Nov)

New Zumo follows the line perfectly in Safe mode on the real track, but too slowly. Next: tune Medium → Fast → Max with the USB tool, try `timeout 1000`, run `docs/TEST_CHECKLIST.md`, mock competition Friday.


## Git reminders for the team (agents: do this every time)

The team are beginners. Whenever they are about to pull, commit, push, switch branches, test a branch or merge, **remind them of the matching checklist in `docs/GIT_GUIDE.md`** and give the exact commands, one per code block. Most important each time:

- Close the sketch in the Arduino IDE before `git pull` / `git checkout` (it auto-saves old code over new).
- `git status` first; `git stash` if there are local changes they do not want to commit.
- Compile (`./scripts/check.sh` or IDE Verify) before every commit and push; test on the robot if driving behaviour changed.
- `git diff` before committing, to catch stray typed or pasted text.
- `git pull` before `git push`; on "CONFLICT" stop and help, never force-push.
- Race-day code only from the shared branch.