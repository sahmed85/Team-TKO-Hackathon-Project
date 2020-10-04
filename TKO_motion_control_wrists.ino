#include <Mouse.h>
#include <Keyboard.h>
#include <Wire.h>

const int MPUs[2] = {0x68,0x69};

long accelX[2], accelY[2], accelZ[2];
long gyroX[2], gyroY[2], gyroZ[2];
float gForceX[2], gForceY[2], gForceZ[2];
float rotX[2], rotY[2], rotZ[2];
float angleYX[2], angleYZ[2];

const int numOfInputs = 6;
int inputPins[numOfInputs] = {9,8,7,6,A0,A1};
int inputState[numOfInputs];
int lastInputState[numOfInputs] = {HIGH,HIGH,HIGH,HIGH,HIGH,HIGH};
bool inputFlags[numOfInputs] = {false,false,false,false,false,false};
int reading[numOfInputs];

int startingfsrValue[numOfInputs] = {0,0,0,0,0,0};
int fsrValue[numOfInputs] = {0,0,0,0,0,0};
int fsrDiff = 250;

long lastDebounceTime[numOfInputs] = {0,0,0,0,0,0};
long debounceDelay = 10;

int angleXSensitivity = 15;
int angleYSensitivity = 5;
float newX, newY;
float startingAngleYX[2] = {0,0}, startingAngleYZ[2] = {0,0};
float degreeDiff = 40.0*3.1415926/180;

const int numOfOrientations = 5;
int rightOrientation[numOfOrientations] = {HIGH,HIGH,HIGH,HIGH,HIGH};
int lastOrientationState[numOfOrientations] = {HIGH,HIGH,HIGH,HIGH,HIGH};
long lastOrientationDebounceTime[numOfOrientations] = {0,0,0,0,0};
int orientationState[numOfOrientations];
bool orientationFlags[numOfOrientations] = {false,false,false,false,false};

const int numOfAllInputs = numOfInputs + numOfOrientations;
int allReading[numOfAllInputs];
int allLastInputState[numOfAllInputs] = {HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH};
int allInputState[numOfAllInputs];
long allLastDebounceTime[numOfAllInputs] = {0,0,0,0,0,0,0,0,0,0,0};
bool allInputFlags[numOfAllInputs] = {false,false,false,false,false,false,false,false,false,false,false};
bool rightHand = false;

void setup(){
  for(int i = 0; i < 2; i++){
    Wire.begin();
    Wire.beginTransmission(MPUs[i]);
    Wire.write(0x6B);
    Wire.write(0b00000000);
    Wire.endTransmission();  
    Wire.beginTransmission(MPUs[i]);
    Wire.write(0x1B);
    Wire.write(0x00000000);
    Wire.endTransmission(); 
    Wire.beginTransmission(MPUs[i]);
    Wire.write(0x1C);
    Wire.write(0b00000000);
    Wire.endTransmission(); 
  }
  for(int i = 0; i < numOfInputs; i++){
    pinMode(inputPins[i],INPUT_PULLUP);  
  }
  for(int i = 4; i < numOfInputs; i++){
    pinMode(inputPins[i],INPUT);
    startingfsrValue[i] = analogRead(inputPins[i]);
  }
  Serial.begin(38400); 
}

void loop(){
  for(int i = 0; i < 2; i++){
    GetMpuValue(i);
  }
  printData(0); Serial.print("\t ||| \t"); printData(1); Serial.println();
  getReading();
  getOrientationReading();//printOrientationStates();
  getAllReadings();
  
  setReleaseInputFlags();
  resolveReleaseInputFlags();

  setInputFlags();
  resolveInputFlags();   
  
  if(startingfsrValue[4] - analogRead(inputPins[4]) >= fsrDiff || digitalRead(inputPins[0]) == LOW){
    moveMouse();
  }
  if(rightHand == true){//(startingfsrValue[5] - analogRead(inputPins[5]) >= fsrDiff){
    setOrientationFlags();
    resolveOrientationFlags();   
  }
  
}

void GetMpuValue(const int input){
  Wire.beginTransmission(MPUs[input]); 
  Wire.write(0x3B);
  Wire.endTransmission();
  Wire.requestFrom(MPUs[input],6);
  while(Wire.available() < 6);
  accelX[input] = Wire.read()<<8|Wire.read(); 
  accelY[input] = Wire.read()<<8|Wire.read(); 
  accelZ[input] = Wire.read()<<8|Wire.read();
  
  Wire.beginTransmission(MPUs[input]);
  Wire.write(0x43);
  Wire.endTransmission();
  Wire.requestFrom(MPUs[input],6);
  while(Wire.available() < 6);
  gyroX[input] = Wire.read()<<8|Wire.read();
  gyroY[input] = Wire.read()<<8|Wire.read();
  gyroZ[input] = Wire.read()<<8|Wire.read(); 


  gForceX[input] = accelX[input] / 16384.0;
  gForceY[input] = accelY[input] / 16384.0; 
  gForceZ[input] = accelZ[input] / 16384.0;
  rotX[input] = gyroX[input] / 131.0;
  rotY[input] = gyroY[input] / 131.0; 
  rotZ[input] = gyroZ[input] / 131.0;  
  angleYX[input] = atan(gForceX[input]/gForceY[input]);//*180/3.1415926;
  angleYZ[input] = atan(gForceZ[input]/gForceY[input]);//*180/3.1415926;
}

void printData(int input){
  Serial.print("gyro:\t");
  Serial.print(rotX[input]);
  Serial.print("\t");
  Serial.print(rotY[input]);
  Serial.print("\t");
  Serial.print(rotZ[input]);
  Serial.print("\tAcc:\t");
  Serial.print(gForceX[input]);
  Serial.print("\t");
  Serial.print(gForceY[input]);
  Serial.print("\t");
  Serial.print(gForceZ[input]);
  Serial.print("\tAngle:\t");
  Serial.print(angleYX[input]);
  Serial.print("\t");
  Serial.print(angleYZ[input]);
}

void getReading(){
  for(int i = 0; i < 4; i++){
    reading[i] = digitalRead(inputPins[i]);
  }
  for(int i = 4; i < numOfInputs; i++){
    fsrValue[i] = analogRead(inputPins[i]);
    if(startingfsrValue[i] - fsrValue[i] >= fsrDiff){
      reading[i] = LOW;
    }
    else{
      reading[i] = HIGH;
    }
  }
}

void getOrientationReading(){
  if(abs(startingAngleYZ[1] - angleYZ[1]) <= degreeDiff){ // wrist is centered
    rightOrientation[0] = true;
  }
  else{
    rightOrientation[0] = false;
  }
  if(angleYZ[1] - startingAngleYZ[1] >= degreeDiff){ // wrist is rotated counter-clockwise
    rightOrientation[1] = true;
  }
  else{
    rightOrientation[1] = false;
  }
  if(startingAngleYZ[1] - angleYZ[1] >= degreeDiff){ // wrist is rotated clockwise
    rightOrientation[2] = true;
  }
  else{
    rightOrientation[2] = false;
  }
  if(angleYX[1] - startingAngleYX[1] > degreeDiff){ // wrist is rotated up
    rightOrientation[3] = true;
  }
  else{
    rightOrientation[3] = false;
  }
  if(startingAngleYX[1] - angleYX[1] > degreeDiff){ // wrist is rotated down
    rightOrientation[4] = true;
  }
  else{
    rightOrientation[4] = false;
  }
}

void printOrientationStates(){
  for (int i = 0; i < numOfOrientations; i++){
    Serial.print(rightOrientation[i]);
    Serial.print(", ");
  }
  Serial.println();
}

void getAllReadings(){
  for(int i = 0; i < numOfInputs; i++){
    allReading[i] = reading[i];
  }
  for(int i = 0; i < numOfOrientations; i++){
    allReading[i + numOfInputs] = rightOrientation[i];
  }
}

void setReleaseInputFlags() {
  for(int i = 0; i < numOfAllInputs; i++) {
    if (allReading[i] != allLastInputState[i]) {
      allLastDebounceTime[i] = millis();
    }
    if ((millis() - allLastDebounceTime[i]) > debounceDelay) {
      if (allReading[i] != allInputState[i]) {
        allInputState[i] = allReading[i];
        if (allInputState[i] == HIGH) {
          allInputFlags[i] = true;
        }
      }
    }
    allLastInputState[i] = allReading[i];
  }
}

void resolveReleaseInputFlags() {
  for(int i = 0; i < numOfAllInputs; i++) {
    if(allInputFlags[i] == true) {
      releaseButtons(i);
      allInputFlags[i] = false;
    }
  }
}

void releaseButtons(int input){
  if(input == 0){ // first button on belt (closest to belly button)
    // mouse control, don't worry about it here
  }
  if(input == 1){ // second button on belt
    Keyboard.release('w');
  }
  if(input == 2){ // third button on belt
    Keyboard.release(KEY_LEFT_SHIFT);
  }
  if(input == 3){ // fourth button on belt
    
  }
  if(input == 4){ // left ring button
    // mouse control, don't worry about it here
  }
  if(input == 5){ // right ring button
    Keyboard.release('w');
    //Keyboard.release('a');
    //Keyboard.release('d');
    Keyboard.release(KEY_LEFT_SHIFT);
    rightHand = false;
  }
  if(input == 6){ // wrist is centered (for right hand)
    //Keyboard.release('w');
  }
  if(input == 7){ // wrist is rotated-counterclockwise
    //Keyboard.release('a');
    Keyboard.release(KEY_LEFT_SHIFT);
  }
  if(input == 8){ // wrist is rotated clockwise
    //Keyboard.release('d');
    Keyboard.release(KEY_LEFT_SHIFT);
  }
  if(input == 9){ // wrist is rotated up
    //Keyboard.release(KEY_LEFT_SHIFT);
  }
  if(input == 10){ // wrist is rotated down
    
  }
}

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
        }
      }
    }
    lastInputState[i] = reading[i];
  }
}

void resolveInputFlags() {
  for(int i = 0; i < numOfInputs; i++) {
    if(inputFlags[i] == true) {
      doSomething(i);
      inputFlags[i] = false;
    }
  }
}

void doSomething(int input){
  if(input == 0){
    startingAngleYX[0] = angleYX[0];
    startingAngleYZ[0] = angleYZ[0];
  }
  if(input == 1){
    Keyboard.press('w');
  }
  else if(input == 2){
    Keyboard.press(KEY_LEFT_SHIFT);
  }
  else if(input == 3){
    Mouse.click(MOUSE_RIGHT);
  }
  else if(input == 4){ // left hand
    startingAngleYX[0] = angleYX[0];
    startingAngleYZ[0] = angleYZ[0];
  }
  else if(input == 5){ // Right hand
    rightHand = true;
    startingAngleYX[1] = angleYX[1];
    startingAngleYZ[1] = angleYZ[1];
    Keyboard.press('w');
  }
}

void moveMouse(){
  newX = (startingAngleYZ[0] - angleYZ[0])*angleXSensitivity;
  newY = -(startingAngleYX[0] - angleYX[0])*angleYSensitivity;
  
  Mouse.move(newX,newY);
}

void setOrientationFlags() {
  for(int i = 0; i < numOfOrientations; i++) {
    if (rightOrientation[i] != lastOrientationState[i]) {
      lastOrientationDebounceTime[i] = millis();
    }
    if ((millis() - lastOrientationDebounceTime[i]) > debounceDelay) {
      if (rightOrientation[i] != orientationState[i]) {
        orientationState[i] = rightOrientation[i];
        if (orientationState[i] == LOW) {
          orientationFlags[i] = true;
        }
      }
    }
    lastOrientationState[i] = rightOrientation[i];
  }
}

void resolveOrientationFlags() {
  for(int i = 0; i < numOfOrientations; i++) {
    if(orientationFlags[i] == true) {
      characterMovement(i);
      orientationFlags[i] = false;
    }
  }
}

void characterMovement(int input){
  if(input == 0){
    //Keyboard.press('w');
  }
  if(input == 1){
    //Keyboard.press('a');
    Keyboard.press(KEY_LEFT_SHIFT);
  }
  if(input == 2){
    //Keyboard.press('d');
    Mouse.click(MOUSE_RIGHT);
  }
  if(input == 3){
    //Keyboard.press(KEY_LEFT_SHIFT);
  }
  if(input == 4){
    //Mouse.click(MOUSE_RIGHT);
  }
}
