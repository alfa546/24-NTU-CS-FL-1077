#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

// Assignment
// Timer Interrupt + Button Debounce + I2C OLED
// Embedded IoT System Fall-2026
// Name: M Ayub Mughal
// Reg#: 24-NTU-CS-FL-1059

#define LED 4
#define BUTTON_PIN 5

// SSD1306 128x64 I2C OLED
U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(
    U8G2_R0,
    U8X8_PIN_NONE
);

hw_timer_t *debounceTimer = NULL;

volatile bool debounceActive = false;
volatile bool ledState = false;
volatile bool buttonPressed = false;

unsigned long pressCount = 0;
bool previousButtonState = false;

// Button interrupt: starts debounce timer
void IRAM_ATTR onButtonISR()
{
    if (!debounceActive)
    {
        debounceActive = true;
        timerWrite(debounceTimer, 0);
        timerAlarmEnable(debounceTimer);
    }
}

// Timer callback: confirms valid button press
void IRAM_ATTR onDebounceTimer()
{
    if (digitalRead(BUTTON_PIN) == LOW)
    {
        ledState = !ledState;
        digitalWrite(LED, ledState);
        buttonPressed = true;
    }

    debounceActive = false;
}

// Update OLED display
void updateDisplay()
{
    display.clearBuffer();

    display.setFont(u8g2_font_6x12_tr);
    display.drawStr(0, 12, "BUTTON COUNTER");
    display.drawHLine(0, 16, 128);

    display.setCursor(0, 32);
    display.print("Button: ");
    display.print(previousButtonState ? "PRESSED" : "RELEASED");

    display.setCursor(0, 46);
    display.print("Press Count: ");
    display.print(pressCount);

    display.setCursor(0, 61);
    display.print("LED: ");
    display.print(ledState ? "ON" : "OFF");

    display.sendBuffer();
}

void setup()
{
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);

    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // I2C pins: SDA 21, SCL 22
    Wire.begin(21, 22);
    display.begin();

    // Show initial screen
    pressCount = 0;
    previousButtonState = false;
    updateDisplay();

    // Timer setup (Arduino-ESP32 2.x API)
    debounceTimer = timerBegin(1, 80, true);

    timerAttachInterrupt(
        debounceTimer,
        &onDebounceTimer,
        true
    );

    // 50 ms one-shot timer
    timerAlarmWrite(debounceTimer, 50000, false);

    attachInterrupt(
        digitalPinToInterrupt(BUTTON_PIN),
        onButtonISR,
        FALLING
    );
}

void loop()
{
    // Read confirmed button event from timer callback
    if (buttonPressed)
    {
        noInterrupts();
        buttonPressed = false;
        interrupts();

        pressCount++;
        previousButtonState = true;
        updateDisplay();
    }

    // Show RELEASED when button is released
    if (digitalRead(BUTTON_PIN) == HIGH &&
        previousButtonState)
    {
        previousButtonState = false;
        updateDisplay();
    }
}