# Lesson 2.1 — Embedded C++ LED Control with Button Interrupt


https://github.com/user-attachments/assets/60784a03-de3b-45f8-acb3-ae964bb9c350


## Опис проекту

Цей проект реалізує неблокуючий керований LED-режим на ESP32 у стилі Embedded C++ з використанням `superloop`-архітектури.

Основні ідеї:
- LED керується через клас `Led`
- режим роботи задається через `enum class`
- таймінг реалізований через `millis()` без `delay()`
- кнопка обробляється через `attachInterrupt()`
- логіка перемикання режимів виконується в `loop()`, а не в ISR
- використані `constexpr` для параметрів і `volatile` для сигналу від переривання

## Функціональність

Проект має три режими роботи LED:

1. `Blinking` — LED мигає з інтервалом 1 секунда
2. `AlwaysOn` — LED постійно увімкнений
3. `AlwaysOff` — LED постійно вимкнений

Кнопка змінює режим циклічно:
- `Blinking -> AlwaysOn -> AlwaysOff -> Blinking`

Перемикання здійснюється через ISR, а сама логіка переходу — в основному циклі `loop()`.

## Схема підключення

- LED: GPIO 18
- Button: GPIO 15
- Pull-up для кнопки: `INPUT_PULLUP`
- Переривання: `FALLING`

## Основні конструкції коду

### `enum class LedState`

Використовується для чітко типізованих станів LED:
- `LedState::ON`
- `LedState::OFF`

### `enum class Mode`

Містить режими роботи:
- `Mode::Blinking`
- `Mode::AlwaysOn`
- `Mode::AlwaysOff`

### Клас `Led`

Клас інкапсулює роботу з піном для LED:
- `init()` — налаштовує GPIO як вихід і встановлює початковий стан
- `setLedState()` — встановлює стан LED

### Неблокуючий таймінг

Замість `delay()` використовується:

```cpp
const unsigned long currentMillis = millis();
if (runtime.mode == Mode::Blinking &&
    currentMillis - runtime.prevMillis >= interval)
```

Це відповідає вимогам Embedded C++ для superloop-архітектури.

### Переривання кнопки

```cpp
attachInterrupt(
    digitalPinToInterrupt(buttonPin),
    onButtonPressed,
    FALLING);
```

ISR:

```cpp
void onButtonPressed()
{
  buttonPressed = true;
}
```

Це є мінімальною ISR-логікою, без `Serial.print()` всередині переривання.

## Конфігурація

Параметри задаються через `constexpr`:

```cpp
constexpr uint8_t ledPin = 18;
constexpr uint8_t buttonPin = 15;
constexpr unsigned long interval = 1000;
```

Це зменшує кількість “магічних чисел” у коді.

## Аналіз реалізації щодо завдання

| Вимога | Статус | Коментар |
|---|---|---|
| `volatile`, `const/constexpr`, `static` | Частково | `volatile` використано для `buttonPressed`; `constexpr` використано для пінів і інтервалу; `static` не використано явно для конфігурації |
| Відмова від динамічної пам’яті та STL | ✅ | У коді немає `new`, `delete`, STL контейнерів |
| `superloop`-архітектура | ✅ | Основна логіка в `loop()` без блокувальних затримок |
| Безпечна робота з периферією | ✅ | LED і кнопка налаштовані безпечним способом через GPIO API |
| Arduino blink у Embedded C++ | ✅ | Реалізовано в стилі класу, enum і неблокуючого керування |
| `enum class` для стану LED | ✅ | `LedState` використовується для станів `ON/OFF` |
| `constexpr` для номера піну та часу | ✅ | Піни та інтервал винесені в `constexpr` |
| Уникнення глобальних змінних | ⚠️ Частково | `led` і `buttonPressed` є глобальними; це не критично, але не зовсім ідеально |
| Не використовувати `delay()` | ✅ | У коді `delay()` використано тільки в `setup()` для стартової паузи; в логіці режимів немає блокування |
| Клас `Led` з методами `init()` та `set(...)` | ✅ Частково | Є `init()` і `setLedState()`, що є логічною реалізацією того ж самого | 
| `static const` для параметрів | ⚠️ Не реалізовано | Для конфігурації краще ввести окремий config-class або `struct` з `static constexpr` |
| Кнопка з перериванням | ✅ | `attachInterrupt` налаштований правильно |
| `volatile bool buttonPressed` | ✅ | Так, сигналізує про натиск кнопки |
| Перемикання режиму в `loop()` | ✅ | Логіка обробки перемикання відбувається в `loop()` |
| ISR мінімальна | ✅ | ISR лише встановлює flag без логіки та без Serial |

## Плюси реалізації

- Чітка структура коду
- Немає блокуючих затримок в робочому циклі
- Правильно використані `enum class` і `constexpr`
- Реалізовано interrupt-driven input
- Безпечне керування LED через метод класу

## Build and run

```bash
pio run
pio run --target upload
pio run --target monitor
```

## Link back

[Back to main project README](../README.md)
