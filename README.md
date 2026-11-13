# RobotLine

Our line-following robot for the Robo League 2026-2027 (Varna): a Pololu Zumo (Arduino Uno + Zumo Shield v1.2) in the **Line following – ready kits** category.

- Round 1: Saturday 14 Nov 2026
- Round 2: 17 Apr 2027

## Start here

- [docs/SETUP.md](docs/SETUP.md) — install the Arduino IDE and library, get the code, run the setup check
- [docs/TEST_CHECKLIST.md](docs/TEST_CHECKLIST.md) — every test before the race, what to record, what to save
- [docs/GIT_GUIDE.md](docs/GIT_GUIDE.md) — what to do before pulling, committing, pushing, testing a branch, merging
- [AGENTS.md](AGENTS.md) — project rules and facts for AI coding agents (Codex, Claude) and teammates

## Plans

- [docs/PLAN.md](docs/PLAN.md) — master plan: this week, race-day strategy, rules, robot, software, practice track, road to Round 2
- [docs/BUILD_PLAN.md](docs/BUILD_PLAN.md) — getting the Zumo race-ready and building the practice track
- [docs/IMPLEMENTATION_PLAN.md](docs/IMPLEMENTATION_PLAN.md) — the software plan, milestones and speed tuning

## Code

- `firmware/SetupCheck/` — tests the buzzer, LED, sensors and motors
- `firmware/ZumoLineFollower/` — the race sketch. How to use it is at the top of the file:
  1. Switch on → short presses choose the mode (1 = Safe, 2 = Medium, 3 = Fast, 4 = Max = full power)
  2. Robot on the line, facing along it, hold the button ~1 s → it calibrates (spins ~4 s), LED on = ready
  3. On the start line: press and release → it starts the instant you let go
  4. It stops by itself only on solid black (the finish). It never stops on white: if it loses the line it spins back towards it. Press during a run = stop. `mode N` over USB (or Reset) = change mode
- [docs/TUNING.md](docs/TUNING.md) — how to make it fast: the USB tuning tool (`kd 16`, `slow 180`, `mode 4`, `save` …) and what to change when
- [docs/logbook.md](docs/logbook.md) — write every tuning test here
- `scripts/check.sh` — compiles both sketches for the Uno (run before every push)

## Working together

Step-by-step checklists: [docs/GIT_GUIDE.md](docs/GIT_GUIDE.md).


- **Shared branch:** `claude/adoring-pasteur-gcprs8` (the default branch). It always has tested code; **race-day code comes from here**.
- **Second teammate (with Codex):** works on **her own branch** per task (e.g. `codex/<task-name>`), then opens a pull request into the shared branch.
- **Before merging a pull request:** `./scripts/check.sh` passes, and we test it on the robot together.
- **Test someone's branch on the robot:**

  ```bash
  git fetch && git checkout <branch-name>
  ```

  Upload and test with `docs/TEST_CHECKLIST.md`. Go back afterwards:

  ```bash
  git checkout claude/adoring-pasteur-gcprs8 && git pull
  ```

- **Before you start:** close the sketch in the Arduino IDE, then `git pull`. **When you finish:** compile, commit, `git push`.
- Tell each other which file you are changing, so two people (or two AI tools) don't edit the same file at once.
- If `git pull` says "your local changes would be overwritten": `git stash`, then `git pull` (your changes are kept in the stash).
