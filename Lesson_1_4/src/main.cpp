#include <Arduino.h>

#define LED_RED_OUT 18
#define LED_BLUE_OUT 17
#define BUTTON 15
#define BOOT_BUTTON 0

#define DEBOUNCE_DELAY 50

#define BLUE_COLOR "\033[34m"
#define YELLOW_COLOR "\033[33m"
#define RESET_COLOR "\033[0m"

// Enum для режимів миготіння світлодіодів
enum BlinkModeType
{
	NONE,
	FAST,
	SLOW,
	SYNC
};

// Структура для зберігання режиму миготіння та часу затримки
struct Mode
{
	BlinkModeType type;
	int delayTime;
};

// Структура для контролю стану кнопки з урахуванням дребезгу контактів
struct ButtonState
{
	int rawState;
	int lastRawState;
	unsigned long lastChangeTime;
	bool stableState;
};

// Ініціалізація стану кнопок
ButtonState bootBtnState = {HIGH, HIGH, 0, HIGH};	// Ініціалізація стану кнопки BOOT_BUTTON, діє бвудований пулап режим
ButtonState pinBtnState = {LOW, LOW, 0, LOW};

// Ініціалізація режимів миготіння світлодіодів
Mode modes[] = {
	{NONE, 0},
	{FAST, 100},
	{SLOW, 1000},
	{SYNC, 500}
};

// Змінні для відстеження стану світлодіодів та часу останнього миготіння
bool redLedState = false;
bool blueLedState = false;
unsigned long lastBlinkTime = 0;

// Змінна для збереження поточного режиму миготіння світлодіодів
BlinkModeType currentMode = NONE;


void setup()
{
	Serial.begin(115200);
	delay(1000);

	// Налаштування режимів роботи пінів для кнопок та світлодіодів
	// BOOT_BUTTON має вбудований резистор підтягування до живлення, тому його режим встановлюємо як INPUT_PULLUP
	// BUTTON підключена через джерело живлення, тому її режим встановлюємо як INPUT_PULLDOWN
	// Щоб при натисканні був 1 (логічна одиниця), а при відпусканні 0 (логічний нуль)
	// Але зважаючи на BOOT то можна було залишити як є і при натисканні було б 0.
	pinMode(BUTTON, INPUT_PULLDOWN);

	pinMode(LED_RED_OUT, OUTPUT);
	digitalWrite(LED_RED_OUT, LOW);
	pinMode(LED_BLUE_OUT, OUTPUT);
	digitalWrite(LED_BLUE_OUT, LOW);
}

// Функція для перевірки зміни стану кнопки з урахуванням дребезгу контактів
// Повертає true, якщо стан кнопки змінився, і false в іншому випадку
// btnState - структура для збереження стану кнопки
// pin - номер піна, до якого підключена кнопка
bool is_state_changed(ButtonState &btnState, int pin)
{
	btnState.rawState = digitalRead(pin);

	// Перевірка, чи змінився сирий стан кнопки
	// Тобто на кожну ітерацію перевіряємо, чи змінився стан кнопки, і якщо так, оновлюємо час останньої зміни та останній сирий стан
	if (btnState.rawState != btnState.lastRawState)
	{
		btnState.lastChangeTime = millis();
		btnState.lastRawState = btnState.rawState;
	}

	// Перевірка, чи пройшов час дребезгу контактів
	// Якщо пройшов, перевіряємо, чи змінився стабільний стан кнопки
	// Якщо змінився, оновлюємо стабільний стан та повертаємо true
	if (millis() - btnState.lastChangeTime > DEBOUNCE_DELAY)
	{
		if (btnState.stableState != btnState.rawState)
		{
			btnState.stableState = btnState.rawState;

			return true;
		}
	}

	return false;
}

// Функція для миготіння світлодіодів у режимі чергування
void blink_alter(const int delayTime)
{
	// Перевірка, чи пройшов час затримки між миготіннями
	// Замість використання delay(), ми використовуємо millis() для відстеження часу, що дозволяє програмі продовжувати виконувати інші завдання
	if (millis() - lastBlinkTime < delayTime)
	{
		return;
	}

	lastBlinkTime = millis();

	if (!redLedState && !blueLedState)
	{
		digitalWrite(LED_RED_OUT, HIGH);
		redLedState = true;
	}
	else if (redLedState && !blueLedState)
	{
		digitalWrite(LED_RED_OUT, LOW);
		redLedState = false;

		digitalWrite(LED_BLUE_OUT, HIGH);
		blueLedState = true;
	}
	else if (!redLedState && blueLedState)
	{
		digitalWrite(LED_BLUE_OUT, LOW);
		blueLedState = false;

		digitalWrite(LED_RED_OUT, HIGH);
		redLedState = true;
	}
}

// Функція для миготіння світлодіодів у режимі синхронного миготіння
void blink_sim(const int delayTime)
{
	// Перевірка, чи пройшов час затримки між миготіннями
	// Замість використання delay(), ми використовуємо millis() для відстеження часу, що дозволяє програмі продовжувати виконувати інші завдання
	if (millis() - lastBlinkTime < delayTime)
	{
		return;
	}

	lastBlinkTime = millis();

	if (redLedState && blueLedState)
	{
		digitalWrite(LED_RED_OUT, LOW);
		redLedState = false;

		digitalWrite(LED_BLUE_OUT, LOW);
		blueLedState = false;
	}
	else
	{
		digitalWrite(LED_RED_OUT, HIGH);
		redLedState = true;

		digitalWrite(LED_BLUE_OUT, HIGH);
		blueLedState = true;
	}
}

// Функція для перевірки, чи була кнопка натиснута
// stableState - стабільний стан кнопки після обробки дребезгу контактів
// pressedState - стан кнопки, який вважається натиснутим (LOW для BOOT_BUTTON, HIGH для BUTTON)
// isStateChanged - прапорець, який вказує, чи змінився стан кнопки
// Повертає true, якщо кнопка була натиснута, і false в іншому випадку
bool is_pressed(int stableState, int pressedState, bool isStateChanged)
{
	bool result = isStateChanged && stableState == pressedState;

	if (result)
	{
		Serial.print(BLUE_COLOR "Button pressed: " RESET_COLOR);
		Serial.println(pressedState == LOW ? "BOOT_BUTTON" : "BUTTON");
	}

	return result;
}

// Функція для встановлення режиму миготіння світлодіодів
// Скидає стан світлодіодів та оновлює час останнього миготіння
// newMode - новий режим миготіння, який потрібно встановити
void set_mode(BlinkModeType newMode)
{
	if (currentMode == newMode)
	{
		return;
	}

	currentMode = newMode;

	Mode modeData = modes[currentMode];

	Serial.printf(YELLOW_COLOR "Mode:" RESET_COLOR " %s | " YELLOW_COLOR "Delay time:" RESET_COLOR " %d ms\n", (modeData.type == FAST ? "FAST" : (modeData.type == SLOW ? "SLOW" : (modeData.type == SYNC ? "SYNC" : "NONE"))), modeData.delayTime);

	digitalWrite(LED_RED_OUT, LOW);
	digitalWrite(LED_BLUE_OUT, LOW);

	redLedState = false;
	blueLedState = false;

	lastBlinkTime = millis();
}

void loop()
{
	// Перевірка стану кнопок та визначення, чи були вони натиснуті
	bool isBootButtonChanged = is_state_changed(bootBtnState, BOOT_BUTTON);
	bool isPinButtonChanged = is_state_changed(pinBtnState, BUTTON);

	// Перевірка, чи була натиснута кнопка BOOT_BUTTON або BUTTON, і встановлення відповідного режиму миготіння світлодіодів
	bool isBootButtonPressed = is_pressed(bootBtnState.stableState, LOW, isBootButtonChanged);
	bool isButtonPressed = is_pressed(pinBtnState.stableState, HIGH, isPinButtonChanged);

	if (isBootButtonPressed && !isButtonPressed)
	{
		set_mode(FAST);
	}
	else if (isButtonPressed && !isBootButtonPressed)
	{
		set_mode(SLOW);
	}
	else if (bootBtnState.stableState == LOW && pinBtnState.stableState == HIGH)
	{
		set_mode(SYNC);
	}

	// Виклик функцій для миготіння світлодіодів відповідно до поточного режиму
	// Окремо від set_mode бо миготіння має продовжуватися навіть якщо стан кнопок не змінився.
	if (currentMode == FAST)
	{
		blink_alter(modes[FAST].delayTime);
	}
	else if (currentMode == SLOW)
	{
		blink_alter(modes[SLOW].delayTime);
	}
	else if (currentMode == SYNC)
	{
		blink_sim(modes[SYNC].delayTime);
	}
}
