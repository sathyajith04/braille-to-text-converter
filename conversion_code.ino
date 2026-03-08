#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int dotPins[6] = {2, 3, 4, 5, 6, 7};
const int submitPin = 8;
const int togglePin = 9;
const int clearPin = 10;
const int backspacePin = 11;
const int spacePin = 12;
const int buzzerPin = 13; // Buzzer pin

bool latchedDots[6] = {0};
bool isNumberMode = false;
bool lastToggleState = HIGH;
bool lastClearState = HIGH;
bool lastBackspaceState = HIGH;
bool lastSpaceState = HIGH;

int cursorPos = 0;

void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);

  for (int i = 0; i < 6; i++) pinMode(dotPins[i], INPUT);
  pinMode(submitPin, INPUT);

  pinMode(togglePin, INPUT_PULLUP);
  pinMode(clearPin, INPUT_PULLUP);
  pinMode(backspacePin, INPUT_PULLUP);
  pinMode(spacePin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);

  lcd.setCursor(0, 0);
  lcd.print("Ready...");
  showMode();
}

void loop() {
  checkToggle();
  checkClear();
  checkBackspace();
  checkSpace();

  for (int i = 0; i < 6; i++) {
    if (digitalRead(dotPins[i]) == HIGH) {
      latchedDots[i] = true;
    }
  }

  if (digitalRead(submitPin) == HIGH) {
    delay(200);
    int brailleValue = 0;
    for (int i = 0; i < 6; i++) {
      if (latchedDots[i]) {
        brailleValue |= (1 << i);
      }
    }

    char result = isNumberMode ? brailleToNumber(brailleValue) : brailleToChar(brailleValue);
    if (cursorPos < 16) {
      lcd.setCursor(cursorPos, 1);
      lcd.print(result);
      cursorPos++;
    }

    if (result == '?') {
      tone(buzzerPin, 300, 30); // beep for invalid input
    }

    for (int i = 0; i < 6; i++) latchedDots[i] = false;
    while (digitalRead(submitPin) == HIGH);
  }
}

void checkToggle() {
  bool currentToggleState = digitalRead(togglePin);
  if (currentToggleState == LOW && lastToggleState == HIGH) {
    isNumberMode = !isNumberMode;
    showMode();
    tone(buzzerPin, 300, 30); // soft beep same as backspace
    if (!isNumberMode) {
      delay(100);
      tone(buzzerPin, 300, 30); // second beep for alpha mode
    }
  }
  lastToggleState = currentToggleState;
}

void checkClear() {
  bool currentClearState = digitalRead(clearPin);
  if (currentClearState == LOW && lastClearState == HIGH) {
    lcd.clear();
    showMode();
    cursorPos = 0;
    delay(200);
  }
  lastClearState = currentClearState;
}

void checkBackspace() {
  bool currentBackspaceState = digitalRead(backspacePin);
  if (currentBackspaceState == LOW && lastBackspaceState == HIGH) {
    if (cursorPos > 0) {
      cursorPos--;
      lcd.setCursor(cursorPos, 1);
      lcd.print(" ");
      tone(buzzerPin, 300, 30); // quick soft backspace beep
    }
    delay(200);
  }
  lastBackspaceState = currentBackspaceState;
}

void checkSpace() {
  bool currentSpaceState = digitalRead(spacePin);
  if (currentSpaceState == LOW && lastSpaceState == HIGH) {
    if (cursorPos < 16) {
      lcd.setCursor(cursorPos, 1);
      lcd.print(" ");
      cursorPos++;
    }
    delay(200);
  }
  lastSpaceState = currentSpaceState;
}

void showMode() {
  lcd.setCursor(0, 0);
  lcd.print("Mode: ");
  lcd.print(isNumberMode ? "Numbers  " : "Alphabets");
  lcd.setCursor(cursorPos, 1);
}

char brailleToChar(int value) {
  switch (value) {
    case 0b000001: return 'a';
    case 0b000011: return 'b';
    case 0b001001: return 'c';
    case 0b011001: return 'd';
    case 0b010001: return 'e';
    case 0b001011: return 'f';
    case 0b011011: return 'g';
    case 0b010011: return 'h';
    case 0b001010: return 'i';
    case 0b011010: return 'j';
    case 0b000101: return 'k';
    case 0b000111: return 'l';
    case 0b001101: return 'm';
    case 0b011101: return 'n';
    case 0b010101: return 'o';
    case 0b001111: return 'p';
    case 0b011111: return 'q';
    case 0b010111: return 'r';
    case 0b001110: return 's';
    case 0b011110: return 't';
    case 0b100101: return 'u';
    case 0b100111: return 'v';
    case 0b111010: return 'w';
    case 0b101101: return 'x';
    case 0b111101: return 'y';
    case 0b110101: return 'z';
    default: return '?';
  }
}

char brailleToNumber(int value) {
  switch (value) {
    case 0b000001: return '1';
    case 0b000011: return '2';
    case 0b001001: return '3';
    case 0b011001: return '4';
    case 0b010001: return '5';
    case 0b001011: return '6';
    case 0b011011: return '7';
    case 0b010011: return '8';
    case 0b001010: return '9';
    case 0b011010: return '0';
    default: return '?';
  }
}
