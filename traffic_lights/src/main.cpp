#include <Arduino.h>

constexpr uint8_t GREEN_LED_PIN = 17;
constexpr uint8_t YELLOW_LED_PIN = 16;
constexpr uint8_t RED_LED_PIN = 15;

typedef enum {
  BLINK,
  ON,
  OFF
} LedMode;

typedef struct {
  uint32_t blinkInterval;
  uint32_t onInterval;
  LedMode mode;
} t_Led_Params;

typedef struct {
  uint8_t pin;
  t_Led_Params params;
} t_Led;

typedef enum {
  START,
  STOP_SOON,
  READY_STOP,
  STOP,
  READY_START,
} TrafficLightPhase;

t_Led_Params greenLedOn = {
  .blinkInterval = 0,
  .onInterval = 5000,
  .mode = ON
};
t_Led_Params greenLedBlink = {
  .blinkInterval = 500,
  .onInterval = 3000,
  .mode = BLINK
};
t_Led_Params yellowLedOn = {
  .blinkInterval = 0,
  .onInterval = 2000,
  .mode = ON
};
t_Led_Params redLedOn = {
  .blinkInterval = 0,
  .onInterval = 5000,
  .mode = ON
};
t_Led_Params redLedReady = {
  .blinkInterval = 0,
  .onInterval = 2000,
  .mode = ON
};
t_Led_Params yellowLedReady = {
  .blinkInterval = 0,
  .onInterval = 2000,
  .mode = ON
};
t_Led_Params ledOff = {
  .blinkInterval = 0,
  .onInterval = 0,
  .mode = OFF
};

t_Led greenLed = {
  .pin = GREEN_LED_PIN,
  .params = greenLedOn
};
t_Led yellowLed = {
  .pin = YELLOW_LED_PIN,
  .params = ledOff
};
t_Led redLed = {
  .pin = RED_LED_PIN,
  .params = ledOff
};

volatile bool phaseChanged = false;
volatile bool blinkState = HIGH;
volatile bool blinkStateChanged = false;
volatile TrafficLightPhase phase = TrafficLightPhase::START;

hw_timer_t *timerPhase = NULL;
hw_timer_t *timerBlink = NULL;

void IRAM_ATTR onTimerAfter() {
  phaseChanged = true;

  switch (phase) {
    case START:
      phase = TrafficLightPhase::STOP_SOON;

      break;
    case STOP_SOON:
      phase = TrafficLightPhase::READY_STOP;

      break;
    case READY_STOP:
      phase = TrafficLightPhase::STOP;

      break;
    case STOP:
      phase = TrafficLightPhase::READY_START;

      break;
    case READY_START:
      phase = TrafficLightPhase::START;

      break;

  }

  timerWrite(timerPhase, 0);
}

void IRAM_ATTR onTimerBlink() {
  blinkState = !blinkState;
  blinkStateChanged = true;

  timerWrite(timerBlink, 0);
}

void updateLedState(t_Led &led) {
  if (led.params.mode == BLINK) {
    if (blinkStateChanged) {
      digitalWrite(led.pin, blinkState);
      blinkStateChanged = false;
    }
  } else if (led.params.mode == ON) {
    digitalWrite(led.pin, HIGH);
  } else {
    digitalWrite(led.pin, LOW);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);

  timerPhase = timerBegin(0, 80, true);
  timerBlink = timerBegin(1, 80, true);

  timerAttachInterrupt(timerPhase, &onTimerAfter, true);
  timerAttachInterrupt(timerBlink, &onTimerBlink, true);
  timerAlarmWrite(timerPhase, greenLed.params.onInterval * 1000, false);
  timerAlarmEnable(timerPhase);

  digitalWrite(greenLed.pin, HIGH);
  digitalWrite(yellowLed.pin, LOW);
  digitalWrite(redLed.pin, LOW);

}

void loop() {
  if (phaseChanged) {
    phaseChanged = false;

    switch (phase) {
      case START:
        greenLed.params = greenLedOn;
        yellowLed.params = ledOff;
        redLed.params = ledOff;

        timerAlarmWrite(timerPhase, greenLed.params.onInterval * 1000, false);
        timerAlarmEnable(timerPhase);

        break;
      case STOP_SOON:
        greenLed.params = greenLedBlink;
        yellowLed.params = ledOff;
        redLed.params = ledOff;

        timerAlarmWrite(timerPhase, greenLed.params.onInterval * 1000, false);
        timerAlarmWrite(timerBlink, greenLed.params.blinkInterval * 1000, true);
        timerAlarmEnable(timerPhase);
        timerAlarmEnable(timerBlink);

        break;
      case READY_STOP:
        greenLed.params = ledOff;
        yellowLed.params = yellowLedOn;
        redLed.params = ledOff;

        timerAlarmDisable(timerBlink);

        timerAlarmWrite(timerPhase, yellowLed.params.onInterval * 1000, false);
        timerAlarmEnable(timerPhase);

        break;
      case STOP:
        greenLed.params = ledOff;
        yellowLed.params = ledOff;
        redLed.params = redLedOn;

        timerAlarmWrite(timerPhase, redLed.params.onInterval * 1000, false);
        timerAlarmEnable(timerPhase);

        break;
      case READY_START:
        greenLed.params = ledOff;
        yellowLed.params = yellowLedReady;
        redLed.params = redLedReady;

        timerAlarmWrite(timerPhase, yellowLed.params.onInterval * 1000, false);
        timerAlarmEnable(timerPhase);

        break;

    }
  }
  updateLedState(greenLed);
  updateLedState(yellowLed);
  updateLedState(redLed);
}
