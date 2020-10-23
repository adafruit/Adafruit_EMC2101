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


  // // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  // if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
  //   Serial.println(F("SSD1306 allocation failed"));
  //   for(;;); // Don't proceed, loop forever
  // }
  // display.display();
  // delay(500); // Pause for half second

  // display.setTextSize(1);
  // display.setTextColor(WHITE);
  // display.setRotation(0);

  Serial.println("EMC2101 Found!");

  

    emc2101.setDutyCycle(40.0);

    emc2101.setFanMinRPM(150);
    Serial.print("FAN minimum RPM:"); Serial.println(emc2101.getFanMinRPM());

}

void loop() {
  // display.clearDisplay();
  // display.setCursor(0,0);

  Serial.print("External Temperature: ");Serial.print(emc2101.getExternalTemperature());Serial.println(" degrees C");

  Serial.print("Internal Temperature: ");Serial.print(emc2101.getInternalTemperature());Serial.println(" degrees C");
  Serial.print("Fan RPM:: ");Serial.print(emc2101.getFanRPM());Serial.println(" RPM");
  Serial.print("LUT Enabled: "); Serial.println(emc2101.LUTEnabled());
  Serial.println("");
  delay(100);


  // display.print("Ext Temp: ");display.print(emc2101.getExternalTemperature());display.println(" deg C");
  // display.print("Int Temp: ");display.print(emc2101.getInternalTemperature());display.println(" deg C");
  // display.print("Fan RPM: ");display.print(emc2101.getFanRPM());display.println(" RPM");
  // display.print("Pot: "); display.print(sliderPercentage());
  // display.print("PWM: "); display.print(emc2101.getDutyCycle());display.println("");
  delay(100);
  for(int i = 0; i<10; i++){
    Serial.print("Setting pwm to "); Serial.println(i+5);
    emc2101.setDutyCycle(i+5);
    delay(3000);
    Serial.print("Fan RPM:: ");
    Serial.print(emc2101.getFanRPM());
    Serial.println(" RPM");
    Serial.print("Fan RPM:: ");Serial.print(emc2101.getDutyCycle());
    Serial.println(" %");
    delay(2000);
  }


  // display.display();
  delay(100);



}
/*
LUT Enabled: 0

Setting pwm to 5
Fan RPM:: Fan speed raw count: 34547
156 RPM
Fan RPM:: 4 %
Setting pwm to 6
Fan RPM:: Fan speed raw count: 31111
173 RPM
Fan RPM:: 4 %
Setting pwm to 7
Fan RPM:: Fan speed raw count: 34403
156 RPM
Fan RPM:: 6 %
Setting pwm to 8
Fan RPM:: Fan speed raw count: 34351
157 RPM
Fan RPM:: 7 %
Setting pwm to 9
Fan RPM:: Fan speed raw count: 31015
174 RPM
Fan RPM:: 7 %
Setting pwm to 10
Fan RPM:: Fan speed raw count: 31231
172 RPM
Fan RPM:: 9 %
Setting pwm to 11
Fan RPM:: Fan speed raw count: 25323
213 RPM
Fan RPM:: 11 %
Setting pwm to 12
Fan RPM:: Fan speed raw count: 21215
254 RPM
Fan RPM:: 11 %
Setting pwm to 13
Fan RPM:: Fan speed raw count: 21047
256 RPM
Fan RPM:: 12 %
Setting pwm to 14
Fan RPM:: Fan speed raw count: 18015
299 RPM
Fan RPM:: 12 %
External Temperature: 23.00 degrees C
Internal Temperature: 22 degrees C
Fan RPM:: Fan speed raw count: 17979
300 RPM
LUT Enabled: 0
*/