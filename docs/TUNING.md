# Tuning guide — making the Zumo fast

The goal: the fastest mode that stays on the line for 3 laps in a row. Tune with the USB tuning tool, so you do not need to upload or recalibrate after each change.

## Before you start

1. **Batteries:** use the same type you will race with. Fresh alkaline AA (about 6 V) make the robot about 25% faster than rechargeable NiMH (about 4.8 V). Use a brand new alkaline set for the fast attempts on race day. Never more than 4 AA cells: the motors are made for about 6 V.
2. **Tracks:** clean both tracks with sticky tape before every tuning session and every race attempt. Grip limits how fast it can corner.
3. **Track:** a matte white surface, black 2 cm line, with a straight, curves, a tight hairpin and a crossing.

## The tuning tool (USB)

1. Upload `firmware/ZumoLineFollower/ZumoLineFollower.ino`.
2. Open the Serial Monitor: **9600 baud**, line ending (bottom right) set to **Newline**.
3. Type a command and press Enter. The robot answers with the new settings of the current mode.

| Command | What it changes | Example |
| --- | --- | --- |
| `help` | Lists the commands | `help` |
| `show` | Shows all 4 modes and the sensor timeout | `show` |
| `mode N` | Switches to mode N (1 Safe, 2 Medium, 3 Fast, 4 Max). Works even after calibrating | `mode 4` |
| `speed N` | Top speed on straights, 0–400 (400 = 100% motor power) | `speed 400` |
| `kp N/D` | Kp, steering strength towards the line | `kp 1/4` |
| `kd N` | Kd, stops the wobble | `kd 16` |
| `slow N` | turnSlowdown, how much it brakes in curves | `slow 180` |
| `rev N` | reverseLimit, how fast the inside track may go backwards in tight turns | `rev 250` |
| `timeout N` | Sensor read timeout in µs, 1000–3000. Lower = faster steering. **Calibrate again afterwards** | `timeout 1000` |
| `save` | Keeps all values after switch-off | `save` |
| `reset` | Back to the numbers written in the code | `reset` |

A change works on the next run. Without `save` it is lost when you switch off. After each run the Serial Monitor shows why it stopped and the settings it used: copy them into `docs/logbook.md`.

The USB cable drags the robot while it drives. Either hold the cable up so it follows the robot, or unplug for the run and plug back in to change settings (the robot keeps its calibration and settings while switched on).

## What to change when

| What you see | Change | Step |
| --- | --- | --- |
| Wobbles left-right on straights | `kd` up | +4 |
| Still wobbles after several Kd steps | `kp` down | 1/4 → 1/5 → 1/6 |
| Slow to react, drifts off before a curve | `kp` up | 1/4 → 1/3 |
| Flies out of curves (too fast to turn) | `slow` up | +40 |
| Slow in curves but never leaves the line | `slow` down | −20 |
| Cannot turn sharply enough in hairpins | `rev` up | +50 |
| Jerky on gentle curves | `rev` down | −50 |

Change **one value at a time**, run **3 laps**, write it down.

## Finish (solid black area)

The robot stops when 5 of 6 sensors are dark for a short distance (120 ms at speed 250, 75 ms at 400). After every run the Serial Monitor prints a line like:

`Finish check: most dark sensors at once: 6 of 6; longest time with 5+ dark: 48 ms (needed more than 75 ms).`

| What the line says | Change in the code (top of `ZumoLineFollower.ino`), then upload |
| --- | --- |
| Longest time is shorter than "needed" | Lower `FINISH_BLACK_MS` (e.g. 120 → 80), but keep it above what a crossing gives |
| "most dark sensors" is below 5 | Lower `FINISH_DARK_LEVEL` (500 → 400), or `FINISH_MIN_DARK` 5 → 4 |
| It stops on a crossing | Raise `FINISH_BLACK_MS` (e.g. 120 → 160) |

## Order of work

1. `mode 1` (Safe): check it still follows perfectly. Note the lap time.
2. `mode 2` (Medium): tune until 3 clean laps. `save`.
3. `mode 3` (Fast): start from the Medium values (`kd`, `slow`, `rev`), raise as needed. `save`.
4. `mode 4` (Max, `speed 400`): same. `save`.
5. `timeout 1000`, calibrate again, check "Calibration OK", run the best mode again. If it is smoother, `save`.
6. Copy the final values into the `DEFAULT_MODES` table in the code and push, so they are safe even if the robot's memory is cleared.
