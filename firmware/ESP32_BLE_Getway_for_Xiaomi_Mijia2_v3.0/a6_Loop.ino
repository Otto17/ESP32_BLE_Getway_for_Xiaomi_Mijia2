// ********** ФУНКЦИИ LOOP ********** //


void loop() {
  esp_task_wdt_reset();            // Сброс WDT
  Uptime uptime = updateUptime();  // Получаем время работы
  ButRes.tick();                   // Тикер для обработчика кнопки (нужен чтобы корректно считались все таймауты)
  client.loop();                   // Тикер для работы MQTT

  ResetButton();  // Постоянно проверяем функцию обработчика кнопки сброса на нажатие, удержание и отпускание

  if (flagRes == true) digitalWrite(INDICATOR, (millis() / 40) & 1);  // Позволяет визуально определить когда можно отпускать кнопку сброса настроек

  // Обнаружение BLE устройств по таймеру
  static uint32_t tmr_BLE_Scan;                   // Таймер для сканирования рекламы с заданной периодичностью
  if (millis() - tmr_BLE_Scan >= SCAN_TIMEOUT) {  // Сканирование BLE рекламы раз в "SCAN_TIMEOUT" миллисекунд
    tmr_BLE_Scan = millis();                      // Обнуляем таймер

    //Выполняем сканирование BLE рекламы
    BLEScanResults* foundDevices = pBLEScan->start(SCAN_TIME, false);  // "SCAN_TIME" определяет в течение какого времени будет производиться сканирование новых устройств
    yield();                                                           // Обработка фоновых задач ESP

#if (DEBUG == 1)  // Выполняется, если отладка включена
    Serial.print("Найденые устройства: ");
    Serial.println(foundDevices->getCount());  // Узнаём кол-во найденых устройств
    Serial.println("Сканирование завершено!");
#endif
    pBLEScan->stop();          // Завершаем сканирование
    pBLEScan->clearResults();  // Удаляем результаты из буфера pBLEScan, чтобы освободить память
    esp_task_wdt_reset();      // Сброс WDT
  }

  if (client.isMqttConnected()) {  // Если MQTT подключен, то...
    // Если флаг поднят, то моргаем LED индикатором раз в 40 мс (без задержки кода)
    digitalWrite(INDICATOR, (millis() / 420) & 1);  // Периодически мигаем индиктором, оповещая, что связь с MQTT установлена

    static uint32_t tmr_MQTT_send;                // Таймер для отправки сообщений с заданной периодичностью
    if (millis() - tmr_MQTT_send >= MQTT_SEND) {  // Публикуем сообщения раз в "MQTT_SEND" миллисекунд
      tmr_MQTT_send = millis();                   // Обнуляем таймер

      // Чтение температуры ESP32
      float temp = (temprature_sens_read() - 32) / 1.8;  // Переводим в °C
      String tmpCPU = String(temp, 1);                   // Сохраняем один знак после запятой

      // Публикуем сообщения о состоянии ESP32
      // Температура ESP32 (true - последние данные не будут стираться после переподключения клиента к серверу)
      client.publish("ESP32/SYS/TempESP32", tmpCPU + "°C", true);

      // RSSI WiFi
      client.publish("ESP32/SYS/WiFi RSSI", String(WiFi.RSSI()) + " dBm", true);

      // Количество доступной кучи в памяти (в КБ)
      client.publish("ESP32/SYS/Free Heap", String(ESP.getFreeHeap() / 1024.0) + " КБ", true);

      // Время непрерывной работы ESP32
      client.publish("ESP32/SYS/Uptime", String(uptime.days) + ":" + String(uptime.hours) + ":" + String(uptime.minutes) + ":" + String(uptime.seconds), true);


      // Публикуем сообщения c датчиков Xiaomi Mijia 2
      // Датчик, который стоит на кухне
      client.publish("ESP32/Xiaomi/Kyxnya/MAC", MAC_Kyxnya, true);                          // Публикуем MAC адрес датчика с флагом сохранения  (true - последние данные не будут стираться после переподключения клиента к серверу)
      client.publish("ESP32/Xiaomi/Kyxnya/RSSI", String(RSSI_Kyxnya) + " dBm", true);       // Публикуем RSSI
      client.publish("ESP32/Xiaomi/Kyxnya/Temperature", temp_Kyxnya + "°C", true);          // Публикуем температуру
      client.publish("ESP32/Xiaomi/Kyxnya/Humidity", String(humidity_Kyxnya) + "%", true);  // Публикуем влажность
      client.publish("ESP32/Xiaomi/Kyxnya/Battery", String(vbat_Kyxnya) + "V", true);       // Публикуем напряжение батареи в Вольтах

      // Датчик, который стоит в комнате
      client.publish("ESP32/Xiaomi/Komnata/MAC", MAC_Komnata, true);                          // Публикуем MAC адрес датчика с флагом сохранения  (true - последние данные не будут стираться после переподключения клиента к серверу)
      client.publish("ESP32/Xiaomi/Komnata/RSSI", (String)RSSI_Komnata + " dBm", true);       // Публикуем RSSI
      client.publish("ESP32/Xiaomi/Komnata/Temperature", temp_Komnata + "°C", true);          // Публикуем температуру
      client.publish("ESP32/Xiaomi/Komnata/Humidity", String(humidity_Komnata) + "%", true);  // Публикуем влажность
      client.publish("ESP32/Xiaomi/Komnata/Battery", String(vbat_Komnata) + "V", true);       // Публикуем напряжение батареи в Вольтах
    }
  }
}