#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

/*
  Using SPI
  Pin  Connected to
  GND      GND
  VDD      3.3V
  SCK/CLK  GPIO18
  SDA/MOSI GPIO23
  RES      GPIO17
  DC       GPIO22
  CS       GPIO5

*/

// Display declaration
U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2 (U8G2_R0, 5, 22, 17);

typedef struct {
  int xpos = 0;
  int ypos = 0;
  int Length = 5;
  int Height = 3;
} meteortp1;

/*
 * Collision detection
 * 
 * if(meteor.ypos < 64){
 *  if(
 *  (meteor.xpos + meteor.length) > ship.xpos && 
 *  meteor.xpos < (ship.xpos + ship.length) &&
 *  (meteor.ypos + meteor.height) > ship.ypos &&
 *  meteor.ypos < (ship.ypos + ship.height)
 *  ){
 *    hit = true;
 *  }
 * }
 * 
 */

void setup() {
  Serial.begin(115200);

  // Starts display without powersave mode
  u8g2.begin();
  u8g2.setPowerSave(0);

  // Set font for when writing
  u8g2.setFont(u8g2_font_ncenB14_tr);

  // Introduction text
  u8g2.clearBuffer();
  u8g2.drawStr(16, 39, "Asteroid");
  u8g2.sendBuffer();
  delay(3000);
  u8g2.clearDisplay();

}

void loop() {

}
