#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

hd44780_I2Cexp lcd;

const int LCD_COLS = 16;
const unsigned long INTERVAL =15000;  // 15 seconds in ms

const int DANCE_STEP_MS = 40;    // speed: lower = faster

// Each entry = {row 1, row 2}, max 16 characters per row
const char* messages[][2] = {
  {"Venture not all",  "in one boat."},
  {"Haste does not",   "bring success."},
  {"Be smart, but",    "never show it."},
  {"You are talented", "in many ways."},
  {"Do your best to",  "make it happen"},
  {"Wise man seldom",  "talks."},
  {"You have a pair",  "of shining eyes."},
  {"You are going to", "take a vacation."},
  {"Home is where",    "your heart is."},
  {"You have a heart", "of gold."},
  {"There is beauty",  "in simplicity."},
  {"Old friends make", "best friends."},
  {"Don't be afraid",  "of fear."},
  {"Peace begins",     "with a smile."},
  {"Every burden is",  "a blessing."},
  {"Prosperity is in", "your fortune."},
  {"You will travel",  "to many places."},
  {"Thirst is a",     "valid hobby."},
  {"Your ex is not",   "a personality."},
  {"Soft is also",     "a weapon."},
  {"Fortune favours",  "the unbothered."},
  {"You will enjoy a", "trip to Asia."},
  {"All generalities", "are false."},
  {"Two people",       "shorten a road."},
  {"Serious trouble",  "will bypass you."},
  {"A feeling is an",  "idea with roots."},
  {"You are not",      "illiterate."},
  {"You are a bright", "individual."},
  {"Enjoy yourself",   "while you can."}
};

const int NUM_MESSAGES = sizeof(messages) / sizeof(messages[0]);

int currentIndex = -1;           // -1 = nothing shown yet
unsigned long lastChange = 0;

// picks a random sentence, never the same one twice in a row
int pickRandomIndex() {
  int next;
  do {
    next = random(NUM_MESSAGES);
  } while (NUM_MESSAGES > 1 && next == currentIndex);
  return next;
}

// position of the n-th cell (0-31), snaking: row 0 left->right, row 1 right->left
void cellPosition(int n, int &col, int &row) {
  row = n / LCD_COLS;
  col = (row == 0) ? (n % LCD_COLS) : (LCD_COLS - 1 - n % LCD_COLS);
}

void danceSequence() {
  lcd.clear();
  int col, row;

  // light up one after another
  for (int n = 0; n < LCD_COLS * 2; n++) {
    cellPosition(n, col, row);
    lcd.setCursor(col, row);
    lcd.write(0xFF);             // filled block
    delay(DANCE_STEP_MS);
  }

  // switch off again in the same order
  for (int n = 0; n < LCD_COLS * 2; n++) {
    cellPosition(n, col, row);
    lcd.setCursor(col, row);
    lcd.write(' ');
    delay(DANCE_STEP_MS);
  }
}

void showMessage(int i) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(messages[i][0]);
  lcd.setCursor(0, 1);
  lcd.print(messages[i][1]);
}

void setup() {
  Serial.begin(9600);

  int status = lcd.begin(LCD_COLS, 2);

  Serial.print("LCD initialization status: ");
  Serial.println(status);

  randomSeed(analogRead(A0));    // unconnected pin gives a different seed on each start

  lcd.backlight();
  currentIndex = pickRandomIndex();
  showMessage(currentIndex);
  lastChange = millis();
}

void loop() {
  if (millis() - lastChange >= INTERVAL) {
    danceSequence();             // after every sentence
    currentIndex = pickRandomIndex();
    showMessage(currentIndex);
    lastChange = millis();       // the 15 s start after the sentence appears
  }
}
