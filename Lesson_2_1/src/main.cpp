#include <Arduino.h>

enum class LedState
{
  ON,
  OFF
};

enum class Mode
{
  AlwaysOn,
  AlwaysOff,
  Blinking
};

class Led
{
private:
  const uint8_t pinNumber;

public:
  Led(uint8_t pin) : pinNumber(pin) {}

  void init()
  {
    pinMode(pinNumber, OUTPUT);
    setLedState(LedState::OFF);
  }

  void setLedState(LedState value)
  {
    digitalWrite(
        pinNumber,
        value == LedState::ON ? HIGH : LOW);
  }
};

struct LedRuntime
{
  unsigned long prevMillis = 0;
  Mode mode = Mode::Blinking;
  LedState state = LedState::OFF;
};

constexpr uint8_t ledPin = 18;
constexpr uint8_t buttonPin = 15;
constexpr unsigned long interval = 1000;
volatile bool buttonPressed;

Led led(ledPin);

void onButtonPressed()
{
  buttonPressed = true;
}

void changeMode(LedRuntime &runtime, unsigned long currentMillis)
{
  switch (runtime.mode)
  {
  case Mode::Blinking:
    runtime.mode = Mode::AlwaysOn;
    runtime.state = LedState::ON;
    led.setLedState(runtime.state);
    break;

  case Mode::AlwaysOn:
    runtime.mode = Mode::AlwaysOff;
    runtime.state = LedState::OFF;
    led.setLedState(runtime.state);
    break;

  case Mode::AlwaysOff:
    runtime.mode = Mode::Blinking;
    runtime.prevMillis = currentMillis;
    break;
  }
}

void setup()
{
  led.init();
  pinMode(buttonPin, INPUT_PULLUP);
  buttonPressed = false;

  attachInterrupt(
      digitalPinToInterrupt(buttonPin),
      onButtonPressed,
      FALLING);

  Serial.begin(115200);
  delay(250);
}

void loop()
{
  static LedRuntime runtime;
  const unsigned long currentMillis = millis();

  if (buttonPressed)
  {
    buttonPressed = false;
    changeMode(runtime, currentMillis);
  }

  if (runtime.mode == Mode::Blinking &&
      currentMillis - runtime.prevMillis >= interval)
  {
    runtime.prevMillis = currentMillis;

    if (runtime.state == LedState::OFF)
    {
      runtime.state = LedState::ON;
    }
    else
    {
      runtime.state = LedState::OFF;
    }

    led.setLedState(runtime.state);
  }
}
