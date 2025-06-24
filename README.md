# ESP32 Data Logger with Web Dashboard

This project logs:
- 🌡️ Temperature & Humidity from DHT11
- 🌡️ Internal ESP32 temperature
- 💾 Saves data to SD Card with time stamps
- 🌐 Shows live data on Wi-Fi dashboard

## Hardware
- ESP32 Dev Board
- DHT11 Sensor
- SD Card Module
- Micro SD Card
- Power Source (e.g., Power Bank)

## Wiring
| ESP32 | DHT11/SD Card        |
|-------|----------------------|
| D4    | DHT11 Data Pin       |
| D5    | SD Card CS           |
| GND   | Common Ground        |
| 3.3V  | DHT11 + SD Power     |

## Sample Output
#1 24/06/2025 17:00:00 → Temp: 29.3°C, Humidity: 60%, ESP32 Temp: 44.5°C



