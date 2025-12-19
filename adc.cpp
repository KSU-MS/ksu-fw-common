#include "adc.hpp"

#ifdef ARDUINO

uint16_t avr_helper(uint8_t pin) {
  return static_cast<uint16_t>(analogRead(pin));
}

uint16_t mcp_helper(uint8_t pin) {
  SPI.beginTransaction(SPISettings(DEFAULT_SPI_SPEED, MSBFIRST, SPI_MODE0));

  digitalWrite(ADC_CS, LOW);

  uint8_t b = B01100000;
  b |= (pin << 2);

  SPI.transfer(b);

  uint8_t result1 = SPI.transfer(0);
  uint8_t result2 = SPI.transfer(0);

  digitalWrite(ADC_CS, HIGH);
  SPI.endTransaction();

  return static_cast<uint16_t>((result1 << 4) | (result2 >> 4));
}

void init_mcp() {
  pinMode(ADC_CS, OUTPUT);
  digitalWrite(ADC_CS, HIGH);
  SPI.begin();
}

void init_mcp(uint8_t cs_pin) {
  ADC_CS = cs_pin;
  pinMode(ADC_CS, OUTPUT);
  digitalWrite(ADC_CS, HIGH);
  SPI.begin();
}

#endif // ARDUINO

void adc::set_helpers(adc_method guy) {
#ifdef ARDUINO
  switch (guy) {
  case avr:
    reader = &avr_helper;
    break;

  case mcp:
    init_mcp();
    reader = &mcp_helper;
    break;

  default:
    break;
  }
#else
  (void)guy;
#endif
}

adc::adc(adc_method guy, uint8_t target_pin) : pin(target_pin) { set_helpers(guy); }

adc::adc(adc_method guy, uint8_t cs_pin, uint8_t target_pin) : pin(target_pin) {
  ADC_CS = cs_pin;
  set_helpers(guy);
}

adc::adc(adc_method guy, uint8_t target_pin, double alpha)
    : pin(target_pin), alpha(alpha), is_filtered(true) {
  set_helpers(guy);
}

adc::adc(adc_method guy, uint8_t cs_pin, uint8_t target_pin, double alpha)
    : pin(target_pin), alpha(alpha), is_filtered(true) {
  ADC_CS = cs_pin;
  set_helpers(guy);
}

void adc::update() {
  if (!reader)
    return;

  if (is_filtered) {
    rawval = reader(pin);
    value.in =
        static_cast<uint16_t>(alpha * value.in + (1.0 - alpha) * rawval);
  } else {
    value.in = reader(pin);
  }
}


