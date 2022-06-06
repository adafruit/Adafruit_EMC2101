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
    Serial.println("Address not found");
    return false;
  }

  return _init();
}

/*!  @brief Initializer for post i2c/spi init
 *   @returns True if chip identified and initialized
 */
bool Adafruit_EMC2101::_init(void) {

  Adafruit_BusIO_Register chip_id =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_WHOAMI, 1);

  // make sure we're talking to the right chip
  if ((chip_id.read() != EMC2101_CHIP_ID) &&
      (chip_id.read() != EMC2101_ALT_CHIP_ID)) {
    Serial.println("Wrong chip ID ");
    return false;
  }

  enableTachInput(true);
  invertFanSpeed(false);
  setPWMFrequency(0x1F);
  configPWMClock(1, 0);
  DACOutEnabled(false); // output PWM mode by default
  LUTEnabled(false);
  setDutyCycle(100);

  enableForcedTemperature(false);

  // Set to highest rate
  setDataRate(EMC2101_RATE_32_HZ);

  return true;
}

/**
 * @brief Enable using the TACH/ALERT pin as an input to read the fan speed
 * signal from a 4-pin fan
 *
 * @param tach_enable true: to enable tach signal input, false to disable and
 * use the tach pin as interrupt & status output
 * @return true: sucess false: failure
 */
bool Adafruit_EMC2101::enableTachInput(bool tach_enable) {
  Adafruit_BusIO_Register reg_config =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_REG_CONFIG);
  Adafruit_BusIO_RegisterBits tach_mode_enable_bit =
      Adafruit_BusIO_RegisterBits(&reg_config, 1, 2);
  return tach_mode_enable_bit.write(tach_enable);
}

/**
 * @brief Set the cotroller to interperate fan speed settings opposite of the
 * normal behavior
 *
 * @param invert_speed If true, fan duty cycle / DAC value settings will work
 * backwards; Setting the highest value (100) will set the fan to it's lowest
 * PWM value, and setting the fan to the lowest value (0) will set the fan
 * output to it's highest setting.
 * @return true:sucess false:failure
 */
bool Adafruit_EMC2101::invertFanSpeed(bool invert_speed) {

  Adafruit_BusIO_Register fan_config =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_FAN_CONFIG);
  Adafruit_BusIO_RegisterBits invert_fan_output_bit =
      Adafruit_BusIO_RegisterBits(&fan_config, 1, 4);
  return invert_fan_output_bit.write(invert_speed);
}

/**
 * @brief Configure the PWM clock by selecting the clock source and overflow bit
 *
 * @param clksel The clock select true: Use a 1.4kHz base PWM clock
 * false: Use the default 360kHz PWM clock
 *
 * @param clkovr Clock override
 * When true, override the base clock selected by `clksel` and use the frequency
 * divisor to set the PWM frequency
 *
 * @return true:success false:failure
 */
bool Adafruit_EMC2101::configPWMClock(bool clksel, bool clkovr) {
  Adafruit_BusIO_Register fan_config =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_FAN_CONFIG);
  Adafruit_BusIO_RegisterBits clksel_bit =
      Adafruit_BusIO_RegisterBits(&fan_config, 1, 3);
  clksel_bit.write(clksel);
  Adafruit_BusIO_RegisterBits clkovr_bit =
      Adafruit_BusIO_RegisterBits(&fan_config, 1, 2);
  return clkovr_bit.write(clkovr);
}

/**
 * @brief Configure the fan's spinup behavior when transitioning from
 * off/minimal speed to a higher speed (except on power up)
 *
 * @param spinup_drive The duty cycle to drive the fan with during spin up
 * **defaults to 100%**
 *
 * @param spinup_time The amount of time to keep the fan at the given drive
 * setting. **Defaults to 3.2 seconds**
 *
 * @return true:success false: failure
 */
bool Adafruit_EMC2101::configFanSpinup(uint8_t spinup_drive,
                                       uint8_t spinup_time) {

  Adafruit_BusIO_Register spin_config =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_FAN_SPINUP);

  Adafruit_BusIO_RegisterBits _spin_drive_bits =
      Adafruit_BusIO_RegisterBits(&spin_config, 2, 3);
  if (!_spin_drive_bits.write(spinup_drive)) {
    return false;
  }

  Adafruit_BusIO_RegisterBits _spin_time_bits =
      Adafruit_BusIO_RegisterBits(&spin_config, 3, 0);
  return _spin_time_bits.write(spinup_time);
}

/**
 * @brief Configure the fan's spinup behavior when transitioning from
 * off/minimal speed to a higher speed (except on power up)
 *
 * @param tach_spinup If true, drive the fan at 100% until the speed is above
 * the speed set with `setFanMinRPM`. If previously set, `spinup_drive` and
 * `spinup_time` are ignored
 *
 * @return true:success false: failure
 */
bool Adafruit_EMC2101::configFanSpinup(bool tach_spinup) {
  // This should be settable by the constructor
  Adafruit_BusIO_Register spin_config =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_FAN_SPINUP);
  Adafruit_BusIO_RegisterBits tach_spinup_en =
      Adafruit_BusIO_RegisterBits(&spin_config, 1, 5);
  return tach_spinup_en.write(tach_spinup);
}

/**
 * @brief Get the amount of hysteresis in Degrees celcius of hysteresis applied
 * to temperature readings used for the LUT. As the temperature drops, the
 * controller will switch to a lower LUT entry when the measured value is
 * belowthe lower entry's threshold, minus the hysteresis value
 *
 * @return uint8_t The current LUT hysteresis value
 */
uint8_t Adafruit_EMC2101::getLUTHysteresis(void) {

  Adafruit_BusIO_Register lut_temperature_hysteresis =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_LUT_HYSTERESIS);
  return lut_temperature_hysteresis.read();
}

/**
 * @brief Set the amount of hysteresis in degrees celcius of hysteresis applied
 * to temperature readings used for the LUT.
 *
 * @param hysteresis  The hysteresis value in degrees celcius. As the
 * temperature drops, the controller will switch to a lower LUT entry when the
 * measured value is `hystersis` degrees below the lower entry's temperature
 * threshold
 *
 * @return uint8_t The current LUT hysteresis value
 */
bool Adafruit_EMC2101::setLUTHysteresis(uint8_t hysteresis) {

  Adafruit_BusIO_Register lut_temperature_hysteresis =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_LUT_HYSTERESIS);
  return lut_temperature_hysteresis.write(hysteresis);
}

/**
 * @brief Create a new mapping between temperature and fan speed in the Look Up
 * Table. Requires the LUT to be enabled with `LUTEnabled(true)`
 *
 * @param index The index in the LUT, from 0-7. Note that the temperature
 * thresholds should increase with the LUT index, so the temperature value for a
 * given LUT entry is higher than the temperature for the previous index:
 * @code
 * // NO!:
 * setLUT(0, 30, 25); // 25% PWM @ 30 Degrees C
 * setLUT(1, 20, 10); // WRONG! 10% PWM @ 20 Degrees C should be before 30
 * // degrees from index 0 setLUT(2, 40, 50); // 50% PWM @ 40 Degrees C
 *
 * // YES:
 * setLUT(0, 20, 10); // 10% PWM @ 20 Degrees C
 * setLUT(1, 30, 25); // 25% PWM @ 30 Degrees C
 * setLUT(2, 40, 50); // 50% PWM @ 40 Degrees C
 *@endcode
 *
 * @param temp_thresh When the temperature is more than this threshold, the fan
 * will be set to the given PWM value
 * @param fan_pwm The pwm-based fan speed for the given temperature threshold.
 * When DAC output is enabled, this determins the percentage of the maximum
 * output voltage to be used for the given temperature threshold
 * @return true:success false:failure
 */
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
 * The speed is  given as the fan's PWM duty cycle and **roughly** approximates
 * the percentage of the fan's maximum speed
 * @return true: success false: failure
 */
bool Adafruit_EMC2101::setDutyCycle(uint8_t pwm_duty_cycle) {
  Adafruit_BusIO_Register _fan_setting =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_REG_FAN_SETTING);

  // convert from a percentage to that percentage of the max duty cycle
  pwm_duty_cycle = map(pwm_duty_cycle, 0, 100, 0, 63);

  bool lut_enabled = LUTEnabled();
  LUTEnabled(false);
  if (!_fan_setting.write(pwm_duty_cycle)) {
    return false;
  }
  return LUTEnabled(lut_enabled);
}

/**
 * @brief Get the LUT enable status
 *
 * @return true: LUT usage enabled false: LUT disabled
 */
bool Adafruit_EMC2101::LUTEnabled(void) {

  Adafruit_BusIO_Register fan_config =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_FAN_CONFIG);
  Adafruit_BusIO_RegisterBits _lut_disable_bit =
      Adafruit_BusIO_RegisterBits(&fan_config, 1, 5);
  return !_lut_disable_bit.read();
}

/**
 * @brief Enable or disable the temperature-to-fan speed Look Up Table (LUT)
 *
 * @param enable_lut True to enable the LUT, setting the fan speed depending on
 * the configured temp to speed mapping. False disables the LUT, defaulting to
 * the speed setting from `setDutyCycle`
 * @return true:success false: failure
 */
bool Adafruit_EMC2101::LUTEnabled(bool enable_lut) {

  Adafruit_BusIO_Register fan_config =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_FAN_CONFIG);
  Adafruit_BusIO_RegisterBits _lut_disable_bit =
      Adafruit_BusIO_RegisterBits(&fan_config, 1, 5);
  return _lut_disable_bit.write(!enable_lut);
}

/**
 * @brief Get the mimnum RPM setting for the attached fan
 *
 * @return uint16_t the current minimum RPM setting
 */
uint16_t Adafruit_EMC2101::getFanMinRPM(void) {

  uint8_t buffer[2];
  Adafruit_BusIO_Register tach_limit_lsb =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_TACH_LIMIT_LSB);
  Adafruit_BusIO_Register tach_limit_msb =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_TACH_LIMIT_MSB);
  tach_limit_msb.read(buffer);
  tach_limit_lsb.read(buffer + 1);

  uint16_t raw_limit = buffer[0] << 8;
  raw_limit |= buffer[1];
  if (raw_limit == 0xFFFF) {
    return 0;
  }
  return EMC2101_FAN_RPM_NUMERATOR / raw_limit;
}

/**
 * @brief Set the minimum speed of the attached fan
 *
 * Used to determine the fan state
 *
 * @param min_rpm The minimum speed of the fan. Any setting below this will
 * return 0 and mark the fan as non-operational
 * @return true: success false: failure
 */
bool Adafruit_EMC2101::setFanMinRPM(uint16_t min_rpm) {

  Adafruit_BusIO_Register tach_limit_lsb =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_TACH_LIMIT_LSB);
  Adafruit_BusIO_Register tach_limit_msb =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_TACH_LIMIT_MSB);
  // speed is given in RPM, convert to raw value (MSB+LSB):
  uint16_t raw_value = EMC2101_FAN_RPM_NUMERATOR / min_rpm;
  if (!tach_limit_lsb.write(raw_value & 0xFF)) {
    return false;
  }
  if (!tach_limit_msb.write((raw_value >> 8) & 0xFF)) {
    return false;
  }
  return true;
}

/**
 * @brief Read the external temperature diode
 *
 * @return float the current temperature in degrees C
 */
float Adafruit_EMC2101::getExternalTemperature(void) {
  // chip doesn't like doing multi-byte reads so we'll get each byte separately
  // and join
  uint8_t buffer[2];
  Adafruit_BusIO_Register ext_temp_lsb =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_EXTERNAL_TEMP_LSB);
  Adafruit_BusIO_Register ext_temp_msb =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_EXTERNAL_TEMP_MSB);

  // Read **MSB** first to match 'Data Read Interlock' behavoior from 6.1 of
  // datasheet
  ext_temp_msb.read(buffer);
  ext_temp_lsb.read(buffer + 1);

  int16_t raw_ext = buffer[0] << 8;
  raw_ext |= buffer[1];

  raw_ext >>= 5;
  return raw_ext * _TEMP_LSB;
}

/**
 * @brief Read the internal temperature sensor
 *
 * @return int8_t the current temperature in degrees celcius
 */
int8_t Adafruit_EMC2101::getInternalTemperature(void) {
  // _INTERNAL_TEMP = const(0x00)
  Adafruit_BusIO_Register int_temp_lsb =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_INTERNAL_TEMP);

  return (int8_t)int_temp_lsb.read();
}

/**
 * @brief Read the current fan speed in RPM.
 *
 * @return uint16_t The current fan speed, 0 if no tachometer input
 */
uint16_t Adafruit_EMC2101::getFanRPM(void) {
  uint8_t buffer[2];
  Adafruit_BusIO_Register fan_speed_lsb =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_TACH_LSB);
  Adafruit_BusIO_Register fan_speed_msb =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_TACH_MSB);

  // Read LSB first to match 'Data Read Interlock' behavoior from 6.1 of
  // datasheet
  fan_speed_lsb.read(buffer + 1);
  fan_speed_msb.read(buffer);

  uint16_t raw_ext = buffer[0] << 8;
  raw_ext |= buffer[1];

  if (raw_ext == 0xFFFF) {
    return 0;
  }

  return EMC2101_FAN_RPM_NUMERATOR / raw_ext;
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
 * @return bool true:success false:failure
 */
bool Adafruit_EMC2101::setDataRate(emc2101_rate_t new_data_rate) {
  Adafruit_BusIO_Register rate_reg =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_REG_DATA_RATE, 1);
  Adafruit_BusIO_RegisterBits data_rate =
      Adafruit_BusIO_RegisterBits(&rate_reg, 4, 0);

  return data_rate.write(new_data_rate);
}

/**
 * @brief Enable or disable outputting the fan control signal as a DC voltage
 * instead of the default PWM output
 *
 * @param enable_dac_out true will enable DAC output, false disables DAC output
 * @return true:success false: failure
 */
bool Adafruit_EMC2101::DACOutEnabled(bool enable_dac_out) {
  Adafruit_BusIO_Register reg_config =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_REG_CONFIG);
  Adafruit_BusIO_RegisterBits dac_output_enabled_bit =
      Adafruit_BusIO_RegisterBits(&reg_config, 1, 4);
  if (!dac_output_enabled_bit.write(enable_dac_out)) {
    return false;
  }
  return true;
}

/**
 * @brief Get the current DAC output enable setting
 *
 * @return true: DAC output enabled
 * @return false DAC output disabled
 */
bool Adafruit_EMC2101::DACOutEnabled(void) {
  Adafruit_BusIO_Register reg_config =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_REG_CONFIG);
  Adafruit_BusIO_RegisterBits dac_output_enabled_bit =
      Adafruit_BusIO_RegisterBits(&reg_config, 1, 4);
  return dac_output_enabled_bit.read();
}

/**
 * @brief Read the final PWM frequency and "effective resolution" of the PWM
 * driver. No effect when DAC output is enabled
 *
 * See the datasheet for additional information:
 * http://ww1.microchip.com/downloads/en/DeviceDoc/2101.pdf
 *
 * @return uint8_t The PWM freq register setting
 */
uint8_t Adafruit_EMC2101::getPWMFrequency(void) {
  Adafruit_BusIO_Register pwm_freq_reg =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_PWM_FREQ);
  return pwm_freq_reg.read();
}

/**
 * @brief Set the final PWM frequency and "effective resolution" of the PWM
 * driver. No effect when DAC output is enabled
 *
 * See the datasheet for additional information:
 * http://ww1.microchip.com/downloads/en/DeviceDoc/2101.pdf
 *
 * @param pwm_freq The new PWM frequency setting
 *
 * @return bool true:success false:failure
 */
bool Adafruit_EMC2101::setPWMFrequency(uint8_t pwm_freq) {
  Adafruit_BusIO_Register pwm_freq_reg =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_PWM_FREQ);
  return pwm_freq_reg.write(pwm_freq);
}

/**
 * @brief Get the alternate PWM frequency digide value to use instead of the
 * clock selection bit when the clock select override is set
 *
 * @return uint8_t The alternate divisor setting
 */
uint8_t Adafruit_EMC2101::getPWMDivisor(void) {
  Adafruit_BusIO_Register pwm_divisor_reg =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_PWM_DIV);
  return pwm_divisor_reg.read();
}

/**
 * @brief Get the alternate PWM frequency digide value to use instead of the
 * clock selection bit when the clock select override is set
 * @param pwm_divisor The alternate divisor setting
 * @return true:success false: failure
 */
bool Adafruit_EMC2101::setPWMDivisor(uint8_t pwm_divisor) {
  Adafruit_BusIO_Register pwm_divisor_reg =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_PWM_DIV);
  return pwm_divisor_reg.write(pwm_divisor);
}

/**
 * @brief Force the LUT to use the temperature set by `setForcedTemperature`.
 *
 * This can be used to use the LUT to set the fan speed based on a different
 * source than the external temperature diode. This can also be used to verify
 * LUT configuration
 *
 * @param enable_forced True to force the LUT to use the forced temperature
 * @return true: success false: failure
 */
bool Adafruit_EMC2101::enableForcedTemperature(bool enable_forced) {
  Adafruit_BusIO_Register fan_config =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_FAN_CONFIG);
  Adafruit_BusIO_RegisterBits forced_temp_en_bit =
      Adafruit_BusIO_RegisterBits(&fan_config, 1, 6);
  return forced_temp_en_bit.write(enable_forced);
}

/**
 * @brief Set the alternate temperature to use to look up a fan setting in the
 * look up table
 *
 * @param forced_temperature The alternative temperature reading used for LUT
 * lookups.
 * @return true: success false: falure
 */
bool Adafruit_EMC2101::setForcedTemperature(int8_t forced_temperature) {

  Adafruit_BusIO_Register forced_temp_reg =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_TEMP_FORCE);
  return forced_temp_reg.write(forced_temperature);
}

/**
 * @brief Get the alternate temperature to use to look up a fan setting in the
 * look up table
 *
 * @return int8_t The alternative temperature reading used for LUT
 * lookups
 */
int8_t Adafruit_EMC2101::getForcedTemperature(void) {

  Adafruit_BusIO_Register forced_temp_reg =
      Adafruit_BusIO_Register(i2c_dev, EMC2101_TEMP_FORCE);
  return forced_temp_reg.read();
}
