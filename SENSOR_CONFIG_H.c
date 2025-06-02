#ifndef SENSOR_CONFIG_H
#define SENSOR_CONFIG_H

// --------------------- PZEM-004T Serial Pins (via SoftwareSerial) ---------------------
#define PZEM_RX_1 16  // GPIO16: RX for Phase 1
#define PZEM_TX_1 17  // GPIO17: TX for Phase 1

#define PZEM_RX_2 18  // GPIO18: RX for Phase 2
#define PZEM_TX_2 19  // GPIO19: TX for Phase 2

#define PZEM_RX_3 21  // GPIO21: RX for Phase 3
#define PZEM_TX_3 22  // GPIO22: TX for Phase 3

// --------------------- DS18B20 Temperature Sensor ---------------------
#define ONE_WIRE_BUS 4  // GPIO4: Data pin for DS18B20 temperature sensor

// --------------------- ADXL345 Vibration Sensor (I2C) ---------------------
#define SDA_PIN 23  // GPIO23: SDA line
#define SCL_PIN 5   // GPIO5 : SCL line

// --------------------- I2C LCD Display ---------------------
#define LCD_SDA 23  // Use same I2C bus if shared
#define LCD_SCL 5
#define I2C_ADDRESS 0x27

// --------------------- Buzzer ---------------------
#define BUZZER_PIN 2  // GPIO2: Digital pin to control buzzer

// --------------------- GSM Module ---------------------
#define GSM_RX 26  // GPIO26: RX for ESP32 (to GSM TX)
#define GSM_TX 27  // GPIO27: TX for ESP32 (to GSM RX)

#endif
