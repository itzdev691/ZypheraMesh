#pragma once
#include <Arduino.h>

/**
 * BoardConfig - Singleton class for board pin configuration
 * 
 * Automatically detects ESP32 chip variant at compile time and
 * sets appropriate default pins for each board type.
 * 
 * Usage:
 *   auto& config = BoardConfig::getInstance();
 *   int pin = config.getGoodStatusPin();
 *   pinMode(pin, INPUT_PULLDOWN);
 */
class BoardConfig {
public:
    /**
     * Get the singleton instance
     * This is the only way to access the BoardConfig
     */
    static BoardConfig& getInstance();
    
    /**
     * Pin accessor methods
     * These return the configured GPIO pin numbers
     */
    int getGoodStatusPin() const { return good_status_pin_; }
    int getBadStatusPin() const { return bad_status_pin_; }
    int getOnboardLedPin() const { return onboard_led_pin_; }
    bool getLedUsesNeopixel() const { return led_uses_neopixel_; }
    String getDetectedChip() const { return detected_chip_; }
    
private:
    /**
     * Private constructor - only getInstance() can create this
     * Detects chip variant and sets appropriate pins
     */
    BoardConfig();
    
    /**
     * Delete copy constructor and assignment operator
     * This prevents copying the singleton
     */
    BoardConfig(const BoardConfig&) = delete;
    BoardConfig& operator=(const BoardConfig&) = delete;
    
    /**
     * Member variables - store the actual pin values
     */
    int good_status_pin_;
    int bad_status_pin_;
    int onboard_led_pin_;
    bool led_uses_neopixel_;
    String detected_chip_;
};