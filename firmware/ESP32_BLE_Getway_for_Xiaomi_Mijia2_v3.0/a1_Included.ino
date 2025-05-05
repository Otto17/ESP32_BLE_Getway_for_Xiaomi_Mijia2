// ********** ПОДКЛЮЧЕНИЕ БИБЛИОТЕК, ОБЪЕКТОВ, КОНСТАНТ И ГЛОБАЛЬНЫХ ПЕРЕМЕННЫХ********** //


#include <BLEDevice.h>              // Библиотека для работы с BLE устройствами [в ядре ESP32]
#include <BLEUtils.h>               // Библиотека для работы с BLE [в ядре ESP32]
#include <BLEScan.h>                // Библиотека для сканирования BLE устройств [в ядре ESP32]
#include <BLEAdvertisedDevice.h>    // Библиотека для сканирования BLE рекламы от устройств [в ядре ESP32]
#include <WiFi.h>                   // Библиотека  для работы с WiFi [в ядре ESP32]
#include <WiFiClient.h>             // Библиотека  для работы с WiFi [в ядре ESP32]
#include <WebServer.h>              // Библиотека веб-сервера [в ядре ESP32]
#include <ESPmDNS.h>                // Библиотека многоадресного протокола DNS для установки домена в локальной сети [в ядре ESP32]
#include <LittleFS.h>               // Библиотека для работы с файловой системой LittleFS для ESP32 [в ядре ESP32]
#include "esp_task_wdt.h"           // Подключаем встроенный WDT ESP32 [в ядре ESP32]
#include <FileData.h>               // Замена EEPROM для ESP8266/32 для хранения любых данных в файлах "https://github.com/GyverLibs/FileData" [используется ver. 1.0.3]
#include <EncButton.h>              // Библиотека для работы с кнопками "https://github.com/GyverLibs/EncButton" [используется ver. 3.7.3]
#include <MOD_SimplePortal-MQTT.h>  // ФОРК библиотеки "SimplePortal". Простой WiFi менеджер с возможностью задания параметром для подключения к MQTT серверу "https://github.com/Otto17/MOD_SimplePortal-MQTT" [используется ver. 0.2]
#include <EspMQTTClient.h>          // Библиотека для обработки MQTT и WiFi для ESP8266 и ESP32 "https://github.com/plapointe6/EspMQTTClient" [используется ver. v1.13.3]



/* СТРУКТУРЫ */

// Uptime
struct Uptime {
  int days = 0;
  int hours = 0;
  int minutes = 0;
  int seconds = 0;
};

// Прототип функции вычисления времени непрерывной работы МК
Uptime updateUptime();



/* ГЛОБАЛЬНАЯ ИНИЦИАЛИЗАЦИЯ ОБЪЕКТОВ */

//FileData для хранения конфиг файла с настройками в FS
FileData data(&LittleFS, "/config.cfg", 'S', &portalCfg, sizeof(portalCfg));  // FileData data(файловая система, "путь и имя файла", 'ключ первой записи', ссылка на массив/структуру/класс, размер переменной)

//BLE
BLEScan* pBLEScan;  // Объявляем указатель на класс "BLEScan"

//Кнопка cброса настроек ESP
Button ButRes(BUT_RES, INPUT, LOW);  // Создаём объект "ButRes" класса "Button", "BUT_RES" - имя дерективы кнопки. Button b(pin, режим пина кнопки, ровень кнопки);

//MQTT
EspMQTTClient client;  //  Создаём объект "client" класса "EspMQTTClient"



/* ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ */

//BLE
//Кухня
String MAC_Kyxnya;    // MAC адрес датчика
int RSSI_Kyxnya;      // Дальность до датчика в децибел-милливаттах (дБм/dBm)
String temp_Kyxnya;   // Температура с датчика "Xiaomi Mijia 2"
int humidity_Kyxnya;  // Влажность с датчика "Xiaomi Mijia 2"
float vbat_Kyxnya;    // Напряжение (в вольтах с 3-мя знаками после запятой) с датчика "Xiaomi Mijia 2"

//Комната
String MAC_Komnata;    // MAC адрес датчика
int RSSI_Komnata;      // Дальность до датчика в децибел-милливаттах (дБм/dBm)
String temp_Komnata;   // Температура с датчика "Xiaomi Mijia 2"
int humidity_Komnata;  // Влажность с датчика "Xiaomi Mijia 2"
float vbat_Komnata;    // Напряжение (в вольтах с 3-мя знаками после запятой) с датчика "Xiaomi Mijia 2"

//Для сброса настроек
bool flagRes = false;  // Флаг для быстрого мигания LED индикатором на модуле ESP при сбросе настроек


//Измерение внутренней температуры чипа ESP32
// Предпроцессорная директива для корректного подключения C-функции в C++ коде.
// Указывает компилятору, что функция "temprature_sens_read()" написана на языке C, чтобы обеспечить правильное связывание и избежать name mangling в C++.
#ifdef __cplusplus
extern "C" {
#endif
  uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
