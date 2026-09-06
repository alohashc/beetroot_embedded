#include <Arduino.h>


constexpr uint8_t LED1_PIN = 15;
constexpr uint8_t LED2_PIN = 16;
constexpr uint8_t LED3_PIN = 18;

constexpr uint32_t LED1_INTERVAL = 200;
constexpr uint32_t LED2_INTERVAL = 500;
constexpr uint32_t LED3_INTERVAL = 1000;


typedef struct {
  uint8_t pin;
  uint32_t interval;
  unsigned long previousMillis;
  bool state;
} t_led;



t_led led1 = {LED1_PIN, LED1_INTERVAL, 0, LOW};
t_led led2 = {LED2_PIN, LED2_INTERVAL, 0, LOW};
t_led led3 = {LED3_PIN, LED3_INTERVAL, 0, LOW};

void toggleLed(t_led *led) {
  unsigned long currentMillis = millis();
  if (currentMillis - led->previousMillis >= led->interval) {
    led->previousMillis = currentMillis;
    led->state = !led->state;
    digitalWrite(led->pin, led->state);
  }
}

void setup() {
  Serial.begin(115200);
  delay(250); // Wait for Serial to initialize

  pinMode(led1.pin, OUTPUT);
  pinMode(led2.pin, OUTPUT);
  pinMode(led3.pin, OUTPUT);
  
}

void loop() {
  toggleLed(&led1);
  toggleLed(&led2);
  toggleLed(&led3);
}
