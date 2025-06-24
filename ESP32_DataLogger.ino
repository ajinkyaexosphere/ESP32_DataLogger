#include <WiFi.h>
#include <WebServer.h>
#include <SPI.h>
#include <SD.h>
#include "DHT.h"

// === DHT11 Setup ===
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// === SD Card Setup ===
#define SD_CS 5

// === Wi-Fi Access Point Setup ===
const char* ssid = "ESP32-AccessPoint";
const char* password = "12345678";
WebServer server(80);

// === Manual Start Time ===
int startYear = 2025, startMonth = 6, startDay = 24;
int startHour = 17, startMinute = 0, startSecond = 0;

int currentYear, currentMonth, currentDay;
int currentHour, currentMinute, currentSecond;
unsigned long startMillis;
int entryCount = 1;

const int daysInMonth[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

// === ESP32 Internal Temp Sensor ===
extern "C" uint8_t temprature_sens_read();
float readInternalTemp() {
  return (temprature_sens_read() - 32) / 1.8;
}

void updateDateTime() {
  unsigned long elapsed = millis() / 1000; // seconds since power-on
  int totalSeconds = startSecond + (elapsed % 60);
  int totalMinutes = startMinute + ((elapsed / 60) % 60);
  int totalHours   = startHour + ((elapsed / 3600) % 24);
  int totalDays    = startDay + (elapsed / 86400);

  currentYear = startYear;
  currentMonth = startMonth;
  currentHour = totalHours % 24;
  currentMinute = totalMinutes % 60;
  currentSecond = totalSeconds % 60;
  currentDay = totalDays;

  while (currentDay > daysInMonth[currentMonth - 1]) {
    currentDay -= daysInMonth[currentMonth - 1];
    currentMonth++;
    if (currentMonth > 12) {
      currentMonth = 1;
      currentYear++;
    }
  }
}

void handleRoot() {
  updateDateTime();

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  float espTemp = readInternalTemp();

  char dateTimeStr[30];
  snprintf(dateTimeStr, sizeof(dateTimeStr), "%02d/%02d/%04d %02d:%02d:%02d",
           currentDay, currentMonth, currentYear,
           currentHour, currentMinute, currentSecond);

  String html = "<html><head><meta http-equiv='refresh' content='5'></head><body>";
  html += "<h2>DHT11 Sensor Data</h2>";
  html += "<p><strong>Time:</strong> " + String(dateTimeStr) + "</p>";
  html += "<p><strong>Entry:</strong> #" + String(entryCount) + "</p>";
  html += "<p>Temperature: " + String(temperature) + " °C</p>";
  html += "<p>Humidity: " + String(humidity) + " %</p>";
  html += "<p>ESP32 Internal Temp: " + String(espTemp) + " °C</p>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Start Wi-Fi Access Point
  WiFi.softAP(ssid, password);
  Serial.println("Access Point started");
  Serial.print("Wi-Fi: "); Serial.println(ssid);
  Serial.print("IP Address: "); Serial.println(WiFi.softAPIP());

  // Initialize SD card
  if (!SD.begin(SD_CS)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");

  // Add session marker
  File dataFile = SD.open("/datalog.txt", FILE_APPEND);
  if (dataFile) {
    dataFile.println("\n=== New session started ===");
    dataFile.close();
  }

  // Start Web Server
  server.on("/", handleRoot);
  server.begin();
  Serial.println("Web server started");

  startMillis = millis();  // Record start time
}

void loop() {
  updateDateTime();

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  float espTemp = readInternalTemp();

  char dateTimeStr[30];
  snprintf(dateTimeStr, sizeof(dateTimeStr), "%02d/%02d/%04d %02d:%02d:%02d",
           currentDay, currentMonth, currentYear,
           currentHour, currentMinute, currentSecond);

  if (!isnan(temperature) && !isnan(humidity)) {
    File dataFile = SD.open("/datalog.txt", FILE_APPEND);
    if (dataFile) {
      dataFile.print("#" + String(entryCount++) + " ");
      dataFile.print(dateTimeStr);
      dataFile.print(" -> Temp: ");
      dataFile.print(temperature);
      dataFile.print(" C, Humidity: ");
      dataFile.print(humidity);
      dataFile.print(" %, ESP32 Temp: ");
      dataFile.print(espTemp);
      dataFile.println(" °C");
      dataFile.close();
      Serial.println("Data logged to SD");
    } else {
      Serial.println("Failed to open file on SD");
    }
  } else {
    Serial.println("Failed to read from DHT sensor");
  }

  server.handleClient();
  delay(5000);  // Log every 5 seconds
}

