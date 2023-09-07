//********** СКАНЕР РЕКЛАМЫ BLE **********//


//Класс обратного вызова для поиска служебных данных от BLE датчиков Xiaomi Mijia 2
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {

  //Функция внутри класса - поиск сервисных данных
  uint8_t* findServiceData(uint8_t* data, size_t length, uint8_t* foundBlockLength) {
    uint8_t* rightBorder = data + length;
    while (data < rightBorder) {
      uint8_t blockLength = *data + 1;

      if (blockLength < 5) {
        data += blockLength;
        continue;
      }
      uint8_t blockType = *(data + 1);
      uint16_t serviceType = *(uint16_t*)(data + 2);

      if (blockType == 0x16) {  // https://www.bluetooth.com/specifications/assigned-numbers/generic-access-profile/
        /* 16-битный UUID для участников 0xFE95 Xiaomi Inc. https://btprodspecificationrefs.blob.core.windows.net/assigned-values/16-bit%20UUID%20Numbers%20Document.pdf */
        if (serviceType == 0xfe95 || serviceType == 0x181a) {  // MI или индивидуальный сервис
          *foundBlockLength = blockLength;
          return data;
        }
      }
      data += blockLength;
      yield();  // Вызываем функцию "yield()" для обработки фоновых задач ESP
    }
    yield();  // Вызываем функцию "yield()" для обработки фоновых задач ESP
    return nullptr;
  }



  // Функция внутри класса - поиск новых BLE устройств
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    char MAC_Char[18];                                                                   // Временный массив для хранения сформированного MAC адреса и последующей переконвертации в строку через sprintf().
    String MAC_Xiaomi = "";                                                              // Тут будут хранится сформированный MAC адрес в формате "FF:FF:FF:FF:FF:FF". С каждым найденным устройством строка будет менять MAC адрес на найденный
    char outstr[5];                                                                      // ТЕСТ
    uint8_t mac[6];                                                                      // Тут будем хранить полученые байты MAC адреса
    uint8_t* payload = advertisedDevice.getPayload();                                    // Получаем полезную информацию
    size_t payloadLength = advertisedDevice.getPayloadLength();                          // Получаем длину полезной информации
    uint8_t serviceDataLength = 0;                                                       // Длина сервисных данных
    uint8_t* serviceData = findServiceData(payload, payloadLength, &serviceDataLength);  // Через Класс получаем информацию об обработанных датчиках
    if (serviceData == nullptr || serviceDataLength < 15) return;                        // Если указатель нулевой или сервисные данные менее 15, то ничего не делаем
    uint16_t serviceType = *(uint16_t*)(serviceData + 2);                                // Иначе получаем сервисные данные


    //Пользовательский формат массива данных. Тип сервиса == 0x181a
    if (serviceDataLength > 18) {
      mac[5] = serviceData[4];
      mac[4] = serviceData[5];
      mac[3] = serviceData[6];
      mac[2] = serviceData[7];
      mac[1] = serviceData[8];
      mac[0] = serviceData[9];


      //sprintf(MAC_Char, "%X:%X:%X:%X:%X:%X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]); - ":%X" - маска, HEX с заглавными буквами и двоеточием. mac[x] - массив байт из которых формируем символы
      sprintf(MAC_Char, "%X:%X:%X:%X:%X:%X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);  // Конвертируем из массива байт MAC адрес по маске "FF:FF:FF:FF:FF:FF" в символьный тип Char

      MAC_Xiaomi = String(MAC_Char);  // Конвертируем Char в String для сравнения строк ниже


      //Кухня
      if (MAC_Xiaomi == SET_MAC_KYXNYA) {                                // Если отсканированный MAC адрес совпадает с заранее известным MAC адресом, то пишем полученные значения в нужные переменные
        MAC_Kyxnya = MAC_Xiaomi;                                         // Пишем MAC адрес найденного датчика в глобальную переменную
        temp_Kyxnya = *(int16_t*)(serviceData + 10) / 100.0;             // Рассчитываем температуру с датчика
        temp_Kyxnya.remove(4, 4);                                        // Обрезаем с 4 по 4 символ из строки, то есть убирает сотую часть градуса, что бы получилось к примеру так - 25,3°C, вместо 25,37°C
        humidity_Kyxnya = *(uint16_t*)(serviceData + 12) / 100;          // Рассчитываем влажность с датчика
        uint16_t int_vbat_Kyxnya = *(uint16_t*)(serviceData + 14);       // Рассчитываем напряжение батарейки с датчика как целое число в милливольтах
        vbat_Kyxnya = (int_vbat_Kyxnya + VBAT_CORRECT_KUXNUA) / 1000.0;  // Переводим напряжение батарейки в float в вольтах с 2-мя знаками после запятой

        RSSI_Kyxnya = advertisedDevice.getRSSI();  // Получаем уровень сигнала RSSI от BLE датчика Xiaomi Mijia 2

#if (DEBUG == 1)  // Выполняется, если отладка включена
        //ОТЛАДКА
        Serial.print("MAC датчика на кухне: ");
        Serial.println(MAC_Kyxnya);
        Serial.print("RSSI датчика на кухне: ");
        Serial.println(RSSI_Kyxnya);
        Serial.print("Температура на кухне: ");
        Serial.println(temp_Kyxnya);
        Serial.print("Влажность на кухне: ");
        Serial.println(humidity_Kyxnya);
        Serial.print("Напряжение батареи на кухне: ");
        Serial.println(vbat_Kyxnya);
        Serial.println();
#endif
      }


      //Комната
      if (MAC_Xiaomi == SET_MAC_KOMNATA) {                                  // Если отсканированный MAC адрес совпадает с заранее известным MAC адресом, то пишем полученные значения в нужные переменные
        MAC_Komnata = MAC_Xiaomi;                                           // Пишем MAC адрес найденного датчика в глобальную переменную
        temp_Komnata = *(int16_t*)(serviceData + 10) / 100.0;               // Рассчитываем температуру с датчика
        temp_Komnata.remove(4, 4);                                          // Обрезаем с 4 по 4 символ из строки, то есть убирает сотую часть градуса, что бы получилось к примеру так - 25,3°C, вместо 25,37°C
        humidity_Komnata = *(uint16_t*)(serviceData + 12) / 100;            // Рассчитываем влажность с датчика
        uint16_t int_vbat_Komnata = *(uint16_t*)(serviceData + 14);         // Рассчитываем напряжение батарейки с датчика как целое число в милливольтах
        vbat_Komnata = (int_vbat_Komnata + VBAT_CORRECT_KOMNATA) / 1000.0;  // Переводим напряжение батарейки в float в вольтах с 2-мя знаками после запятой

        RSSI_Komnata = advertisedDevice.getRSSI();  // Получаем уровень сигнала RSSI от BLE датчика Xiaomi Mijia 2

#if (DEBUG == 1)  // Выполняется, если отладка включена
        //ОТЛАДКА
        Serial.print("MAC датчика в комнате: ");
        Serial.println(MAC_Komnata);
        Serial.print("RSSI датчика в комнате: ");
        Serial.println(RSSI_Komnata);
        Serial.print("Температура в комнате: ");
        Serial.println(temp_Komnata);
        Serial.print("Влажность в комнате: ");
        Serial.println(humidity_Komnata);
        Serial.print("Напряжение батареи в комнате: ");
        Serial.println(vbat_Komnata);
        Serial.println();
#endif
      }
      yield();  // Вызываем функцию "yield()" для обработки фоновых задач ESP
    }
  }
};
