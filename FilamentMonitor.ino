
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Konfiguracja sprzetu
constexpr uint8_t SDA_PIN = 4;
constexpr uint8_t SCL_PIN = 5;
constexpr uint8_t OLED_ADDR = 0x3C;
constexpr uint8_t BUTTON_PIN = 1;

// Czasy
constexpr uint32_t SCREEN_TIME = 30000;
constexpr uint32_t MEASURE_INTERVAL = 1000;

constexpr uint64_t AUTO_WAKE_TIME = 30ULL * 60ULL * 1000000ULL;

// Urządzenia
Adafruit_SSD1306 oled(128, 64, &Wire, -1);
Adafruit_AHTX0 aht;

// Dane zachowane w Deep Sleep
RTC_DATA_ATTR float lastRh = 0;
RTC_DATA_ATTR float lastTemp = 0;
RTC_DATA_ATTR uint32_t measureCount = 0;
RTC_DATA_ATTR bool humidityAlarm = false;
RTC_DATA_ATTR uint32_t autoMeasureCount = 0;

// Odczyt czujnika
bool measureSensor() {
  sensors_event_t humidity, temperature;

  if (!aht.getEvent(&humidity, &temperature)) {
    Serial.println("Blad pomiaru AHT20!");
    return false;
  }

  lastRh = humidity.relative_humidity;
  lastTemp = temperature.temperature;
  measureCount++;

  if (lastRh > 30.0f) {
    humidityAlarm = true;
  } else if (lastRh < 27.0f) {
    humidityAlarm = false;
  }

  Serial.printf("RH: %.1f%% | T: %.1f C | Pomiar: %lu\n", lastRh, lastTemp, (unsigned long)measureCount);

  return true;
}

// Rysowanie ekranu
void drawScreen() {
  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);

  oled.setTextSize(1);
  oled.setCursor(0, 0);
  oled.println("FILAMENT BOX");
  oled.drawLine(0, 12, 127, 12, SSD1306_WHITE);

  oled.setTextSize(2);
  oled.setCursor(0, 20);
  oled.print(lastRh, 1);
  oled.print(" %RH");

  oled.setTextSize(1);
  oled.setCursor(0, 43);
  oled.print("Temp: ");
  oled.print(lastTemp, 1);
  oled.print(" C");

  oled.setCursor(0, 55);

  if (humidityAlarm) {
    oled.print("UWAGA: WILGOC!");
  } else {
    oled.print("STATUS: OK");
  }

  oled.display();
}

// Wygaszenie OLED bez pelnej inicjalizacji
void turnOffDisplay() {
  Wire.beginTransmission(OLED_ADDR);
  Wire.write(0x00);
  Wire.write(SSD1306_DISPLAYOFF);
  Wire.endTransmission();
}

// Przejscie w Deep Sleep
void goToSleep() {
  // Nie zasypiamy z wcisnietym przyciskiem
  while (digitalRead(BUTTON_PIN) == LOW) {
    delay(10);
  }

  delay(50);

  esp_err_t gpioStatus =
    esp_deep_sleep_enable_gpio_wakeup(
      1ULL << BUTTON_PIN,
      ESP_GPIO_WAKEUP_GPIO_LOW
    );
  
  esp_err_t timerStatus = 
    esp_sleep_enable_timer_wakeup(AUTO_WAKE_TIME);
  
  if (gpioStatus != ESP_OK || timerStatus != ESP_OK) {
    Serial.println("Blad konfiguracji Deep Sleep!");
    while (true) delay(1000);
  }

  Serial.println("Przechodze w Deep Sleep...");
  Serial.flush();

  esp_deep_sleep_start();
}

void setup() {
  Serial.begin(115200);
  delay(300);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Wire.begin(SDA_PIN, SCL_PIN);

  esp_sleep_wakeup_cause_t cause =
    esp_sleep_get_wakeup_cause();
  
  // Nowy start urzadzenia
  if (cause == ESP_SLEEP_WAKEUP_UNDEFINED) {
    measureCount = 0;
  }

  // Inicjalizacja AHT20
  if (!aht.begin(&Wire)) {
    Serial.println("Blad AHT20!");
    goToSleep();
  }

  // Wybudzenie przyciskiem
  if (cause == ESP_SLEEP_WAKEUP_GPIO) {
    Serial.println("Wybudzenie przyciskiem");
    Serial.printf("Pomiary automatyczne: %lu\n", (unsigned long)autoMeasureCount);

    // Kontrola polaczenia OLED
    Wire.beginTransmission(OLED_ADDR);

    if (Wire.endTransmission() != 0) {
      Serial.println("Blad OLED!");
      goToSleep();
    }

    if (!oled.begin(
        SSD1306_SWITCHCAPVCC,
        OLED_ADDR, true, false )) {
      Serial.println("Blad inicjalizacji OLED!");
      goToSleep();
    }

    uint32_t screenStart = millis();
    uint32_t lastMeasure =
      screenStart - MEASURE_INTERVAL;
    
    // Aktywny ekran przez 30 sekund
    while (millis() - screenStart < SCREEN_TIME) {
      uint32_t now = millis();

      if (now - lastMeasure >= MEASURE_INTERVAL) {
        lastMeasure = now;

        if (measureSensor()) {
          drawScreen();
        } else {
          oled.clearDisplay();
          oled.setTextSize(1);
          oled.setCursor(0, 0);
          oled.println("BLAD POMIARU!");
          oled.display();
        }
      }

      delay(10);
    }

    oled.ssd1306_command(SSD1306_DISPLAYOFF);
    Serial.println("Ekran wylaczony");
  }

  // Wybudzenie timera lub pierwszy start
  else {
    Serial.println("Pomiar automatyczny");

    // Nie uruchamiamy całego OLED
    turnOffDisplay();

    if (measureSensor()) {
      autoMeasureCount++;
    }
  }

  goToSleep();
}

void loop() {
 
}
