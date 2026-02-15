#include <Arduino.h>
#include <U8g2lib.h>
#include "DHT.h"
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#include <Wire.h>
#endif

#define DHTPIN 2
#define DHTTYPE DHT11

U8G2_SH1106_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, SCL, SDA, U8X8_PIN_NONE);
DHT dht(DHTPIN, DHTTYPE);

float temp = 0;
int change = 0;
float ratio = 0;
float pwm =0;

void setup(void) {
  u8g2.begin();
  Serial.begin(9600);
  drawBackground();
  dht.begin();
}

//clears screen and draws white background with black arc and tik marks
void drawBackground(){
  u8g2.clearBuffer();
  u8g2.setDrawColor(1);
  u8g2.drawBox(0,0,128,64);
  u8g2.setDrawColor(0);
  u8g2.drawArc(63, 100, 80, 28, 100);
  u8g2.drawLine(2, 51, 8, 60);
  u8g2.drawLine(124, 51, 116, 60);
  u8g2.drawLine(63, 22, 63, 29);
  u8g2.sendBuffer();
}

// draws F functions draw the degrees farenheit in the middle as well as the right and left values
void drawF80_100(){
  u8g2.setDrawColor(0);
  u8g2.setFont(u8g2_font_unifont_t_symbols);
  u8g2.drawGlyph(55,48, 176);
  u8g2.drawStr(62,48,"F");
  u8g2.drawStr(4, 62, "80");
  u8g2.drawStr(100,62, "100");
  u8g2.sendBuffer();
}
void drawF60_80(){
  u8g2.setDrawColor(0);
  u8g2.setFont(u8g2_font_unifont_t_symbols);
  u8g2.drawGlyph(55,48, 176);
  u8g2.drawStr(62,48,"F");
  u8g2.drawStr(4, 62, "60");
  u8g2.drawStr(106,62, "80");
  u8g2.sendBuffer();
}
void drawF40_60(){
  u8g2.setDrawColor(0);
  u8g2.setFont(u8g2_font_unifont_t_symbols);
  u8g2.drawGlyph(55,48, 176);
  u8g2.drawStr(62,48,"F");
  u8g2.drawStr(4, 62, "40");
  u8g2.drawStr(106,62, "60");
  u8g2.sendBuffer();
}
void drawF20_40(){
  u8g2.setDrawColor(0);
  u8g2.setFont(u8g2_font_unifont_t_symbols);
  u8g2.drawGlyph(55,48, 176);
  u8g2.drawStr(62,48,"F");
  u8g2.drawStr(4, 62, "20");
  u8g2.drawStr(106,62, "40");
  u8g2.sendBuffer();
}
void drawF0_20(){
  u8g2.setDrawColor(0);
  u8g2.setFont(u8g2_font_unifont_t_symbols);
  u8g2.drawGlyph(55,48, 176);
  u8g2.drawStr(62,48,"F");
  u8g2.drawStr(4, 62, "0");
  u8g2.drawStr(106,62, "20");
  u8g2.sendBuffer();
}

//function to decide what temp values to display
//uses change value to determine if screen needs to be redrawn
void displayTemp(){
  if (temp >=80 and temp <= 100){
    if(change != 0){
      change = 0;
      drawBackground();
    }
    drawF80_100();
  }
  if (temp >=60 and temp < 80){
    if(change != 1){
      change = 1;
      drawBackground();
    }
    drawF60_80();
  }
  if (temp >=40 and temp < 60){
    if(change != 2){
      change = 2;
      drawBackground();
    }
    drawF40_60();
  }
  if (temp >=20 and temp < 40){
    if(change != 3){
      change = 3;
      drawBackground();
    }
    drawF20_40();
  }
  if (temp >=0 and temp < 20){
    if(change != 4){
      change = 4;
      drawBackground();
    }
    drawF0_20();
  }
}

//function to calculate needle value from temp
// calculates ratio which is the percentage of the needle. 1.00 is full
void temp2Needle(){
  // with no resistor, 8.5 is start, 17 is half, 23.5 is max
  //24.74
  if (change == 0){
    ratio = ((float)temp - 80.0)/20.0;
  }
  if (change == 1){
    ratio = ((float)temp - 60.0)/20.0;
  }
  if (change == 2){
    ratio = ((float)temp - 40.0)/20.0;
  }
  if (change == 3){
    ratio = ((float)temp - 20.0)/20.0;
  }
  if (change == 4){
    ratio = (float)temp/20.0;
  }
  int intRatio = 100 * ratio;
  if (intRatio >= 50){
    pwm = map(intRatio, 50, 95, 17, 23.5);
  }
  if (intRatio < 50){
    pwm = map(intRatio, 0, 50, 8.5, 17);
  }
  analogWrite(4,pwm);

}


void loop(void) {
  temp = dht.readTemperature(true);
    if (isnan(temp)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  displayTemp();
  temp2Needle();
  Serial.println(change);
  Serial.println(temp);
  Serial.println(ratio);
  Serial.println(pwm);
  Serial.println("-----");
  delay(1000);
}
