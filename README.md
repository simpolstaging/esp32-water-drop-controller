# ESP32 Water Drop Effect Controller

**For The Wedding Singer - Fame Water Drop Recreation**

A theatrical effect controller using ESP32 to trigger a solenoid valve for creating synchronized water drops during live performances. This project provides both WiFi and Bluetooth control options with identical functionality.

---

## 🎭 Project Overview

This system recreates the iconic "Fame" water drop effect from *The Wedding Singer* using an ESP32 microcontroller to control a solenoid valve. The controller can be operated via either WiFi (web interface) or Bluetooth (mobile app), providing flexibility for different venue setups and technical requirements.

### Key Features

- **Dual Control Options**: WiFi web interface or Bluetooth serial terminal
- **LED Status Indicators**: Visual feedback for connection and firing status
- **Safety Features**: Arm/disarm system, minimum timing between triggers, auto-disarm
- **Two Operating Modes**: Test mode for setup, Show mode for performance
- **Reliable Timing**: 150ms solenoid pulse optimized for water drop effect

---

## 📋 Hardware Components

### Required Components

1. **ESP32 DUBEUYEW ESP-WROOM-32** (Type-C USB)
   - WiFi + Bluetooth capable microcontroller
   - 5V power via USB or external supply

2. **5V Relay Module**
   - Controls high-current solenoid from low-current ESP32 signal
   - Connected to GPIO 14

3. **Heschen HS-0530B Solenoid Valve** (12V)
   - Electromagnetic valve for precise water control
   - Powered by external 12V supply

4. **Status LEDs** (with 220Ω resistors)
   - **Green LED** (GPIO 16): WiFi/Bluetooth connection status
   - **Red LED** (GPIO 17): Trigger/firing indicator

5. **Power Supplies**
   - 5V for ESP32 (USB or external)
   - 12V for solenoid valve

### Pin Configuration

| Component | GPIO Pin | Description |
|-----------|----------|-------------|
| Relay IN | GPIO 14 | Triggers relay to activate solenoid |
| Status LED (Green) | GPIO 16 | Connection indicator (ON = connected) |
| Trigger LED (Red) | GPIO 17 | Flash during fire/test operations |

---

## 🔧 Hardware Setup

### Wiring Diagram

```
ESP32                     5V Relay Module            Solenoid Valve
                                                     
GPIO 14 ──────────────→  IN                          
GND ──────────────────→  GND                         
5V ────────────────────→ VCC                         
                         
                         COM ───────────┐
                         NO ────────────┼───→ Solenoid (+)
                                        │
                         12V Supply ────┘
                         GND ──────────────→ Solenoid (-)

GPIO 16 ──────┬──220Ω──→ Green LED ──→ GND
GPIO 17 ──────┬──220Ω──→ Red LED ──→ GND
```

### LED Connections

1. **Green Status LED (GPIO 16)**
   - GPIO 16 → 220Ω resistor → LED Anode (+)
   - LED Cathode (-) → GND

2. **Red Trigger LED (GPIO 17)**
   - GPIO 17 → 220Ω resistor → LED Anode (+)
   - LED Cathode (-) → GND

---

## 💻 Software Installation

### Prerequisites

1. **Arduino IDE** (Version 2.0 or higher recommended)
   - Download from: https://www.arduino.cc/en/software

2. **ESP32 Board Support**
   - File → Preferences
   - Add to "Additional Board Manager URLs":
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Tools → Board → Boards Manager
   - Search "ESP32" and install "esp32 by Espressif Systems"

### Uploading Code

#### WiFi Version (`water_drop_wifi/water_drop_wifi.ino`)

1. Open `water_drop_wifi.ino` in Arduino IDE

2. **IMPORTANT:** Update WiFi credentials (lines 20-21):
   ```cpp
   const char* ssid = "YOUR_WIFI_NAME";
   const char* password = "YOUR_WIFI_PASSWORD";
   ```

3. Select board: Tools → Board → esp32 → ESP32 Dev Module

4. Select port: Tools → Port → (your ESP32 port)

5. Click Upload (→ arrow button)

6. Open Serial Monitor (115200 baud) to see the assigned IP address

#### Bluetooth Version (`water_drop_bluetooth/water_drop_bluetooth.ino`)

1. Open `water_drop_bluetooth.ino` in Arduino IDE

2. No configuration changes needed

3. Select board and port as above

4. Click Upload

5. Device will appear as "WaterDrop_Control" in Bluetooth settings

---

## 🎮 Operation Guide

### WiFi Control

**Accessing the Web Interface:**

1. Ensure ESP32 is powered and connected to WiFi
   - Green LED should be ON
   
2. Find IP address from Serial Monitor or use mDNS:
   - **Direct IP**: `http://192.168.x.x` (from Serial Monitor)
   - **mDNS**: `http://waterdrop.local` (may not work on all devices)

3. Open the IP address in any web browser (phone, tablet, laptop)

**Web Interface Controls:**

- **Mode Selection**
  - 🧪 **Test Mode**: For setup and testing (no arming required)
  - 🎭 **Show Mode**: Production mode with safety features
  
- **Test Mode**
  - "Test Relay" button: Fires 3 quick pulses (Red LED flashes)
  - No arming required
  
- **Show Mode**
  - "ARM SYSTEM" button: Enables firing capability
  - "FIRE!" button: Triggers solenoid (only when armed)
  - "DISARM SYSTEM" button: Disables firing
  - Auto-disarms 5 seconds after firing

**Status Indicators:**
- Connection indicator (top-right): Shows WiFi status
- WiFi signal meter: Signal strength bars
- System status: Shows armed/disarmed state

### Bluetooth Control

**Pairing:**

1. Power on ESP32
   - Green LED turns ON when Bluetooth is ready
   
2. On mobile device: Settings → Bluetooth
   
3. Find and pair with "WaterDrop_Control"

**Terminal App:**

- **Android**: "Serial Bluetooth Terminal" (Google Play)
- **iPhone**: "BLE Terminal" or "LightBlue" (App Store)

**Commands:**

| Command | Description |
|---------|-------------|
| `FIRE` | Trigger the solenoid (Show mode + armed only) |
| `TEST` | Run 3 quick relay test pulses |
| `ARM` | Enable firing (Show mode only) |
| `DISARM` | Disable firing (safety lock) |
| `MODE TEST` | Switch to test mode |
| `MODE SHOW` | Switch to show mode |
| `STATUS` | Display system information |

**Command Examples:**
```
MODE SHOW
ARM
FIRE
STATUS
DISARM
MODE TEST
TEST
```

---

## 🚦 Operating Modes

### Test Mode (Default)

**Purpose**: Safe setup, testing, and rehearsal

**Features:**
- Cannot arm the system
- "TEST" command works freely
- No accidental firing risk
- Red LED flashes during test pulses

**Best for:**
- Initial hardware testing
- Tech rehearsals
- Pre-show verification
- Troubleshooting

### Show Mode

**Purpose**: Live performance operation

**Features:**
- ARM/DISARM functionality enabled
- Must arm before firing
- Auto-disarm 5 seconds after fire
- Safety timeout (5 seconds minimum between fires)
- Full firing capability when armed

**Best for:**
- Live performances
- Dress rehearsals
- Final tech runs

---

## 💡 LED Behavior Reference

### Green Status LED (GPIO 16)

| State | Meaning |
|-------|---------|
| **ON (solid)** | WiFi connected / Bluetooth active |
| **OFF** | Not connected / System off |

### Red Trigger LED (GPIO 17)

| State | Meaning |
|-------|---------|
| **Flash (150ms)** | Solenoid firing |
| **3 Quick flashes** | Test mode relay test |
| **OFF** | Idle/ready |

---

## ⚙️ Configuration Options

### Timing Adjustments

Edit these constants in the `.ino` files to adjust timing:

```cpp
#define SOLENOID_PULSE_MS 150    // Solenoid activation time (ms)
#define SAFETY_TIMEOUT 5000      // Minimum time between fires (ms)
#define AUTO_DISARM_DELAY 5000   // Auto-disarm after fire (ms)
```

**Recommendations:**
- `SOLENOID_PULSE_MS`: 100-200ms for water drops (test to find optimal)
- `SAFETY_TIMEOUT`: Keep at 5000ms to prevent rapid-firing accidents
- `AUTO_DISARM_DELAY`: 5000ms provides good safety margin

### WiFi Configuration

For WiFi version, update network credentials:

```cpp
const char* ssid = "YOUR_NETWORK_NAME";
const char* password = "YOUR_PASSWORD";
```

For venue WiFi, get credentials from technical director. Alternatively, create a mobile hotspot.

---

## 🎪 Theatrical Use - Best Practices

### Pre-Show Checklist

- [ ] ESP32 powered and LEDs functional
- [ ] Green LED ON (connection established)
- [ ] Water reservoir filled
- [ ] Solenoid connections secure
- [ ] Test fire in TEST mode
- [ ] Switch to SHOW mode
- [ ] Verify ARM/DISARM functionality
- [ ] Cue sheet distributed with timing marks

### During Performance

1. **Before Scene**: 
   - Switch to SHOW mode
   - ARM system when scene approaches
   
2. **Cue Execution**:
   - Stage manager calls "Water drop GO"
   - Operator presses FIRE
   - Verify Red LED flash
   
3. **After Cue**:
   - System auto-disarms after 5 seconds
   - Can re-arm for next cue if needed

### Emergency Procedures

- **Immediate Stop**: Press DISARM or switch to TEST mode
- **Power Kill**: Disconnect ESP32 USB power
- **System Reset**: Power cycle if unresponsive

---

## 🔍 Troubleshooting

### WiFi Version Issues

| Problem | Solution |
|---------|----------|
| Can't connect to WiFi | Check SSID/password, verify network availability |
| Green LED off | WiFi connection failed, check Serial Monitor for errors |
| Can't access web page | Verify IP address, try mDNS link, check same network |
| Connection drops | Improve WiFi signal, reduce distance to router |

### Bluetooth Version Issues

| Problem | Solution |
|---------|----------|
| Device not appearing | Power cycle ESP32, check Bluetooth enabled on phone |
| Can't pair | Re-start Bluetooth, forget device and re-pair |
| Commands not working | Check terminal app connection status |
| Green LED off | Bluetooth initialization failed, re-upload code |

### Hardware Issues

| Problem | Solution |
|---------|----------|
| Solenoid doesn't activate | Check 12V power, verify relay wiring, test relay module |
| Red LED doesn't flash | Check GPIO 17 wiring, verify 220Ω resistor |
| Relay clicks but no solenoid | Check solenoid 12V power, test solenoid separately |
| Random firing | Check relay wiring, ensure GPIO 14 not floating |

### General Debug

1. **Serial Monitor** (115200 baud): View system messages
2. Check all power connections (5V ESP32, 12V solenoid)
3. Test components individually before integration
4. Verify all ground connections share common ground

---

## 📁 Project Structure

```
ESP32-water-drop/
├── README.md                              # This file
├── README_SETUP_INSTRUCTIONS.md           # Original setup guide
│
├── water_drop_wifi/
│   └── water_drop_wifi.ino               # WiFi web control version
│
└── water_drop_bluetooth/
    └── water_drop_bluetooth.ino          # Bluetooth serial control version
```

---

## 🔒 Safety Notes

1. **Electrical Safety**
   - Keep electronics away from water
   - Use proper enclosures for wet environments
   - Ensure solenoid power supply has proper grounding

2. **Timing Safety**
   - 5-second safety timeout prevents rapid firing
   - Auto-disarm prevents accidental re-trigger
   - Test mode prevents armed accidents

3. **Performance Safety**
   - Always test before show
   - Have backup trigger method
   - Brief all crew on emergency stop procedures

---

## 🎬 Performance Context

This project recreates the water drop effect from the film *The Wedding Singer*, specifically the "Fame" musical number where precision-timed water drops create a dramatic visual effect. The ESP32 provides theatrical-grade reliability with modern wireless control.

---

## 📝 Technical Specifications

### ESP32 Microcontroller
- **Model**: ESP-WROOM-32
- **Processor**: Dual-core Tensilica LX6 @ 240MHz
- **WiFi**: 802.11 b/g/n
- **Bluetooth**: Classic BT + BLE
- **Operating Voltage**: 3.3V (5V USB power input)

### Solenoid Valve
- **Model**: Heschen HS-0530B
- **Operating Voltage**: 12V DC
- **Type**: Normally closed
- **Response Time**: ~10ms

### Relay Module
- **Type**: 5V single-channel relay
- **Contact Rating**: 10A @ 250VAC / 30VDC
- **Trigger**: Active HIGH (3.3V from ESP32)

---

## 🛠️ Future Enhancements

Potential additions for future versions:

- [ ] Multiple solenoid support (sequential drops)
- [ ] DMX512 integration for lighting desk control
- [ ] Show cue programming (pre-set timing sequences)
- [ ] Water level sensor monitoring
- [ ] Battery backup for wireless operation
- [ ] MQTT integration for advanced control systems

---

## 📞 Support & Contributions

For questions, bug reports, or improvements:

- Check troubleshooting section above
- Review Serial Monitor output (115200 baud)
- Verify hardware connections against wiring diagram
- Test components individually before reporting issues

---

## 📜 License & Credits

**Created for**: The Wedding Singer - 2026 Production  
**Effect Inspiration**: "Fame" musical number water drop sequence  
**Controller**: ESP32 DUBEUYEW ESP-WROOM-32  
**Development**: SIMPOL Productions

---

## 🎯 Quick Start Summary

**WiFi Version:**
1. Edit WiFi credentials in code
2. Upload to ESP32
3. Note IP address from Serial Monitor
4. Open IP in web browser
5. Select TEST mode → run test
6. Switch to SHOW mode → ARM → FIRE

**Bluetooth Version:**
1. Upload code to ESP32
2. Pair phone with "WaterDrop_Control"
3. Open Bluetooth terminal app
4. Send `MODE TEST` → `TEST`
5. Send `MODE SHOW` → `ARM` → `FIRE`

**Both versions now have identical functionality - the only difference is the communication method!**

---

*Break a leg! 🎭💦*
