//********** НАСТРОЙКИ **********//


//Пины
#define BUT_RES 32    // Кнопки для сброса настроек ESP и перехода в режим AP
#define INDICATOR 13  // LED индикатор на плате ESP32, служит для визуального определения, когда отпускать кнопку сброса настроек


// Отладка
#define DEBUG 0  // Отладка (1 - Включить, 0 - Выключить)


//BLE
#define SCAN_TIME 5                          // Время сканирования BLE устройств в секундах (Оптимально в моём случае вышло 5 секунд для двух датчиков)
#define SCAN_TIMEOUT 10000                   // Время таймаута (в мс.), после окончания сканирования BLE устройств. То есть, сканируем BLE в течении 5 секунд каждые 10 секунд
#define SET_MAC_KOMNATA "A4:C1:38:E9:36:E4"  // MAC адрес датчика "Xiaomi Mijia 2", который установлен в комнате. Можно узнать MAC через любой BLE сканер со смарфона
#define SET_MAC_KYXNYA "A4:C1:38:83:8A:B5"   // MAC адрес датчика "Xiaomi Mijia 2", который установлен на кухне. Можно узнать MAC через любой BLE сканер со смарфона


// Корректировка напряжения батарейки для датчиков, так как показания приходят неверные, с занижением напряжения.
// Замерил напряжение батарейки точным мультиметром и отнял от него значение полученное от переменной "int_vbat_Kyxnya" и "int_vbat_Komnata", разницу вписал в коэффициент. Либо подбирать опытным путём
#define VBAT_CORRECT_KUXNUA 210   // Коэффициент для увеличения показания напряжения на эту величину
#define VBAT_CORRECT_KOMNATA 130  // Коэффициент для увеличения показания напряжения на эту величину


//Таймаут работы портала (в мс), после истичения которого портал настроек закроется и вкл. режим "WIFI_STA"
#define PORTAL_TIMEOUT 300000  // Портал закроется через 5 минут


//Настройка таймаута удержания кнопки на ESP
//Из-за сканирования BLE сетей, время удержания может увеличиться до 3-х раз, смотря в какой момент нажётся кнопка
#define SET_HOLD_TIMEOUT 3000  // Установить время удержания кнопки, мс (до 8 000). Для сброса настроек ESP


//OTA (ElegantOTA)
#define OTA_LOGIN "admin"   // Логин для доступа на страницу OTA
#define OTA_PASSWD "admin"  // Пароль для доступа на страницу OTA


//Имя хоста
#define HOSTNAME "ESP32-BLE-GATEWAY"  // Будет виден в сети с таким названием. Необходимо соблюдать правила назначения имён устройствам hostname (смотреть в интернете)
#define M_DNS "ble-gateway"           // mDNS для локальной сети (в конце автоматом подставляется ".local"). Может работать не во всех системах!


//MQTT
#define MQTT_SEND 12000  // Публикация сообщений в MQTT раз 1 раз в указанное кол-во мс.


//Указываем версию скетча, для публикации её по MQTT
#define VERSION "v2.7.1"
