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
  static uint32_t last_millis = 0;
  uint32_t delta = millis() - last_millis;
  last_millis = millis();

  static Uptime uptime;
  uptime.seconds += delta / 1000;

  // Конвертация секунд в дни:часы:минуты
  uptime.minutes = uptime.seconds / 60;
  uptime.hours = uptime.minutes / 60;
  uptime.days = uptime.hours / 24;

  uptime.seconds %= 60;
  uptime.minutes %= 60;
  uptime.hours %= 24;

  return uptime;
}