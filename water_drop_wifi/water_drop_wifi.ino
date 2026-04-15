/*
 * Water Drop Effect - WiFi Web Server Control (Simplified)
 * For The Wedding Singer - Fame Water Drop Recreation
 * 
 * Copyright (c) 2026 SIMPOL Productions
 * Licensed under the MIT License - see LICENSE file for details
 * 
 * Hardware:
 * - ESP32 DUBEUYEW ESP-WROOM-32 (Type-C)
 * - 5V Relay Module (connected to GPIO 5)
 * - Heschen HS-0530B Solenoid (12V)
 * - External WiFi LED (GPIO 16) - Green LED with 220Ω resistor to GND
 * - External Trigger LED (GPIO 17) - Red LED with 220Ω resistor to GND
 * 
 * Control via web browser - connects to WiFi and creates a web page
 * Access from any device on the same network
 * 
 * SETUP: Change WiFi credentials below!
 */

#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

// ============================================
// WIFI CREDENTIALS - CHANGE THESE!
// ============================================
const char* ssid = "<wifi name>";      // Replace with your WiFi name
const char* password = "<wifi password>";  // Replace with your WiFi password

// mDNS hostname - access via http://waterdrop.local
const char* mdnsName = "waterdrop";

// Pin definitions
#define RELAY_PIN 14          // GPIO 14 connected to relay IN pin (D14 on board)
#define WIFI_LED_PIN 16       // GPIO 16 for external WiFi status LED (Green)
#define TRIGGER_LED_PIN 17    // GPIO 17 for external trigger indicator LED (Red)

// Timing settings
#define SOLENOID_PULSE_MS 150  // How long to activate solenoid (milliseconds)
#define SAFETY_TIMEOUT 5000    // Minimum time between triggers (5 seconds)

// Web server on port 80
WebServer server(80);

// State variables
unsigned long lastTriggerTime = 0;
bool isArmed = false;  // Safety: false = disarmed by default
bool isShowMode = false;  // false = test mode, true = show mode
unsigned long lastFireTime = 0;
const unsigned long AUTO_DISARM_DELAY = 5000;  // Auto-disarm after 5 seconds

// ============================================
// CORS Helper Functions
// ============================================
void setCORSHeaders() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

void handleOptions() {
  setCORSHeaders();
  server.send(204);  // No content
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n=================================");
  Serial.println("Water Drop Controller Starting...");
  Serial.println("=================================\n");
  
  // Configure pins
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(WIFI_LED_PIN, OUTPUT);
  pinMode(TRIGGER_LED_PIN, OUTPUT);
  
  // Ensure all are OFF at startup
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(WIFI_LED_PIN, LOW);
  digitalWrite(TRIGGER_LED_PIN, LOW);
  
  // Connect to WiFi
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    
    // Turn on WiFi status LED
    digitalWrite(WIFI_LED_PIN, HIGH);
    
    // Start mDNS service
    if (MDNS.begin(mdnsName)) {
      Serial.print("mDNS started: http://");
      Serial.print(mdnsName);
      Serial.println(".local");
    } else {
      Serial.println("mDNS failed to start");
    }
  } else {
    Serial.println("\nWiFi Connection Failed!");
    Serial.println("Please check credentials and try again");
    return;
  }
  
  // Set up web server routes
  server.on("/", handleRoot);
  server.on("/fire", handleFire);
  server.on("/test", handleTest);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);
  server.on("/status", handleStatus);
  server.on("/ping", handlePing);
  server.on("/wifi", handleWiFiInfo);
  server.on("/setmode", handleSetMode);
  
  // Handle OPTIONS requests for CORS preflight
  server.on("/fire", HTTP_OPTIONS, handleOptions);
  server.on("/test", HTTP_OPTIONS, handleOptions);
  server.on("/arm", HTTP_OPTIONS, handleOptions);
  server.on("/disarm", HTTP_OPTIONS, handleOptions);
  server.on("/status", HTTP_OPTIONS, handleOptions);
  server.on("/ping", HTTP_OPTIONS, handleOptions);
  server.on("/wifi", HTTP_OPTIONS, handleOptions);
  server.on("/setmode", HTTP_OPTIONS, handleOptions);
  
  server.begin();
  Serial.println("Web server started");
  Serial.println("\n=== Ready for connections! ===\n");
}

void loop() {
  server.handleClient();
  
  // Auto-disarm after fire
  if (isArmed && lastFireTime > 0 && (millis() - lastFireTime) > AUTO_DISARM_DELAY) {
    isArmed = false;
    Serial.println("Auto-disarmed after fire");
  }
}

// ============================================
// WEB PAGE HTML
// ============================================
String getHTML() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Water Drop Control</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); ";
  html += "color: white; text-align: center; padding: 20px; margin: 0; min-height: 100vh; }";
  html += "h1 { font-size: 2.5em; margin-bottom: 10px; }";
  html += ".subtitle { font-size: 1.2em; margin-bottom: 40px; opacity: 0.9; }";
  html += ".container { max-width: 500px; margin: 0 auto; background: rgba(255,255,255,0.1); ";
  html += "padding: 30px; border-radius: 20px; backdrop-filter: blur(10px); }";
  html += "button { background: white; color: #667eea; border: none; padding: 20px 40px; ";
  html += "font-size: 1.5em; border-radius: 50px; cursor: pointer; margin: 10px; ";
  html += "font-weight: bold; transition: all 0.3s; min-width: 200px; }";
  html += "button:hover { transform: scale(1.05); box-shadow: 0 10px 30px rgba(0,0,0,0.3); }";
  html += "button:active { transform: scale(0.95); }";
  html += ".fire-btn { background: #ff4757; color: white; font-size: 2em; padding: 30px 50px; display: block; margin: 20px auto; }";
  html += ".fire-btn.disabled { opacity: 0.5; cursor: not-allowed; }";
  html += ".test-btn { background: #ffa502; color: white; display: block; margin: 20px auto; }";
  html += ".arm-btn { background: #26de81; color: white; }";
  html += ".disarm-btn { background: #fc5c65; color: white; }";
  html += ".button-row { display: flex; justify-content: center; gap: 10px; margin: 20px 0; }";
  html += ".status { background: rgba(255,255,255,0.2); padding: 15px; border-radius: 10px; ";
  html += "margin: 20px 0; font-size: 1.1em; }";
  html += ".armed { color: #26de81; font-weight: bold; }";
  html += ".disarmed { color: #ff4757; font-weight: bold; }";
  html += ".connection-status { position: fixed; top: 20px; right: 20px; background: rgba(255,255,255,0.95); ";
  html += "padding: 10px 15px; border-radius: 20px; display: flex; align-items: center; gap: 8px; ";
  html += "box-shadow: 0 4px 12px rgba(0,0,0,0.2); z-index: 1000; }";
  html += ".status-dot { width: 12px; height: 12px; border-radius: 50%; }";
  html += ".status-connected { background: #26de81; box-shadow: 0 0 8px #26de81; }";
  html += ".status-disconnected { background: #ff4757; box-shadow: 0 0 8px #ff4757; }";
  html += ".wifi-meter { display: flex; align-items: flex-end; gap: 3px; height: 20px; }";
  html += ".wifi-bar { width: 4px; background: #ccc; border-radius: 2px; transition: all 0.3s; }";
  html += ".wifi-bar.active { background: #26de81; }";
  
  // Mode toggle styles
  html += ".mode-toggle { background: rgba(255,255,255,0.95); border-radius: 15px; padding: 20px; ";
  html += "margin: 20px 0; box-shadow: 0 4px 12px rgba(0,0,0,0.2); }";
  html += ".mode-buttons { display: flex; gap: 10px; margin-top: 10px; }";
  html += ".mode-btn { flex: 1; padding: 15px; border: 2px solid #ddd; border-radius: 10px; ";
  html += "background: white; cursor: pointer; font-weight: bold; transition: all 0.3s; font-size: 1em; min-width: auto; }";
  html += ".mode-btn.active { background: #667eea; color: white; border-color: #667eea; }";
  
  // Hide/show based on mode
  html += ".test-only { display: none; }";
  html += ".show-only { display: none; }";
  html += "body.test-mode .test-only { display: block; }";
  html += "body.show-mode .show-only { display: block; }";
  
  html += "</style></head><body class='test-mode'>";
  
  html += "<h1>&#128167; Water Drop Control</h1>";
  html += "<div class='subtitle'>The Wedding Singer - Fame Effect</div>";
  
  // Connection status indicator
  html += "<div class='connection-status' id='connStatus'>";
  html += "<div class='status-dot status-connected' id='statusDot'></div>";
  html += "<div style='display:flex; flex-direction:column; gap:2px;'>";
  html += "<div style='font-weight:bold; font-size:0.9em;' id='connText'>Connected</div>";
  html += "<div class='wifi-meter' id='wifiMeter'>";
  html += "<div class='wifi-bar' style='height:30%;'></div>";
  html += "<div class='wifi-bar' style='height:50%;'></div>";
  html += "<div class='wifi-bar' style='height:70%;'></div>";
  html += "<div class='wifi-bar' style='height:100%;'></div>";
  html += "</div></div></div>";
  
  html += "<div class='container'>";
  
  // Mode toggle
  html += "<div class='mode-toggle'>";
  html += "<div style='font-weight:bold; font-size:1.1em; margin-bottom:5px; color:#333;'>Mode Selection</div>";
  html += "<div class='mode-buttons'>";
  html += "<button class='mode-btn active' id='testModeBtn' onclick='setMode(false)'>&#x1F9EA; Test Mode</button>";
  html += "<button class='mode-btn' id='showModeBtn' onclick='setMode(true)'>&#x1F3AD; Show Mode</button>";
  html += "</div></div>";
  
  // Status display (show mode only)
  html += "<div class='show-only'>";
  html += "<div class='status' id='statusDisplay'>";
  html += "System Status: <span class='";
  html += isArmed ? "armed'>ARMED &#x2713;" : "disarmed'>DISARMED &#x2717;";
  html += "</span></div>";
  
  // Arm/Disarm buttons (show mode only) - side by side
  html += "<div class='button-row'>";
  html += "<button class='arm-btn' id='armBtn' onclick='arm()'>&#x1F513; ARM SYSTEM</button>";
  html += "<button class='disarm-btn' id='disarmBtn' onclick='disarm()'>&#x1F512; DISARM SYSTEM</button>";
  html += "</div>";
  html += "</div>";
  
  // Main fire button (show mode only) - centered
  String fireClass = !isArmed ? " disabled" : "";
  html += "<button class='fire-btn show-only" + fireClass + "' id='fireBtn' onclick='fire()'>&#127919; FIRE!</button>";
  
  // Test button (test mode only) - centered
  html += "<button class='test-btn test-only' onclick='test()'>Test Relay</button>";
  
  html += "</div>";
  
  // JavaScript
  html += "<script>";
  html += "let connectionOK = true;";
  html += "let failedPings = 0;";
  html += "const MAX_FAILED_PINGS = 3;";
  html += "let isShowMode = " + String(isShowMode ? "true" : "false") + ";";
  html += "let isArmed = " + String(isArmed ? "true" : "false") + ";";
  
  // Mode switching
  html += "function setMode(showMode) {";
  html += "  isShowMode = showMode;";
  html += "  document.body.className = showMode ? 'show-mode' : 'test-mode';";
  html += "  document.getElementById('testModeBtn').classList.toggle('active', !showMode);";
  html += "  document.getElementById('showModeBtn').classList.toggle('active', showMode);";
  html += "  fetch('/setmode?mode=' + (showMode ? 'show' : 'test'));";
  html += "  if (!showMode && isArmed) { disarm(); }";
  html += "}";
  
  // Ping/heartbeat function
  html += "async function checkConnection() {";
  html += "  try {";
  html += "    const response = await fetch('/ping', { signal: AbortSignal.timeout(2000) });";
  html += "    if (response.ok) {";
  html += "      failedPings = 0;";
  html += "      if (!connectionOK) {";
  html += "        connectionOK = true;";
  html += "        updateConnectionStatus(true);";
  html += "      }";
  html += "    } else { throw new Error('Ping failed'); }";
  html += "  } catch(e) {";
  html += "    failedPings++;";
  html += "    if (failedPings >= MAX_FAILED_PINGS && connectionOK) {";
  html += "      connectionOK = false;";
  html += "      updateConnectionStatus(false);";
  html += "    }";
  html += "  }";
  html += "}";
  
  // Update WiFi signal strength
  html += "async function updateWiFi() {";
  html += "  try {";
  html += "    const response = await fetch('/wifi');";
  html += "    const data = await response.json();";
  html += "    const quality = data.quality;";
  html += "    const bars = document.querySelectorAll('.wifi-bar');";
  html += "    bars.forEach((bar, i) => {";
  html += "      if ((i + 1) * 25 <= quality) { bar.classList.add('active'); }";
  html += "      else { bar.classList.remove('active'); }";
  html += "    });";
  html += "  } catch(e) { console.error('WiFi update failed', e); }";
  html += "}";
  
  // Update connection status display
  html += "function updateConnectionStatus(connected) {";
  html += "  const dot = document.getElementById('statusDot');";
  html += "  const text = document.getElementById('connText');";
  html += "  if (connected) {";
  html += "    dot.className = 'status-dot status-connected';";
  html += "    text.textContent = 'Connected';";
  html += "  } else {";
  html += "    dot.className = 'status-dot status-disconnected';";
  html += "    text.textContent = 'Connection Lost!';";
  html += "    alert('WARNING: Lost connection to ESP32!\\nCheck WiFi and power.');";
  html += "  }";
  html += "}";
  
  // Start monitoring
  html += "setInterval(checkConnection, 2000);";
  html += "setInterval(updateWiFi, 5000);";
  html += "updateWiFi();";
  
  // Main action functions
  html += "function fire() { ";
  html += "  if(!isArmed) { alert('System is DISARMED!'); return; }";
  html += "  fetch('/fire').then(() => { ";
  html += "    setTimeout(() => { isArmed = false; updateStatus(); }, 5000); ";
  html += "  }); ";
  html += "}";
  
  html += "function test() { ";
  html += "  fetch('/test').then(() => alert('Test complete - relay clicked 3 times')); ";
  html += "}";
  
  html += "function arm() { ";
  html += "  fetch('/arm').then(response => {";
  html += "    if(response.ok) {";
  html += "      isArmed = true;";
  html += "      updateStatus();";
  html += "    }";
  html += "  });";
  html += "}";
  
  html += "function disarm() { ";
  html += "  fetch('/disarm').then(response => {";
  html += "    if(response.ok) {";
  html += "      isArmed = false;";
  html += "      updateStatus();";
  html += "    }";
  html += "  });";
  html += "}";
  
  html += "function updateStatus() {";
  html += "  const statusDisplay = document.getElementById('statusDisplay');";
  html += "  const fireBtn = document.getElementById('fireBtn');";
  html += "  if(isArmed) {";
  html += "    statusDisplay.innerHTML = \"System Status: <span class='armed'>ARMED &#x2713;</span>\";";
  html += "    fireBtn.classList.remove('disabled');";
  html += "  } else {";
  html += "    statusDisplay.innerHTML = \"System Status: <span class='disarmed'>DISARMED &#x2717;</span>\";";
  html += "    fireBtn.classList.add('disabled');";
  html += "  }";
  html += "}";
  
  html += "</script>";
  
  html += "</body></html>";
  return html;
}

// ============================================
// WEB SERVER HANDLERS
// ============================================

void handleRoot() {
  setCORSHeaders();
  server.send(200, "text/html", getHTML());
}

void handleFire() {
  setCORSHeaders();
  
  // FIRE!
  Serial.println(">>> FIRING SOLENOID (Web) <<<");
  
  digitalWrite(TRIGGER_LED_PIN, HIGH);  // Flash trigger indicator
  digitalWrite(RELAY_PIN, HIGH);
  delay(SOLENOID_PULSE_MS);
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(TRIGGER_LED_PIN, LOW);
  
  lastTriggerTime = millis();
  lastFireTime = millis();  // Record for auto-disarm
  
  server.send(200, "text/plain", "FIRED!");
  Serial.println("Pulse complete - will auto-disarm in 5 seconds");
}

void handleTest() {
  setCORSHeaders();
  Serial.println("Relay test (Web)");
  
  for (int i = 0; i < 3; i++) {
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(TRIGGER_LED_PIN, HIGH);
    delay(100);
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(TRIGGER_LED_PIN, LOW);
    delay(200);
  }
  
  server.send(200, "text/plain", "Test complete");
}

void handleArm() {
  setCORSHeaders();
  
  if (!isShowMode) {
    server.send(403, "text/plain", "Cannot arm in TEST mode");
    return;
  }
  
  isArmed = true;
  server.send(200, "text/plain", "ARMED");
  Serial.println("System ARMED (Web)");
}

void handleDisarm() {
  setCORSHeaders();
  
  isArmed = false;
  lastFireTime = 0;  // Reset auto-disarm timer
  server.send(200, "text/plain", "DISARMED");
  Serial.println("System DISARMED (Web)");
}

void handleStatus() {
  setCORSHeaders();
  
  String json = "{";
  json += "\"armed\":" + String(isArmed ? "true" : "false") + ",";
  json += "\"mode\":\"" + String(isShowMode ? "show" : "test") + "\"";
  json += "}";
  
  server.send(200, "application/json", json);
}

void handlePing() {
  setCORSHeaders();
  server.send(200, "text/plain", "pong");
}

void handleWiFiInfo() {
  setCORSHeaders();
  
  int rssi = WiFi.RSSI();
  
  // Convert RSSI to quality percentage
  int quality;
  if (rssi >= -50) {
    quality = 100;
  } else if (rssi <= -100) {
    quality = 0;
  } else {
    quality = 2 * (rssi + 100);
  }
  
  String json = "{";
  json += "\"rssi\":" + String(rssi) + ",";
  json += "\"quality\":" + String(quality) + ",";
  json += "\"ssid\":\"" + String(WiFi.SSID()) + "\",";
  json += "\"ip\":\"" + WiFi.localIP().toString() + "\"";
  json += "}";
  
  server.send(200, "application/json", json);
}

void handleSetMode() {
  setCORSHeaders();
  
  if (server.hasArg("mode")) {
    String mode = server.arg("mode");
    isShowMode = (mode == "show");
    
    if (!isShowMode && isArmed) {
      // Auto-disarm when switching to test mode
      isArmed = false;
      Serial.println("Switched to TEST mode - auto-disarmed");
    }
    
    Serial.print("Mode changed to: ");
    Serial.println(isShowMode ? "SHOW" : "TEST");
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Missing mode parameter");
  }
}
