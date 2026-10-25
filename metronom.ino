#include <LiquidCrystal.h>

#define POT A0
#define BUTTONS_STATE A1
#define LED_1 2
#define LED_2 3
#define BUZZER 8

const unsigned long LCD_UPDATE_MS = 50;
const unsigned long LED_FLASH_MS = 50;
const unsigned long TONE_DURATION_MS = 45;
const unsigned long DEBOUNCE_MS = 200;

const int BUTTONS_THRESHOLD_HIGH = 1023;
const int BUTTONS_THRESHOLD_MID = 505;
const int BUTTONS_THRESHOLD_RANGE = 15;

LiquidCrystal lcd(12, 11, 5, 4, 9, 7);

int previousInterval = 0;
int interval = 0;
int potVal = 0;
int bpm = 0;
unsigned long elapsedLcd = 0;
unsigned long elapsedInterval = 0;
unsigned long elapsedTick = 0;
unsigned long lastButtonChange = 0;

bool withSound = false;
bool withTick = false;
int lastButtonPressed = -1;

int maxCount = 4;
int count = 0;

byte play[8] = {
  B10000,
  B11000,
  B11100,
  B11110,
  B11110,
  B11100,
  B11000,
  B10000
};

byte stopped[8] = {
  B00000,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B00000
};

byte soundOn[8] = {
  B00000,
  B00010,
  B00110,
  B11010,
  B11010,
  B00110,
  B00010,
  B00000
};


byte soundOff[8] = {
  B10001,
  B01010,
  B00110,
  B11010,
  B11010,
  B00110,
  B01010,
  B10001
};



void setup() {
  lcd.begin(16, 2);
  lcd.createChar(0, play);
  lcd.createChar(1, stopped);
  lcd.createChar(2, soundOn);
  lcd.createChar(3, soundOff);
  lcd.noCursor();
  pinMode(POT, INPUT);
  pinMode(BUTTONS_STATE, INPUT_PULLUP);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(BUZZER, OUTPUT);
}

void showBPM() {
  
  if(previousInterval != interval) {    
    previousInterval = interval;
    lcd.setCursor(0,0);
    lcd.print("BPM ");
    if(bpm < 100) {
      lcd.print(" ");
    }
    lcd.print(bpm);
  }
  
}

void showStatus() {
  
  if (withTick) {
    lcd.setCursor(15,0);
    lcd.write(byte(0));
  } else {
    lcd.setCursor(15,0);
    lcd.write(byte(1));
  }
  
  if (withSound) {
    lcd.setCursor(14,0);
    lcd.write(byte(2));
  } else {
    lcd.setCursor(14,0);
    lcd.write(byte(3));
  }
}

void showCount() {
    lcd.setCursor(0,1);
    lcd.print("Count ");
    lcd.print(maxCount);
}

void updateLCD() {
  if (millis() - elapsedLcd >= LCD_UPDATE_MS) {
    showBPM();
    showStatus();
    showCount();
    elapsedLcd = millis();
  }
  
}

void handleSwitchCount() {
  maxCount = maxCount == 8 ? 1 : maxCount + 1;
}

void handleSwitchSound() {
  withSound = !withSound;
}

void handleSwitchTick() {

  withTick = !withTick;

  if (!withTick) {
    count = 0;
    
    digitalWrite(LED_1, LOW);
    digitalWrite(LED_2, LOW);
  }
}

void handleStateButtons() {
  if (millis() - lastButtonChange < DEBOUNCE_MS) {
    return;
  }
  
  int buttonState = analogRead(BUTTONS_STATE);
  int buttonPressed = -1;
  
  if (buttonState < BUTTONS_THRESHOLD_MID) {
    buttonPressed = 2;
  } else if (buttonState < BUTTONS_THRESHOLD_MID + BUTTONS_THRESHOLD_RANGE) {
    buttonPressed = 1;
  } else if (buttonState >= BUTTONS_THRESHOLD_HIGH - BUTTONS_THRESHOLD_RANGE) {
    buttonPressed = 0;
  }
  
  if (buttonPressed == lastButtonPressed) {
    return;
  }
  
  if (buttonPressed == 0) {
    handleSwitchTick();
    lastButtonChange = millis();
  } else if (buttonPressed == 1) {
    handleSwitchSound();
    lastButtonChange = millis();
  } else if (buttonPressed == 2) {
    handleSwitchCount();
    lastButtonChange = millis();
  }
  lastButtonPressed = buttonPressed;
  
}

void handleBpm() {
  potVal = analogRead(POT);
  bpm = map(potVal, 0, 1023, 40, 240);
  interval = (60L * 1000 / bpm);
}

void loop() {
  handleStateButtons();
  handleBpm();
  updateLCD();
  if (!withTick) {
    return;
  }
  if (millis() - elapsedInterval >= interval) {
    elapsedInterval = millis();
    elapsedTick = elapsedInterval;
    if (count == 0) {
      digitalWrite(LED_1, HIGH);
      if(withSound) {
        tone(BUZZER, 440, TONE_DURATION_MS);
      }
    } else {
      digitalWrite(LED_2, HIGH);
      if(withSound) {
        tone(BUZZER, 220, TONE_DURATION_MS);
      }
    }
    count++;
    if (count >= maxCount) {
      count = 0;
    }
  }
  if (millis() - elapsedTick >= LED_FLASH_MS) {
    digitalWrite(LED_1, LOW);
    digitalWrite(LED_2, LOW);
  }
}
