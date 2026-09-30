// Week3-Lecture2
// Timer Interrupt (Internal)
// Embedded IoT System Fall-2026

// Name: xyz                  Reg#: 1234

#include <Arduino.h>

#define LED 4

hw_timer_t *My_timer = NULL;
volatile bool ledState = false;

void IRAM_ATTR onTimer() {
  ledState = !ledState;
  digitalWrite(LED, ledState);
}

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  My_timer = timerBegin(0, 80, true);            // timer 0, prescaler 80 -> 1 MHz (1 tick = 1 µs)
  timerAttachInterrupt(My_timer, &onTimer, true);
  timerAlarmWrite(My_timer, 1000000, true);      // every 1 s, auto-reload
  timerAlarmEnable(My_timer);
}

void loop() {
  // nothing needed, all handled by interrupts
}