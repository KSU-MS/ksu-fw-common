#pragma once
#include <stdint.h>

#ifndef DEFAULT_SPI_SPEED
#define DEFAULT_SPI_SPEED 1000000
#endif

#ifdef ARDUINO
#include <Arduino.h>
#include <SPI.h>
#endif

// ======================
// Global configuration
// ======================

// C++17 inline variable → exactly one definition across all TUs
inline uint8_t ADC_CS = 10; // Default SPI CS pin

// Helper functions (implemented in adc.cpp)
#ifdef ARDUINO
uint16_t avr_helper(uint8_t pin);
uint16_t mcp_helper(uint8_t pin);
void init_mcp();
void init_mcp(uint8_t cs_pin);
#endif // ARDUINO

// ======================
// ADC class
// ======================

enum adc_method {
  avr,
  mcp,
};

class adc {
private:
  uint8_t pin;
  uint16_t (*reader)(uint8_t) = nullptr;
  uint16_t rawval = 0;
  double alpha = 0.0;
  bool is_filtered = false;

  void set_helpers(adc_method guy);

public:
  union {
    uint16_t in = 0;
    uint8_t b[2];
  } value;

  adc(adc_method guy, uint8_t target_pin);
  adc(adc_method guy, uint8_t cs_pin, uint8_t target_pin);
  adc(adc_method guy, uint8_t target_pin, double alpha);
  adc(adc_method guy, uint8_t cs_pin, uint8_t target_pin, double alpha);

  void update();
};
