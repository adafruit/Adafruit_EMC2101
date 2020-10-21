// Basic demo for readings from Adafruit EMC2101
#include <Wire.h>
#include <Adafruit_EMC2101.h>
#include <Adafruit_SSD1306.h>

#define SLIDER_PIN A0
Adafruit_EMC2101  emc2101;
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
float slider_val = 10.0;
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
  emc2101.setLUT(3, 60, 100);
  // Finally we need to enable the LUT to give it control over the fan speed
  emc2101.LUTEnabled(true);


}

void loop() {
  display.clearDisplay();
  display.setCursor(0,0);

  Serial.print("External Temperature: ");Serial.print(emc2101.getExternalTemperature());Serial.println(" degrees C");

  Serial.print("Internal Temperature: ");Serial.print(emc2101.getInternalTemperature());Serial.println(" degrees C");
  Serial.print("Fan RPM:: ");Serial.print(emc2101.getFanRPM());Serial.println(" RPM");

  display.print("Temp E/I: ");display.print(emc2101.getExternalTemperature(),1);display.print("/");display.print(emc2101.getInternalTemperature(),1);display.println(" C");
  display.print("Fan RPM: ");display.print(emc2101.getFanRPM());display.println(" RPM");
  display.print("PWM: "); display.print(emc2101.getDutyCycle());display.println("");
  delay(100);


  display.display();
  delay(100);

}