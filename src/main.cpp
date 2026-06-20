#include <Arduino.h>
#include <LGFX_ATM0130B3.h>
#include <Scd4x.hpp>
#include <WiFi.h>
#include <HTTPClient.h>
#include <wifi_credidential.h>

const char* serverUrl = "http://pi5.local:3002/api/env_sensor";

static MySCD4X sensor;
static LGFX lcd;

static LGFX_Sprite sprite(&lcd);
static LGFX_Sprite sprite_network(&lcd);
const int network_area_height = 30;

void setup() {
  sensor.setup();
  lcd.begin();
  lcd.setRotation(3);
  lcd.fillScreen(lcd.color565(0, 0, 0));
  
  sprite.setColorDepth(8);
  sprite.createSprite(lcd.width(), lcd.height()-network_area_height);
  sprite.setBaseColor(lcd.color888(0, 0, 0));
  sprite.setTextColor(lcd.color888(255, 255, 255));

  sprite_network.setColorDepth(8);
  sprite_network.createSprite(lcd.width(), network_area_height);
  sprite_network.setBaseColor(lcd.color888(0, 0, 0));
  sprite_network.setTextColor(lcd.color888(255, 255, 255));

  lcd.setTextSize(3);
  lcd.println("Connecting to WiFi");
  WiFi.begin(MY_WIFI_SSID, MY_WIFI_PASSWORD);
  const int maxRetries = 10;
  int retries = 0;
  while(WiFi.status() != WL_CONNECTED && retries < maxRetries){
    delay(500);
    lcd.print(".");
    retries++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    lcd.println("\nConnected!");
  } else {
    lcd.println("\nFailed to connect");
    esp_deep_sleep(10 * 60 * 1000000);
  }
}

void sendDataToServer(String co2, String temp, String humid) {
  sprite_network.setCursor(0, 5);
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"co2\": \"" + co2 + "\", \"temperature\": \"" + temp + "\", \"humidity\": \"" + humid + "\"}";
    
    int httpResponseCode = http.POST(payload);
    
    if(httpResponseCode > 0) {
      String response = http.getString();
      sprite_network.println("Response: " + response);
    } else {
      sprite_network.println("Error on sending POST: " + String(httpResponseCode));
    }
    
    http.end();
  } else {
    sprite_network.println("WiFi not connected");
    esp_deep_sleep(10 * 60 * 1000000); // Sleep for 10 minutes if WiFi is not connected
  }

}

void drawData(int x, int y, String label, String value, String unit) {
  const float label_size = 2.5;
  const float value_size = 3.5;
  const float unit_size = 2.5;
  const uint32_t label_color = lcd.color888(200, 200, 200);
  const uint32_t value_color = lcd.color888(255, 255, 255);
  const uint32_t unit_color = lcd.color888(200, 200, 200);
  const int label_width = 80; // Width for the label text
  const int value_width = 110; // Width for the value text
  
  const int max_value_len = 5;
  const int value_len = value.length();
  if (value_len > max_value_len) {
    value = value.substring(0, max_value_len);
  } else if (value_len < max_value_len) {
    String padding = "";
    for(int i = 0; i < (max_value_len - value_len); i++)
      padding += " ";
    value = padding + value;
  }

  sprite.setCursor(x, y+10);
  sprite.setTextSize(label_size);
  sprite.setTextColor(label_color);
  sprite.print(label);

  sprite.setCursor(x + label_width, y);
  sprite.setTextSize(value_size);
  sprite.setTextColor(value_color);
  sprite.print(value);

  sprite.setCursor(x + label_width + value_width, y+10);
  sprite.setTextSize(unit_size);
  sprite.setTextColor(unit_color);
  sprite.print(unit);
}

int getSignalBars(long rssi) {
  if (rssi > -50) return 4;
  else if (rssi > -60) return 3;
  else if (rssi > -70) return 2;
  else if (rssi > -80) return 1;
  else return 0;
}

void drawSignalBars(){
  long rssi = WiFi.RSSI();
  int bars = getSignalBars(rssi);
  sprite_network.drawLine(0, 0, sprite_network.width(), 0, TFT_WHITE);
  // アンテナ棒を描画
  for (int i = 0; i < 4; i++) {
    int x = sprite_network.width() - 50 + i * 10;
    int h = (i+1) * 5;
    if (i < bars) {
      sprite_network.fillRect(x, 30-h, 5, h, TFT_WHITE); // 塗りつぶし
    } else {
      sprite_network.drawRect(x, 30-h, 5, h, TFT_WHITE); // 枠だけ
    }
  }
}

void loop() {
  uint16_t co2Concentration;
  float temperature;
  float relativeHumidity;
  sensor.readData(co2Concentration, temperature, relativeHumidity);
  
  sprite.clear();

  drawData(0, 35, "CO2", String(co2Concentration), "ppm");
  drawData(0, 95, "Temp", String(temperature), "'C");
  drawData(0, 155, "Humid", String(relativeHumidity), "%");

  sprite.pushSprite(0, 0);

  sprite_network.clear();

  sendDataToServer(String(co2Concentration), String(temperature), String(relativeHumidity));
  drawSignalBars();

  sprite_network.pushSprite(0, lcd.height() - network_area_height);

  delay(10*60*1000);
}
