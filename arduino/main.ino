#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

// Define OLED display width and height
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// Declaration for SSD1306 display connected using I2C
#define OLED_RESET -1 // Reset pin (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define I2C pins for the NodeMCU
#define SDA_PIN 4 // GPIO 4 (D2)
#define SCL_PIN 5 // GPIO 5 (D1)

// Define LED pin
#define LED_PIN 14 // GPIO 14 (D5) - PWM pin

// Variables for brightness control
int brightness = 50; // Initial brightness level (0-255)
int fadeAmount = 10; // Amount by which to change brightness

// Define button pins
#define button1Pin D6 // GPIO0 (D3)
#define button2Pin D3 // GPIO14 (D5)
#define vibrate D8

// Variables for button states
int button1State = 0;
int button2State = 0;
unsigned long pressStart = 0;
bool buttonPressed = false;

// Data for October
int octoberData[31][7] = {
    {1, 7, 13, 18, 51, 13, 2},   {2, 7, 15, 18, 48, 13, 2},
    {3, 7, 16, 18, 46, 13, 2},   {4, 7, 18, 18, 44, 13, 1},
    {5, 7, 19, 18, 42, 13, 1},   {6, 7, 21, 18, 40, 13, 1},
    {7, 7, 22, 18, 38, 13, 1},   {8, 7, 23, 18, 36, 13, 0},
    {9, 7, 25, 18, 34, 13, 0},   {10, 7, 26, 18, 32, 13, 0},
    {11, 7, 28, 18, 30, 12, 59}, {12, 7, 29, 18, 28, 12, 59},
    {13, 7, 31, 18, 26, 12, 59}, {14, 7, 32, 18, 24, 12, 59},
    {15, 7, 34, 18, 22, 12, 59}, {16, 7, 35, 18, 21, 12, 58},
    {17, 7, 37, 18, 19, 12, 58}, {18, 7, 38, 18, 17, 12, 58},
    {19, 7, 40, 18, 15, 12, 58}, {20, 7, 41, 18, 13, 12, 58},
    {21, 7, 43, 18, 11, 12, 57}, {22, 7, 44, 18, 9, 12, 57},
    {23, 7, 46, 18, 8, 12, 57},  {24, 7, 48, 18, 6, 12, 57},
    {25, 7, 49, 18, 4, 12, 57},  {26, 7, 51, 18, 2, 12, 57},
    {27, 7, 52, 18, 1, 12, 57},  {28, 7, 54, 17, 59, 12, 57},
    {29, 7, 55, 17, 57, 12, 57}, {30, 7, 57, 17, 56, 12, 57},
    {31, 7, 58, 17, 54, 12, 57},
};

int day = 25;
int hour = 3;
int minute = 30;

const int minuteInterval = 60000;
unsigned long previousMillis = millis();

float lightPercentage = 0.42;

int state = 0; // 0 is the hour view of the day, 1 is the select day view.

int selected = day;

void setup() {
  // Initialize serial communication for brightness control
  Serial.begin(115200);

  // Initialize I2C communication with OLED
  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  // Clear the display buffer
  display.clearDisplay();

  // Set up the LED pin as output
  pinMode(LED_PIN, OUTPUT);
  analogWrite(LED_PIN, brightness); // Set initial brightness

  // Display text on OLED
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Hello, NodeMCU!"));
  display.println(F("128x32 OLED"));
  display.display();

  // Show initial brightness on Serial Monitor
  Serial.println("Press 'u' to increase brightness, 'd' to decrease.");

  // Set up buttons as inputs with internal pull-up resistors
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);

  Serial.println("Hello world");
}

void loop() {
  // Read the state of the buttons
  button1State = digitalRead(button1Pin);
  button2State = digitalRead(button2Pin);

  // Check if Button 1 is pressed (D3)
  if (button1State == LOW) {
    Serial.println("Button 1 Pressed!");
    buttonPressed = true;
    pressStart = millis();

    if (state == 0) {
      lightPercentage =
          calcLight(hour, minute, octoberData[day][1], octoberData[day][2],
                    octoberData[day][3], octoberData[day][4], octoberData[day][5],
                    octoberData[day][6]);
      minute += 15;
      if (minute >= 60) {
        minute -= 60; // Reset minutes to a valid range
        hour += 1;    // Increment hour
        if (hour > 23) {
          hour = 0;
          day++;
        } // Wrap around to 0 if past 23
      }
    } else {
      selected++;
    }
  }

  // Check if Button 2 is pressed (D5)
  if (button2State == LOW) {
    Serial.println("Button 2 Pressed!");


    if (state == 0) {
    minute -= 15;
    if (minute < 0) {
      minute += 60; // Reset minutes to a valid range
      hour -= 1;    // Decrement hour
      if (hour < 0)
        hour = 23; // Wrap around to 23 if below 0
    }

    lightPercentage =
        calcLight(hour, minute, octoberData[day][1], octoberData[day][2],
                  octoberData[day][3], octoberData[day][4], octoberData[day][5],
                  octoberData[day][6]);
    } else {
      selected--;
    }
  }

  if (button2State == LOW && button1State == LOW) {
    Serial.println("HIT HIT HIT!");
    if (state == 0) {
      state = 1;
    } else {
      day = selected;
      state = 0;
    }

    Serial.println(state);
  }

  // Update the LED brightness
  analogWrite(LED_PIN, (int)(255 * lightPercentage));

  // Display the current brightness value on the OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);

  if (state == 0) {
    char buffer[20];
    int tmpHour = hour;
    const char *time = "AM"; // Declare as a pointer to support reassignment
    if (hour >= 12) {
      if (hour > 12)
        tmpHour = hour - 12; // Adjust for PM hours
      time = "PM";           // Change to PM if hour is 12 or more
    } else if (hour == 0) {
      tmpHour = 12; // Midnight is represented as 12 AM
    }
    sprintf(buffer, "Oct %02d: %02d:%02d%s", day, tmpHour, minute, time);

    display.println(buffer);

    char buffer2[32];
    sprintf(buffer2, "Rise%1d:%02dAM Set %1d:%02dPM", octoberData[day][1],
            octoberData[day][2], octoberData[day][3] - 12, octoberData[day][4]);
    display.println(buffer2);

    // Serial.println((lightPercentage/100) * 90);

    if (lightPercentage > 0) {
      display.fillRect(0, 16, (int)(lightPercentage * 80), 32, SSD1306_WHITE);
      display.drawLine(80, 18, 80, 30, SSD1306_WHITE);
      display.setCursor(90, 16);
      display.print(lightPercentage * 100);
      display.print("%");
      display.setCursor(85, 24);
      display.print(octoberData[day][5]);
      display.print(":");
      display.print(octoberData[day][6]);
      display.print("PM");
    } else {
      display.setCursor(58, 24);
      display.println("Dark");
    }
  }

  if (state == 1) {
    display.print("Oct ");
    display.print(selected);
  }

  display.display();

  delay(100); // Delay to slow down updates

  unsigned long currentMillis = millis();

  // Check if a minute has passed
  if (currentMillis - previousMillis >= minuteInterval) {
    previousMillis = currentMillis;
    incrementTime();
  }
}

// Function to increase LED brightness
void increaseBrightness() {
  brightness += fadeAmount;
  if (brightness > 255)
    brightness = 255; // Cap at maximum brightness
  Serial.print("Increasing brightness: ");
  Serial.println(brightness);
}

// Function to decrease LED brightness
void decreaseBrightness() {
  brightness -= fadeAmount;
  if (brightness < 0)
    brightness = 0; // Cap at minimum brightness
  Serial.print("Decreasing brightness: ");
  Serial.println(brightness);
}

void incrementTime() {
  minute++;

  if (minute >= 60) {
    minute = 0;
    hour++;
  }

  if (hour >= 24) {
    hour = 0;
    day++;
  }
}

float calcLight(int hour, int minute, int sunriseHour, int sunriseMinute,
                int sunsetHour, int sunsetMinute, int solarHour,
                int solarMinute) {
  int currentMinutes = hour * 60 + minute;
  int sunriseMinutes = sunriseHour * 60 + sunriseMinute;
  int sunsetMinutes = sunsetHour * 60 + sunsetMinute;
  int solarNoonMinutes = solarHour * 60 + solarMinute;

  // If it's before sunrise or after sunset, return 0
  if (currentMinutes < sunriseMinutes || currentMinutes > sunsetMinutes) {
    Serial.println("Dark");
    return 0.0;
  }

  // If it's before solar noon, calculate the percentage as it increases
  if (currentMinutes <= solarNoonMinutes) {
    float percent = ((float)(currentMinutes - sunriseMinutes) /
                     (solarNoonMinutes - sunriseMinutes));
    Serial.println(percent);
    return percent;
  }
  // If it's after solar noon, calculate the percentage as it decreases
  else {
    float percent = ((float)(sunsetMinutes - currentMinutes) /
                     (sunsetMinutes - solarNoonMinutes));
    Serial.println(percent);
    return percent;
  }
}
