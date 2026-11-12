# Build Plan — getting the Zumo race-ready

As of 11 Nov 2026. Race: **Saturday 14 Nov 2026**, Line following – ready kits. Companion to [PLAN.md](PLAN.md) and [IMPLEMENTATION_PLAN.md](IMPLEMENTATION_PLAN.md).

The robot is already built: Pololu Zumo chassis + Zumo Shield v1.2 + Elegoo Uno R3 + Zumo reflectance sensor array. "Building" this week means four things: **check it, power it, make it fast, and build the practice track.** Nothing below needs a soldering iron.

## Speed: what actually limits the Zumo

Read this first, because it decides where the effort goes.

| What sets the speed | How much we control it this week | What we do |
| --- | --- | --- |
| **Software** (how well it stays on the line at full power) | Fully | The biggest lever. See IMPLEMENTATION_PLAN.md |
| **Battery voltage** | Fully | The motors run straight from the 4 AA cells. Fresh, fully charged cells = faster. Charge right before racing |
| **Grip** (tracks on the surface) | Fully | Clean tracks; correct tension |
| **Motor gear ratio** | Not this week | Fixed by the motors inside. Possible Round 2 upgrade (section 7) |
| **Weight** | A little | Nothing extra on the robot |

The LineFollower example already drives the motors at full power (400 out of 400) on straights. So the Zumo will not get faster by "turning the speed up". It gets faster by **staying on the line at full power**, cutting less speed in the turns and never losing the line.

## 1. Inspection (Wednesday, 20 minutes)

- [ ] Every screw tight: shield to chassis, sensor array to chassis.
- [ ] The Zumo Shield is pushed fully onto the Uno's headers; no bent pins.
- [ ] Tracks sit fully on both sprockets on each side, not twisted.
- [ ] Nothing loose, dangling or sticking out (rule: the robot must not change size or drop parts).
- [ ] Note the motor gear ratio if it's printed on the motors or in the order (for example 75:1 or 100:1). Write it in PLAN.md's open questions.

## 2. Jumpers (Wednesday, 5 minutes)

| Jumper | Where | Correct setting for an Uno | Why |
| --- | --- | --- | --- |
| LEDON | Sensor array, blue shorting block at the end | Connects LEDON to **pin 2** | Lets the code switch the sensor LEDs; the library expects pin 2 on an Uno |
| Bz. | Shield, next to "328P" / "32U4" | **328P** side (pin 3) | Buzzer works with the Uno |
| Battery level | Shield, two holes marked "Battery Level" near "A1" | Disconnected by default; joined with a short **soldered wire** | Optional. Then set `BATTERY_WIRE_SOLDERED = true` in the code to get a low-battery warning (and optional battery compensation) |

## 3. Power (Thursday)

- [ ] Two sets of 4 NiMH AA, labelled **A** and **B** (marker or tape). Never mix cells between sets.
- [ ] Charge set B while testing with set A.
- [ ] Power test: switch ON → the shield's power LEDs light; the Uno runs; the two small red LEDs on the sensor array glow (that means the sensor LEDs are on).
- [ ] Note in the logbook which set you used for every timed lap. Speed drops as a set empties, so compare lap times on fresh sets only.
- [ ] Friday night: charge both sets fully. Saturday: race the fast attempts on the freshest set.

## 4. Grip and weight (Thursday)

- [ ] Clean both tracks with sticky tape (press and peel) until no dust comes off. Repeat before every attempt on Saturday.
- [ ] Do not add anything: no blade, no decorations, no extra boards. Weight costs acceleration and braking.
- [ ] Keep the battery cover closed so cells cannot shift.

## 5. Practice track (Wednesday)

Build one track that contains every hard element the rules allow (PLAN.md section 8), and rebuild the layout every session.

**Making the line:**

- **Straights and gentle curves:** black insulating tape, 19–20 mm wide.
- **Tight 3 cm-radius turns:** tape wrinkles on curves that tight. Cut the curve from black paper (or matte black card) and glue it down, or colour it with a wide black permanent marker. Make the line 2 cm wide, like the real one.
- **Crossing:** two strips at exactly 90°, with at least 10 cm of straight line after the crossing.

**Layout (fits on 2–3 poster boards, roughly 1.5 × 1 m):**

1. Start line → 1.5 m straight (top speed).
2. Hairpin with a 3 cm radius (hardest turn).
3. S-curve.
4. Perpendicular crossing.
5. Two parallel lines 20 cm apart.
6. Finish line.

## 6. Laptop and toolkit (Wednesday)

- [ ] Arduino IDE installed; board set to "Arduino Uno"; the right COM/USB port selected.
- [ ] ZumoShield library installed (Library Manager → search "ZumoShield").
- [ ] Example "ZumoBuzzerExample" uploads and plays (proves the toolchain without batteries).
- [ ] Spare USB cable; code also copied to a USB stick.
- [ ] Race-day bag packed Friday night (PLAN.md section 2).

## 7. Round 2 hardware upgrades (decide after Saturday)

Only after the software is reliable. Ask the organizers first whether modified kits stay in the ready-kits category.

| Upgrade | Effect | Cost / risk |
| --- | --- | --- |
| **Faster motors** (lower gear ratio, for example 50:1 instead of 75:1, or 75:1 instead of 100:1) | Top speed rises in proportion: 75:1 → 50:1 is 1.5× faster | Less torque, harder to control in 3 cm turns; must be re-tuned. Use "HP" (high-power) micro metal gearmotors, same size as the current ones |
| **Wheel encoders** (magnetic encoder kits for micro metal gearmotors; need motors with an extended back shaft) | Measure real speed and distance; enables track memory | Needs free interrupt pins and wiring on the shield's prototyping area |
| **Battery-level monitoring** (soldered wire on the battery-level holes) | Low-battery warning; same behaviour on full and tired batteries | One short wire; the code is already written |
| **Better batteries** (high-capacity low-self-discharge NiMH, all matched) | More consistent voltage | Cheap |
