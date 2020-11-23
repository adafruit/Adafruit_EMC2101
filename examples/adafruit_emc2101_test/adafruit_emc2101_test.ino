// Basic demo for readings from Adafruit EMC2101
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

  //  emc2101.setDataRate(EMC2101_RATE_1_16_HZ);
  Serial.print("Data rate set to: ");
  switch (emc2101.getDataRate()) {
    case EMC2101_RATE_1_16_HZ: Serial.println("1/16_HZ"); break;
    case EMC2101_RATE_1_8_HZ: Serial.println("1/8_HZ"); break;
    case EMC2101_RATE_1_4_HZ: Serial.println("1/4_HZ"); break;
    case EMC2101_RATE_1_2_HZ: Serial.println("1/2_HZ"); break;
    case EMC2101_RATE_1_HZ: Serial.println("1 HZ"); break;
    case EMC2101_RATE_2_HZ: Serial.println("2 HZ"); break;
    case EMC2101_RATE_4_HZ: Serial.println("4 HZ"); break;
    case EMC2101_RATE_8_HZ: Serial.println("8 HZ"); break;
    case EMC2101_RATE_16_HZ: Serial.println("16 HZ"); break;
    case EMC2101_RATE_32_HZ: Serial.println("32 HZ"); break;
  }

  emc2101.enableTachInput(true);
  emc2101.setPWMDivisor(0);
  emc2101.setDutyCycle(50);
}



void loop() {
  Serial.print("External Temperature: ");
  Serial.print(emc2101.getExternalTemperature());
  Serial.println(" degrees C");

  Serial.print("Internal Temperature: ");
  Serial.print(emc2101.getInternalTemperature());
  Serial.println(" degrees C");

  Serial.print("Duty Cycle: ");
  Serial.print(emc2101.getDutyCycle());
  Serial.print("% / Fan RPM: ");
  Serial.print(emc2101.getFanRPM());
  Serial.println(" RPM");
  Serial.println("");

  delay(100);
}