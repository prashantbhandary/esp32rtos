#include <Arduino.h>
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif
static const int LED_BUILTIN = 2;

void toggleLed(void * parameter) {
  pinMode(LED_BUILTIN, OUTPUT);
  while (true) {
    digitalWrite(LED_BUILTIN, HIGH); // Turn the LED on
    vTaskDelay(1000 / portTICK_PERIOD_MS); 
    digitalWrite(LED_BUILTIN, LOW); // Turn the LED off
    vTaskDelay(1000 / portTICK_PERIOD_MS); 
  }
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    xTaskCreatePinnedToCore(
        toggleLed,       // Function to implement the task
        "Task1",     // Name of the task
        1024,           // Stack size in words
        NULL,           // Task input parameter
        1,              // Priority of the task
        NULL,           // Task handle
        app_cpu         // Core where the task should run
    );
}
void loop() {
}