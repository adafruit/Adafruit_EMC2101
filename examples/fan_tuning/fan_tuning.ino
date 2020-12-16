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
    emc2101.setDutyCycle(40);
    emc2101.setFanMinRPM(150);
    Serial.print("FAN minimum RPM:"); Serial.println(emc2101.getFanMinRPM());
}

void loop() {


  Serial.print("External Temperature: ");
  Serial.print(emc2101.getExternalTemperature());Serial.println(" degrees C");

  Serial.print("Internal Temperature: ");
  Serial.print(emc2101.getInternalTemperature());Serial.println(" degrees C");

  Serial.print("Fan RPM:: ");Serial.print(emc2101.getFanRPM());Serial.println(" RPM");
  Serial.print("LUT Enabled: "); Serial.println(emc2101.LUTEnabled());
  Serial.println("");

  delay(100);

  Serial.println("Comparing default behavior of fan speed to RPM");

  emc2101.invertFanSpeed(false);
  delay(100);

  for(int i = 0; i<10; i++){
    Serial.print("Setting pwm to "); Serial.println(i+5);
    emc2101.setDutyCycle(i+5);
    delay(3000);

    Serial.print("Fan RPM:: ");
    Serial.print(emc2101.getFanRPM());
    
    Serial.println(" RPM");

    Serial.print("Fan PWM: ");Serial.print(emc2101.getDutyCycle());
    Serial.println(" %");

    delay(2000);
  }

  Serial.println("\nInverting fan speed polarity...");
  emc2101.invertFanSpeed(true);

  for(int i = 0; i<10; i++){
    Serial.print("Setting pwm to "); Serial.println(i+5);
    emc2101.setDutyCycle(i+5);

    delay(4000);

    Serial.print("Fan RPM:: ");
    Serial.print(emc2101.getFanRPM());

    Serial.println(" RPM");
    Serial.print("Fan RPM:: ");Serial.print(emc2101.getDutyCycle());
    Serial.println(" %");

    delay(2000);
  }
  delay(100);
}
