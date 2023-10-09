//********** ФУНКЦИЯ SETUP **********//


void setup() {
  pinMode(BUT_RES, INPUT);       // Кнопка сброса настроек ESP
  pinMode(INDICATOR, OUTPUT);    // LED индикатор
  digitalWrite(INDICATOR, LOW);  // По умолчанию LED индикатор выключен

#if (DEBUG == 1)         // Выполняется, если отладка включена
  Serial.begin(115200);  // Инициализируем Serial
  delay(3000);           // Что бы успеть открыть монитор порта для получения самых первых данных
  Serial.println("ESP32 запустилась");
#endif

  Connect_WiFi_MQTT();  // Функция подключения к WiFi и MQTT

  ButRes.setHoldTimeout(SET_HOLD_TIMEOUT);  // Устанавливаем время удержания для кнопки cброса настроек

  //OTA
  client.enableHTTPWebUpdater(OTA_LOGIN, OTA_PASSWD, "/");  // Включить OTA WEB обновления. Логин, Пароль, Адрес по которому доступено OTA

  //BLE
  BLEDevice::init("");                                                              // Инициализируем BLE
  pBLEScan = BLEDevice::getScan();                                                  // Создаём новое сканирование
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(), true);  // Устанавливаем обратные вызовы для нового покаживызова
  pBLEScan->setInterval(625);                                                       // Установка интервала рекламы (по умолчанию 100)
  pBLEScan->setWindow(625);                                                         // Окно сканирования. Меньше или равно значению setInterval (по умолчанию 99)
  pBLEScan->setActiveScan(true);                                                    // Устанавливаем активный режим сканирования, позволяет быстрее получить результаты сканирования

  wdt.begin();  // Запускаем сторожевой таймер
}
