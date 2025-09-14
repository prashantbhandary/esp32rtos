#include <Arduino.h>
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif
static const int LED_BUILTIN = 2;

const char msg[] = "Hello from Task pras hant arr.";
static TaskHandle_t task1 = NULL;
static TaskHandle_t task2 = NULL;

// void toggleLed(void * parameter) {
//   pinMode(LED_BUILTIN, OUTPUT);
//   while (true) {
//     digitalWrite(LED_BUILTIN, HIGH); // Turn the LED on
//     vTaskDelay(1000 / portTICK_PERIOD_MS); 
//     digitalWrite(LED_BUILTIN, LOW); // Turn the LED off
//     vTaskDelay(1000 / portTICK_PERIOD_MS); 
//   }
// }
void starttask1(void * parameter) {
  int msglen = strlen(msg);
  while (true) {
    for(int i=0; i<msglen; i++) {
      Serial.print(msg[i]);
      vTaskDelay(50 / portTICK_PERIOD_MS); // Delay between each character
    }
    Serial.println();
    vTaskDelay(1000 / portTICK_PERIOD_MS); // block state for 1 second before next message
  }
}

void starttask2(void * parameter) {
  while (true) {
    Serial.println("*");
    vTaskDelay(500 / portTICK_PERIOD_MS); // Print asterisk every 500ms
  }
}

void setup() {
    // pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(300);
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    Serial.println("Free RTOS ");
    Serial.print("Setup running on core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" with priority ");
    Serial.println(uxTaskPriorityGet(NULL));
    
    xTaskCreatePinnedToCore(
        starttask1,        // Function to implement the task
        "Task1",          // Name of the task
        1024,             // Stack size in words
        (void *) msg,           // Task input parameter
        1,              // Priority of the task
        &task1,           // Task handle
        app_cpu         // Core where the task should run
    );

    xTaskCreatePinnedToCore(
        starttask2,
        "Task2",     // Name of the task
        1024,           // Stack size in words
        NULL,           // Task input parameter 
        2,              // Priority of the task
        &task2,        // Task handle
        app_cpu        // Core where the task should run
    );
    
}
void loop() {

    for (int i=0 ; i<3 ; i++){
        vTaskSuspend(task2);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        vTaskResume(task2);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    if(task1!=NULL)
    {
        vTaskDelete(task1);
        task1=NULL;
    }

}