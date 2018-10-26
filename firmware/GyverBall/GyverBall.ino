/*
  Скетч к проекту "Bluetooth Лабиринт"
  Страница проекта (схемы, описания): https://alexgyver.ru/GyverBall/
  Исходники на GitHub: https://github.com/AlexGyver/GyverBall
  Нравится, как написан и закомментирован код? Поддержи автора! https://alexgyver.ru/support_alex/
  Автор: AlexGyver Technologies, 2018
  http://AlexGyver.ru/
*/
// ******** НАСТРОЙКИ ********
#define SRV_X 2			// серво оси X
#define SRV_Y 3			// серво оси Y

#define START_BTN 4		// "кнопка" старт таймера
#define STOP_BTN 5		// "кнопка" стоп таймера

// ******** ДЛЯ РАЗРАБОТЧИКОВ ********
#define PARSE_AMOUNT 2    // максимальное количество значений в массиве, который хотим получить
#define header '$'        // стартовый символ
#define divider ' '       // разделительный символ
#define ending ';'        // завершающий символ

#include <Servo.h>
Servo servoX, servoY;
byte prevX, prevY;

int intData[PARSE_AMOUNT];     // массив численных значений после парсинга
boolean recievedFlag;

void setup() {
  Serial.begin(9600);
  pinMode(START_BTN, INPUT_PULLUP);
  pinMode(STOP_BTN, INPUT_PULLUP);
  servoX.attach(SRV_X);
  servoY.attach(SRV_Y);
  servoX.write(90);
  servoY.write(90);
}

void loop() {
  if (!digitalRead(START_BTN)) Serial.print(0);
  if (!digitalRead(STOP_BTN)) Serial.print(1);
  
  parsing();       				// функция парсинга
  if (recievedFlag) {			// если получены данные
    recievedFlag = false;
    byte thisX = 90 - intData[0];
    byte thisY = 90 + intData[1];
    if (prevX != thisX) {
      prevX = thisX;
      servoX.write(thisX);
    }
    if (prevY != thisY) {
      prevY = thisY;
      servoY.write(thisY);
    }    
  }
}

boolean getStarted;
byte index;
String string_convert = "";
void parsing() {
  if (Serial.available() > 0) {
    char incomingByte = Serial.read();        // обязательно ЧИТАЕМ входящий символ
    if (getStarted) {                         // если приняли начальный символ (парсинг разрешён)
      if (incomingByte != divider && incomingByte != ending) {   // если это не пробел И не конец
        string_convert += incomingByte;       // складываем в строку
      } else {                                // если это пробел или ; конец пакета
        intData[index] = string_convert.toInt();  // преобразуем строку в int и кладём в массив
        string_convert = "";                  // очищаем строку
        index++;                              // переходим к парсингу следующего элемента массива
      }
    }
    if (incomingByte == header) {                // если это $
      getStarted = true;                      // поднимаем флаг, что можно парсить
      index = 0;                              // сбрасываем индекс
      string_convert = "";                    // очищаем строку
    }
    if (incomingByte == ending) {                // если таки приняли ; - конец парсинга
      getStarted = false;                     // сброс
      recievedFlag = true;                    // флаг на принятие
    }
  }
}
