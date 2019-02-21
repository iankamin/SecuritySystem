#include <EEPROM.h>

const int sensorPin = 6;
const int buzzerPin = 10;
const int vibratePin = A5;
const int GREEN = 12;
const int RED = 13;

int code[4];
int lockState = 0;
int currentNumber = 0;
const int totalButtons= 4;
int button[totalButtons] = {2, 3, 4, 5};
int lastButtonState[totalButtons]; // HIGH = not pressed, LOW = pressed (held down)
bool buttonActive[totalButtons]; // TRUE = if active, FALSE = otherwise
unsigned long lastDebounceTime[totalButtons]; // millis() recorded here 

//ian added here
unsigned long sensorConnectionHistory = 0;
bool sensorBroken; 
//ian end add here

int debounceDelay=20;
int longPressDelay=750;
void setup() {
  Serial.begin(9600);
  for (int i=0; i < totalButtons; i++) {
    lastButtonState[i] = HIGH;
    buttonActive[i] = false;
    lastDebounceTime[i] = 0;
    pinMode(button[i], INPUT);
  }  

  pinMode(sensorPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  pinMode(vibratePin, OUTPUT);
  
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  digitalWrite(RED, HIGH);
  
  if (EEPROM.read(0) != 0xff)
    for (int i = 0; i < 4; ++i)
        code[i] = EEPROM.read(i);
  else
    for (int i = 0; i < 4; ++i)
      code[i] = 0;
}
void loop() { 
  digitalWrite(vibratePin, HIGH); 
  updateSensor();
  //ian end add here 
  updateButtons();
  //ian added here
  checkLock();
}

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

void checkLock(){ 
  if(sensorBroken && !lockState){
    Serial.println("intruder");
    tone(buzzerPin, 1000); // Send 1KHz sound signal...
    delay(1000);        // ...for 1 sec
    noTone(buzzerPin);     // Stop sound...
    delay(1000);        // ...for 1sec
  }
}



/*
Loops through all the buttons to get the state of the button
to update the state of the lock.
*/
void updateButtons() {
  for (int i = 0; i < totalButtons; i++) {
    int buttonState = changeButtonState(i);
    if (buttonState == 1)
      changeLockState(i, 0);
    if (buttonState == 2)
      changeLockState(0, 1);
  }
}
int changeButtonState(int number)
{
  int reading = digitalRead(button[number]);
  //State change in button
  if (reading != lastButtonState[number]) {
    if (reading==HIGH && !buttonActive[number]){
      lastButtonState[number] = reading;
      return 0; // Not active and not pressed
    }
    
    // Was active, is now released
    if (reading==HIGH && buttonActive[number]) {
      if (millis() - lastDebounceTime[number] > debounceDelay) {
        lastButtonState[number] = reading;
        buttonActive[number]=true;
        return 1; // Held for a short time
      }
      lastButtonState[number] = reading;
      buttonActive[number]=false;
      return 0; // Held for too short a time, unpressed
    }
    
    // Is currently being held
    else if (reading==LOW) {
      // Was not active
      if (!buttonActive[number]) {
        lastDebounceTime[number]=millis();        
        lastButtonState[number] = reading;
        buttonActive[number]=true;
        return 0; //Currently not pressed (technically needs to be released to count)
      }
      return 0; // Currently not pressed (technically) and was active
    }
  }
  // Was not held, no state change
  if (reading==HIGH) {
    lastButtonState[number] = reading;
    buttonActive[number]=false;
    return 0;
  }
  // Being held, no state change
  if (reading==LOW) {
    if (millis() - lastDebounceTime[number] > longPressDelay && buttonActive[number]) {
      lastButtonState[number] = reading;
      buttonActive[number]=false;
      return 2; // Held for long enough to count as long press
    } else {
      return 0; // Not held for long enough time, does not count as pressed yet
    }
  }
}
/*
State 0 => locked
State 1 => unlocked
State 2 => enter new code
Duration 0 => short
Duration 1 => long
*/
void changeLockState(int buttonPressed, int duration) {
  if (lockState == 0 && buttonPressed == code[currentNumber] && duration == 0) {
    currentNumber++;
    Serial.println("Correct");
    blink(RED);
  }
  else if(lockState == 1 && buttonPressed == code[currentNumber] && duration == 0) {
    currentNumber++;
    Serial.println("correct");
    blink(GREEN);
  } else if(lockState != 2) {
    currentNumber = 0;
    Serial.println("incorrect, retry");
    for(int i = 0; i < 5; i++) {
      blink(RED);
      delay(50);
    }
  }
  if(lockState == 1 && duration == 1) {
    Serial.println("changing code");
    currentNumber = 0;
    lockState = 2;
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, HIGH);
  } else if(lockState == 2) {
    code[currentNumber] = buttonPressed;
    currentNumber++;
    Serial.println(buttonPressed);
  }
  if(currentNumber == 4 && lockState == 0){
    lockState = 1;
    Serial.println("Unlocked");
    currentNumber = 0;
  }
  else if(currentNumber == 4 && lockState == 1) {
    lockState = 0;
    Serial.println("Locked");
    currentNumber = 0;
  }
  else if(currentNumber == 4 && lockState == 2) {
    lockState = 0;
    Serial.println("Locked");
    currentNumber = 0;
    for (int i = 0; i < 4; ++i )
      EEPROM.write(i, code[i]);
  }
  if(lockState == 0) {
    light(RED);
  } 
  else if(lockState == 1) {
    light(GREEN);
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

