# [ESP32] Weather box


An Arduino-based weather display project using ESP32, CO₂ sensor (MH-Z19B), BME280, and a graphic LCD. Also fetches real-time from NTP or HTTPS API (`timeapi.io`) and shows current time, date, and weekday.

## Features

- 🕒 Real-time clock via NTP or HTTPS API
- 🌡️ BME280 for temperature and humidity
- 🫁 MH-Z19B for indoor CO₂ monitoring
- 🖥️ U8g2 graphics display (128x64 LCD)
- 📊 Historical graphs for temperature, humidity, and CO₂
- 📦 Wi-Fi support for internet time sync
- ⚠️ Alerts on high CO₂ levels with RGB feedback

## Hardware

- ESP32 DevKit v1
- BME280
- MH-Z19B
- LCD 128x64 (ST7565 via SPI)
- 2x push buttons
- RGB LED (optional)

## Libs (included)
GyverBME280@^1.5.3: https://github.com/GyverLibs/GyverBME280
EncButton@^3.7.2: https://github.com/GyverLibs/EncButton
GyverOLED@^1.6.4: https://github.com/GyverLibs/GyverOLED
mhz19_uart@^0.31: https://github.com/nara256/mhz19_uart
GRGB@^1.4.1: https://github.com/GyverLibs/GRGB

## Wiring

| Component | ESP32 Pin |
|----------|-----------|
| BME280   | SDA: 21, SCL: 22 |
| MH-Z19B  | RX: 17, TX: 16 |
| LCD (SPI)| SCL: 18, SI: 23, CS: 5, RS: 15, RSE: 4 |
| Buttons  | 34, 35 |
| Backlight| 13 |

> ⚠️ MH-Z19 sensor connections are logically swapped in code (TX == 17, RX == 16)!

## `secrets.h` Example

```cpp
const char* ssid = "YourNetwork";
const char* password = "YourPassword";
const char* timeZone = "CET-1CEST,M3.5.0/2,M10.5.0/3"; // Belgrade
const char* rootCACert = R"rawliteral(
-----BEGIN CERTIFICATE-----
... your CA certificate here ...
-----END CERTIFICATE-----
)rawliteral";
```
> ⚠️ Don’t forget to add secrets.h to your .gitignore.

## Controls
Button 1: Change screen

Button 2: Manual data update

Button 3: (optional) Future use

## Screens
* 📊 Main screen: time, date, indoor temp/humidity
* 🌤 Weather forecast (optional)
* 📈 Graphs: CO₂, Temp, Humidity (24h)

## Screenshots
