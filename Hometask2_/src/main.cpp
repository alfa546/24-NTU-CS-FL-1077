// Task 2 - Dual LED PWM with OLED
// Mughal brand 1059

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// LED pins
const int LED1_PIN = 18;
const int LED2_PIN = 19;

// PWM channels for ESP32 2.x
const int LED1_CHANNEL = 0;
const int LED2_CHANNEL = 1;

// Different PWM frequencies
const int LED1_FREQ = 1000;
const int LED2_FREQ = 5000;

// 8-bit resolution: duty values 0-255
const int PWM_RESOLUTION = 8;
const int MAX_DUTY = 255;

// OLED settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(
    SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET
);

// Current duty-cycle values
int duty1 = 0;
int duty2 = 0;

// Fading directions
int direction1 = 1;
int direction2 = 1;

// Update timers
unsigned long lastFade1 = 0;
unsigned long lastFade2 = 0;
unsigned long lastOLED = 0;

const unsigned long FADE_INTERVAL_1 = 12;
const unsigned long FADE_INTERVAL_2 = 20;
const unsigned long OLED_INTERVAL = 150;

// Display OLED information
void updateOLED() {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    display.setCursor(0, 0);
    display.println("DUAL PWM MONITOR");

    display.setCursor(0, 16);
    display.print("LED1: ");
    display.print((duty1 * 100 + 127) / 255);
    display.print("% F:");
    display.print(LED1_FREQ);
    display.println("Hz");

    display.setCursor(0, 32);
    display.print("LED2: ");
    display.print((duty2 * 100 + 127) / 255);
    display.print("% F:");
    display.print(LED2_FREQ);
    display.println("Hz");

    display.setCursor(0, 50);
    display.println("PWM: 8-bit");

    display.display();
}

// Update LED brightness
void updateFade(
    int &duty,
    int &direction,
    int channel
) {
    duty += direction;

    if (duty >= MAX_DUTY) {
        duty = MAX_DUTY;
        direction = -1;
    }
    else if (duty <= 0) {
        duty = 0;
        direction = 1;
    }

    ledcWrite(channel, duty);
}

void setup() {
    Serial.begin(115200);
    delay(200);

    // Initialize I2C: SDA = GPIO 21, SCL = GPIO 22
    Wire.begin(21, 22);

    // Configure LED1 PWM
    ledcSetup(LED1_CHANNEL, LED1_FREQ, PWM_RESOLUTION);
    ledcAttachPin(LED1_PIN, LED1_CHANNEL);

    // Configure LED2 PWM
    ledcSetup(LED2_CHANNEL, LED2_FREQ, PWM_RESOLUTION);
    ledcAttachPin(LED2_PIN, LED2_CHANNEL);

    // Start LEDs at zero brightness
    ledcWrite(LED1_CHANNEL, 0);
    ledcWrite(LED2_CHANNEL, 0);

    // Initialize OLED
    if (!display.begin(
            SSD1306_SWITCHCAPVCC, OLED_ADDRESS
        )) {
        Serial.println("ERROR: OLED initialization failed!");

        while (true) {
            delay(1000);
        }
    }

    display.clearDisplay();
    updateOLED();

    Serial.println("Dual LED PWM and OLED monitor ready.");
    Serial.println("LED1 Frequency: 1000 Hz");
    Serial.println("LED2 Frequency: 5000 Hz");
    Serial.println("PWM Resolution: 8-bit");
}

void loop() {
    unsigned long now = millis();

    // Fade LED1
    if (now - lastFade1 >= FADE_INTERVAL_1) {
        lastFade1 = now;
        updateFade(duty1, direction1, LED1_CHANNEL);
    }

    // Fade LED2
    if (now - lastFade2 >= FADE_INTERVAL_2) {
        lastFade2 = now;
        updateFade(duty2, direction2, LED2_CHANNEL);
    }

    // Refresh OLED display
    if (now - lastOLED >= OLED_INTERVAL) {
        lastOLED = now;
        updateOLED();
    }
}