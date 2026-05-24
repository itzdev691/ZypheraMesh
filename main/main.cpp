#include <Arduino.h>

#include "Config/app_config.h"
#include "Display/oled_ui.h"
#include "Input/status_button.h"
#include "Network/mesh.h"
#include "Network/node_state.h"
#include "Network/peer_registry.h"
#include "Status/onboard_led.h"
#include "CrashLog/CrashLogger.h"
#include <esp_task_wdt.h>
#include "Config/BoardConfig.h"


CrashLogger crashLogger;

static void onPeerNewUi(const uint8_t* macAddr) {
  setDisplayStatus("Peer learned");
  setDisplayEventFromPeer("NEW", macAddr);
  renderDisplay();
}

static void onPeerLostUi(const uint8_t* macAddr) {
  setDisplayStatus("Peer timed out");
  setDisplayEventFromPeer("LOST", macAddr);
  renderDisplay();
}

void setup() {
  Serial.begin(115200);
  delay(1000); // Longer delay for C5 USB CDC
  
  Serial.println("\n\n=== ZypheraMesh Boot ===");
  
 // Get board configuration
auto& config = BoardConfig::getInstance();
Serial.printf("Detected chip: %s\n", config.getDetectedChip().c_str());
Serial.printf("GOOD_STATUS_PIN: GPIO %d\n", config.getGoodStatusPin());
Serial.printf("BAD_STATUS_PIN: GPIO %d\n", config.getBadStatusPin());
Serial.printf("ONBOARD_LED_PIN: GPIO %d\n", config.getOnboardLedPin());

  
  // Initialize crash logger FIRST - before watchdog
  Serial.println("Initializing crash logger...");
  crashLogger.begin();
  
  
  Serial.println("System ready");
  pinMode(config.getGoodStatusPin(), INPUT_PULLDOWN);
pinMode(config.getBadStatusPin(), INPUT_PULLDOWN);

  
  Serial.println("Configuring WiFi...");
  
  configureWiFi();
  
  Serial.println("Initializing onboard LED...");
  initOnboardLed();

  PeerUiCallbacks peerUi{};
  peerUi.onNewPeer = onPeerNewUi;
  peerUi.onPeerLost = onPeerLostUi;
  peerRegistrySetUiCallbacks(peerUi);

  Serial.println("Initializing display...");

  
  bool displayOk = initDisplay();
  
  if (!displayOk) {
    Serial.println("WARNING: Display initialization failed, continuing anyway");
  }

  // Show any crash logs from the previous boot on the OLED before continuing
  if (displayOk) {
    showCrashLogOnDisplay();
  }

  Serial.println("Initializing ESP-NOW...");
  
  initEspNow();

  if (displayOk) {
    setDisplayStatus("ESP-NOW ready");
    delay(1000);
    setDisplayEvent("Waiting...");
    renderDisplay();
  }

  Serial.printf("MAC: %s\n", macToString(selfMac).c_str());
  Serial.printf("Node ID: %s\n", selfNodeId);
 Serial.printf("Good status pin: GPIO %u\n", config.getGoodStatusPin());
Serial.printf("Bad status pin: GPIO %u\n", config.getBadStatusPin());
Serial.printf("Onboard LED pin: GPIO %u\n", config.getOnboardLedPin());

  Serial.printf("Free heap: %u bytes\n", ESP.getFreeHeap());

  esp_task_wdt_reset();
  sendDiscovery();
  
  Serial.println("=== Boot Complete ===\n");
}

void loop() {
  
  loopDisplay();
  loopOnboardLed();
  loopMesh();

  handleStatusInputs();

  if (millis() - lastDiscoveryMs >= DISCOVERY_INTERVAL_MS) {
    lastDiscoveryMs = millis();
    sendDiscovery();
  }

  prunePeers();
  delay(20);
}