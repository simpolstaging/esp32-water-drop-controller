# Water Drop Effect - Hardware Components

## Components Purchased from Amazon UK

### Core Components

| Component | Model/Description | Quantity | Link |
|-----------|------------------|----------|------|
| **ESP32 Board** | DUBEUYEW ESP-WROOM-32 Type-C CH340C | 2 | https://www.amazon.co.uk/dp/B0C8HYQB39 |
| **Relay Module** | DUBEUYEW DC 5V 1-Channel Optocoupler | 3 | https://www.amazon.co.uk/dp/B0BZCDDF1Z |
| **Solenoid** | Heschen HS-0530B DC12V 1.7A 10mm stroke push-pull | 1 | https://www.amazon.co.uk/dp/B07MJJB12M |
| **Power Supply** | 12V 2A DC Adapter 5.5mm x 2.1mm barrel jack | 1 | https://www.amazon.co.uk/dp/B0FP8JHN5D |
| **DC Panel Socket** | 2.1mm x 5.5mm Round Panel Mount Female DC Jack | 1 | https://www.amazon.co.uk/dp/B00WZ82NC8 |

### Status Indicator LEDs

| Component | Model/Description | Quantity | Link |
|-----------|------------------|----------|------|
| **Panel Mount LEDs** | Gebildet 8mm 3V-9VDC LED Metal Indicator Lights | 2 packs | https://www.amazon.co.uk/dp/B09L7XD5QY |

### Additional Items Needed (Not Purchased)

- Female-to-female jumper wires (Dupont connectors)
- 2-core cable (for solenoid run to hemp bar)
- Small project enclosure/junction box
- USB power bank (10,000mAh+ for ESP32 power)
- Zip ties
- Soldering iron + solder
- Wire strippers

---

## Pin Configuration Summary

**ESP32 Pin Usage:**
- **D14 (GPIO 14)** → Relay IN pin
- **D16 (GPIO 16)** → WiFi Status LED (Green) - positive wire
- **D17 (GPIO 17)** → Trigger LED (Red) - positive wire
- **GND** → Common ground for LEDs and relay DC-

**LED Wiring:**
- WiFi LED (Green): GPIO 16 → LED+ (red wire), LED- (black wire) → GND
- Trigger LED (Red): GPIO 17 → LED+ (red wire), LED- (black wire) → GND

---

## Power Setup

- **ESP32:** USB power bank (10,000mAh recommended)
- **Relay + Solenoid:** 12V 2A power supply via panel mount socket
- **IMPORTANT:** Connect ESP32 GND to 12V supply negative for common ground

---

## Total Estimated Cost

- Core components: ~£35-40
- LEDs: ~£16-20
- Additional items: ~£10-15

**Total:** ~£60-75
