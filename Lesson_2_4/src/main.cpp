#include <Arduino.h>

// Allow overriding IO and timing from build_flags (-D) or other headers
#ifndef BUTTON_PIN
#define BUTTON_PIN 15
#endif

#ifndef POLL_DELAY
#define POLL_DELAY 10
#endif

#ifndef DEBOUNCE_DELAY
#define DEBOUNCE_DELAY 50
#endif

#if defined(USE_FSM)

typedef enum
{
  BUTTON_STATE_IDLE,
  BUTTON_STATE_DEBOUNCE_PRESS,
  BUTTON_STATE_PRESSED,
  BUTTON_STATE_DEBOUNCE_RELEASE
} ButtonState_t;

ButtonState_t buttonState = BUTTON_STATE_IDLE;

uint32_t counter_fsm = 0;
uint32_t lastPollTime = 0;
uint32_t stateChangeTime = 0;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT);
}

void loop() {
  uint32_t currentMillis = millis();

  if (currentMillis - lastPollTime < POLL_DELAY) {
    return;
  }

  lastPollTime = currentMillis;

  bool pressed = digitalRead(BUTTON_PIN) == LOW;

  switch (buttonState) {
    case BUTTON_STATE_IDLE:
      if (pressed) {
        buttonState = BUTTON_STATE_DEBOUNCE_PRESS;
        stateChangeTime = currentMillis;
      }
      break;

    case BUTTON_STATE_DEBOUNCE_PRESS:
      if (!pressed) {
        buttonState = BUTTON_STATE_IDLE;
      } else if (currentMillis - stateChangeTime >= DEBOUNCE_DELAY) {
        buttonState = BUTTON_STATE_PRESSED;
        counter_fsm++;
        Serial.print("Button pressed! Counter: ");
        Serial.println(counter_fsm);
      }
      break;

    case BUTTON_STATE_PRESSED:
      if (!pressed) {
        buttonState = BUTTON_STATE_DEBOUNCE_RELEASE;
        stateChangeTime = currentMillis;
      }
      break;

    case BUTTON_STATE_DEBOUNCE_RELEASE:
      if (pressed) {
        buttonState = BUTTON_STATE_PRESSED;
      } else if (currentMillis - stateChangeTime >= DEBOUNCE_DELAY) {
        buttonState = BUTTON_STATE_IDLE;
      }
      break;

    default:
      buttonState = BUTTON_STATE_IDLE;
      break;
  }
}

#elif defined(USE_ISR_SIMPLE)

volatile int counter_simple;
volatile bool buttonPressed_simple = false;

void IRAM_ATTR buttonISR_simple() {
  counter_simple++;
  buttonPressed_simple = true;
}

void setup() {
  Serial.begin(115200);
  delay(250);
  pinMode(BUTTON_PIN, INPUT);
  counter_simple = 0;

  attachInterrupt(BUTTON_PIN, buttonISR_simple, FALLING);
}

void loop() {
  if (buttonPressed_simple) {
    Serial.print("Button pressed! Counter: ");
    Serial.println(counter_simple);
    buttonPressed_simple = false;
  }
}

#elif defined(USE_ISR_DEBOUNCE)

volatile int counter_deb;
int counterWithDebounce_deb;
volatile bool buttonPressed_deb = false;
unsigned long lastInterruptTime_deb = 0;

void IRAM_ATTR buttonISR_deb() {
  counter_deb++;
  buttonPressed_deb = true;
}

void setup() {
  Serial.begin(115200);
  delay(250);
  pinMode(BUTTON_PIN, INPUT);
  counter_deb = 0;
  counterWithDebounce_deb = 0;

  attachInterrupt(BUTTON_PIN, buttonISR_deb, FALLING);
}

void loop() {
  if (buttonPressed_deb) {
    buttonPressed_deb = false;

    unsigned long currentMillis = millis();

    if (currentMillis - lastInterruptTime_deb >= DEBOUNCE_DELAY) {
      counterWithDebounce_deb++;
      lastInterruptTime_deb = currentMillis;
      Serial.print("Button pressed! Counter: ");
      Serial.println(counterWithDebounce_deb);
    }
  }
}

#else

#error "No implementation selected. Define USE_FSM, USE_ISR_SIMPLE or USE_ISR_DEBOUNCE."

#endif
