# RobotLine

Our line-following robot for the Robo League 2026-2027 (Varna): a Pololu Zumo (Arduino Uno + Zumo Shield v1.2) in the **Line following – ready kits** category.

- Round 1: Saturday 14 Nov 2026
- Round 2: 17 Apr 2027

## Start here

- [docs/SETUP.md](docs/SETUP.md) — install the Arduino IDE and library, get the code, run the setup check

## Plans

- [docs/PLAN.md](docs/PLAN.md) — master plan: this week, race-day strategy, rules, robot, software, practice track, road to Round 2
- [docs/BUILD_PLAN.md](docs/BUILD_PLAN.md) — getting the Zumo race-ready and building the practice track
- [docs/IMPLEMENTATION_PLAN.md](docs/IMPLEMENTATION_PLAN.md) — the software plan, milestones and speed tuning

## Code

- `firmware/SetupCheck/` — tests the buzzer, LED, sensors and motors
- `firmware/ZumoLineFollower/` — the race sketch. How to use it is at the top of the file:
  1. Switch on → short presses choose the mode (1 beep = Safe, 2 = Fast)
  2. Robot on the line, facing along it, hold the button ~1 s → it calibrates (spins ~4 s), LED on = ready
  3. On the start line: press and release → it starts the instant you let go
  4. Press during a run = stop. Reset button = choose a mode again
- [docs/logbook.md](docs/logbook.md) — write every tuning test here
