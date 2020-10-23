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

  Adafruit_BusIO_Register reg_config =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_REG_CONFIG);
  Adafruit_BusIO_RegisterBits tach_mode_enable_bit =
      Adafruit_BusIO_RegisterBits(&reg_config, 1, 2);
  if (!tach_mode_enable_bit.write(true)) {
    return false;
  }
  Adafruit_BusIO_RegisterBits dac_output_enabled_bit =
      Adafruit_BusIO_RegisterBits(&reg_config, 1, 4);
  if (!dac_output_enabled_bit.write(false)) {
    return false;
  }
  //"""When set, the fan control signal is output as a DC voltage instead of a
  // PWM signal"""

  LUTEnabled(false);
  setDutyCycle(100.0);
  // setFanMaxRPM(1700); // from constructor

  Adafruit_BusIO_Register fan_config =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_FAN_CONFIG);
  Adafruit_BusIO_RegisterBits forced_temp_en_bit =
      Adafruit_BusIO_RegisterBits(&fan_config, 1, 6);
  //"""When True, the external temperature measurement will always be read as
  // the value in `forced_ext_temp`"""

  forced_temp_en_bit.write(false);

  Adafruit_BusIO_RegisterBits fan_pwm_clock_override =
      Adafruit_BusIO_RegisterBits(&fan_config, 1, 2);
  fan_pwm_clock_override.write(true);

  // Set to highest rate
  setDataRate(EMC2101_RATE_32_HZ);

  Adafruit_BusIO_RegisterBits invert_fan_output_bit =
      Adafruit_BusIO_RegisterBits(&fan_config, 1, 4);
  //"""When set to True, the magnitude of the fan output signal is inverted,
  // making 0 the maximum

  return true;
}
// /*            REG STOCKPILE
void Adafruit_EMC2101::holdingPen(void){

    // fan spin-upt
    // self._spin_tach_limit = False
    // This should be settable by the constructor
    Adafruit_BusIO_Register _spin_tach_limit = Adafruit_BusIO_Register(i2c_dev, EMC2101_FAN_SPINUP);Adafruit_BusIO_RegisterBits _spin_tach_limit_bits = Adafruit_BusIO_RegisterBits(&_spin_tach_limit, 1, 5);

    Adafruit_BusIO_Register _tach_spinup_limit_lsb = Adafruit_BusIO_Register(i2c_dev, EMC2101_TACH_LIMIT_LSB);
    Adafruit_BusIO_Register _tach_spinup_limit_msb = Adafruit_BusIO_Register(i2c_dev, EMC2101_TACH_LIMIT_MSB);
    Adafruit_BusIO_Register _spin_drive = Adafruit_BusIO_Register(i2c_dev, EMC2101_FAN_SPINUP); Adafruit_BusIO_RegisterBits _spin_drive_bits = Adafruit_BusIO_RegisterBits(&_spin_drive, 1, 3); Adafruit_BusIO_Register _spin_time = Adafruit_BusIO_Register(i2c_dev, EMC2101_FAN_SPINUP);
    Adafruit_BusIO_RegisterBits _spin_time_bits = Adafruit_BusIO_RegisterBits(&_spin_time, 1, 0);
}

/**
 * @brief Get the amount of hysteresis in Degrees celcius of hysteresis applied to temperature
 *  readings used for the LUT. As the temperature drops, the controller will switch to a lower
 *  LUT entry when the measured value is belowthe lower entry's threshold, minus the hysteresis
 *  value
 *
 * @return uint8_t The current LUT hysteresis value
 */
uint8_t Adafruit_EMC2101::getLUTHysteresis(void){

    Adafruit_BusIO_Register lut_temperature_hysteresis = Adafruit_BusIO_Register(i2c_dev, EMC2101_LUT_HYSTERESIS);
    return lut_temperature_hysteresis.read();

}

/**
 * @brief Set the amount of hysteresis in degrees celcius of hysteresis applied to temperature
 *  readings used for the LUT. As the temperature drops, the controller will switch to a lower
 *  LUT entry when the measured value is belowthe lower entry's threshold, minus the hysteresis
 *  value
 *
 * @return uint8_t The current LUT hysteresis value
 */
bool Adafruit_EMC2101::setLUTHysteresis(uint8_t hysteresis){

    Adafruit_BusIO_Register lut_temperature_hysteresis = Adafruit_BusIO_Register(i2c_dev, EMC2101_LUT_HYSTERESIS);
    return lut_temperature_hysteresis.write(hysteresis);

}

bool Adafruit_EMC2101::setLUT(uint8_t index, uint8_t temp_thresh,
                              uint8_t fan_pwm) {
  if (index > 7) {
    return false;
  }
  if (temp_thresh > MAX_LUT_TEMP) {
    return false;
  }
  if (fan_pwm > 100) {
    return false;
  }

  uint8_t temp_reg_addr = EMC2101_LUT_START + (2 * index); // speed/pwm is +1
  Adafruit_BusIO_Register lut_temp =
      Adafruit_BusIO_Register(i2c_dev, temp_reg_addr);
  Adafruit_BusIO_Register lut_pwm =
      Adafruit_BusIO_Register(i2c_dev, temp_reg_addr + 1);

  float scalar = (float)fan_pwm / 100.0;
  uint8_t scaled_pwm = (uint8_t)(scalar * MAX_LUT_SPEED);

  bool lut_enabled = LUTEnabled();
  LUTEnabled(false);
  if (!lut_temp.write(temp_thresh)) {
    return false;
  }
  if (!lut_pwm.write(scaled_pwm)) {
    return false;
  }
  LUTEnabled(lut_enabled);

  return true;
}

/**
 * @brief Get the fan speed setting used while the LUT is being updated and is
 * unavailable or not in use. The speed is  given as the fan's PWM duty cycle
 * represented as a float percentage. The value **roughly** approximates the
 * percentage of the fan's maximum speed"""
 *
 * @return float The current manually set fan duty cycle
 */
uint8_t Adafruit_EMC2101::getDutyCycle(void) {
  Adafruit_BusIO_Register _fan_setting =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_REG_FAN_SETTING);

  uint8_t raw_duty_cycle = _fan_setting.read() & MAX_LUT_SPEED;
  return (uint8_t)((raw_duty_cycle / (float)MAX_LUT_SPEED) * 100);
}
/**
 * @brief Set the fan speed. 
 * 
 * 
 * @param pwm_duty_cycle The  duty cycle percentage as an integer

 * The speed is  given as the fan's PWM duty cycle and **roughly** approximates the
 * percentage of the fan's maximum speed
 */
void Adafruit_EMC2101::setDutyCycle(uint8_t pwm_duty_cycle) {
  Adafruit_BusIO_Register _fan_setting =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_REG_FAN_SETTING);

  // convert from a percentage to that percentage of the max duty cycle
  pwm_duty_cycle = (uint8_t) ( 64 * ((float)pwm_duty_cycle / 100));

  bool lut_enabled = LUTEnabled();
  LUTEnabled(false);
  _fan_setting.write(pwm_duty_cycle);
  LUTEnabled(lut_enabled);
}

bool Adafruit_EMC2101::LUTEnabled(void) {

  Adafruit_BusIO_Register fan_config =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_FAN_CONFIG);
  Adafruit_BusIO_RegisterBits _lut_disable_bit =
      Adafruit_BusIO_RegisterBits(&fan_config, 1, 5);
  return !_lut_disable_bit.read();
}

bool Adafruit_EMC2101::LUTEnabled(bool enable_lut) {

  Adafruit_BusIO_Register fan_config =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_FAN_CONFIG);
  Adafruit_BusIO_RegisterBits _lut_disable_bit =
      Adafruit_BusIO_RegisterBits(&fan_config, 1, 5);
  return _lut_disable_bit.write(!enable_lut);
}

uint16_t Adafruit_EMC2101::getFanMaxRPM(void) {}
void Adafruit_EMC2101::setFanMaxRPM(float uint16_t) {}

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
