#include <Arduino.h>
#include <BluetoothSerial.h>
#include "endian.h"

#define X_I 36
#define Y_I 39
#define B_I 12

BluetoothSerial SerialBT;
unsigned long milliT = 0;
bool inter = false;

void interupt()
{
  // detach so the loop have time to register the press and prevent unintended multi tap.
  detachInterrupt(B_I);
  inter = true;
}

void setup()
{
  Serial.begin(115200);
  // pinMode(X_I, INPUT);
  // pinMode(Y_I, INPUT);
  pinMode(B_I, INPUT_PULLUP);
  SerialBT.begin();
  attachInterrupt(B_I, interupt, RISING);
}

void loop()
{
  union
  {
    u_int8_t b[8];
    u_int16_t w[4];
    u_int32_t i[2];
  } joyStick;

  joyStick.w[0] = analogRead(X_I);
  joyStick.w[1] = analogRead(Y_I);
  // Serial.printf("x: %x y: %x, %i, %i\n", joyStick.w[0], joyStick.w[1], joyStick.b[0], joyStick.b[1]);
  joyStick.i[0] = htobe32(joyStick.i[0]); // convert to big endian

  // SerialBT.printf("%x", joyStick.b);

  // for (int i; i < 4; i++)
  // {
  //   Serial.printf("%i", joyStick.b[i]);
  // }
  joyStick.b[4] = 0;
  // if an interupt has happend
  if (inter)
  {
    delay(1); // remove unintended multi tap
    // Serial.println(micros() - milliT);
    // milliT = micros();
    joyStick.b[4] = 1;
    inter = false;
    attachInterrupt(B_I, interupt, FALLING);
  }
  Serial.printf("%i", joyStick.b[4]);
  delay(50);
  SerialBT.write(joyStick.b, 5);
}