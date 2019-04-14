// include the ResponsiveAnalogRead library
#include <ResponsiveAnalogRead.h>

const byte numChars = 32; // max number of character
char receivedChars[numChars]; // holder for incomming data
char signalType[numChars] = {0};
int signalVal = 0;
boolean newData = false; // boolean to keep track if new data has been read

//| -------- |//
//| PIN VARS |
//| -------- |//

// motor
const int motor_pwm_PIN = 3;

// arm
const int boom_pwm_PIN   = 32;
const int dipper_pwm_PIN = 10;

const int boom_dir_PIN   = 34;
const int dipper_dir_PIN = 12;

const int boom_analog_PIN   = A8;
const int dipper_analog_PIN = A6;

const int base_pwm_PIN = 7;
const int base_dir_PIN = 9;
const int base_ena_PIN = 11;
const int base_alm_PIN = 13;

// drive
const int left_pwm_PIN  = 6;
const int right_pwm_PIN = 2;

const int left_dir_PIN  = 8;
const int right_dir_PIN = 4;

const int left_analog_PIN  = A2;
const int right_analog_PIN = A4;


//| --- |//
//| END |
//| --- |//


void setup() {
  // Declare PIN mode
  pinMode(motor_pwm_PIN, OUTPUT);
  
  pinMode(boom_pwm_PIN, OUTPUT);
  pinMode(dipper_pwm_PIN, OUTPUT);
  
  pinMode(boom_dir_PIN, OUTPUT);
  pinMode(dipper_dir_PIN, OUTPUT);
  
  pinMode(base_pwm_PIN, OUTPUT);
  pinMode(base_dir_PIN, OUTPUT);
  pinMode(base_ena_PIN, OUTPUT);
  pinMode(base_alm_PIN, OUTPUT);
  
  pinMode(left_pwm_PIN, OUTPUT);
  pinMode(right_pwm_PIN, OUTPUT);
  
  pinMode(left_dir_PIN, OUTPUT);
  pinMode(right_dir_PIN, OUTPUT);
  
  
  Serial.begin(57600);
  Serial.println("<Arduino is ready>");
}

void loop() {
  // Fetch command from serial
  recvWithStartEndMarkers();

  // Process the command
  response();
}


void response() {
  // NOTE: only response if new data is true

  if(newData == true) {
    // NEW DATA RECIEVED
    // -----------------

    // Printing recieved data
    // Serial.println(receivedChars);

    // Start parsing data into controlType and controlVal
    char * strtokIndx; // this is used by strtok() as an index
  
    strtokIndx = strtok(receivedChars,",");      // get the first part - the string
    strcpy(signalType, strtokIndx); // copy it to messageFromPC
  
    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    signalVal = atoi(strtokIndx);     // convert this part to an integer

    // printing parsed data
    Serial.print(signalType);
    Serial.print(',');
    Serial.println(signalVal);


    // BOOM
    if(strcmp(signalType, "RAX") == 0){
      if(signalVal < 0){
          digitalWrite(boom_dir_PIN, LOW);
          signalVal = -signalVal;
        }else{
          digitalWrite(boom_dir_PIN, HIGH);
          }
          
      analogWrite(boom_pwm_PIN, signalVal);
      }
      
    // DIPPER
    if(strcmp(signalType, "RAY") == 0){
      if(signalVal < 0){
          digitalWrite(dipper_dir_PIN, LOW);
          signalVal = -signalVal;
        }else{
          digitalWrite(dipper_dir_PIN, HIGH);
        }  
      analogWrite(dipper_pwm_PIN, signalVal);
      }


    // BASE
    // - cw
    if(strcmp(signalType, "LB") == 0){
      digitalWrite(base_dir_PIN, HIGH);
      if(signalVal == 1){
          analogWrite(base_pwm_PIN, 50);
        }else{
          digitalWrite(base_pwm_PIN, LOW);
        }  
      }
    // -ccw
    if(strcmp(signalType, "RB") == 0){
      digitalWrite(base_dir_PIN, LOW);
      if(signalVal == 1){
          analogWrite(base_pwm_PIN, 50);
        }else{
          digitalWrite(base_pwm_PIN, LOW);
        }  
      }

    // Steer
    if(strcmp(signalType, "LAX") == 0){
      if(signalVal < 0){
          digitalWrite(left_dir_PIN, LOW);
          digitalWrite(right_dir_PIN, LOW);
          signalVal = -signalVal;
        }else{
          digitalWrite(left_dir_PIN, HIGH);
          digitalWrite(right_dir_PIN, HIGH);
        }  
        
      analogWrite(left_pwm_PIN, signalVal);
      analogWrite(right_pwm_PIN, signalVal);
      }

    // -----------------
    // End of cycle & start new
    newData = false;
    }  
    //showNewData();
  }

void digitalSignal(int pin, char *typ, int val){
  // digital signal
  if(val == 0){
    digitalWrite(pin, LOW);
    }else{
      digitalWrite(pin, HIGH);
    }
    delay(10);
  }


void analogSignal(char *btn, int pin, char *typ, int val){
  // analog signal
  if(val == 0){
    digitalWrite(pin, LOW);
    }else{
      analogWrite(pin, val);
    }
    delay(10);
  }

//| ---------------------------  |//
//| SENDING AND RECIEVING SIGNAL |
//| ---------------------------  |//
// source: https://forum.arduino.cc/index.php?topic=288234.0

void recvWithStartEndMarkers() {
  /*
   * Function to read incomming signal
   * 
   * NOTE: incomming signal must begin with '<' and end with '>'
   */

  static boolean recvInProgress = false;
  static byte ndx = 0; // Initialize starting position

  // declare start and end marker
  char startMarker = '<';
  char endMarker = '>';
  
  char rc; // recieving character

   
  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1; // if the buffer is full replace the last character
          }
        }
      else {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
        }
      }
    else if (rc == startMarker) {
      // only start recieving character when the starting character is recieved
      recvInProgress = true;
      }
    }
}

void showNewData() {
  /*
   * Print out data
   */
  if (newData == true) {
    Serial.println(receivedChars);
    newData = false;
    }
}

//| --- |//
//| END |
//| --- |//
