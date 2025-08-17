#include <Arduino.h>
#include <LGFX_ATM0130B3.h>
#include <Scd4x.hpp>

static MySCD4X sensor;
static LGFX lcd;

static LGFX_Sprite sprite(&lcd);

void setup() {
  Serial.begin(115200);
  sensor.setup();
  lcd.begin();
  lcd.setRotation(1);
  lcd.fillScreen(lcd.color565(0, 0, 0));

  sprite.setColorDepth(8);
  sprite.createSprite(lcd.width(), lcd.height());
  sprite.setBaseColor(lcd.color888(0, 0, 0));
  sprite.setTextColor(lcd.color888(255, 255, 255));
  

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
    String padding = String(':', max_value_len - value_len);
    String value_new = padding + value;
    sprite.setCursor(x + 20, y-10);
    sprite.print(value_new);
  }
  sprite.setCursor(x,y-10);
  sprite.print(value_len);

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

void loop() {
  uint16_t co2Concentration;
  float temperature;
  float relativeHumidity;
  sensor.readData(co2Concentration, temperature, relativeHumidity);
  
  sprite.clear();

  drawData(0, 50, "CO2", String(co2Concentration), "ppm");
  drawData(0, 110, "Temp", String(temperature), "'C");
  drawData(0, 170, "Humid", String(relativeHumidity), "%");

  sprite.pushSprite(0, 0);

  delay(1000);
}
