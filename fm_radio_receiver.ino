#include <TEA5767.h> 

#define LOW_SIGNAL 13
#define MODERATE_SIGNAL 12
#define HIGH_SIGNAL 11

#define PREV_FREQ 9
#define NEXT_FREQ 10 

#define FIRST_COLUMN 2
#define SECOND_COLUMN 3
#define THIRD_COLUMN 4
#define FOURTH_COLUMN 5

#define DS 8
#define STCP 7
#define SHCP 6

bool prevFreqPressed = false; 
bool nextFreqPressed = false; 

float fmFrequencies[25] = {
  87.5, 88.3, 89.1, 89.9, 90.7, 
  91.5, 92.3, 93.1, 93.9, 94.7, 
  95.5, 96.3, 97.1, 97.9, 98.7, 
  99.5, 100.3, 101.1, 102.7, 103.5, 
  104.3, 105.1, 105.9, 106.7, 107.5
};

byte num[10] = {
  0b00000011, 
  0b10011111, 
  0b00100101, 
  0b00001101, 
  0b10011001, 
  0b01001001, 
  0b01000001,  
  0b00011111, 
  0b00000001, 
  0b00011001  
};

TEA5767 fmReceiver = TEA5767(); 

int freqIndex = 0; 

float currentFrequency = fmFrequencies[freqIndex]; 

void displayValue(byte num, bool isDecimal) {
  int posInt = 128; 

  if (isDecimal) {
    num >>= 1; 
    num <<= 1; 
  }

  for (byte mask = 00000001; mask > 0; mask <<= 1) {
    if (!(num & mask)) {
      digitalWrite(STCP, LOW); 
      shiftOut(DS, SHCP, MSBFIRST, posInt); 
      digitalWrite(STCP, HIGH); 
    }
    posInt /= 2; 
  }
}

void display(float freq) {
  int column = 4;

  int intFreqWhole = (int)freq; 
  float floatFreqWhole = (float)intFreqWhole; 

  float floatDecimalNumber = (freq - floatFreqWhole) * 10;
  int intDecimalNumber = (int)floatDecimalNumber; 

  digitalWrite(FOURTH_COLUMN, LOW); 
  displayValue(num[intDecimalNumber], false); 
  digitalWrite(FOURTH_COLUMN, HIGH); 

  while (intFreqWhole != 0) {    
    int digit = intFreqWhole % 10; 

    digitalWrite(column, LOW); 

    if (column == 4){
      displayValue(num[digit], true); 
    } else {
      displayValue(num[digit], false); 
    }

    digitalWrite(column, HIGH);

    column--; 

    intFreqWhole /= 10;  
  }
}

void activateLight(int pos) {
  int arrPos[3] = {13, 12, 11}; 
  for (int i = 0; i <= 2; i++) {
    if (i == pos) {
      digitalWrite(arrPos[i], HIGH); 
    } else {
      digitalWrite(arrPos[i], LOW); 
    }
  }
}

void displaySignalLevel() {
  short signal = fmReceiver.getSignalLevel(); 

  if (signal <= 5) {
    activateLight(0); 
  } else if (signal >= 6 && signal <= 11) {
    activateLight(1); 
  } else if (signal >= 12 && signal <= 15) {
    activateLight(2); 
  }
}

void setup() {
  for (int i = 2; i <= 13; i++) {
    if (i == 9 || i == 10) {
      pinMode(i, INPUT); 
    } else {
      pinMode(i, OUTPUT); 
      if (i >= 2 && i <= 5) {
        digitalWrite(i, HIGH); 
      }
    }
  }

  fmReceiver.setFrequency(currentFrequency); 
  displaySignalLevel(); 
}

void loop() {
  display(fmFrequencies[freqIndex]); 

  if (digitalRead(PREV_FREQ) == 1 && digitalRead(NEXT_FREQ) == 0) {
    if (prevFreqPressed == false) {
      prevFreqPressed = true; 
      if (freqIndex == 0) {
        freqIndex = 24; 
      } else {
        freqIndex--; 
      }
      fmReceiver.setFrequency(fmFrequencies[freqIndex]); 
      displaySignalLevel(); 
    }
  } else {
    prevFreqPressed = false; 
  }

  if (digitalRead(PREV_FREQ) == 0 && digitalRead(NEXT_FREQ) == 1) {
    if (nextFreqPressed == false) {
      nextFreqPressed = true; 
      if (freqIndex == 24) {
        freqIndex = 0; 
      } else {
        freqIndex++; 
      }
      fmReceiver.setFrequency(fmFrequencies[freqIndex]); 
      displaySignalLevel(); 
    }
  } else {
    nextFreqPressed = false; 
  }
}
