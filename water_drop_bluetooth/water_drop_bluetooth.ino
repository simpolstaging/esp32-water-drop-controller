/*
 * Water Drop Effect - Bluetooth Control
 * For The Wedding Singer - Fame Water Drop Recreation
 * 
 * Copyright (c) 2026 SIMPOL Productions
 * Licensed under the MIT License - see LICENSE file for details
 * 
 * Hardware:
 * - ESP32 DUBEUYEW ESP-WROOM-32 (Type-C)
 * - 5V Relay Module (connected to GPIO 14)
 * - Heschen HS-0530B Solenoid (12V)
 * - External Bluetooth LED (GPIO 16) - Green LED with 220Ω resistor to GND
 * - External Trigger LED (GPIO 17) - Red LED with 220Ω resistor to GND
 * 
 * Control via Bluetooth Serial from phone/tablet
 * 
 * Commands:
 * - Send "FIRE" to trigger the solenoid (Show Mode only)
 * - Send "TEST" to test the relay (short pulse)
 * - Send "ARM" to enable firing
 * - Send "DISARM" to disable firing (safety)
 * - Send "STATUS" to check system status
 * - Send "MODE TEST" to switch to test mode
 * - Send "MODE SHOW" to switch to show mode
 */

#include "BluetoothSerial.h"

// Check if Bluetooth is available
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to enable it
#endif

// Pin definitions
#define RELAY_PIN 14          // GPIO 14 connected to relay IN pin (D14 on board)
#define BLUETOOTH_LED_PIN 16  // GPIO 16 for external Bluetooth status LED (Green)
#define TRIGGER_LED_PIN 17    // GPIO 17 for external trigger indicator LED (Red)

// Timing settings
#define SOLENOID_PULSE_MS 150  // How long to activate solenoid (milliseconds)
#define SAFETY_TIMEOUT 5000    // Minimum time between triggers (5 seconds)
#define AUTO_DISARM_DELAY 5000 // Auto-disarm after fire (5 seconds)

// Bluetooth
BluetoothSerial SerialBT;

// State variables
unsigned long lastTriggerTime = 0;
unsigned long lastFireTime = 0;
bool isArmed = false;  // Safety: false = disarmed by default
bool isShowMode = false;  // false = test mode, true = show mode

void setup() {
  // Initialize serial for debugging
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n=================================");
  Serial.println("Water Drop Controller Starting...");
  Serial.println("Bluetooth Mode");
  Serial.println("=================================\n");
  
  // Configure pins
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BLUETOOTH_LED_PIN, OUTPUT);
  pinMode(TRIGGER_LED_PIN, OUTPUT);
  
  // Ensure all are OFF at startup
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(BLUETOOTH_LED_PIN, LOW);
  digitalWrite(TRIGGER_LED_PIN, LOW);
  
  // Initialize Bluetooth with a custom name
  SerialBT.begin("WaterDrop_Control"); // This is the Bluetooth device name
  
  Serial.println("Bluetooth Started!");
  Serial.println("Device Name: WaterDrop_Control");
  Serial.println("Ready to pair with phone/tablet");
  
  // Turn on Bluetooth status LED
  digitalWrite(BLUETOOTH_LED_PIN, HIGH);
  
  Serial.println("");
  Serial.println("Commands:");
  Serial.println("  FIRE      - Trigger the water drop (Show Mode only)");
  Serial.println("  TEST      - Quick relay test");
  Serial.println("  ARM       - Enable triggering");
  Serial.println("  DISARM    - Disable triggering (safety)");
  Serial.println("  MODE TEST - Switch to test mode");
  Serial.println("  MODE SHOW - Switch to show mode");
  Serial.println("  STATUS    - Check system status");
  Serial.println("");
  Serial.println("=== Ready for connections! ===\n");
}

void loop() {
  // Check for Bluetooth commands
  if (SerialBT.available()) {
    String command = SerialBT.readStringUntil('\n');
    command.trim();
    command.toUpperCase();
    
    Serial.print("Received command: ");
    Serial.println(command);
    
    handleCommand(command);
  }
  
  // Also check USB Serial for debugging
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toUpperCase();
    
    Serial.print("USB command: ");
    Serial.println(command);
    
    handleCommand(command);
  }
  
  // Auto-disarm after fire
  if (isArmed && lastFireTime > 0 && (millis() - lastFireTime) > AUTO_DISARM_DELAY) {
    isArmed = false;
    SerialBT.println("Auto-disarmed after fire");
    Serial.println("Auto-disarmed after fire");
    lastFireTime = 0;
  }
}

void handleCommand(String cmd) {
  if (cmd == "FIRE") {
    fireSolenoid();
  }
  else if (cmd == "TEST") {
    testRelay();
  }
  else if (cmd == "ARM") {
    if (!isShowMode) {
      SerialBT.println("Cannot ARM in TEST mode. Switch to SHOW mode first.");
      Serial.println("Cannot ARM in TEST mode");
      return;
    }
    isArmed = true;
    SerialBT.println("System ARMED - Ready to fire");
    Serial.println("System ARMED");
  }
  else if (cmd == "DISARM") {
    isArmed = false;
    lastFireTime = 0;  // Reset auto-disarm timer
    SerialBT.println("System DISARMED - Safety engaged");
    Serial.println("System DISARMED");
  }
  else if (cmd.startsWith("MODE ")) {
    String mode = cmd.substring(5);
    if (mode == "TEST") {
      isShowMode = false;
      if (isArmed) {
        isArmed = false;
        SerialBT.println("Switched to TEST mode - auto-disarmed");
        Serial.println("Switched to TEST mode - auto-disarmed");
      } else {
        SerialBT.println("Switched to TEST mode");
        Serial.println("Switched to TEST mode");
      }
    }
    else if (mode == "SHOW") {
      isShowMode = true;
      SerialBT.println("Switched to SHOW mode");
      Serial.println("Switched to SHOW mode");
    }
    else {
      SerialBT.println("Invalid mode. Use: MODE TEST or MODE SHOW");
    }
  }
  else if (cmd == "STATUS") {
    sendStatus();
  }
  else {
    SerialBT.println("Unknown command. Try: FIRE, TEST, ARM, DISARM, MODE TEST, MODE SHOW, STATUS");
  }
}

void fireSolenoid() {
  // Safety checks
  if (!isArmed) {
    SerialBT.println("ERROR: System is DISARMED. Send ARM command first.");
    Serial.println("Trigger blocked - system disarmed");
    return;
  }
  
  unsigned long timeSinceLastFire = millis() - lastTriggerTime;
  if (timeSinceLastFire < SAFETY_TIMEOUT && lastTriggerTime > 0) {
    SerialBT.print("ERROR: Safety timeout. Wait ");
    SerialBT.print((SAFETY_TIMEOUT - timeSinceLastFire) / 1000);
    SerialBT.println(" more seconds.");
    Serial.println("Trigger blocked - safety timeout");
    return;
  }
  
  // All checks passed - FIRE!
  SerialBT.println("FIRING SOLENOID!");
  Serial.println(">>> FIRING SOLENOID <<<");
  
  // Flash trigger indicator LED
  digitalWrite(TRIGGER_LED_PIN, HIGH);
  
  // Activate relay (and solenoid)
  digitalWrite(RELAY_PIN, HIGH);
  
  // Wait for pulse duration
  delay(SOLENOID_PULSE_MS);
  
  // Deactivate relay and LED
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(TRIGGER_LED_PIN, LOW);
  
  // Update last trigger time
  lastTriggerTime = millis();
  lastFireTime = millis();  // Record for auto-disarm
  
  SerialBT.println("Solenoid pulse complete - will auto-disarm in 5 seconds");
  Serial.println("Pulse complete - will auto-disarm in 5 seconds");
  Serial.println("");
}

void testRelay() {
  SerialBT.println("Testing relay - 3 quick pulses");
  Serial.println("Relay test started");
  
  for (int i = 0; i < 3; i++) {
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(TRIGGER_LED_PIN, HIGH);
    delay(100);
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(TRIGGER_LED_PIN, LOW);
    delay(200);
  }
  
  SerialBT.println("Test complete");
  Serial.println("Test complete");
}

void sendStatus() {
  SerialBT.println("=== System Status ===");
  SerialBT.print("Mode: ");
  SerialBT.println(isShowMode ? "SHOW" : "TEST");
  SerialBT.print("Armed: ");
  SerialBT.println(isArmed ? "YES" : "NO");
  SerialBT.print("Relay Pin: GPIO ");
  SerialBT.println(RELAY_PIN);
  SerialBT.print("Bluetooth LED: GPIO ");
  SerialBT.println(BLUETOOTH_LED_PIN);
  SerialBT.print("Trigger LED: GPIO ");
  SerialBT.println(TRIGGER_LED_PIN);
  SerialBT.print("Pulse Duration: ");
  SerialBT.print(SOLENOID_PULSE_MS);
  SerialBT.println(" ms");
  SerialBT.print("Safety Timeout: ");
  SerialBT.print(SAFETY_TIMEOUT / 1000);
  SerialBT.println(" seconds");
  
  if (lastTriggerTime > 0) {
    SerialBT.print("Last fired: ");
    SerialBT.print((millis() - lastTriggerTime) / 1000);
    SerialBT.println(" seconds ago");
  } else {
    SerialBT.println("Never fired");
  }
  SerialBT.println("====================");
  
  // Also print to Serial
  Serial.println("Status sent via Bluetooth");
}
