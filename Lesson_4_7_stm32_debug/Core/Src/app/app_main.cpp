#include "main.h"

extern "C" volatile uint32_t uwTick;

static constexpr uint32_t LED_PIN = 5;
static constexpr uint32_t BUTTON_PIN = 0;
static constexpr uint32_t DEBOUNCE_MS = 30;

static uint32_t lastBlink = 0;
static bool ledState = false;

static bool lastRawState = false;
static bool stableButtonState = false;
static uint32_t lastDebounceTime = 0;
static uint32_t blinkInterval = 500;


static uint32_t myTick()
{
    return uwTick;
}

static void LED_Init()
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // PA5 = Output
    GPIOA->MODER &= ~(3U << (LED_PIN * 2));
    GPIOA->MODER |=  (1U << (LED_PIN * 2));

    // Push-Pull
    GPIOA->OTYPER &= ~(1U << LED_PIN);

    // Low Speed
    GPIOA->OSPEEDR &= ~(3U << (LED_PIN * 2));

    // No Pull
    GPIOA->PUPDR &= ~(3U << (LED_PIN * 2));

    // LED OFF
    GPIOA->BSRR = (1U << (LED_PIN + 16));
}

static void LED_Update()
{
    uint32_t now = myTick();

    if ((uint32_t)(now - lastBlink) >= blinkInterval)
    {
        lastBlink = now;
        ledState = !ledState;

        if (ledState)
        {
            GPIOA->BSRR = (1U << LED_PIN);
        }
        else
        {
            GPIOA->BSRR = (1U << (LED_PIN + 16));
        }
    }
}

static void Button_Init()
{
    // PA0 = Input
    GPIOA->MODER &= ~(3U << (BUTTON_PIN * 2));

    // PA0 = Pull-up
    GPIOA->PUPDR &= ~(3U << (BUTTON_PIN * 2));
    GPIOA->PUPDR |=  (1U << (BUTTON_PIN * 2));
}

bool Button_IsPressed()
{
    return (GPIOA->IDR & (1U << 0)) == 0;
}

static void Button_Update()
{
    uint32_t now = myTick();
    bool currentState = Button_IsPressed();

    if (currentState != lastRawState)
    {
        lastDebounceTime = now;
    }

    if ((uint32_t)(now - lastDebounceTime) >= DEBOUNCE_MS)
    {
        if (currentState != stableButtonState)
        {
            stableButtonState = currentState;

            if (stableButtonState)
            {
                if (blinkInterval == 200)
                    blinkInterval = 500;
                else if (blinkInterval == 500)
                    blinkInterval = 1000;
                else
                    blinkInterval = 200;
            }
        }
    }

    lastRawState = currentState;
}

extern "C" void app_main()
{
    LED_Init();
    Button_Init();

    lastBlink = myTick();

    while (1)
    {
        Button_Update();
        LED_Update();
    }
}
