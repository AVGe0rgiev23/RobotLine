# Setup — computer, Arduino IDE and first tests

Do this on the laptop you will take to the competition. Takes about 30 minutes. Steps 1–6 work **without batteries**; step 7 needs the AA batteries.

What you need: the Zumo (Uno + Zumo Shield + sensor array, already assembled), the USB cable (USB-A to USB-B, the "printer" type), a laptop with internet.

## 1. Install the Arduino IDE

1. Go to <https://www.arduino.cc/en/software> and download **Arduino IDE 2** for your system (Windows, macOS or Linux).
2. Install it with the default options. On Windows, say **Yes** when it asks to install USB drivers.
3. Open the Arduino IDE.

## 2. Connect the robot and pick the board

1. Plug the USB cable into the **Uno** (the board under the Zumo Shield) and into the laptop. The Uno's green "ON" LED lights up.
2. In the IDE: **Tools → Board → Arduino AVR Boards → Arduino Uno**.
3. **Tools → Port** → pick the port that appeared when you plugged in the robot (Windows: `COM3`, `COM4`…; macOS: `/dev/cu.usbmodem…` or `/dev/cu.usbserial…`; Linux: `/dev/ttyACM0` or `/dev/ttyUSB0`). Tip: unplug the cable and look again; the port that disappears is the robot.

**No port appears?** Try another USB cable: some cables only charge and carry no data. Some Elegoo boards use a "CH340" USB chip that needs a driver on older Windows or macOS; search for "CH340 driver" from Elegoo or the chip maker (WCH), install it, restart the IDE.

## 3. Install the ZumoShield library

1. **Tools → Manage Libraries…** (or the books icon on the left).
2. Search **ZumoShield**. Install **ZumoShield by Pololu** (version 2.1.0 or newer).
3. If it asks to install dependencies, choose **Install all**.

## 4. First upload: prove everything works

1. **File → Examples → ZumoShield → ZumoBuzzerExample**.
2. Click **Upload** (the → arrow button). Wait for "Done uploading".
3. The Zumo plays tunes. That proves the laptop, cable, board, library and buzzer all work.

No sound? Check the **"Bz." jumper** on the shield is on the **328P** side.

## 5. Get our code from GitHub

Our repo: <https://github.com/AVGe0rgiev23/RobotLine>. Our work is on the branch `claude/adoring-pasteur-gcprs8` until it is merged into `main`.

**Easiest: GitHub Desktop** (recommended, lets you get updates with one click)

1. Install GitHub Desktop from <https://desktop.github.com> and sign in.
2. **File → Clone repository** → pick `AVGe0rgiev23/RobotLine` → Clone.
3. At the top, **Current branch** → choose `claude/adoring-pasteur-gcprs8`.
4. Later, click **Fetch origin** / **Pull** to get new code.

**Without GitHub Desktop:** on the GitHub page, switch to the branch, then **Code → Download ZIP**, and unzip it.

## 6. Upload the setup check program

1. In the Arduino IDE: **File → Open** → `RobotLine/firmware/SetupCheck/SetupCheck.ino`.
2. Click **Upload**.
3. You hear a short beep and the yellow LED (pin 13) blinks: the program is running.
4. Open the **Serial Monitor** (magnifying-glass icon, top right) and set it to **9600 baud**. Lines of 6 numbers scroll by: the 6 sensors, left to right.

Without batteries the numbers do not mean anything yet: the sensors' infrared lights need battery power.

## 7. Full check with batteries (Thursday)

Put 4 charged AA batteries in, keep the USB cable connected, and slide the power switch to **ON**.

**Sensors**

1. The two small red LEDs on the sensor array glow (its infrared lights are on).
2. Hold the robot over white paper: all 6 numbers are low (a few hundred).
3. Move a strip of black tape under the robot from left to right: each number jumps high (over 1000, up to 2000) in turn, **first number = leftmost sensor**.

**Motors** (lift the robot first, so the tracks touch nothing)

1. Press and release the **user button** (next to the power switch; not the Reset button).
2. The Serial Monitor names each step while it happens: left track forward, left backward, right forward, right backward, both forward, spin.
3. Write down any step that does the wrong thing. A track that runs backwards when it should go forwards is fixed in code later; tell us.

**Straight-drive test** (robot on the floor, 1.5 m of free space in front)

1. **Hold** the user button about 1 second, then let go.
2. After 1 s the robot drives straight ahead for 2 s.
3. If it curves, one motor is weaker: curving left = left side slower.

When all of this works, the robot is ready for the race code (milestone M4 in IMPLEMENTATION_PLAN.md).

## Troubleshooting

| Problem | Most likely fix |
| --- | --- |
| Upload fails: "port not found" or "not in sync" | Wrong port or board selected; close the Serial Monitor; try another USB cable |
| Upload fails: "ZumoShield.h: No such file" | The library is not installed (step 3) |
| No beep | "Bz." jumper not on 328P |
| All 6 sensor numbers stay at the maximum (2000) or do not change | Batteries in and switch ON? Blue LEDON jumper on the sensor array connected to pin 2? |
| Numbers change but one sensor never does | Dirt on that sensor, or a loose array: check its screws |
| Motors do not move | Batteries in and switch ON; batteries charged |
| Robot restarts when the motors start | Weak batteries: charge them |

## Pin reference (fixed by the Zumo Shield)

| Part | Pins |
| --- | --- |
| Sensors (left to right) | 4, A3, 11, A0, A2, 5 |
| Sensor LEDs on/off | 2 |
| Motors | 7, 8 (direction), 9, 10 (speed) |
| User button | 12 |
| Buzzer | 3 |
| Yellow LED | 13 |
