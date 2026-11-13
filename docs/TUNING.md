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
| `edge N` | Sharp-corner pivot: when the line reaches an outer sensor, the outer track drives at N and the inner track goes backwards at `rev`. 0 = off | `edge 200` |
| `hold N` | After a sharp corner, stay at curve speed for N ms, then speed up smoothly. 0 = off | `hold 120` |
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
| Flies out of **sharp** corners / hairpins at high speed | `edge` on (start 200), `hold` on (start 120); then `rev` up | `edge` ±20, `hold` +30 |
| Pivots are too jerky or lose too much speed | `edge` up (gentler pivot, outer track faster), or `edge 0` to switch the pivot off | +20 |
| Shoots out of the second half of a hairpin or S-curve | `hold` up | +30 |
| Slow in gentle curves now that sharp corners are safe | `slow` down | −20 |
| Flies out of curves (too fast to turn) | `slow` up | +40 |
| Slow in curves but never leaves the line | `slow` down | −20 |
| Cannot turn sharply enough in hairpins | `rev` up | +50 |
| Jerky on gentle curves | `rev` down | −50 |

Change **one value at a time**, run **3 laps**, write it down.

## Corner assist: fast AND no mistakes in sharp corners

At high speed the robot reaches a sharp corner before normal steering can turn it, and slides off on the outside. The corner assist (modes 2–4, off in Safe) fixes this without slowing the straights:

1. **Edge pivot (`edge`):** as soon as the line reaches an outer sensor, the robot stops steering gently and pivots: outer track at `edge`, inner track backwards at `rev`. It turns almost on the spot.
2. **Hold (`hold`):** after the corner it stays at curve speed for `hold` ms (so it finishes hairpins and S-curves), then accelerates back to full speed over 0.1 s.

Because sharp corners are now handled, **lower `slow`** in the fast modes so the robot keeps more speed in gentle curves. That is where the time is won.

Tuning the fastest mode (`mode 4`):

1. Start: `edge 200`, `hold 120`, `rev 250`. Run 3 laps on a track with your sharpest corner.
2. Still leaves the line in the sharpest corner → `rev 300`, then `edge 160`.
3. Clean in corners → lower `slow` by 20 at a time until gentle curves get untidy, then go back one step.
4. Finally try `speed 400` if not already, and `timeout 1000`.

## Finish and lost line

- **Finish:** if 5 of 6 sensors see black for a moment, the robot stops; the Serial Monitor says "finish (5+ sensors black)". The moment is `FINISH_BLACK_MS` = 120 ms at speed 200 and shorter when faster (60 ms at speed 400), because it is the same distance. A 2 cm crossing or zebra stripe passes much faster, so they do not stop it; a black start line under the robot is ignored.
  - Stops on a zebra stripe or crossing by mistake → raise `FINISH_BLACK_MS` (e.g. 160).
  - Drives over the finish without stopping → lower `FINISH_BLACK_MS` (e.g. 90).
- **Zebra stripes** (black bars, white gaps): after each bar the robot keeps driving straight at full speed through the white gap (up to `ZEBRA_GAP_MS` = 250 ms), so it neither slows down nor searches. If the gaps are very long and it starts searching between bars, raise `ZEBRA_GAP_MS`.
- **Lost line:** the robot never stops on white. If it overshoots a sharp turn it spins back towards the side the line was last clearly on, finds it and continues (with the `hold` slow-down after it, if `hold` is on). Each recovery costs time, so if it happens often in one corner: `hold` up, then `rev` up, then `slow` up.

## Order of work

1. `mode 1` (Safe): check it still follows perfectly. Note the lap time.
2. `mode 2` (Medium): tune until 3 clean laps. `save`.
3. `mode 3` (Fast): start from the Medium values (`kd`, `slow`, `rev`), raise as needed. `save`.
4. `mode 4` (Max, `speed 400`): same. `save`.
5. `timeout 1000`, calibrate again, check "Calibration OK", run the best mode again. If it is smoother, `save`.
6. Copy the final values into the `DEFAULT_MODES` table in the code and push, so they are safe even if the robot's memory is cleared.
