#include <Arduino.h>

enum LedState {
  ON,
  OFF
};

class Led {
  private:
    const uint8_t pinNumber;
    volatile LedState state;

  public:
    Led(uint8_t pin): pinNumber(pin) {};

    void init() {
      pinMode(pinNumber, OUTPUT);
    }

    void setLedState(LedState value) {
      if (value == LedState::OFF) {
        digitalWrite(pinNumber, LOW);
      } else {
        digitalWrite(pinNumber, HIGH);
      }
    }
};

constexpr uint8_t ledPin = PB0;
constexpr uint8_t buttonPin = PA1;
constexpr int blinkTime = 100;
volatile LedState ledState = LedState::OFF;
volatile bool buttonPressed;

Led led(buttonPin);

void setup() {
    led.init();

    buttonPressed = false;

    Serial.begin(115200);
    while (!Serial) {
    }
    Serial.println("UART ready!\r\n");
}

void loop() {
    led.setLedState(ledState);
}

