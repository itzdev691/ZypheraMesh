#include "BoardConfig.h"

BoardConfig& BoardConfig::getInstance() {
    static BoardConfig instance;
    return instance;
}

BoardConfig::BoardConfig() {
    #if defined(CONFIG_IDF_TARGET_ESP32S3)
        detected_chip_ = "ESP32-S3";
        good_status_pin_ = 4;
        bad_status_pin_ = 5;
        onboard_led_pin_ = 48;
        led_uses_neopixel_ = true;
    #elif defined(CONFIG_IDF_TARGET_ESP32C5)
        detected_chip_ = "ESP32-C5";
        good_status_pin_ = 8;
        bad_status_pin_ = 9;
        onboard_led_pin_ = 8;
        led_uses_neopixel_ = false;
    #elif defined(CONFIG_IDF_TARGET_ESP32C6)
        detected_chip_ = "ESP32-C6";
        good_status_pin_ = 7;
        bad_status_pin_ = 6;
        onboard_led_pin_ = 8;
        led_uses_neopixel_ = false;
    #else
        detected_chip_ = "ESP32";
        good_status_pin_ = 2;
        bad_status_pin_ = 3;
        onboard_led_pin_ = 2;
        led_uses_neopixel_ = false;
    #endif
    
    #ifdef USER_GOOD_STATUS_PIN
        good_status_pin_ = USER_GOOD_STATUS_PIN;
    #endif
    #ifdef USER_BAD_STATUS_PIN
        bad_status_pin_ = USER_BAD_STATUS_PIN;
    #endif
    #ifdef USER_ONBOARD_LED_PIN
        onboard_led_pin_ = USER_ONBOARD_LED_PIN;
    #endif
}