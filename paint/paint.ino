#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define VRX_PIN 32
#define VRY_PIN  33
#define SW 18

#define SW_ROT 23
#define DT 4
#define CLK 19

#define LEFT_THRESHOLD_LARGE 50
#define LEFT_THRESHOLD_SMALL 1000
#define RIGHT_THRESHOLD_LARGE 4000
#define RIGHT_THRESHOLD_SMALL 3000
#define UP_THRESHOLD_LARGE 50
#define UP_THRESHOLD_SMALL 1000
#define DOWN_THRESHOLD_LARGE 4000
#define DOWN_THRESHOLD_SMALL 3000

#define COMMAND_NO 0x00
#define COMMAND_LEFT_LARGE 0x01
#define COMMAND_RIGHT_LARGE 0x02
#define COMMAND_UP_LARGE 0x04
#define COMMAND_DOWN_LARGE 0x08
#define COMMAND_LEFT_SMALL 0x10
#define COMMAND_RIGHT_SMALL 0x20
#define COMMAND_UP_SMALL 0x40
#define COMMAND_DOWN_SMALL 0x80

bool canvas[SCREEN_WIDTH][SCREEN_HEIGHT] = {false};

int valueX = 0;
int valueY = 0;
int command = COMMAND_NO;

int posX = 59;
int posY = 32;

bool isPainting = false; 

// Button states and non-blocking timers
bool lastButtonState = HIGH;
bool lastButtonStateRot = HIGH;
unsigned long lastDebounceTimeSW = 0;
unsigned long lastDebounceTimeRot = 0;
const unsigned long debounceDelay = 50; 

volatile int size = 3;

// Hardware Interrupt Routine for the Rotary Encoder
void IRAM_ATTR handleRotaryISR() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  
  if (interruptTime - lastInterruptTime > 10) {
    if (digitalRead(DT) == HIGH) {
      if (size < 10) size++;
    } else {
      if (size > 1) size--;
    }
    lastInterruptTime = interruptTime;
  }
}

void setup() {
  Serial.begin(115200);
  analogSetAttenuation(ADC_11db);
  Wire.begin(21, 22);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  pinMode(SW, INPUT_PULLUP);
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DT, INPUT_PULLUP);
  pinMode(SW_ROT, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(CLK), handleRotaryISR, FALLING);

  display.clearDisplay();     
  display.fillRect(posX-1, posY-1, 3, 3, INVERSE);
  display.display();
}

void loop() {
  unsigned long currentMillis = millis();
  
  valueX = analogRead(VRX_PIN);
  valueY = analogRead(VRY_PIN);
  command = COMMAND_NO;

  bool currentButtonState = digitalRead(SW);
  if (currentButtonState != lastButtonState) {
    if (currentMillis - lastDebounceTimeSW > debounceDelay) {
      if (currentButtonState == LOW) {
        isPainting = !isPainting;
        Serial.print("Brush toggled: ");
        Serial.println(isPainting ? "ON" : "OFF");
      }
      lastDebounceTimeSW = currentMillis;
    }
  }
  lastButtonState = currentButtonState;

  // Joystick Control
  if (valueX < LEFT_THRESHOLD_SMALL)
  {
    if (valueX < LEFT_THRESHOLD_LARGE)
    {
      command |= COMMAND_LEFT_LARGE;
    }
    else
      command |= COMMAND_LEFT_SMALL;
  }
  else if (valueX > RIGHT_THRESHOLD_SMALL) 
  {
    if (valueX > RIGHT_THRESHOLD_LARGE) 
    {
      command |= COMMAND_RIGHT_LARGE;
    }
    else
      command |= COMMAND_RIGHT_SMALL;
  }

  if (valueY < UP_THRESHOLD_SMALL) 
  { 
    if (valueY < UP_THRESHOLD_LARGE) 
    { 
      command |= COMMAND_UP_LARGE;
    }
    else
      command |= COMMAND_UP_SMALL;
  }
  else if (valueY > DOWN_THRESHOLD_SMALL) 
  {
    if (valueY > DOWN_THRESHOLD_LARGE) 
    {
      command |= COMMAND_DOWN_LARGE;
    }
    else
      command |= COMMAND_DOWN_SMALL;
  }

  if (command & COMMAND_LEFT_LARGE) posX -= 2;
  if (command & COMMAND_RIGHT_LARGE) posX += 2;
  if (command & COMMAND_UP_LARGE) posY -= 2;
  if (command & COMMAND_DOWN_LARGE) posY += 2;
  if (command & COMMAND_LEFT_SMALL) posX -= 1;
  if (command & COMMAND_RIGHT_SMALL) posX += 1;
  if (command & COMMAND_UP_SMALL) posY -= 1;
  if (command & COMMAND_DOWN_SMALL) posY += 1;

  if (posX < 0) posX = 0;
  if (posX > SCREEN_WIDTH - size) posX = SCREEN_WIDTH - size;
  if (posY < 0) posY = 0;
  if (posY > SCREEN_HEIGHT - size) posY = SCREEN_HEIGHT - size;

  // Painting Logic
  if (isPainting) {
    for (int dx = 0; dx < size; dx++) {
      for (int dy = 0; dy < size; dy++) {
        canvas[posX + dx][posY + dy] = true;
      }
    }
  }

  // Non-blocking Rotary Button (SW_ROT) Debounce for Screen Clear
  bool currentButtonStateRot = digitalRead(SW_ROT);
  if (currentButtonStateRot != lastButtonStateRot) {
    if (currentMillis - lastDebounceTimeRot > debounceDelay) {
      if (currentButtonStateRot == LOW) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
          for (int y = 0; y < SCREEN_HEIGHT; y++) {
            canvas[x][y] = false;
          }
        }
      }
      lastDebounceTimeRot = currentMillis;
    }
  }
  lastButtonStateRot = currentButtonStateRot;

  // Rendering
  display.clearDisplay();
  
  for (int x = 0; x < SCREEN_WIDTH; x++) {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
      if (canvas[x][y]) {
        display.drawPixel(x, y, WHITE);
      }
    }
  }
  
  if (isPainting) {
    display.fillRect(posX, posY, size, size, WHITE);
  } else {
    display.drawRect(posX, posY, size, size, WHITE);
  }

  display.display();
}