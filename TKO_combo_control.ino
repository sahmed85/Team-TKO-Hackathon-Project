//Author: Team TKO: Shadman Ahmed, Mohammed Ahsan, Charles (Trey) Scarborough  
//Flowers Invention Studio Hackathon
//Date: 09/26/2020
//Last Modified: 10/03/2020
//Overview: This file contains the code flashed on the Arduino Leonardo. This file contains the implemenation to read in Analog signals from
//          the force sensors and process them to inputs for keyboard and mouse for the performing attack combos in the game. 
//Github/Git: https://github.com/sahmed85/Team-TKO-Hackathon-Project


#include <Keyboard.h>;
#include <Mouse.h>;

//set the number of inputs (velosat sensors)
//The input pins are set here in an array
//target holds the data for which is being pressed
const int numOfInputs = 10;
const int inputPins[numOfInputs] = {A3,A6,A7, A10,A8,A2,A9, A1,A11,A0};
int target[numOfInputs] = {2,5,8, 1,4,7,10, 3,6,9};
  // LEFT
    // Target: 2   Pin: A3
    // Target: 5   Pin: A6
    // Target: 8   Pin: A7
  // MIDDLE
    // Target: 1   Pin: A10
    // Target: 4   Pin: A8
    // Target: 7   Pin: A2
    // Target: 10  Pin: A9
  // RIGHT
    // Target: 3   Pin: A1
    // Target: 6   Pin: A11
    // Target: 9   Pin: A0

//these variables hold input states
//the fsr values are also init here
//the fsrdiff holds the threshold for a target being pressed 
int inputState[numOfInputs];
int lastInputState[numOfInputs] = {HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH};
bool inputFlags[numOfInputs] = {false,false,false,false,false,false,false,false,false,false};
int startingfsrValue[numOfInputs];
int fsrValue[numOfInputs];
int fsrDiff = 100;
int reading[numOfInputs];

//holds an array for debounce corrections
//threshold for debounce
long lastDebounceTime[numOfInputs] = {0,0,0,0,0,0,0,0,0,0};
long debounceDelay = 10;

//variables for storing time when a target is hit
long unsigned currentMillis[numOfInputs];
int const combinationLength = 2;
int combinationStorage[combinationLength];
bool targetFlag[numOfInputs] = {false,false,false,false,false,false,false,false,false,false};
int crosscutFlowPeriod = 1000;


void setup() {
//inits the starting values in the loop
  for(int i = 0; i < numOfInputs; i++) {
    pinMode(inputPins[i], INPUT);
    startingfsrValue[i] = analogRead(inputPins[i]);
  }
  Serial.begin(9600);
}

void loop() {
//main code calls these functions 
  getAnalogValues();
  setInputFlags();
  resolveInputFlags();
  //printCombinationStorage();
  //printTargetFlags();
  //printInputFlags();
  //delay(100);
}

/*
This function loops through and read analog inputs.
*/
void getAnalogValues(){
  for(int i = 0; i < numOfInputs; i++){
    fsrValue[i] = analogRead(inputPins[i]);
    if(startingfsrValue[i] - fsrValue[i] >= fsrDiff){
      reading[i] = LOW;
    }
    else{
      reading[i] = HIGH;
    }
    Serial.print(fsrValue[i]);
    Serial.print(", ");
  }
  Serial.println("");
}

/*
After reading the value, it sets a flag using the Debounce function. 
*/
void setInputFlags() {
  for(int i = 0; i < numOfInputs; i++) {
    if (reading[i] != lastInputState[i]) {
      lastDebounceTime[i] = millis();
    }
    if ((millis() - lastDebounceTime[i]) > debounceDelay) {
      if (reading[i] != inputState[i]) {
        inputState[i] = reading[i];
        if (inputState[i] == LOW) {
          inputFlags[i] = true;
          for(int j = 0; j < combinationLength - 1; j++){
            combinationStorage[j] = combinationStorage[j+1];
          }
          combinationStorage[combinationLength-1] = i;
          currentMillis[i] = millis();
        }
      }
    }
    lastInputState[i] = reading[i];
  }
}

/*
This function resolves the input flags set above and calls the fighting/combo action functions.
*/
void resolveInputFlags() {
  for(int i = 0; i < numOfInputs; i++) {
    if(inputFlags[i] == true) {
      if(targetFlag[0] == true){
        userInterface(i);
      }
      else if(targetFlag[4] == true){
        combinations(i);
      }
      else{
        fightingMoves(i);
      }
      inputFlags[i] = false;
    }
  }
}

//debugging
void printCombinationStorage(){
  for(int i = 0; i < combinationLength; i++){
    Serial.print(combinationStorage[i]);
    Serial.print(", ");
  }
  Serial.println("");
}

//debugging
void printTargetFlags(){
  for(int i = 0; i < numOfInputs; i++){
    Serial.print(targetFlag[i]);
    Serial.print(", ");
  }
  Serial.println("");
}

//debugging
void printInputFlags(){
  for(int i = 0; i < numOfInputs; i++){
    Serial.print(inputFlags[i]);
    Serial.print(", ");
  }
  Serial.println("");  
}

//menu button control based on input
//menu for the game
void userInterface(int input){
  // LEFT
  if(input == 0){
    targetFlag[0] = false;
  }
  else if(input == 1){
    Keyboard.write(KEY_LEFT_CTRL);
  }
  else if(input == 2){
    Keyboard.write('c');
  }
  // MIDDLE
  else if(input == 3){
    Keyboard.write(KEY_ESC);
  }
  else if(input == 4){
    Mouse.click(MOUSE_LEFT);
  }
  else if(input == 5){
    Keyboard.write(KEY_RETURN);
  }
  else if(input == 6){
    Mouse.move(0,0,-100);
  }
  // RIGHT
  else if(input == 7){
    Keyboard.write('r');
  }
  else if(input == 8){
    Mouse.click(MOUSE_RIGHT);
  }
  else if(input == 9){
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.write(KEY_TAB);
    Keyboard.release(KEY_LEFT_ALT);    
  }
}

/*
This function executes the combo. This function is called from the resolve flag function.
Uses the Keyboard and mouse library to send keyboard and mouse commands.
*/
void combinations(int input){
  if(combinationStorage[combinationLength-1] == 4 && combinationStorage[combinationLength-2] == 4){
    Mouse.click(MOUSE_LEFT);
    targetFlag[4] = false;
  }
  else if(combinationStorage[combinationLength-1] == 3 && combinationStorage[combinationLength-2] == 4){
    Keyboard.press('w');
    Keyboard.write('f');
    Keyboard.release('w');
    targetFlag[4] = false;
  }
  else if(combinationStorage[combinationLength-1] == 5 && combinationStorage[combinationLength-2] == 4){
    Keyboard.write('Q');
    targetFlag[4] = false;
  }
  else if(combinationStorage[combinationLength-1] == 6 && combinationStorage[combinationLength-2] == 4){
    Keyboard.write('e');
    targetFlag[4] = false;
  }
}

/*
This function executes more common fighting moves for the character. Executes based on input number. 
Uses the Keyboard and mouse library to send keyboard and mouse commands. 
*/
void fightingMoves(int input){
  // LEFT
  if(input == 0){
    targetFlag[0] = true;
  }
  else if(input == 1){
    Keyboard.press('a');
    Mouse.click(MOUSE_LEFT);
    Keyboard.release('a');
  }
  else if(input == 2){
    Keyboard.press(KEY_LEFT_SHIFT);
    Mouse.click(MOUSE_LEFT);
    Keyboard.release(KEY_LEFT_SHIFT);
  }
  // MIDDLE
  else if(input == 3){
    Keyboard.write('f');
  }
  else if(input == 4){
    targetFlag[4] = true;
  }
  else if(input == 5){
    Keyboard.write('F');
  }
  else if(input == 6){
    Keyboard.write('E');
  }
  // RIGHT
  //this logic is for time sensitve combos.
  else if(input == 7){
    if(currentMillis[7] - currentMillis[1] <= crosscutFlowPeriod){
      Keyboard.press('a');
      Mouse.click(MOUSE_RIGHT);
      Keyboard.release('a');
    }
    else if(currentMillis[7] - currentMillis[8] <= crosscutFlowPeriod){
      Keyboard.press('d');
      Mouse.click(MOUSE_RIGHT);
      Keyboard.release('d');
    }    
    else{
      Keyboard.press(KEY_LEFT_SHIFT);
      Mouse.click(MOUSE_RIGHT);
      Keyboard.release(KEY_LEFT_SHIFT);      
    }
  }
  else if(input == 8){
    Keyboard.press('d');
    Mouse.click(MOUSE_LEFT);
    Keyboard.release('d');
  }
  else if(input == 9){
    if(currentMillis[9] - currentMillis[1] <= crosscutFlowPeriod){
      Keyboard.press('a');
      Keyboard.write(' ');
      Keyboard.release('a');
    }
    else if(currentMillis[9] - currentMillis[8] <= crosscutFlowPeriod){
      Keyboard.press('d');
      Keyboard.write(' ');
      Keyboard.release('d');
    }
    else{
      Keyboard.press('s');
      Keyboard.write('f');
      Keyboard.release('s');     
    }
  }
}
