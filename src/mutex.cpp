#include <Arduino.h>
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Shared resource - a counter
volatile int sharedCounter = 0;

// Task handles
static TaskHandle_t task1Handle = NULL;
static TaskHandle_t task2Handle = NULL;

static SemaphoreHandle_t mutex;
// Function to exculude the race condition using semaphore ....
void incrementTask(void *parameters)
{
  int taskId = *(int *)parameters;
  while (1)
  {
    if (xSemaphoreTake(mutex, 0) == pdTRUE) // acuire lock
    {
      int local = sharedCounter;
      vTaskDelay(50 / portTICK_PERIOD_MS);
      local++;
      sharedCounter = local;
      xSemaphoreGive(mutex); // relese lock
      // Print the result
      Serial.print("Task ");
      Serial.print(taskId);
      Serial.print(" incremented counter to: ");
      Serial.print(sharedCounter); // result
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    else
    {
      // do smth else
    }
  }
}

void setup()
{
  // Initialize serial
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  // Task parameters - task IDs
  static int task1Id = 1;
  static int task2Id = 2;
  mutex = xSemaphoreCreateMutex();
  // Create two tasks that will increment the same counter
  xTaskCreatePinnedToCore(
      incrementTask,    // Function to run
      "IncrementTask1", // Task name
      1024,             // Stack size (bytes)
      &task1Id,         // Parameter to pass
      1,                // Task priority
      &task1Handle,     // Task handle
      app_cpu           // CPU core
  );

  xTaskCreatePinnedToCore(
      incrementTask,    // Function to run
      "IncrementTask2", // Task name
      1024,             // Stack size (bytes)
      &task2Id,         // Parameter to pass
      1,                // Task priority
      &task2Handle,     // Task handle
      app_cpu           // CPU core
  );

  // Delete the "setup and loop" task
  vTaskDelete(NULL);
}
void loop()
{
}