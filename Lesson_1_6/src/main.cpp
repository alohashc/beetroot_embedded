#include <Arduino.h>

#define COLOR_BLUE   "\033[34m"
#define COLOR_GREEN  "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_RESET  "\033[0m"

#define LED_OUT 15
#define ADC_IN 4

#define ADC_MAX 4095
#define U_REF_MV 3100
#define DELAY_MS 100
#define SAMPLES_NUM 15

#define THRESHOLD_MV 1800
#define HYSTERESIS_MV 100
#define LOW_THRESHOLD_MV (THRESHOLD_MV - HYSTERESIS_MV)
#define HIGH_THRESHOLD_MV (THRESHOLD_MV + HYSTERESIS_MV)

void setup()
{
  Serial.begin(115200);
  pinMode(LED_OUT, OUTPUT);
  delay(500);
}

float calcFromRaw(uint16_t raw)
{
  return raw / (float)ADC_MAX * U_REF_MV;
}

float deltaPercentCalc(uint32_t calculatedValue, float calibratedValue)
{
  return fabs(calculatedValue - calibratedValue) / calibratedValue * 100;
}

void loop()
{
  static bool ledState = false;

  uint16_t raw = analogRead(ADC_IN);
  uint32_t calibratedMiliVolts = analogReadMilliVolts(ADC_IN);

  float calcMilitVolts = calcFromRaw(raw);

  float deltaPercentValue = deltaPercentCalc(calcMilitVolts, calibratedMiliVolts);

  Serial.printf(
    COLOR_YELLOW "|" COLOR_RESET
    COLOR_BLUE " RAW: " COLOR_GREEN "%u "
    COLOR_YELLOW "|" COLOR_RESET
    COLOR_BLUE " Voltage: " COLOR_GREEN "%.1f mV "
    COLOR_YELLOW "|" COLOR_RESET
    COLOR_BLUE " Calibrated Voltage: " COLOR_GREEN "%u mV "
    COLOR_YELLOW "|" COLOR_RESET
    COLOR_BLUE " Delta: " COLOR_GREEN "%.1f %% "
    COLOR_YELLOW "|" COLOR_RESET "\n",
    raw,
    calcMilitVolts,
    calibratedMiliVolts,
    deltaPercentValue
  );

  if (calibratedMiliVolts < LOW_THRESHOLD_MV)
  {
    ledState = true;
  }
  else if (calibratedMiliVolts > HIGH_THRESHOLD_MV)
  {
    ledState = false;
  }

  digitalWrite(LED_OUT, ledState ? HIGH : LOW);

  delay(DELAY_MS);
}
