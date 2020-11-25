#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define potentioPin 36 // Pin defined for easier reading

/*
    Conversion multiplier for width of display and analog input
    The fraction 123/4095 correlates to the maximum
*/
const float analogWidthConvert = 0.030037;

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

// Building structure for first meteor size
typedef struct {
  int xpos = 0;
  int ypos = 0;
  int Length = 5;
  int Height = 3;
} meteortp1;

// Building structure for ship
typedef struct {
  int xpos = 0;
  int ypos = 0;
  int Length = 4;
  int Height = 2;
} ship;

// Create meteor array
meteortp1 meteorArr[8];

// Create ship
ship player;

// Define integer for potentiometer input
int potentioRawInput;
int potentioInput;

// Define longs for the timer
long previousMillis = 0;
long interval = 100;

// Boolean for determining whether a hit was done
bool isHit = false;


/*
   Function: DrawObjects
   Input: Nothing(?)
   Output: Nothing
   Remarks: Draws frames of the objects at this point in time
*/
void DrawObjects(int potentioInput) {
  u8g2.clearBuffer();

  // Player ship
  u8g2.drawFrame(potentioInput, 63 - player.Height, player.Length, player.Height);

  // Meteors


  u8g2.sendBuffer();
}

/*
 * Function: CheckCollision
 * Input: The meteor that collision is being checked on
 * Output: Whether or not the ship has collided
 * Remarks: First checks whether the meteor is off-screen, then does collision check
 */
bool CheckCollision(meteortp1 meteor) {
  if (meteor.ypos < 64) {
    if (
      (meteor.xpos + meteor.Length) > player.xpos &&
      meteor.xpos < (player.xpos + player.Length) &&
      (meteor.ypos + meteor.Height) > player.ypos &&
      meteor.ypos < (player.ypos + player.Height)
    ) {
      return true;
    }
  }
  return false;
}

/*
 * Function: GameOver
 * Input: None
 * Output: None
 * Remarks: Triggered when isHit = true. Will loop until the ESP is reset
 */
void GameOver(){
  // Display Game over
  u8g2.clearBuffer();
  u8g2.drawStr(16, 39, "Game over");
  u8g2.sendBuffer();

  exit(0); // Forces the computer to stop looping
}

void setup() {
  Serial.begin(115200);

  // Potentiometer pin declared
  pinMode(potentioPin, INPUT);

  // Starts display without powersave mode
  u8g2.begin();
  u8g2.setPowerSave(0);

  // Set font for writing
  u8g2.setFont(u8g2_font_ncenB14_tr);

  // Introduction text
  u8g2.clearBuffer();
  u8g2.drawStr(16, 39, "Poggers");
  u8g2.sendBuffer();
  delay(3000);
  u8g2.clearDisplay();

}

void loop() {
  // Current millis based on time since Arduino turned on
  unsigned long currentMillis = millis();

  // Starts processes if the time reaches the interval
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis; // Sets previousMillis to be currentMillis to compare next run

    // Troubleshooting code
    int potentioRawInput = analogRead(potentioPin);
    int potentioInput = (analogRead(potentioPin) * analogWidthConvert);

    Serial.println("Raw player input: " + String(potentioRawInput));
    Serial.println("Calculated player input: " + String(potentioInput));

    // Drawing all objects on screen
    DrawObjects(potentioInput);

    // Collision check
    for (int i = 0; i < sizeof(meteorArr); i++) {
      bool isHit = CheckCollision(meteorArr[i]);
      if(isHit){ // There is no reason to continue checking if the player is hit
        i = 8;
      }
    }

    if (isHit) {
      // Game over code here
    }
  }
}
