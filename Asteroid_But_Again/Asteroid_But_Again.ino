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
  int lgt = 5; // Length
  int hgt = 3; // Height
  int xpos = 0;
  int ypos = 0;
} meteortp1;

// Building structure for ship
typedef struct {
  int lgt = 4; // Length
  int hgt = 2; // Height
  int xpos = 0;
  int ypos = 63 - hgt;
} ship;

// Create meteor array
const int meteorArrSize = 8;
meteortp1 meteorArr[meteorArrSize];

// Create ship
ship player;

// Define longs for the timer
long previousMillis = 0;
long interval = 100;

void SpawnMeteor(meteortp1 *meteorArrPtr, int meteorArrPos) {
  float randomSpawnSpot = random(0, 127 - meteorArrPtr -> lgt); // random always returns a float
  round(randomSpawnSpot); // as random returns a float, it is rounded to have a integer position

  // The meteor's isActive is set to true, and the x position is set to the random position
  (meteorArrPtr + meteorArrPos) -> isActive = true;
  (meteorArrPtr + meteorArrPos) -> xpos = randomSpawnSpot;
}

void MoveMeteors(meteortp1 *meteorArrPtr, int meteorArrPos) {
  // Edge cases first. First meteor
  if (meteorArrPos == 0) {
    if ((meteorArrPtr + meteorArrPos) -> isActive != true) {
      SpawnMeteor(meteorArrPtr, meteorArrPos);
    }
    if ((meteorArrPtr + meteorArrPos) -> isActive) {
      (meteorArrPtr + meteorArrPos) -> ypos += 1;
    }
  }

  // Edge cases first. Last meteor
  else if (meteorArrPos == (meteorArrSize - 1)) {
    if ((meteorArrPtr + meteorArrPos) -> isActive != true) {
      if ((meteorArrPtr + meteorArrPos - 1) -> ypos >= 3 + (meteorArrPtr -> lgt)) {
        SpawnMeteor(meteorArrPtr, meteorArrPos);
      }
    }
    if ((meteorArrPtr + meteorArrPos) -> isActive) {
      (meteorArrPtr + meteorArrPos) -> ypos += 1;
    }
  }

  // The rest of the meteors
  else {
    if ((meteorArrPtr + meteorArrPos) -> isActive != true) {
      if ((meteorArrPtr + meteorArrPos - 1) -> ypos >= 3 + (meteorArrPtr -> lgt)) {
        SpawnMeteor(meteorArrPtr, meteorArrPos);
      }
    }
    if ((meteorArrPtr + meteorArrPos) -> isActive) {
      (meteorArrPtr + meteorArrPos) -> ypos += 1;
    }
  }

  if ((meteorArrPtr + meteorArrPos) -> ypos >= 64) {
    (meteorArrPtr + meteorArrPos) -> isActive = false;
    (meteorArrPtr + meteorArrPos) -> ypos = 0;
  }

}

/*
   Function: DrawObjects
   Input: Nothing(?)
   Output: Nothing
   Remarks: Draws frames of the objects at this point in time
*/
void DrawObjects(ship *player, meteortp1 *meteorArrPtr) {
  u8g2.clearBuffer();

  // Player ship
  player -> xpos = (analogRead(POTENTIOPIN) * analogWidthConvert);
  u8g2.drawFrame(player -> xpos, player -> ypos, player -> lgt, player -> hgt);

  // Meteors
  for (int i = 0; i < (sizeof(meteorArr) / sizeof(meteortp1)); i++) {
    MoveMeteors(meteorArr, i);
    if ((meteorArrPtr + i) -> isActive) {
      u8g2.drawFrame((meteorArrPtr + i) -> xpos, (meteorArrPtr + i) -> ypos, (meteorArrPtr + i) -> lgt, (meteorArrPtr + i) -> hgt);
    }
  }

  u8g2.sendBuffer();
}

/*
   Function: CheckCollision
   Input: The meteor that collision is being checked on
   Output: Whether or not the ship has collided
   Remarks: First checks whether the meteor is off-screen, then does collision check
*/
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

  // Seed for random function
  randomSeed(analogRead(6));

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

  // Boolean for determining whether a hit was done
  bool isHit = false;

  // Starts processes if the time reaches the interval
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis; // Sets previousMillis to be currentMillis to compare next run

    // Drawing all objects on screen
    DrawObjects(&(player), meteorArr);

    // Collision check
    for (int i = 0; i < (sizeof(meteorArr) / sizeof(meteortp1)); i++) {
      isHit = CheckCollision(&(meteorArr[i]));
      if (isHit) { // There is no reason to continue checking if the player is hit
        break;
      }
    }

    if (isHit) {
      GameOver();
    }
  }
}
