#include <Arduino.h>
#include <esp_task_wdt.h>

constexpr uint8_t MOTOR_PIN = 15;
constexpr uint8_t OFF_PIN = 16;
constexpr uint8_t ON_PIN = 18;

constexpr uint32_t MOTOR_WAIT_TIME = 10000;
constexpr uint32_t MOTOR_WORK_TIME = 5000;

volatile bool motorState = false;
volatile bool stateChanged = false;

hw_timer_t *timerWait = NULL;
hw_timer_t *timerWork = NULL;

void IRAM_ATTR onTimerAfterWait() {
  // Safe check to avoid starting the motor and reset timer if it's already running
  if (motorState) {
    return;
  }

  motorState = true;
  stateChanged = true;

  digitalWrite(MOTOR_PIN, HIGH);
  
  timerWrite(timerWork, 0);
  timerAlarmEnable(timerWork);
}

void IRAM_ATTR timerAfterWork() {
  motorState = false;
  stateChanged = true;

  digitalWrite(MOTOR_PIN, LOW);
}

uint32_t getRemainingTime(hw_timer_t *timer) {
  if (timer == nullptr) {
    return 0;
  }
  double seconds = timerReadSeconds(timer);

  if (timer == timerWait) {
    return MOTOR_WAIT_TIME / 1000 - seconds;
  } else if (timer == timerWork) {
    return MOTOR_WORK_TIME / 1000 - seconds;
  }
  return 0;
}

void setup() {
  Serial.begin(115200);
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(OFF_PIN, OUTPUT);
  pinMode(ON_PIN, OUTPUT);

  digitalWrite(OFF_PIN, HIGH);

  timerWait = timerBegin(0, 80, true);
  timerWork = timerBegin(1, 80, true);

  timerAttachInterrupt(timerWait, &onTimerAfterWait, true);
  timerAttachInterrupt(timerWork, &timerAfterWork, true);
  timerAlarmWrite(timerWait, MOTOR_WAIT_TIME * 1000, true);
  timerAlarmWrite(timerWork, MOTOR_WORK_TIME * 1000, false);

  timerAlarmEnable(timerWait);

  esp_task_wdt_add(NULL);

}

void loop() {
  esp_task_wdt_reset();

  static unsigned long lastPrintTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastPrintTime >= 1000) {
    uint32_t remaining = 0;

    if (!motorState) {
      remaining = getRemainingTime(timerWait);

      Serial.print("Motor is OFF. Time until start: ");
      Serial.print(remaining);
      Serial.println(" seconds");
    } else {
      remaining = getRemainingTime(timerWork);

      Serial.print("Motor is ON. Time until stop: ");
      Serial.print(remaining);
      Serial.println(" seconds");
    }
    lastPrintTime = currentTime;
  }

  if (stateChanged) {
    stateChanged = false;

    if (motorState) {
      digitalWrite(ON_PIN, HIGH);
      digitalWrite(OFF_PIN, LOW);
      Serial.println("Motor is ON");
    } else {
      digitalWrite(ON_PIN, LOW);
      digitalWrite(OFF_PIN, HIGH);
      Serial.println("Motor is OFF");
    }
  }
}
