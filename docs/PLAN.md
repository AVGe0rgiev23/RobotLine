# Line Follower Master Plan — Robo League 2026-2027

As of 11 Nov 2026. Live, editable version: the team's Claude Doc "Line Follower Master Plan — Robo League 2026-2027".

**Competition: this Saturday, 14 Nov 2026, Junior Hall, Varna.** We race our Pololu Zumo (Arduino Uno + Zumo Shield) in the **Line following – ready kits** category. Goal: finish a run first, then go faster. Start with section 1 (this week) and section 2 (race day); the rest explains the rules, the robot and the software. The league also has a Round 2 on 17 Apr 2027 (section 9).

See also: [BUILD_PLAN.md](BUILD_PLAN.md) (getting the robot race-ready) and [IMPLEMENTATION_PLAN.md](IMPLEMENTATION_PLAN.md) (the software, step by step).

## 1. This week, day by day

Batteries arrive Thursday, so today is for everything that needs no batteries: category, software setup and the track. Thursday and Friday are for driving and tuning.

**Wednesday (today): no batteries yet**

- [ ] Ask the organizers to move the team to the ready-kits category.
- [ ] Check the two jumpers: sensor array LEDON on pin 2, buzzer "Bz." on 328P.
- [ ] Install the Arduino IDE and the ZumoShield library. Upload a buzzer or button example over USB to prove programming works.
- [ ] Open the LineFollower example and read it together until you can explain each part (section 7).
- [ ] Build the practice track (section 8).

**Thursday: batteries arrive**

- [ ] Buy the batteries and charger. Rechargeables like Energizer usually come pre-charged, so test straight away; charge the second set meanwhile.
- [ ] Upload the LineFollower example unchanged, at low speed. Goal: full laps.
- [ ] Tune Kp and Kd (section 7). Clean the tracks with sticky tape: dust costs grip.

**Friday: features and freeze**

- [ ] Add crossings, line-lost recovery, Safe and Fast modes and the instant start (features 3–6).
- [ ] Mock competition: new track layout, 5 attempts, real rules, phone timing.
- [ ] Freeze the code by 20:00 and save it to GitHub. Charge both battery sets. Pack the bag (section 2).

**Saturday: race**

- [ ] Arrive early. At the technical check, confirm the ready-kits category.
- [ ] Calibrate on the practice track; attempt 1 in Safe mode, Fast only after a finish.
- [ ] Write down the track surface, timing gate, finish and winning times, for Round 2.

## 2. Saturday: the 5-attempt strategy

Bank a safe time first, then climb in speed one step at a time. Only the best attempt counts, so a crash late on costs nothing once an early attempt has a time.

| Attempt | Mode | Rule |
| --- | --- | --- |
| 1 | Safe | Must finish. This time protects us from a zero |
| 2 | Safe or Fast | Fast only if attempt 1 finished cleanly |
| 3 | Fast | If attempt 2 failed, back to Safe with small fixes |
| 4 | Fast | Only if the last attempt was clean |
| 5 | Best mode that worked | Go all-in only if we already have a good time |

**On the day:**

1. Arrive early; pass the technical check with the robot race-ready.
2. Use the practice track: calibrate and test Safe mode.
3. Before each attempt: fresh battery set if needed, clean the tracks, recalibrate if the light changed. Keep the robot switched on between calibration and start.
4. Roles: the **pilot** places and starts the robot and does any rescue; the **engineer** handles batteries, modes and the logbook.
5. If the robot loses the line and does not recover within about 5 s, rescue it (+15 s). A finish with a penalty beats no time.

**Race-day bag:**

- [ ] Zumo + 2 charged battery sets + charger + emergency alkaline AA
- [ ] Laptop with Arduino IDE, the ZumoShield library and the final code (backup on a USB stick); USB cable + spare
- [ ] Small screwdriver, tape (for cleaning tracks), cloth
- [ ] Logbook with the tested settings for each mode

## 3. Key dates and our category

The Zumo is a bought, ready-made kit, so it belongs in **Line following – ready kits (assembled or for assembly)**. The "made by participants" category bans bought kits, and racing the Zumo there risks disqualification at the technical check. Ask the organizers today to move the team to the ready-kits category, same age group.

| Event | Date | Where / what |
| --- | --- | --- |
| Round 1 | 14 Nov 2026 | Junior Hall, Varna. Goal: finish, then go faster |
| Round 2 registration closes | 2 Apr 2027 | Register early, in the ready-kits category |
| Round 2 | 17 Apr 2027 | Junior Hall, Varna. Goal: first place |

The league is a championship: points from both rounds add up (1st = 10, 2nd = 8, 3rd = 6, 4th = 4, 5th–8th = 3, 9th–12th = 2, the rest = 1). Robots that never finish all share the lowest points, so one slow finish on Saturday already earns more than a failed fast robot.

## 4. The rules that shape the plan

The best time out of 5 attempts wins, so our order is always: **finish first, then go fast**.

| Rule | What it means for us |
| --- | --- |
| Must fit in a 25 × 25 × 25 cm cube; no weight limit; fully autonomous | The Zumo (about 10 × 10 cm) fits easily. Nothing may be added that sticks out or falls off |
| White track, black line 2 cm wide | The Zumo's sensor array is made for this |
| Turns can be as tight as 3 cm radius | The robot must slow down in turns. Tracks let the Zumo spin on the spot, which helps |
| Perpendicular crossings: go straight; at least 10 cm straight after | Software must notice "all sensors black" and drive straight through |
| Adjacent lines at least 20 cm apart | The sensor array is narrower, so it cannot see a neighbouring line |
| The track is revealed on competition day | We cannot practise the real layout; the robot must handle any layout |
| 5 attempts, best time counts | A safe attempt first, faster ones later (section 2) |
| Start on the judge's signal, no delay; timer starts at the automatic gate | One button press starts the run; no start music or countdown |
| 3 minutes maximum per attempt | Generous: even a slow Zumo finishes in time |
| Hand help to put the robot back = +15 s | A rescue is better than losing the attempt |
| Must not skip turns; must not stand still more than 30 s | Line-lost recovery must find the same line again and keep moving |
| Must not change size after the technical check | Race-ready shape before the check |
| Practice track available before the competition starts | Arrive early to calibrate and test there |

## 5. Our robot: what the Zumo has and what to check

Everything a line follower needs is already on the robot and wired to fixed Arduino pins. Our job is to check a few jumpers and then write the software.

| Part | Arduino pin | What to check |
| --- | --- | --- |
| Elegoo Uno R3 (the brain) | — | Sits under the Zumo Shield; programmed over USB as usual |
| Two gear motors + tracks, motor driver on the shield | 7, 8 (direction), 9, 10 (speed) | Tracks clean and not loose |
| Reflectance sensor array, 6 sensors, at the front | 4, A3, 11, A0, A2, 5 | The blue jumper on the array must connect LEDON to **pin 2**, the setting for an Uno |
| Power switch and voltage booster | — | Runs everything from 4 AA batteries |
| User button | 12 | Start and speed-mode selection |
| Buzzer | 3 | The "Bz." jumper must be on the **328P** side (pin 3), the setting for an Uno |
| Yellow LED | 13 | Status light |
| Battery level | A1 | Only works after the two "Battery Level" holes are joined with a soldered wire; then set `BATTERY_WIRE_SOLDERED = true` |
| Accelerometer, compass, gyro | I²C (SDA, SCL) | Not needed for line following |

**Without batteries (today):** USB powers the Uno, so you can upload programs and test the buzzer, LED and button. The motors and the sensors' infrared LEDs need the AA batteries and the power switch on.

The loose N20 motor, its cable and the Zumo sensor board from the first photos are not needed. Keep them as spares.

## 6. What to get: batteries, charger, track materials

The only things missing are AA batteries, a charger and a practice track. Buy them tomorrow.

- **8 rechargeable NiMH AA batteries, 2000 mAh or more:** two sets of 4, so one set charges while the other runs. Label them A and B, and never mix old and new cells.
- **A NiMH charger** that charges 4 AA at once.
- **Practise and race on the same battery type.** Alkaline AA give 6 V instead of 4.8 V, so the robot runs faster and the tuning changes. Bring a pack of alkalines only as an emergency spare.
- **Practice track:** a large white matte surface (poster boards, a white banner or A1 sheets) and black insulating tape about 19–20 mm wide.

One option at Elimex Burgas (stock checked on 11 Nov, tel. +359 89 22 33 142, open Mon–Fri 9:00–13:30 and 14:30–18:00):

| Item | Code | Qty | Each | Total |
| --- | --- | --- | --- | --- |
| [Energizer Maxi charger + 4 × AA 2000 mAh](https://elimex.bg/product/83382-zaryadno-ustroystvo-energizer-maxi-emg-962472-962473-4xaa-2000mah) | 83382 | 1 | €25.90 | €25.90 |
| [LAVA AA NiMH 1000 mAh, second set](https://elimex.bg/product/75777-akumulator-1-2v1000mah-ni-mh-aa-r6-lava) | 75777 | 4 | €1.90 | €7.60 |
| [Black tape 19 mm](https://elimex.bg/product/94501-pvc-izolacionna-lenta-emos-20-19mm-black) | 94501 | 2 | €1.48 | €2.96 |
| **Total** | | | | **€36.46** |

## 7. Software: how the robot thinks and the order we build it

The software decides the race now. We start from Pololu's free **ZumoShield** Arduino library (install it from the Library Manager in the Arduino IDE). Its **LineFollower** example is made for this exact robot: it calibrates by spinning over the line, then follows it with PD steering. We run it unchanged first, then add one feature at a time and test after each. Details: [IMPLEMENTATION_PLAN.md](IMPLEMENTATION_PLAN.md).

**The main loop** (repeats hundreds of times per second):

1. **Read the 6 sensors.**
2. **Find the line position**, a number from 0 (line under the far-left sensor) to 5000 (far right); 2500 means centred.
3. **Error** = position − 2500: negative = line to the left, positive = line to the right.
4. **PD steering:** correction = Kp × error + Kd × (change in error since the last loop). Kp turns towards the line; Kd stops the wobble.
5. **Motors:** left = base speed + correction, right = base speed − correction.

**Features, in the order we add them:**

| # | Feature | What it does | When |
| --- | --- | --- | --- |
| 1 | Example unchanged, low speed | Proves motors, sensors and calibration work | Thursday |
| 2 | Tuned PD | Kp and Kd set on our practice track | Thursday |
| 3 | Crossings | Almost all sensors black → ignore steering and drive straight a few cm | Friday |
| 4 | Line-lost recovery | All sensors white → spin towards the side where the line was last seen, until found | Friday |
| 5 | Speed modes | Press the button 1 or 2 times before the start for Safe or Fast; the buzzer beeps the mode back | Friday |
| 6 | Instant start | The run starts when the button is released; the example's start music is removed | Friday |
| 7 | Slow down in turns | Base speed drops when the error is big and rises on straights | Round 2 |
| 8 | Battery compensation | Read the battery level (A1) and scale motor power, so tuning holds on full and tired batteries | Round 2 |
| 9 | More speed modes | Safe / Medium / Fast / Max, each with its own tested Kp, Kd and speed | Round 2 |
| 10 | Stretch: track memory | Needs wheel encoders added to the motors; sprint the straights, brake before curves | Only if 1–9 are perfect |

**Tuning method** (on the practice track): set Kd = 0 and a low base speed; raise Kp until the robot follows but wobbles; raise Kd until the wobble stops; raise the base speed a little and repeat. Write every setting and lap time in a logbook.

**Code rules:** calibrate before the run, not on the start line; no delay() and no Serial printing in the race loop; all tuning numbers at the top of the program; save every working version to this GitHub repo before changing anything.

## 8. Practice track

The real track is a secret until race day, so our track contains every hard element the rules allow, and we change its layout often.

**Materials:** a large white matte surface and black tape about 19–20 mm wide. Matte, not glossy: shiny surfaces reflect infrared light and confuse the sensors.

**Every practice track must include:**

- [ ] A straight of at least 1.5 m, for top speed and braking.
- [ ] Tight turns with a 3 cm radius (the minimum), including a full hairpin.
- [ ] Gentle curves and an S-curve.
- [ ] A perpendicular crossing with 10 cm of straight line after it.
- [ ] Two parallel lines 20 cm apart.
- [ ] A start and a finish line; time every lap with a phone.

**Habits:** test in daylight and under lamps; change the layout every session; keep a logbook (date, mode, battery set, time, what went wrong).

## 9. After Saturday: road to Round 2

After Saturday, the Zumo gets better software in four phases. Each phase ends with a test the robot must pass before the next one starts. (The live doc has a timeline drawing of these phases.)

| Phase | Dates | Gate |
| --- | --- | --- |
| 0. Round 1 week | 11–13 Nov 2026 | Round 1 race on 14 Nov |
| 1. Review and learn | 16 Nov – 6 Dec | Both of us can explain every line of our code |
| 2. Software v2 | 7 Dec – 31 Jan | 10 clean laps in a row in Fast mode |
| 3. Speed push | 1 Feb – 14 Mar | 5 laps in a row in Max mode with no line losses |
| 4. Race preparation | 15 Mar – 16 Apr | A mock competition with 5 finishes out of 5 |
| Register for Round 2 | by 2 Apr 2027 | Ready-kits category |
| Code freeze | 10 Apr 2027 | Only tuning numbers change after this |
| Round 2 | 17 Apr 2027 | Race day |

The software and speed phases are the longest on purpose: races are won by testing, not by writing more code.

**Phase 1 — Review and learn (16 Nov – 6 Dec).**

- [ ] Write up Round 1: what worked, what broke, the track and the winning times.
- [ ] Rebuild the practice track to match the real surface and style.
- [ ] Learn PD properly: change one number at a time and log the effect.

**Phase 2 — Software v2 (7 Dec – 31 Jan).**

- [ ] Slow down in turns, battery compensation (solder the battery-level wire), four speed modes, better line-lost recovery.

**Phase 3 — Speed push (1 Feb – 14 Mar).**

- [ ] Raise speed step by step; test new layouts and lighting every session.
- [ ] Stretch: wheel encoders and track memory.

**Phase 4 — Race preparation (15 Mar – 16 Apr).**

- [ ] Register for Round 2 (ready kits) before 2 April.
- [ ] Weekly mock competitions on new layouts; freeze the code on 10 April.

**Optional second robot:** if time allows, build our own robot for the "made by participants" category as a second entry. First ask the organizers whether one team may enter both line-following categories with two different robots.

## 10. Risks and fallbacks

With a ready robot, most failures come from batteries, dirt, settings and last-minute code changes.

| Risk | How we prevent it | Fallback on the day |
| --- | --- | --- |
| Wrong category | Ask the organizers today | Settle it at registration, before the technical check |
| Flat or mixed batteries | Two labelled sets, charged the night before | Swap sets; emergency alkalines (robot faster: use Safe) |
| Sensor LEDs off | Check the LEDON jumper is on pin 2 | Check the jumper, then recalibrate |
| Hall lighting or surface confuses sensors | Calibrate on the practice track; test under different lights | Recalibrate right before each attempt |
| Dusty tracks slip in turns | Clean the tracks with sticky tape | Clean between attempts |
| Robot fails on an unseen layout | Change the practice layout every session | Safe mode first (section 2) |
| Last-minute code change breaks it | Freeze the code Friday 20:00; every working version in GitHub | Upload the last version that worked |
| Laptop or cable problem | Code on a USB stick too; spare USB cable | Borrow a laptop, install the library, upload |

## 11. Open questions

- [ ] **Category:** have the organizers moved us to "Line following – ready kits"?
- [ ] **Motor gear ratio:** which motors are in our Zumo (often 75:1 or 100:1)? Check the label on the motors or the order. It tells us the top speed.
- [ ] **Age group:** grades 4–7 or 8–12? It tells us whose times to compare against.
- [ ] **Track details (from Round 1):** surface, timing gate, how the finish works, winning times.
- [ ] **Round 2 registration:** who registers the team before 2 April?

## Sources

- Robo League 2026-2027 rules (PDF provided by the team): dates, points, general rules, and the line-following rules for ready kits and for robots made by participants.
- [Pololu Zumo Reflectance Sensor Array](https://www.pololu.com/product/1419) and its [pinout in the Zumo Shield user's guide](https://www.pololu.com/docs/0J57/2.c): sensor pins and the LEDON jumper.
- [Elimex Burgas product pages](https://elimex.bg/product/83382-zaryadno-ustroystvo-energizer-maxi-emg-962472-962473-4xaa-2000mah) and [store locator](https://elimex.bg/stores-locator): prices, stock and opening hours, checked on 11 Nov 2026.
