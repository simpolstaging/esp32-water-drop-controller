# Water Drop Effect - ESP32 Control System
## For The Wedding Singer - Fame Water Drop Recreation

> **📖 NEW: Comprehensive documentation now available!**  
> See [README.md](README.md) for complete hardware setup, wiring diagrams, troubleshooting, and operational guides.
> This file provides quick setup instructions. For detailed information, refer to the main README.

---

## ⚡ Quick Setup Summary

**Both WiFi and Bluetooth versions now have identical functionality!**
- Same pin configuration (GPIO 14 for relay)
- Same LED indicators (GPIO 16 green status, GPIO 17 red trigger)
- Same operating modes (Test mode & Show mode)
- Same safety features (ARM/DISARM, auto-disarm, timeouts)
- **Only difference**: Communication method (WiFi web vs Bluetooth serial)

---

## 📦 You Have Two Code Options

### Option 1: Bluetooth Control (`water_drop_bluetooth.ino`)
- Control from phone/tablet via Bluetooth
- No WiFi network needed
- Range: ~10 meters
- External LED indicators (green status, red trigger)
- **Best for:** Backstage control from stage manager's phone

### Option 2: WiFi Web Control (`water_drop_wifi.ino`)
- Control from any device with a web browser
- Requires WiFi network (venue WiFi or phone hotspot)
- Potentially longer range
- Nice web interface with big buttons
- External LED indicators (green status, red trigger)
- **Best for:** Control from sound desk or lighting booth

**Both versions support:**
- Test Mode (safe testing without arming)
- Show Mode (production mode with ARM/DISARM)
- Auto-disarm safety feature
- Visual LED feedback

---

## 🚀 Getting Started - Upload Code to ESP32

### Step 1: Install Arduino IDE
1. Download from: https://www.arduino.cc/en/software
2. Install and open Arduino IDE

### Step 2: Add ESP32 Board Support
1. Open Arduino IDE
2. Go to **File → Preferences**
3. In "Additional Board Manager URLs" paste:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Click **OK**
5. Go to **Tools → Board → Boards Manager**
6. Search for "ESP32"
7. Install "**esp32 by Espressif Systems**"
8. Wait for installation to complete

### Step 3: Select Your Board
1. Plug in your ESP32 via USB-C cable
2. In Arduino IDE, go to **Tools → Board → esp32**
3. Select "**ESP32 Dev Module**"
4. Go to **Tools → Port**
5. Select the port with your ESP32 (usually has "USB" or "CH340" in the name)
   - Windows: `COM3`, `COM4`, etc.
   - Mac: `/dev/cu.usbserial-...`
   - Linux: `/dev/ttyUSB0`

### Step 4: Upload Code

**For Bluetooth Version:**
1. Open `water_drop_bluetooth.ino` in Arduino IDE
2. Click the **Upload** button (→ arrow icon)
3. Wait for "Done uploading" message
4. Open **Tools → Serial Monitor** (set to 115200 baud)
5. You should see "Bluetooth Started! Device Name: WaterDrop_Control"

**For WiFi Version:**
1. Open `water_drop_wifi.ino` in Arduino IDE
2. **IMPORTANT:** Edit these lines at the top:
   ```cpp
   const char* ssid = "YOUR_WIFI_NAME";
   const char* password = "YOUR_WIFI_PASSWORD";
   ```
3. Replace with your actual WiFi name and password
4. Click **Upload**
5. Wait for completion
6. Open **Serial Monitor** (115200 baud)
7. Look for the IP address - something like: `http://192.168.1.123`

---

## 📱 Using Bluetooth Control

### Pairing:
1. On your phone/tablet, open Bluetooth settings
2. Look for "**WaterDrop_Control**"
3. Pair with it (no PIN needed)

### Controlling:
You need a **Bluetooth Serial Terminal app**:

**Android:**
- "Serial Bluetooth Terminal" (free on Play Store)
- Install → Connect to "WaterDrop_Control"

**iPhone:**
- "BLE Terminal" or "LightBlue"
- Note: iOS Bluetooth apps can be finicky

### Commands:
Once connected, type these commands:
- `FIRE` - Trigger the water drop (Show mode only, when armed)
- `TEST` - Test the relay (3 quick clicks) - works in both modes
- `ARM` - Enable firing (Show mode only)
- `DISARM` - Safety lock (prevents firing)
- `MODE TEST` - Switch to test mode (safe testing)
- `MODE SHOW` - Switch to show mode (production)
- `STATUS` - Check system info

**LED Indicators:**
- **Green LED (GPIO 16)**: ON when Bluetooth connected
- **Red LED (GPIO 17)**: Flashes when firing or testing

---

## 🌐 Using WiFi Control

### Connecting:
1. Make sure ESP32 is powered on and connected to WiFi
   - **Green LED should be ON** (indicates WiFi connected)
2. Check Serial Monitor for IP address (like `192.168.1.123`)
3. On any device on the same WiFi:
   - Open web browser
   - Type in the IP address: `http://192.168.1.123`
   - Or try: `http://waterdrop.local`
4. You'll see a nice web page with big buttons

### Web Interface:
- **Mode Selection** - Switch between Test and Show modes
- **Test Mode**: Test relay button (no arming needed)
- **Show Mode**: ARM → FIRE → Auto-disarm
- **Big Red FIRE Button** - Triggers the solenoid (Show mode only, when armed)
- **Test Relay** - Does 3 quick clicks to test
- **Arm/Disarm** - Safety controls (Show mode only)
- Status shows if system is armed

### LED Indicators:
- **Green LED (GPIO 16)**: ON when WiFi connected
- **Red LED (GPIO 17)**: Flashes when firing or testing
- **Connection indicator**: Web interface shows WiFi signal strength

### Tips:
- Bookmark the IP address on your phone
- Add to home screen for quick access
- If you lose the IP, check Serial Monitor

---

## 🔧 Configuration & Safety Features

### Hardware Setup:
**⚠️ Both versions now use identical pin configuration:**

```cpp
#define RELAY_PIN 14             // GPIO 14 to relay IN pin
#define WIFI_LED_PIN 16          // GPIO 16 - Green status LED (WiFi version)
#define BLUETOOTH_LED_PIN 16     // GPIO 16 - Green status LED (Bluetooth version)
#define TRIGGER_LED_PIN 17       // GPIO 17 - Red trigger indicator LED
```

**LED Wiring:**
- Green LED: GPIO 16 → 220Ω resistor → LED (+) → GND
- Red LED: GPIO 17 → 220Ω resistor → LED (+) → GND

### Timing Settings (in the code):
```cpp
#define SOLENOID_PULSE_MS 150   // How long solenoid fires (milliseconds)
#define SAFETY_TIMEOUT 5000     // Minimum time between fires (5 seconds)
#define AUTO_DISARM_DELAY 5000  // Auto-disarm after fire (5 seconds)
```

**To adjust:**
- `SOLENOID_PULSE_MS`: If water drop doesn't work well, try 100-200ms
- `SAFETY_TIMEOUT`: Prevents accidental double-triggers
- `AUTO_DISARM_DELAY`: Time before auto-disarm after firing

### Built-in Safety Features:
1. **Test/Show Modes** - Test mode prevents arming accidents
2. **Arm/Disarm** - Must be armed to fire in Show mode
3. **Auto-Disarm** - Automatically disarms 5 seconds after fire
4. **Safety Timeout** - 5 second minimum between triggers
5. **Visual Feedback** - LEDs show connection and firing status
6. **Status Checking** - Always know if system is ready

---

## 🧪 Testing Without Solenoid

Before you connect the solenoid, test the code:

1. Upload code to ESP32
2. Watch the **LED indicators**:
   - **Green LED** (GPIO 16) - Should turn ON when connected
   - **Red LED** (GPIO 17) - Should flash when firing/testing
3. Send FIRE (Show mode) or TEST command
4. Red LED should flash when relay triggers
5. You should hear the relay **click**

**If relay doesn't click:**
- Check wiring: ESP32 GPIO 14 → Relay IN
- Check power: ESP32 5V → Relay VCC, GND → Relay GND
- Verify relay module LED lights up
- Check Serial Monitor for error messages

**LED Troubleshooting:**
- If green LED doesn't light: Check GPIO 16 wiring and 220Ω resistor
- If red LED doesn't flash: Check GPIO 17 wiring and 220Ω resistor
- Ensure LED polarity is correct (long leg = anode/+)

---

## 🎭 Show Day Checklist

Before each performance:

- [ ] ESP32 powered via USB (powerbank or mains)
- [ ] **Green LED is ON** (Bluetooth paired OR WiFi connected)
- [ ] Switch to **SHOW MODE** (via command or web interface)
- [ ] Send TEST command - relay should click 3 times, red LED flashes
- [ ] Water reservoir ready and solenoid positioned
- [ ] System is **ARMED** (Show mode only)
- [ ] Trigger device (phone/tablet) is charged and ready
- [ ] Verify LEDs: Green = ON, Red = flashes when testing
- [ ] Backup plan if tech fails!

**During Show:**
- ARM before cue
- FIRE on cue
- System auto-disarms after 5 seconds
- Can re-arm for next cue if needed

---

## 🐛 Troubleshooting

> **💡 For detailed troubleshooting, see [README.md](README.md)**

### ESP32 won't connect to computer:
- Install CH340 drivers: https://sparks.gogo.co.nz/ch340.html
- Try different USB cable (some are charge-only)
- Try different USB port

### Bluetooth won't pair:
- Make sure Bluetooth is enabled in code upload
- Restart ESP32
- Forget device and re-pair
- Check Serial Monitor for "Bluetooth Started!" message

### WiFi won't connect:
- Double-check SSID and password (case-sensitive!)
- Make sure WiFi is 2.4GHz (ESP32 doesn't support 5GHz)
- Check Serial Monitor for error messages
- Verify green LED turns ON after connecting

### LED Issues:
- **Green LED not lighting**: Check GPIO 16 wiring, 220Ω resistor, LED polarity
- **Red LED not flashing**: Check GPIO 17 wiring, 220Ω resistor, LED polarity
- Check power (5V to ESP32, common ground for all components)

### Relay clicks but solenoid doesn't fire:
- Check 12V power supply is plugged in
- Check solenoid wiring: Relay NO → Solenoid, 12V- → Solenoid
- Test solenoid directly with 12V to confirm it works
- Verify relay module indicator LED lights when triggered

### Solenoid fires but water drop doesn't work well:
- Adjust `SOLENOID_PULSE_MS` (try 100-200ms range)
- Check valve positioning
- Verify water pressure/flow
- Test with different pulse durations

---

## 📞 Need Help?

Check these in order:
1. **Serial Monitor** - Almost all problems show up here
2. **Wiring Diagram** - Compare your setup to the diagram
3. **Test each component separately** - ESP32, relay, solenoid

---

## 🎬 Break a Leg!

This system has been designed with safety and reliability in mind. Test thoroughly in tech rehearsals, and have a backup plan just in case. Theatre tech always has surprises!

Remember: The show must go on, with or without technology 😊

---

**Created for The Wedding Singer Production**
**Good luck with the show!**
