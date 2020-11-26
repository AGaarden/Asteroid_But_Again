#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define POTENTIOPIN 36 // Pin defined for easier reading

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
  bool isActive = false;
  int xpos = 0;
  int ypos = 0;
  int lgt = 5; // Length
  int hgt = 3; // Height
} meteortp1;

// Building structure for ship
typedef struct {
  int xpos = 0;
  int ypos = 0;
  int lgt = 4; // Length
  int hgt = 2; // Height
} ship;

// Create meteor array
const int meteorArrSize = 8;
meteortp1 meteorArr[meteorArrSize];

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



void MoveMeteors(meteortp1 *meteorArrPtr, int meteorArrPos) {
  // Edge cases first. First meteor
  if (meteorArrPos == 0) {
    if(meteorArrPtr -> isActive != true){
      
    }
  }

  // Edge cases first. Last meteor
  else if (meteorArrPos == (meteorArrSize - 1)) {

  }

  // The rest of the meteors
  else {

  }


  // How they should go
  // Check if meteor is active
  // If no, check if meteor can be spawned
  // If yes, spawn it
  // If no, move on
  // If meteor is active, move it

}

/*
   Function: DrawObjects
   Input: Nothing(?)
   Output: Nothing
   Remarks: Draws frames of the objects at this point in time
*/
void DrawObjects(int potentioInput) {
  u8g2.clearBuffer();

  // Player ship
  u8g2.drawFrame(potentioInput, 63 - player.hgt, player.lgt, player.hgt);

  // Meteors
  for (int i = 0; i < (sizeof(meteorArr) / sizeof(meteortp1)); i++) {
    MoveMeteors(meteorArr, i);
  }


  u8g2.sendBuffer();
}

/*
   Function: CheckCollision
   Input: The meteor that collision is being checked on
   Output: Whether or not the ship has collided
   Remarks: First checks whether the meteor is off-screen, then does collision check
*/
/*bool CheckCollision(meteortp1 meteor) {
  if (meteor.ypos < 64) {
    if (
      (meteor.ypos + meteor.hgt) > player.ypos &&
      meteor.ypos < (player.ypos + player.hgt) &&
      (meteor.xpos + meteor.lgt) > player.xpos &&
      meteor.xpos < (player.xpos + player.lgt)
    ) {
      return true;
    }
  }
  return false;
  }*/

bool CheckCollision(meteortp1 *meteorPtr) {
  if (meteorPtr -> ypos < 64) {
    if (
      (meteorPtr -> ypos + meteorPtr -> hgt) > player.ypos &&
      meteorPtr -> ypos < (player.ypos + player.hgt) &&
      (meteorPtr -> xpos + meteorPtr -> lgt) > player.xpos &&
      meteorPtr -> xpos < (player.xpos + player.lgt)
    ) {
      return true;
    }
  }
  return false;
}


/*
   Function: GameOver
   Input: None
   Output: None
   Remarks: Triggered when isHit = true. Will loop until the ESP is reset
*/
void GameOver(void) {
  // Display Game over
  u8g2.clearBuffer();
  u8g2.drawStr(16, 39, "Game over");
  u8g2.sendBuffer();

  exit(0); // Forces the microprocessor to stop looping
}

void setup(void) {
  Serial.begin(115200);

  // Potentiometer pin declared
  pinMode(POTENTIOPIN, INPUT);

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

void loop(void) {
  // Current millis based on time since Arduino turned on
  unsigned long currentMillis = millis();

  // Starts processes if the time reaches the interval
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis; // Sets previousMillis to be currentMillis to compare next run

    // Troubleshooting code
    int potentioRawInput = analogRead(POTENTIOPIN);
    int potentioInput = (analogRead(POTENTIOPIN) * analogWidthConvert);

    Serial.println("Raw player input: " + String(potentioRawInput));
    Serial.println("Calculated player input: " + String(potentioInput));

    // Drawing all objects on screen
    DrawObjects(potentioInput);

    // Collision check
    for (int i = 0; i < (sizeof(meteorArr) / sizeof(meteortp1)); i++) {
      bool isHit = CheckCollision(&(meteorArr[i]));
      //bool isHit = CheckCollision(meteorArr[i]);
      if (isHit) { // There is no reason to continue checking if the player is hit
        i = 8;
      }
    }

    if (isHit) {
      GameOver();
    }
  }
}
