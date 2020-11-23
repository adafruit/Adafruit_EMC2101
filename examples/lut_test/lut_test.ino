// Basic demo for readings from Adafruit EMC2101
#include <Wire.h>
#include <Adafruit_EMC2101.h>

Adafruit_EMC2101  emc2101;

void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit EMC2101 test!");

  // Try to initialize!
  if (!emc2101.begin()) {
    Serial.println("Failed to find EMC2101 chip");
    while (1) { delay(10); }
  }

  Serial.println("EMC2101 Found!");
  emc2101.setDutyCycle(50);

 /************ LUT SETUP ************/
 // set the first LUT entry to set the fan to 10% duty cycle
 // when the temperature goes over 20 degrees C
  emc2101.setLUT(0, 20, 10);

 // NOTE!
// The temperature threshold must be MORE THAN the previous entries.
// In this case 30>20 degrees, so the 30 degree threshold is second

 // set the second LUT entry to set the fan to 20% duty cycle
 // when the temperature goes over 30 degrees C
  emc2101.setLUT(1, 30, 20);

  /// the highest temperature threshold goes last
  emc2101.setLUT(2, 60, 100);

  // Finally we need to enable the LUT to give it control over the fan speed
  emc2101.LUTEnabled(true);
  emc2101.setLUTHysteresis(5); // 5 degree C fudge factor
  Serial.print("LUT Hysteresis: "); Serial.println(emc2101.getLUTHysteresis());

  Serial.print("LUT enabled: "); Serial.println(emc2101.LUTEnabled());

  // use these settings to hard-code the temperature to test the LUT
  Serial.print("enable force success: "); Serial.println(emc2101.enableForcedTemperature(true));

}

void loop() {
  Serial.print("Forcing to 25 degrees: ");
  emc2101.setForcedTemperature(25);
  Serial.print("Forced Temperature: ");Serial.print(emc2101.getForcedTemperature());Serial.println(" degrees C");

 delay(3000);

  Serial.print("PWM: "); Serial.print(emc2101.getDutyCycle());Serial.println("%");
  Serial.print("Fan RPM:: ");Serial.print(emc2101.getFanRPM());Serial.println(" RPM");
  Serial.println("");

  Serial.print("Forcing to 100 degrees: ");
  emc2101.setForcedTemperature(100);
  Serial.print("Forced Temperature: ");Serial.print(emc2101.getForcedTemperature());
  Serial.println(" degrees C");
  delay(3000);

  Serial.print("PWM: "); Serial.print(emc2101.getDutyCycle());Serial.println("%");
  Serial.print("Fan RPM:: ");Serial.print(emc2101.getFanRPM());Serial.println(" RPM");
  Serial.println("");

}