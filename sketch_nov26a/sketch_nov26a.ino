// Building structure for first meteor size
typedef struct {
  bool isActive = false;
  int xpos = 0;
  int ypos = 0;
  int lgt = 5; // Length
  int hgt = 3; // Height
} meteortp1;

// Create meteor array
const int meteorArrSize = 8;
meteortp1 meteorArr[meteorArrSize];

void ChangeActive(meteortp1 *meteorArrPtr, int meteorArrPos) {
  if ((meteorArrPtr + meteorArrPos) -> isActive != true) {
    (meteorArrPtr + meteorArrPos) -> isActive = true;
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println("Size of array: " + String((sizeof(meteorArr) / sizeof(meteortp1))));

  for (int i = 0; i < (sizeof(meteorArr) / sizeof(meteortp1)); i++) {
    Serial.println("Meteor " + String(i) + " is " + String(meteorArr[i].isActive));
  }

  for (int i = 0; i < (sizeof(meteorArr) / sizeof(meteortp1)); i++) {
    ChangeActive(meteorArr, i);

    Serial.println("Meteor " + String(i) + " is " + String(meteorArr[i].isActive));
  }

}

void loop() {
  // put your main code here, to run repeatedly:

}
