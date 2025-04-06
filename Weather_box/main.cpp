#include <Arduino.h>
// #include <GRGB.h>
#include <SPI.h>
#include "U8g2lib.h"
#include <GyverBME280.h>
#include <MHZ19_uart.h>



// // Button definitions using EncButton library
#define EB_NO_FOR
#define EB_NO_CALLBACK
#define EB_NO_COUNTER
#define EB_NO_BUFFER
#define EB_DEB_TIME 30    // Button debounce timeout 50
#define EB_CLICK_TIME 300 // Click wait timeout 500
#define EB_HOLD_TIME 600  // Hold timeout
#define EB_STEP_TIME 200  // Pulse hold timeout
#include <EncButton.h>

#include "weather_station.h"
const char* timeZone = "Europe/Belgrade";
#include "get_real_time_api.h"


// #define R_PIN 9 // for RGB led
// #define G_PIN 10 // for RGB led
// #define B_PIN 11 // for RGB led


// display
#define DISPLAY_SCL 18
#define DISPLAY_SI 23
#define DISPLAY_CS 5
#define DISPLAY_RS 15
#define DISPLAY_RSE 4
#define DISPLAY_BACKLIGHT 13 

#define SCL 22 // for BME280 
#define SDA 21 // for BME280 
#define RX_PIN 16 // BUT "REAL" WIRE FROM RX CONNECTED TO PIN 17!!!
#define TX_PIN 17 // BUT "REAL" WIRE FROM TX CONNECTED TO PIN 16!!!
#define BTN_NEXT_SCREEN_PIN 34 // btn_next_screen
#define BTN_UPDATE_PIN 35 // btn_update

// GRGB led(COMMON_CATHODE, R_PIN, G_PIN, B_PIN);
U8G2_ST7565_JLX12864_F_4W_SW_SPI display(U8G2_R0, DISPLAY_SCL, DISPLAY_SI, DISPLAY_CS, DISPLAY_RS, DISPLAY_RSE);
GyverBME280 bme;
MHZ19_uart mhz19;
Button btn_next_screen(BTN_NEXT_SCREEN_PIN);
Button btn_update(BTN_UPDATE_PIN);

// Timers initialization
uint32_t last_sensor_read_time = 0;
uint32_t last_display_blink_time = 0;
#define DISPLAY_TIME_BLINK_INTERVAL 1000
const uint32_t SENSOR_READ_INTERVAL = 5000;//1800000; // every half hour

// display
uint8_t display_font_w = 6; // u8g2_font_profont11_mf
uint8_t display_font_h = 11; // u8g2_font_profont11_mf
uint8_t current_screen = 0; // can be changed for screen debug
// uint8_t display_font_size = 1; 

void setup() {
  randomSeed(analogRead(A0));
  Serial.begin(9600);

  display.begin();
  display.clearBuffer();                  
  ledcSetup(0, 5000, 8);           // Канал 0, 5 кГц, 8 бит
  ledcAttachPin(DISPLAY_BACKLIGHT, 0);
  ledcWrite(0, 94);               // Яркость 0–255 (50%)
  // display.setContrast(255);
  // display.setFont(display_font_5x7_tf); // w = 5, h = 7
  display.setFont(u8g2_font_profont10_mf); // w = 5, h = 10
  // display.setFont(u8g2_font_profont11_mf); // w = 6, h = 11
  display_font_w = 5;
  display_font_h = 10;
  display.setCursor(CURSOR_COLUMN_INDEX(0), CURSOR_ROW_INDEX(0));
  display.print("[initializing...");
  display.sendBuffer();

  display.setCursor(CURSOR_COLUMN_INDEX(0), CURSOR_ROW_INDEX(1));
  if (!bme.begin(0x76)){
    Serial.println("BME280 Error!");
    display.print("[1/5]BME280 Error!");
  } else {
    display.print("[1/5]BME280 is fine.");
  }
  display.sendBuffer();

  mhz19.begin(RX_PIN, TX_PIN);
  mhz19.setAutoCalibration(false);
  delay(1000);
  // Attempt to read CO₂ concentration
  display.setCursor(CURSOR_COLUMN_INDEX(0), CURSOR_ROW_INDEX(2));
  int16_t co2 = mhz19.getCO2PPM();
  if (co2 >= 400 && co2 <= 5000) {
      Serial.println("MH-Z19B is fine.");
      display.print("[2/5]MH-Z19B is fine.");
  } else {
      Serial.println("MH-Z19B not detected or not responding.");
      display.print("[2/5]MH-Z19B Error!");
  }
  display.sendBuffer();

  display.setCursor(CURSOR_COLUMN_INDEX(0), CURSOR_ROW_INDEX(3));
  if (!connectToWiFi()){
    Serial.println("WiFI Error!");
    display.print("[3/5]WiFI Error!");
  } else {
    display.print("[3/5]WiFI is connected.");
  }
  display.sendBuffer();


  String currentTime;
  display.setCursor(CURSOR_COLUMN_INDEX(0), CURSOR_ROW_INDEX(4));
  if (!getTimeFromAPI(currentTime)) {
    Serial.println("Cur. time API Error!");
    display.print("[4/5]Time API Error!");
  } else {
    Serial.println("Time: " + currentTime);
    display.print("[4/5]Got time via API.");
  }  
  display.sendBuffer();

  initHistoryBuffers();
  readTempAndHumInside(); // must be after initHistoryBuffers!!!
  delay(2000);
  display.clear();
  // readCO2(); MH-z19b must warm up! So, the first measurement will be in SENSOR_READ_INTERVAL milsec
  setMainScreen();
}

void loop() {
  uint32_t now = millis();

  // Handle button inputs
  btn_next_screen.tick();
  if (btn_next_screen.click()) {
    Serial.println("btn_next_screen");
  }
  //   changeScreen();

  btn_update.tick();
  if (btn_update.click()) {
    Serial.println("btn_update");
  //   switch (current_screen)
  //   {
  //   case 5:
  //     //CO2genRandomValue(); //for debug
  //     drawHistoryGraph(getCO2_HoursAgo, 2000, 400);
  //     break;
  //   case 6:
  //     //TempInGenRandomValue(); //for debug
  //     drawHistoryGraph(getTempIn_HoursAgo, 30, -10);  
  //     break;
  //   }
  //   // setRGBColor(200, 200, 200); //for RGB debug
  }

  // Blink time colon on main screen
  // if ((current_screen == 0) && (now - last_display_blink_time >= DISPLAY_TIME_BLINK_INTERVAL))
  // {
  //   last_display_blink_time = now;
  //   blinkTimeSeparator();
  // }

  // Read sensors at regular intervals
  if (now - last_sensor_read_time >= SENSOR_READ_INTERVAL)
  {
    last_sensor_read_time = now;
    readTempAndHumInside();
    readCO2();
  }
}