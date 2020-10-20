/*!
 *  @file Adafruit_EMC2101.cpp
 *
 *  @mainpage Adafruit EMC2101 Temperature monitor and fan controller
 *
 *  @section intro_sec Introduction
 *
 * 	I2C Driver for the Library for the EMC2101 Temperature monitor and fan
 * controller
 *
 * 	This is a library for the Adafruit EMC2101 breakout:
 * 	https://www.adafruit.com/product/47nn
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *  @section dependencies Dependencies
 *  This library depends on the Adafruit BusIO library
 *
 *  This library depends on the Adafruit Unified Sensor library
 *
 *  @section author Author
 *
 *  Bryan Siepert for Adafruit Industries
 *
 * 	@section license License
 *
 * 	BSD (see license.txt)
 *
 * 	@section  HISTORY
 *
 *     v1.0 - First release
 */

#include "Arduino.h"
#include <Wire.h>

#include "Adafruit_EMC2101.h"

/**
 * @brief Construct a new Adafruit_EMC2101::Adafruit_EMC2101 object
 *
 */
Adafruit_EMC2101::Adafruit_EMC2101(void) {}

/**
 * @brief Destroy the Adafruit_EMC2101::Adafruit_EMC2101 object
 *
 */
Adafruit_EMC2101::~Adafruit_EMC2101(void) {}

/*!
 *    @brief  Sets up the hardware and initializes I2C
 *    @param  i2c_address
 *            The I2C address to be used.
 *    @param  wire
 *            The Wire object to be used for I2C connections.
 *    @return True if initialization was successful, otherwise false.
 */
bool Adafruit_EMC2101::begin(uint8_t i2c_address, TwoWire *wire) {
  if (i2c_dev) {
    delete i2c_dev; // remove old interface
  }

  i2c_dev = new Adafruit_I2CDevice(i2c_address, wire);

  if (!i2c_dev->begin()) {
    return false;
  }

  return _init();
}

/*!  @brief Initializer for post i2c/spi init
 *   @param sensor_id Optional unique ID for the sensor set
 *   @returns True if chip identified and initialized
 */
bool Adafruit_EMC2101::_init(void) {

  Adafruit_BusIO_Register chip_id =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_WHOAMI, 1);

  // make sure we're talking to the right chip
  if ((chip_id.read() != EMC2101_CHIP_ID) &&
      (chip_id.read() != EMC2101_ALT_CHIP_ID)) {
    return false;
  }

  // do any software reset or other initial setup
  // self._tach_mode_enable = True
  // self.lut_enabled = False
  // self._enabled_forced_temp = False
  // self._fan_pwm_clock_override = True
  // self._spin_tach_limit = False
  // Set to highest rate
  setDataRate(EMC2101_RATE_32_HZ);

  return true;
}

float Adafruit_EMC2101::getExternalTemperature(void) {
  // chip doesn't like doing multi-byte reads so we'll get each byte separately
  // and join
  uint8_t buffer[2];
  Adafruit_BusIO_Register ext_temp_lsb =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_EXTERNAL_TEMP_LSB);
  Adafruit_BusIO_Register ext_temp_msb =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_EXTERNAL_TEMP_MSB);
  ext_temp_msb.read(buffer);
  ext_temp_lsb.read(buffer + 1);

  int16_t raw_ext = buffer[0] << 8;
  raw_ext |= buffer[1];
  raw_ext >>= 5;

  return raw_ext * 0.125;
}

int8_t Adafruit_EMC2101::getInternalTemperature(void) {
  // _INTERNAL_TEMP = const(0x00)
  Adafruit_BusIO_Register ext_temp_lsb =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_INTERNAL_TEMP);

  return (int8_t)ext_temp_lsb.read();
}

uint16_t Adafruit_EMC2101::getFanRPM(void) {
  uint8_t buffer[2];
  Adafruit_BusIO_Register fan_speed_lsb =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_TACH_LSB);
  Adafruit_BusIO_Register fan_speed_msb =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_TACH_MSB);

  fan_speed_msb.read(buffer);
  fan_speed_lsb.read(buffer + 1);

  uint16_t raw_ext = buffer[0] << 8;
  raw_ext |= buffer[1];

  return 5400000 / raw_ext;
}
// ////////////////////////////

//     @property
//     def fan_speed(self):
//         """The current speed in Revolutions per Minute (RPM)"""

//         val = self._tach_read_lsb
//         val |= self._tach_read_msb << 8
//         return _FAN_RPM_DIVISOR / val

//     @property
//     def manual_fan_speed(self):
//         """The fan speed used while the LUT is being updated and is
//         unavailable. The speed is given as the fan's PWM duty cycle
//         represented as a float percentage. The value roughly approximates the
//         percentage of the fan's maximum speed""" raw_setting =
//         self._fan_setting & MAX_LUT_SPEED return (raw_setting /
//         MAX_LUT_SPEED) * 100

//     @manual_fan_speed.setter
//     def manual_fan_speed(self, fan_speed):
//         if fan_speed not in range(0, 101):
//             raise AttributeError("manual_fan_speed must be from 0-100 ")

//         # convert from a percentage to an lsb value
//         percentage = fan_speed / 100.0
//         fan_speed_lsb = round(percentage * MAX_LUT_SPEED)
//         lut_disabled = self._fan_lut_prog
//         self._fan_lut_prog = True
//         self._fan_setting = fan_speed_lsb
//         self._fan_lut_prog = lut_disabled
//////////////////
/**
 * @brief Gets the current rate at which pressure and temperature measurements
 * are taken
 *
 * @return emc2101_rate_t The current data rate
 */
emc2101_rate_t Adafruit_EMC2101::getDataRate(void) {
  Adafruit_BusIO_Register rate_reg =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_REG_DATA_RATE, 1);
  Adafruit_BusIO_RegisterBits data_rate =
      Adafruit_BusIO_RegisterBits(&rate_reg, 4, 0);
  // _conversion_rate = RWBits(4, 0x04, 0)

  return (emc2101_rate_t)data_rate.read();
}
/**
 * @brief Sets the rate at which pressure and temperature measurements
 *
 * @param new_data_rate The data rate to set. Must be a `emc2101_rate_t`
 */
bool Adafruit_EMC2101::setDataRate(emc2101_rate_t new_data_rate) {
  Adafruit_BusIO_Register rate_reg =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_REG_DATA_RATE, 1);
  Adafruit_BusIO_RegisterBits data_rate =
      Adafruit_BusIO_RegisterBits(&rate_reg, 4, 0);

  return data_rate.write(new_data_rate);
}

/******************* Adafruit_Sensor functions *****************/
/*!
 *     @brief  Updates the measurement data for all sensors simultaneously
 */
/**************************************************************************/
void Adafruit_EMC2101::_read(void) {}
