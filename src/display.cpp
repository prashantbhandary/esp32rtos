#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Define I2C pins for ESP32
#define I2C_SDA 21
#define I2C_SCL 22

// Define LCD properties
// Set the LCD address to 0x27 for a 16 chars and 2 line display
// If 0x27 doesn't work, try 0x3F
LiquidCrystal_I2C lcd(0x27, 16, 2);
// Timer variables
int initialMinutes = 5; // 5 minute countdown
int seconds = 0;
int minutes = 0;

// Task handles
TaskHandle_t countdownTaskHandle = NULL;

// Countdown task
void countdownTask(void *parameter)
{
    minutes = initialMinutes;
    seconds = 0;

    while (true)
    {
        // Clear second line of the LCD
        lcd.setCursor(0, 1);
        lcd.print("                "); // Clear with 16 spaces

        lcd.setCursor(0, 1);
        // Display minutes with leading zero if needed
        if (minutes < 10)
        {
            lcd.print("0");
        }
        lcd.print(minutes);
        lcd.print(":");

        // Display seconds with leading zero if needed
        if (seconds < 10)
        {
            lcd.print("0");
        }
        lcd.print(seconds);

        // Decrement the counter
        if (seconds > 0)
        {
            seconds--;
        }
        else
        {
            if (minutes > 0)
            {
                minutes--;
                seconds = 59;
            }
            else
            {
                // Timer reached zero
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Time's up!");
                lcd.setCursor(0, 1);
                lcd.print("----------------");

                // Blink the display a few times
                for (int i = 0; i < 5; i++)
                {
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

void setup()
{
    // Initialize Serial for debugging
    Serial.begin(115200);

    // Initialize I2C LCD with custom pins
    Wire.begin(I2C_SDA, I2C_SCL);
    lcd.clear();
    // Initialize LCD
    lcd.init();
    lcd.backlight();
    // Show initial message
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Countdown Timer:");

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
}

void loop()
{
    // The main loop is empty as we're using FreeRTOS task
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}