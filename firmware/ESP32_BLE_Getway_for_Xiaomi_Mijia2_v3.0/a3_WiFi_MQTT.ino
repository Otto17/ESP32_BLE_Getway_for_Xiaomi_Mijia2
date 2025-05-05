// ********** ФУНКЦИЯ НАСТРОЙКИ WiFi и MQTT ********** //


void Connect_WiFi_MQTT() {
  // Инициализируем FS
  if (!LittleFS.begin(true)) {
#if (DEBUG == 1)  // Выполняется, если отладка включена
    Serial.println("Ошибка инициализации FS, пробуем ещё раз...");
#endif
    delay(2000);        // Немного ждём и повторяем попытку
    while (1) yield();  // Ждём, пока FS не инициализируется
  }


  //При первом запуске в конфиг запишутся данные из структуры
  FDstat_t stat = data.read();  // Считываем данные из конфига с FS в переменную


#if (DEBUG == 1)  // Выполняется, если отладка включена
  //ОТЛАДКА
  switch (stat) {
    case FD_FS_ERR:
      Serial.println("Ошибка FS");
      break;
    case FD_FILE_ERR:
      Serial.println("Ошибка открытия конфига");
      break;
    case FD_WRITE:
      Serial.println("СОЗДАНИЕ пустого конфига");
      break;
    case FD_ADD:
      Serial.println("ДОБАВЛЕНИЕ данных в конфиг");
      break;
    case FD_READ:
      Serial.println("ЧТЕНИЕ данных из конфига");
      break;
  }
#endif


  // Если при старте ESP: нажата кнопка сброса настроек или FS отформатирована и создался конфиг / ошибка открытия конфига / пустые строки с SSID или MQTT, то...
  if (!digitalRead(BUT_RES) || stat == FD_WRITE || stat == FD_FILE_ERR || String(portalCfg.ssid) == "" || String(portalCfg.loginMQTT) == "") {
#if (DEBUG == 1)  // Выполняется, если отладка включена
    Serial.print("Сбрасываем ключ: ");
    Serial.println(data.reset());  // Сбрасываем ключ и выводим в Serial
#else
    data.reset();  // Сбрасываем ключ
#endif
    portalRun(PORTAL_TIMEOUT);  // Запускаем портал настроек WiFi/MQTT с таймаутом "PORTAL_TIMEOUT"
  }

  //Получение данных с портала и сохранение их в конфиг файл в FS
  if (portalStatus() == SP_SUBMIT) {  // Если кнопка "Сохранить" с портала нажата, то получаем данные с формы через структуру
#if (DEBUG == 1)                      // Выполняется, если отладка включена
    Serial.print("Данные с портала записаны в конфиг: ");
    Serial.println(data.write());  // Записываем структуру в файл конфига и выводим в Serial
#else
    data.write();  // Записываем структуру в файл конфига
#endif
    WiFi.softAPdisconnect();  // Отключаем режим AP
  }


#if (DEBUG == 1)  // Выполняется, если отладка включена
  //ОТЛАДКА
  Serial.print("Размер Структуры: ");
  Serial.println(sizeof(portalCfg));
  Serial.print("SSID с портала: ");
  Serial.println(portalCfg.ssid);
  Serial.print("Pass WiFi с портала: ");
  Serial.println(portalCfg.pass);
  Serial.print("Адрес брокера: ");
  Serial.println(portalCfg.brokerMQTT);
  Serial.print("Порт: ");
  Serial.println(portalCfg.portMQTT);
  Serial.print("Логин MQTT: ");
  Serial.println(portalCfg.loginMQTT);
  Serial.print("Пароль MQTT: ");
  Serial.println(portalCfg.passMQTT);
  Serial.print("ID клиента: ");
  Serial.println(portalCfg.IDClientMQTT);


  //Подключаемся к WiFi
  Serial.print("Подключение к: ");
  Serial.println(portalCfg.ssid);
#endif
  WiFi.mode(WIFI_STA);                         // Запускаем WiFi в режиме "Станция"
  WiFi.setHostname(HOSTNAME);                  // Задаём имя хоста
  WiFi.begin(portalCfg.ssid, portalCfg.pass);  // Берём данные из структуры
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
#if (DEBUG == 1)  // Выполняется, если отладка включена
    Serial.print(".");
#endif
  }
#if (DEBUG == 1)  // Выполняется, если отладка включена
  Serial.println();
  Serial.print("Подключено! Локальный IP: ");
  Serial.println(WiFi.localIP());

  client.enableDebuggingMessages();  // Включаем отладку для MQTT
#endif

  client.setMqttClientName(portalCfg.IDClientMQTT);                                                         // Установка ID клиента MQTT
  client.setMqttServer(portalCfg.brokerMQTT, portalCfg.loginMQTT, portalCfg.passMQTT, portalCfg.portMQTT);  // Запуск подключения к серверу MQTT
}



//Функция для обработки MQTT
//Эта функция ОБЯЗАТЕЛЬНО должна присутствовать, если используется библиотека "EspMQTTClient", хоть даже и будет пустой!
void onConnectionEstablished() {  // Возвращает количество раз, когда onConnectionEstablished вызывался с самого начала.

  //Преобразования адресов в удобно читаемый для человека формат для разовой пубикации по MQTT после установления соединения
  // IP адрес в String
  char ip_Dec[16];                                                                                             // IP адрес занимает 15 символов + 1 нулевой '\0' символ в конце строки
  sprintf(ip_Dec, "%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);  // Конвертируем из массива байт по маске "255:255:255:255" в числовом тип Char
  String ip_addr = String(ip_Dec);                                                                             // Конвертируем из Char в String

  // Маска подсети в String
  char subnetMsk_Dec[16];                                                                                                         // Маска подсети занимает 15 символов + 1 нулевой '\0' символ в конце строки
  sprintf(subnetMsk_Dec, "%d.%d.%d.%d", WiFi.subnetMask()[0], WiFi.subnetMask()[1], WiFi.subnetMask()[2], WiFi.subnetMask()[3]);  // Конвертируем из массива байт по маске "255:255:255:255" в числовом тип Char
  String subnMsk = String(subnetMsk_Dec);                                                                                         // Конвертируем из Char rsв String

  // Шлюз в String
  char getway_Dec[16];                                                                                                     // Шлюз занимает 15 символов + 1 нулевой '\0' символ в конце строки
  sprintf(getway_Dec, "%d.%d.%d.%d", WiFi.gatewayIP()[0], WiFi.gatewayIP()[1], WiFi.gatewayIP()[2], WiFi.gatewayIP()[3]);  // Конвертируем из массива байт по маске "255:255:255:255" в числовом тип Char
  String getway = String(getway_Dec);                                                                                      // Конвертируем из Char в String

  // DNS в String
  char dns_Dec[16];                                                                                     // DNS занимает 15 символов + 1 нулевой '\0' символ в конце строки
  sprintf(dns_Dec, "%d.%d.%d.%d", WiFi.dnsIP()[0], WiFi.dnsIP()[1], WiFi.dnsIP()[2], WiFi.dnsIP()[3]);  // Конвертируем из массива байт по маске "255:255:255:255" в числовом тип Char
  String dns = String(dns_Dec);                                                                         // Конвертируем из Char в String


  // Статические публикации (отправляются единожды после успешного подключения), true - последние данные не будут стираться после переподключения клиента к серверу
  client.publish("ESP32/SYS/Local IP", ip_addr, true);             // Публикуем IP адрес ESP
  client.publish("ESP32/SYS/MAC ESP", WiFi.macAddress(), true);    // Публикуем MAC адрес ESP
  client.publish("ESP32/SYS/Hostname", WiFi.getHostname(), true);  // Публикуем имя хоста ESP
  client.publish("ESP32/SYS/Subnet", subnMsk, true);               // Публикуем маску подсети
  client.publish("ESP32/SYS/Gateway", getway, true);               // Публикуем шлюз
  client.publish("ESP32/SYS/DNS", dns, true);                      // Публикуем DNS

  client.publish("ESP32/SYS/Freq. CPU", String(ESP.getCpuFreqMHz()) + " МГц", true);                       // Публикуем частоту процессора в МГц
  client.publish("ESP32/SYS/Flash Chip Speed", String(ESP.getFlashChipSpeed() / 1000000) + " МГц", true);  // Публикуем частоту FLASH памяти, в МГц
  client.publish("ESP32/SYS/Flash Size", String(ESP.getFlashChipSize() / 1024) + " КБ", true);             // Публикуем размер флеш памяти в Килобайтах

  client.publish("ESP32/SYS/Chip Model", String(ESP.getChipModel()), true);        // Публикуем идентификатор чипа ESP
  client.publish("ESP32/SYS/Chip Revision", String(ESP.getChipRevision()), true);  // Публикуем номер ревизии чипа ESP
  client.publish("ESP32/SYS/Sdk Version", ESP.getSdkVersion(), true);              // Публикуем версию "ядра" платы в Arduino IDE

  client.publish("ESP32/SYS/Router MAC", WiFi.BSSIDstr(), true);                                     // Публикуем MAC адрес роутера, к которому подключились
  client.publish("ESP32/SYS/Time-Date Compile", String(__DATE__) + " в " + String(__TIME__), true);  // Публикуем дату и время компиляции скетча через макросы
  client.publish("ESP32/SYS/Firmvare", VERSION, true);                                               // Публикуем версию пршивки

  // Подписка на тему перезагрузки
  client.subscribe("ESP32/SYS/Reboot", handleRebootCommand);
}

// Функция-обработчик команды перезагрузки
void handleRebootCommand(const String &message) {
  // Любое сообщение считается командой, можно проверить содержимое
  if (message.equalsIgnoreCase("reboot")) {
#if DEBUG == 1
    Serial.println("MQTT: получена команда перезагрузки ESP32, перезагружаем...");
#endif
    ESP.restart();  // Перезагружаем ESP
  }
}