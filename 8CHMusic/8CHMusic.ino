/*
   Copyright 2017, OYMotion Inc.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
   OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
   AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
   OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
   THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
   DAMAGE.

*/
#include "pitches.h"
#include "TimerThree.h"

#define CH             8
#define TIMER_FREQ     50000/*HZ*/
#define FREQ_TO_CNT(f) (int)((float)TIMER_FREQ / f + 0.5)

TimerThree timer3;

int pinLevel = 0;
int count[CH] = {0};
bool musicEnable[CH];
int countMax[CH] =
{
  FREQ_TO_CNT(NOTE_C4),
  FREQ_TO_CNT(NOTE_D4),
  FREQ_TO_CNT(NOTE_E4),
  FREQ_TO_CNT(NOTE_F4),
  FREQ_TO_CNT(NOTE_G4),
  FREQ_TO_CNT(NOTE_A4),
  FREQ_TO_CNT(NOTE_B4),
  FREQ_TO_CNT(NOTE_C5)
};

const int ledPin = 13;

void pinOutControl();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  for (int i = 2; i < 10; i++)
  {
    pinMode(i, OUTPUT);
  }
  //Serial.print("ok");
  Timer3.initialize();
  Timer3.attachInterrupt(timerCallBack, 1E6 / TIMER_FREQ/* freq to us*/);
}

void loop() {
  if (Serial.available() > 0)
  {
    int n;

    int chData = Serial.read();
    //Serial.print(ch);

    if (chData != -1)
    {
      for (n = 0; n < CH; n++)
      {
        musicEnable[n] = ((chData & (0x01 << n)) != 0);
      }

      pinLevel = 1 - pinLevel;
      digitalWrite(ledPin, pinLevel);
    }
  }
}

void timerCallBack()
{
  int *pCnt = count;
  int *pCountMax = countMax;
  int pin = 2;
  bool *pEnb = musicEnable;

  for (int i = 0; i < CH; i++)
  {
    (*pCnt)++;

    if ((*pCnt) >= (*pCountMax))
    {
      *pCnt = 0;

      if (*pEnb)
      {
        digitalWrite(pin, HIGH);
      }
    }
    else if (*pCnt == (*pCountMax >> 1))
    {
      digitalWrite(pin, LOW);
    }

    pCnt++;
    pCountMax++;
    pEnb++;
    pin++;
  }
}

