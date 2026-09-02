#include <Arduino.h>

#if defined(TASK_1)

// =====================================================
// TASK 1
// Relay activation time measurement
// =====================================================

constexpr uint8_t RELAY_PIN = 18;
constexpr uint8_t CONTACT_PIN = 15;
constexpr uint8_t RUN_NUMBER = 10;

volatile bool contactTriggered = false;
volatile unsigned long contactMillis = 0;

unsigned long relayStartMillis = 0;

void IRAM_ATTR onRelayContact()
{
    if (contactTriggered == false)
    {
        contactMillis = millis();
        contactTriggered = true;
    }
}

void setup()
{
    Serial.begin(115200);

    delay(2250);

    pinMode(RELAY_PIN, OUTPUT);
    pinMode(CONTACT_PIN, INPUT_PULLUP);

    digitalWrite(RELAY_PIN, LOW);

    attachInterrupt(
        digitalPinToInterrupt(CONTACT_PIN),
        onRelayContact,
        FALLING
    );
}

void loop()
{
    static uint8_t count = 0;
    static unsigned long sum = 0;
    static unsigned long relayOffMillis = 0;

    static bool relayIsOn = false;
    static bool stopMeasuring = false;

    if (stopMeasuring)
    {
        return;
    }

    if (!relayIsOn &&
        millis() - relayOffMillis >= 250)
    {
        contactTriggered = false;

        relayStartMillis = millis();

        digitalWrite(RELAY_PIN, HIGH);

        relayIsOn = true;
    }

    if (relayIsOn && contactTriggered)
    {
        unsigned long activationTime =
            contactMillis - relayStartMillis;

        Serial.printf(
            "Count: %u | Time: %lu ms\n",
            count + 1,
            activationTime
        );

        count++;
        sum += activationTime;

        digitalWrite(RELAY_PIN, LOW);

        relayOffMillis = millis();

        relayIsOn = false;
    }

    if (count >= RUN_NUMBER)
    {
        Serial.printf(
            "Average: %lu ms\n",
            sum / count
        );

        stopMeasuring = true;
    }
}

#elif defined(TASK_2)

// =====================================================
// TASK 2
// Motor speed control using potentiometer
// and Software PWM
// =====================================================

constexpr uint8_t MOTOR_PIN = 18;
constexpr uint8_t POT_PIN = 4;

constexpr unsigned long PWM_PERIOD_MICROS = 1000;
constexpr uint16_t ADC_MAX = 4095;

unsigned long previousMicros = 0;

bool motorState = false;

void setup()
{
    pinMode(MOTOR_PIN, OUTPUT);
    pinMode(POT_PIN, INPUT);

    digitalWrite(MOTOR_PIN, LOW);
}

void loop()
{
    // Read potentiometer
    uint16_t adcValue = analogRead(POT_PIN);

    // Calculate HIGH and LOW time
    unsigned long highTimeMicros =
        (PWM_PERIOD_MICROS * adcValue) / ADC_MAX;

    unsigned long lowTimeMicros =
        PWM_PERIOD_MICROS - highTimeMicros;

    unsigned long currentMicros = micros();

    // 0% Duty Cycle
    if (highTimeMicros == 0)
    {
        digitalWrite(MOTOR_PIN, LOW);

        motorState = false;
        previousMicros = currentMicros;

        return;
    }

    // 100% Duty Cycle
    if (lowTimeMicros == 0)
    {
        digitalWrite(MOTOR_PIN, HIGH);

        motorState = true;
        previousMicros = currentMicros;

        return;
    }

    // Software PWM
    if (motorState)
    {
        if (currentMicros - previousMicros >= highTimeMicros)
        {
            digitalWrite(MOTOR_PIN, LOW);

            motorState = false;
            previousMicros = currentMicros;
        }
    }
    else
    {
        if (currentMicros - previousMicros >= lowTimeMicros)
        {
            digitalWrite(MOTOR_PIN, HIGH);

            motorState = true;
            previousMicros = currentMicros;
        }
    }
}

#else

#error "No task selected. Define TASK_1 or TASK_2 in platformio.ini"

#endif