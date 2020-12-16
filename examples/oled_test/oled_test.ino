// Demo to show readings from an EMC2101 on a handy-dandy little OLED
#include <Wire.h>
#include <Adafruit_EMC2101.h>
#include <Adafruit_SSD1306.h>

Adafruit_EMC2101  emc2101;
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

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

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  delay(500); // Pause for half second

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setRotation(0);

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

  emc2101.setDutyCycle(40);
}


void loop() {
  display.clearDisplay();
  display.setCursor(0,0);

  Serial.print("External Temperature: ");
  Serial.print(emc2101.getExternalTemperature());
  Serial.println(" degrees C");

  Serial.print("Internal Temperature: ");
  Serial.print(emc2101.getInternalTemperature());
  Serial.println(" degrees C");

  Serial.print("Fan RPM:: ");
  Serial.print(emc2101.getFanRPM());
  Serial.println(" RPM");

  Serial.print("LUT Enabled: ");
  Serial.println(emc2101.LUTEnabled());
  Serial.println("");

  display.print("Ext Temp: ");
  display.print(emc2101.getExternalTemperature());
  display.println(" deg C");

  display.print("Int Temp: ");
  display.print(emc2101.getInternalTemperature());
  display.println(" deg C");

  display.print("Fan RPM: ");
  display.print(emc2101.getFanRPM());
  display.println(" RPM");

  display.print("PWM: ");
  display.print(emc2101.getDutyCycle());
  display.println("");

  display.display();
  delay(100);
}
