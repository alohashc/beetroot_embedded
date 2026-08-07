#include <Arduino.h>

#ifdef CASE_POLICE_BLINK_SIMPLE
#define LED_RED_OUT 7
#define LED_BLUE_OUT 17

bool redLedState = false;
bool blueLedState = false;
#endif

#ifdef CASE_POLICE_BLINK_ONE_GPIO
#define LED_OUT 18

bool ledState = false;
#endif


void setup() {
    Serial.begin(115200);
    delay(1000);

#ifdef CASE_POLICE_BLINK_SIMPLE
    pinMode(LED_RED_OUT, OUTPUT);
    digitalWrite(LED_RED_OUT, LOW);
    pinMode(LED_BLUE_OUT, OUTPUT);
    digitalWrite(LED_BLUE_OUT, LOW);
#endif

#ifdef CASE_POLICE_BLINK_ONE_GPIO
    pinMode(LED_OUT, OUTPUT);
    digitalWrite(LED_OUT, LOW);
#endif

}

void loop() {

  #ifdef CASE_POLICE_BLINK_SIMPLE
    if (!redLedState && !blueLedState) {
        digitalWrite(LED_RED_OUT, HIGH);
        redLedState = true;
        Serial.println("\033[31m" "\u2588" "\033[0m" "\u2588" "RED ON");
        Serial.println("================================");
    } else if (redLedState && !blueLedState) {
        digitalWrite(LED_RED_OUT, LOW);
        redLedState = false;

        digitalWrite(LED_BLUE_OUT, HIGH);
        blueLedState = true;
        Serial.println("\033[0m" "\u2588" "\033[34m" "\u2588" "\033[0m" "BLUE ON");
        Serial.println("\033[0m""================================");
    } else if (!redLedState && blueLedState) {
        digitalWrite(LED_BLUE_OUT, LOW);
        blueLedState = false;

        digitalWrite(LED_RED_OUT, HIGH);
        redLedState = true;
        Serial.println("\033[31m" "\u2588" "\033[0m" "\u2588" "RED ON");
        Serial.println("================================");
    }
  #endif

  #ifdef CASE_POLICE_BLINK_ONE_GPIO
    if (!ledState) {
        digitalWrite(LED_OUT, HIGH);
        ledState = true;
        Serial.println("\033[34m" "\u2588" "\033[0m" "\u2588" "BLUE ON");
        Serial.println("================================");
    } else {
        digitalWrite(LED_OUT, LOW);
        ledState = false;
        Serial.println("\033[0m" "\u2588" "\033[31m" "\u2588" "RED ON");
        Serial.println("================================");
    }
  #endif

    delay(1000);
}
