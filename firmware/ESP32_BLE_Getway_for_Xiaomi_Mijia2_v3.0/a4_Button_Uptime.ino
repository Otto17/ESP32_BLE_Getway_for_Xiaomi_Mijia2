// ********** ОБРАБОТКА ФИЗИЧЕСКОЙ КНОПКИ И UPTIME ********** //


// Функция для сброса настроек
void ResetButton() {
  esp_task_wdt_reset();  // Сброс WDT
  //При долгом удержании кнопки "BUT_RES" информируем миганием LED индикатора через флаг
  if (ButRes.step()) {  // Если кнопка удерживается заданное время в "SET_HOLD_TIMEOUT", то...
    flagRes = true;     // Поднимаем флаг
#if (DEBUG == 1)        // Выполняется, если отладка включена
    Serial.println("Кнопка сброса удерживается");
#endif
  }

  //При отпускании кнопки после долгого удержания сбрасываем настроки ESP
  if (ButRes.releaseStep()) {  // Как только отпустили кнопку после долгого удержания
    flagRes = false;           // Опускаем флаг
#if (DEBUG == 1)               // Выполняется, если отладка включена
    Serial.println("Кнопка отпущена, форматируем FS...");
#endif

    LittleFS.format();  // Форматируем FS
    delay(2000);        // Ждём корректного стирания

#if (DEBUG == 1)  // Выполняется, если отладка включена
    Serial.println("Форматирование завершено, перезагружаемся");
#endif

    ESP.restart();  // Перезагружаем ESP
  }
}

// Вычисление времени непрерывной работы МК (с защитой от перепнения)
Uptime updateUptime() {
  static unsigned long previousMillis = 0;
  static unsigned long totalMillis = 0;

  unsigned long currentMillis = millis();

  // Обработка переполнения millis()
  if (currentMillis < previousMillis) {
    totalMillis += (ULONG_MAX - previousMillis) + currentMillis;
  } else {
    totalMillis += currentMillis - previousMillis;
  }
  previousMillis = currentMillis;

  // Преобразование миллисекунд в дни:часы:минуты:секунды
  unsigned long uptimeSeconds = totalMillis / 1000;
  Uptime uptime;
  uptime.days = uptimeSeconds / 86400;
  uptime.hours = (uptimeSeconds % 86400) / 3600;
  uptime.minutes = (uptimeSeconds % 3600) / 60;
  uptime.seconds = uptimeSeconds % 60;

  return uptime;
}