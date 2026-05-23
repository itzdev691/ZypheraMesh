#include "Status/onboard_led.h"

#include <Arduino.h>

#include "Config/app_config.h"
#include "Config/BoardConfig.h"

#if ONBOARD_LED_USES_NEOPIXEL
#include <esp32-hal-rgb-led.h>
#endif

static bool g_onboardLedReady = false;
static uint32_t g_receiveLedUntilMs = 0;

// Cache LED pin and type from BoardConfig
static int getLedPin() {
  static int pin = BoardConfig::getInstance().getOnboardLedPin();
  return pin;
}

static bool usesNeopixel() {
  static bool neopixel = BoardConfig::getInstance().getLedUsesNeopixel();
  return neopixel;
}

static void setOnboardLed(bool enabled) {
  if (usesNeopixel()) {
    const uint8_t red = enabled ? ONBOARD_LED_RX_RED : 0;
    const uint8_t green = enabled ? ONBOARD_LED_RX_GREEN : 0;
    const uint8_t blue = enabled ? ONBOARD_LED_RX_BLUE : 0;
    neopixelWrite(getLedPin(), red, green, blue);
  } else {
    const uint8_t inactiveLevel = ONBOARD_LED_ACTIVE_HIGH ? LOW : HIGH;
    const uint8_t activeLevel = ONBOARD_LED_ACTIVE_HIGH ? HIGH : LOW;
    digitalWrite(getLedPin(), enabled ? activeLevel : inactiveLevel);
  }
}

void initOnboardLed() {
  if (!usesNeopixel()) {
    pinMode(getLedPin(), OUTPUT);
  }
  setOnboardLed(false);
  g_onboardLedReady = true;
}

void noteReceiveActivity() {
  if (!g_onboardLedReady) {
    return;
  }

  g_receiveLedUntilMs = millis() + RX_LED_HOLD_MS;
  setOnboardLed(true);
}

void loopOnboardLed() {
  if (!g_onboardLedReady || g_receiveLedUntilMs == 0) {
    return;
  }

  if (static_cast<int32_t>(millis() - g_receiveLedUntilMs) < 0) {
    return;
  }

  g_receiveLedUntilMs = 0;
  setOnboardLed(false);
}
