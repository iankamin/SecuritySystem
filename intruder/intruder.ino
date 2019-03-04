
#include <Keypad.h>
#include <EEPROM.h>
#include <Timer.h>


bool lockState=false;
char saved_code[4];
char inprogress_code[4];
int currentNumber = 0;

const int sensorPin = 11;
const int buzzerPin = 10;
const int vibratePin = A5;
const int GREEN = 12;
const int RED = 13;


const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2}; 

Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
unsigned long sensorConnectionHistory = 0;
bool sensorBroken;

unsigned long duration = 1000;
unsigned long last = 0;
int state = 0;

Timer t;

void setup(){
  Serial.begin(9600);
  //keypad.addEventListener(buttonPress);

  pinMode(sensorPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  pinMode(vibratePin, OUTPUT);
  
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);

  EEPROM.write(99, 0);
  if (EEPROM.read(99) == 0x3C) {
    for (int i = 0; i < 4; ++i){
        saved_code[i] = EEPROM.read(i);
    }
    lockState = true;
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, LOW);
  }
  else {
    for (int i = 0; i < 4; ++i) {
      saved_code[i] = 0;
    }
    lockState = false;
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, HIGH);
  }
}
  
void loop() { 
  //digitalWrite(vibratePin, HIGH);
  //Serial.print(lockState);
  updateSensor();
  buttonPress();
  checkLock();
}

// what happens when any button is pressed
void buttonPress(){
  char key = keypad.getKey();
  Serial.print(key);
  t.update();
  switch (key){
      case '#': 
      
        break;
      case '*': 
     
      break;
      case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case '0':
        inprogress_code[currentNumber] = key;
        
        Serial.println();
        Serial.println(currentNumber);
        currentNumber++;    
        Serial.println(sizeof(saved_code));
        Serial.print("state = ");
        Serial.println(lockState);

          t.pulse(vibratePin, 100, LOW);

        if (currentNumber >= sizeof(saved_code)){
          if (lockState){ unlock();  } else { setlock(); }
        }
      break;
    }

}

// unlocks device
void unlock(){
  bool match = true;
  for (int i=0; i<sizeof(saved_code); i++ ){
    if (match){
      match = saved_code[i] == inprogress_code[i];
    }
  }
  
  Serial.print("match = ");
  Serial.println(match);
  Serial.print("saved_code = ");
  Serial.println(saved_code[0]+saved_code[1]+saved_code[2]+saved_code[3]);
   Serial.print("inprogress_code = ");
  Serial.println(inprogress_code[0]+inprogress_code[1]+inprogress_code[2]+inprogress_code[3]);
  
  if (match){  // if entered code is correct
    blink(GREEN);
    digitalWrite(RED, LOW);
    lockState = false;
    for (int i=0; i<sizeof(saved_code); i++){
      inprogress_code[i] = 0;
      EEPROM.write(i,0);
    }
    EEPROM.write(99,0);  
    tone(buzzerPin, 33, 500);
    tone(buzzerPin, 41, 500);
    tone(buzzerPin, 49, 500);
  } else {  // if entered code is wrong
    blink(RED);
    delay(50);
    blink(RED);
  }
  currentNumber=0;
}

// stores the locked code
void setlock(){
  for (int i=0; i<sizeof(inprogress_code); i++){
    saved_code[i] = inprogress_code[i];
    EEPROM.write(i,inprogress_code[i]);
  }
  EEPROM.write(99,0x3C);
  currentNumber=0;
  lockState=true;
  digitalWrite(RED, HIGH);
  digitalWrite(GREEN, LOW);

  tone(buttonPress, 49, 500);
  tone(buttonPress, 41, 500);
  tone(buttonPress, 33, 500);
}

// modifies sensorBroken variable
void updateSensor(){
  int val = digitalRead(sensorPin);
  sensorConnectionHistory = sensorConnectionHistory << 1 ;
  sensorConnectionHistory += val;
  //Serial.println(val);
  if (sensorConnectionHistory == 0){
    sensorBroken = true;
  } else { 
    sensorBroken = false;
  }
}

// if in locked state and sensor is broken
void checkLock(){
  
  if ((millis() - last) > duration){
    if (state){

      if(sensorBroken && lockState){
        tone(buzzerPin, 900,1000);
        state = 0;
        //Serial.println("on");
      }
    } else{
        state=1;
    }
    last = millis();
  }
  
}

void blink(int led) {
  digitalWrite(led, LOW);
  delay(50);
  digitalWrite(led, HIGH);
}
void light(int led) {
  digitalWrite(GREEN, LOW);
  digitalWrite(RED, LOW);
  digitalWrite(led, HIGH);
}
