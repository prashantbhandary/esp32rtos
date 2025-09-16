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

// Function to show the race condition
void incrementTask(void *parameters) {
  int taskId = *(int*)parameters;
  
  while(1) {
    // Read the shared counter value
    int local = sharedCounter;
    
    // Add a small delay to increase chance of race condition
    // This simulates some processing time
    vTaskDelay(50 / portTICK_PERIOD_MS);
    
    // Increment the local copy
    local++;
    
    // Write back to the shared counter
    sharedCounter = local;
    
    // Print the result
    Serial.print("Task ");
    Serial.print(taskId);
    Serial.print(" incremented counter to: ");
    Serial.print(sharedCounter);
    Serial.print(" (Expected: ");
    Serial.print(taskId == 1 ? sharedCounter + 1 : sharedCounter);
    Serial.println(")");
    
    // Wait before next increment
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void setup() {
  // Initialize serial
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("=== Race Condition Demonstration ===");
  Serial.println("Watch how the counter becomes inconsistent!");
  Serial.println();
  
  // Task parameters - task IDs
  static int task1Id = 1;
  static int task2Id = 2;
  
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
void loop() {
}