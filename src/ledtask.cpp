#include <Arduino.h>
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif
static const int LED_BUILTIN = 2;
static int delayTime = 500;
static TaskHandle_t task1 = NULL;
static TaskHandle_t task2 = NULL;
static const uint8_t buf_len = 20;
void blink(void *parameter)
{
  pinMode(LED_BUILTIN, OUTPUT);
  while (true)
  {
    digitalWrite(LED_BUILTIN, HIGH); // Turn the LED on
    vTaskDelay(delayTime / portTICK_PERIOD_MS);
    digitalWrite(LED_BUILTIN, LOW); // Turn the LED off
    vTaskDelay(delayTime / portTICK_PERIOD_MS);
  }
}
void readvalue(void *parameter)
{
  Serial.setTimeout(10);  // set small timeout once
  while (true) {
    if (Serial.available() > 0) {
      int val = Serial.parseInt();
      if (val > 0) {   // only accept positive numbers
        delayTime = val;
        Serial.print("Updated LED delay to: ");
        Serial.println(delayTime);
      }
    }
    vTaskDelay(1 / portTICK_PERIOD_MS); // yield CPU
  }

  // while (true) {
  //   if (Serial.available() > 0) {
  //   Serial.setTimeout(10); 
  //   delayTime = Serial.parseInt();

  //   }
  // }
}
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  xTaskCreatePinnedToCore(
      blink,   // Function to implement the task
      "Task1", // Name of the task
      1024,    // Stack size in words
      NULL,    // Task input parameter
      1,       // Priority of the task
      &task1,  // Task handle
      app_cpu  // Core where the task should run
  );

  xTaskCreatePinnedToCore(
      readvalue,
      "Task2", // Name of the task
      1024,    // Stack size in words
      NULL,    // Task input parameter
      1,       // Priority of the task
      &task2,  // Task handle
      app_cpu  // Core where the task should run
  );
}
void loop()
{
}