#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Define which core to use
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Define I2C pins for ESP32
#define I2C_SDA 21
#define I2C_SCL 22

// Define LED pin
static const int LED_BUILTIN = 2;
static int ledDelayTime = 500;

// Define LCD properties
// Set the LCD address to 0x27 for a 16 chars and 2 line display
// If 0x27 doesn't work, try 0x3F
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Timer variables
int initialMinutes = 5; // 5 minute countdown
int seconds = 0;
int minutes = 0;

// Task handles
TaskHandle_t ledTaskHandle = NULL;
TaskHandle_t countdownTaskHandle = NULL;
TaskHandle_t readSerialTaskHandle = NULL;

// LED blinking task
void ledTask(void *parameter) {
  pinMode(LED_BUILTIN, OUTPUT);
  while (true) {
    digitalWrite(LED_BUILTIN, HIGH); // Turn the LED on
    vTaskDelay(ledDelayTime / portTICK_PERIOD_MS);
    digitalWrite(LED_BUILTIN, LOW); // Turn the LED off
    vTaskDelay(ledDelayTime / portTICK_PERIOD_MS);
  }
}

// Countdown task
void countdownTask(void *parameter) {
  minutes = initialMinutes;
  seconds = 0;

  while (true) {
    // Clear second line of the LCD
    lcd.setCursor(0, 1);
    lcd.print("                "); // Clear with 16 spaces

    // Format and display the time
    lcd.setCursor(0, 1);
    
    // Display minutes with leading zero if needed
    if (minutes < 10) {
      lcd.print("0");
    }
    lcd.print(minutes);
    lcd.print(":");
    
    // Display seconds with leading zero if needed
    if (seconds < 10) {
      lcd.print("0");
    }
    lcd.print(seconds);
    
    // Decrement the counter
    if (seconds > 0) {
      seconds--;
    } else {
      if (minutes > 0) {
        minutes--;
        seconds = 59;
      } else {
        // Timer reached zero
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Time's up!");
        lcd.setCursor(0, 1);
        lcd.print("----------------");
        
        // Blink the display a few times
        for (int i = 0; i < 5; i++) {
          lcd.noDisplay();
          vTaskDelay(500 / portTICK_PERIOD_MS);
          lcd.display();
          vTaskDelay(500 / portTICK_PERIOD_MS);
        }
        
        // Reset the timer
        minutes = initialMinutes;
        seconds = 0;
        
        // Clear and show initial message
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Countdown Timer:");
      }
    }
    
    // Wait for one second
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

// Serial read task to control LED blinking rate
void readSerialTask(void *parameter) {
  Serial.setTimeout(10);  // set small timeout
  while (true) {
    if (Serial.available() > 0) {
      String input = Serial.readStringUntil('\n');
      
      // Try to parse as integer for LED delay
      int val = input.toInt();
      if (val > 0) {
        ledDelayTime = val;
        Serial.print("Updated LED delay to: ");
        Serial.println(ledDelayTime);
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS); // yield CPU
  }
}

void setup() {
  // Initialize Serial for debugging
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println("Combined LED and LCD Countdown Demo");

  // Initialize I2C LCD with custom pins
  Wire.begin(I2C_SDA, I2C_SCL);
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Countdown Timer:");
  
  // Create the LED task
  xTaskCreatePinnedToCore(
    ledTask,            // Function to implement the task
    "LEDTask",          // Name of the task
    1024,               // Stack size in words
    NULL,               // Task input parameter
    1,                  // Priority of the task
    &ledTaskHandle,     // Task handle
    app_cpu             // Core where the task should run
  );

  // Create the countdown task
  xTaskCreatePinnedToCore(
    countdownTask,        // Function to implement the task
    "CountdownTask",      // Name of the task
    3000,                 // Stack size in words
    NULL,                 // Task input parameter
    1,                    // Priority of the task
    &countdownTaskHandle, // Task handle
    app_cpu               // Core where the task should run
  );
  
  // Create the serial input task
  xTaskCreatePinnedToCore(
    readSerialTask,       // Function to implement the task
    "SerialTask",         // Name of the task
    1024,                 // Stack size in words
    NULL,                 // Task input parameter
    2,                    // Priority of the task (higher than others)
    &readSerialTaskHandle,// Task handle
    app_cpu               // Core where the task should run
  );

  // Print instructions
  Serial.println("Enter a number to change LED blink delay (in ms)");
}

void loop() {
  // The main loop is empty as we're using FreeRTOS tasks
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}