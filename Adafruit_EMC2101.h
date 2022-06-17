/*!
 *  @file Adafruit_EMC2101.h
 *
 * 	I2C Driver for the Adafruit EMC2101 Temperature monitor and fan
 *controller This is a library is written to work with the Adafruit EMC2101
 *breakout: https://www.adafruit.com/products/47nn
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *
 *	BSD license (see license.txt)
 */

#ifndef _ADAFRUIT_EMC2101_H
#define _ADAFRUIT_EMC2101_H

#include "Arduino.h"
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>
#include <Wire.h>

#define EMC2101_I2CADDR_DEFAULT 0x4C ///< EMC2101 default i2c address
#define EMC2101_CHIP_ID 0x16         ///< EMC2101 default device id from part id
#define EMC2101_ALT_CHIP_ID 0x28 ///< EMC2101 alternate device id from part id
#define EMC2101_WHOAMI 0xFD      ///< Chip ID register

#define EMC2101_INTERNAL_TEMP 0x00 ///< The internal temperature register
#define EMC2101_EXTERNAL_TEMP_MSB                                              \
  0x01 ///< high byte for the external temperature reading
#define EMC2101_EXTERNAL_TEMP_LSB                                              \
  0x10 ///< low byte for the external temperature reading

#define EMC2101_STATUS 0x02        ///< Status register
#define EMC2101_REG_CONFIG 0x03    ///< configuration register
#define EMC2101_REG_DATA_RATE 0x04 ///< Data rate config
#define EMC2101_TEMP_FORCE 0x0C    ///< Temp force setting for LUT testing
#define EMC2101_TACH_LSB 0x46      ///< Tach RPM data low byte
#define EMC2101_TACH_MSB 0x47      ///< Tach RPM data high byte
#define EMC2101_TACH_LIMIT_LSB                                                 \
  0x48 ///< Tach low-speed setting low byte. INVERSE OF THE SPEED
#define EMC2101_TACH_LIMIT_MSB                                                 \
  0x49 ///< Tach low-speed setting high byte. INVERSE OF THE SPEED
#define EMC2101_FAN_CONFIG 0x4A ///< General fan config register
#define EMC2101_FAN_SPINUP 0x4B ///< Fan spinup behavior settings
#define EMC2101_REG_FAN_SETTING                                                \
  0x4C ///< Fan speed for non-LUT settings, as a % PWM duty cycle
#define EMC2101_PWM_FREQ 0x4D ///< PWM frequency setting
#define EMC2101_PWM_DIV 0x4E  ///< PWM frequency divisor
#define EMC2101_LUT_HYSTERESIS                                                 \
  0x4F ///< The hysteresis value for LUT lookups when temp is decreasing

#define EMC2101_LUT_START 0x50 ///< The first temp threshold register

#define EMC2101_TEMP_FILTER                                                    \
  0xBF ///< The external temperature sensor filtering behavior
#define EMC2101_REG_PARTID 0xFD ///< 0x16
#define EMC2101_REG_MFGID 0xFE  ///< 0xFF16

#define MAX_LUT_SPEED 0x3F ///< 6-bit value
#define MAX_LUT_TEMP 0x7F  ///<  7-bit

#define EMC2101_I2C_ADDR 0x4C ///< The default I2C address
#define EMC2101_FAN_RPM_NUMERATOR                                              \
  5400000               ///< Conversion unit to convert LSBs to fan RPM
#define _TEMP_LSB 0.125 ///< single bit value for internal temperature readings

/**
 * @brief
 *
 * Allowed values for `setDataRate`.
 */
typedef enum {
  EMC2101_RATE_1_16_HZ, ///< 1_16_HZ
  EMC2101_RATE_1_8_HZ,  ///< 1_8_HZ
  EMC2101_RATE_1_4_HZ,  ///< 1_4_HZ
  EMC2101_RATE_1_2_HZ,  ///< 1_2_HZ
  EMC2101_RATE_1_HZ,    ///< 1_HZ
  EMC2101_RATE_2_HZ,    ///< 2_HZ
  EMC2101_RATE_4_HZ,    ///< 4_HZ
  EMC2101_RATE_8_HZ,    ///< 8_HZ
  EMC2101_RATE_16_HZ,   ///< 16_HZ
  EMC2101_RATE_32_HZ,   ///< 32_HZ
} emc2101_rate_t;

/*!
 *    @brief  Class that stores state and functions for interacting with
 *            the EMC2101 Temperature monitor and fan controller
 */
class Adafruit_EMC2101 {
public:
  Adafruit_EMC2101();
  ~Adafruit_EMC2101();

  bool begin(uint8_t i2c_addr = EMC2101_I2CADDR_DEFAULT, TwoWire *wire = &Wire);

  // Enable/disable & status functions:
  bool LUTEnabled(void);
  bool LUTEnabled(bool enable_lut);

  bool DACOutEnabled(bool enable_dac_out);
  bool DACOutEnabled(void);

  // Accessors:
  float getExternalTemperature(void);
  int8_t getInternalTemperature(void);
  uint16_t getFanRPM(void);

  uint8_t getDutyCycle(void);
  bool setDutyCycle(uint8_t pwm_duty_cycle);

  uint16_t getFanMinRPM(void);
  bool setFanMinRPM(uint16_t min_rpm);

  emc2101_rate_t getDataRate(void);
  bool setDataRate(emc2101_rate_t data_rate);

  bool setLUT(uint8_t index, uint8_t temp_thresh, uint8_t fan_pwm);

  uint8_t getPWMFrequency(void);
  bool setPWMFrequency(uint8_t pwm_freq);

  uint8_t getPWMDivisor(void);
  bool setPWMDivisor(uint8_t pwm_divisor);

  bool configPWMClock(bool clksel, bool clkovr);

  bool setLUTHysteresis(uint8_t hysteresis);
  uint8_t getLUTHysteresis(void);

  bool configFanSpinup(uint8_t spinup_drive, uint8_t spinup_time);
  bool configFanSpinup(bool tach_spinup);

  bool enableForcedTemperature(bool enable_forced);
  bool setForcedTemperature(int8_t forced_temperature);
  int8_t getForcedTemperature(void);

  bool enableTachInput(bool tach_enable);
  bool invertFanSpeed(bool invert_speed);

private:
  bool _init(void);

  Adafruit_I2CDevice *i2c_dev = NULL; ///< Pointer to I2C bus interface
};

#endif
