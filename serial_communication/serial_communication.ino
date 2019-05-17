// include the ResponsiveAnalogRead library
#include <ResponsiveAnalogRead.h>
#include <Thread.h>
#include <SCServo.h>

// claw stuff
SMSCL sm;

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
const int motor_forward_PIN = 29;
const int motor_reverse_PIN = 5;
int motor_activated = 0;

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

const int left_center_VAL = 500;
const int right_center_VAL = 550;

// mist
const int battery_analog_PIN = A0;
int left_analog_0 = 0;
int right_analog_0 = 0;

int left_analog_1 = 0;
int right_analog_1 = 0;

int left_error = 0;
int right_error = 0;

int steer_val_input = 0;
int steer_control_left = 0;
int steer_control_right = 0;

int claw_speed = 0;

// Thread
Thread steer_thread = Thread();
Thread battery_thread = Thread();

//| --- |//
//| END |
//| --- |//


// Responsive analog read

ResponsiveAnalogRead boomAnalog(boom_analog_PIN, true);
ResponsiveAnalogRead dipperAnalog(dipper_analog_PIN, true);

ResponsiveAnalogRead leftAnalog(left_analog_PIN, true);
ResponsiveAnalogRead rightAnalog(right_analog_PIN, true);

ResponsiveAnalogRead batteryAnalog(battery_analog_PIN, true);

void setup() {
  // Declare PIN mode
  pinMode(motor_pwm_PIN, OUTPUT);
  pinMode(motor_forward_PIN, OUTPUT);
  pinMode(motor_reverse_PIN, OUTPUT);
  // Initialize motor
  digitalWrite(motor_pwm_PIN, LOW);
  digitalWrite(motor_forward_PIN, LOW);
  digitalWrite(motor_reverse_PIN, LOW);
  
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

  // Attach thread with function
  steer_thread.onRun(steer);
  steer_thread.setInterval(250);

  battery_thread.onRun(check_battery);
  battery_thread.setInterval(1000);

  Serial.begin(57600);

  // setting up claw control
  Serial2.begin(115200);
  sm.pSerial = &Serial2;


//  sm.WritePos(1,  2000, 0, 1000);
//  sm.WritePos(2,  2000, 0, 1000);
//  sm.WritePos(3,  2000, 0, 1000);
//
//  sm.WritePos(4,  2000, 0, 1000);
//  sm.WritePos(5,  2000, 0, 1000);
//  sm.WritePos(6,  2000, 0, 1000);

  delay(2000);

  // CENTER CLAW
  // pwm mode for claw rotation
  sm.wheelMode(1);
  sm.wheelMode(2);
  sm.wheelMode(3);

  sm.wheelMode(4);
  sm.wheelMode(5);
  sm.wheelMode(6);

  Serial.println("<Arduino is ready>");
}

void loop() {
  // Fetch command from serial
  recvWithStartEndMarkers();

  response();
  if(steer_thread.shouldRun()){steer_thread.run();}

//  if(battery_thread.shouldRun()){battery_thread.run();}
}

void check_battery(){
  batteryAnalog.update();
  Serial.print("<BAT:");
  Serial.print(batteryAnalog.getValue());
  Serial.println(">");
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
//    Serial.print(signalType);
//    Serial.print(',');
//    Serial.println(signalVal);


    // BOOM
    if(strcmp(signalType, "RAX") == 0){
      if(signalVal < 0){
          digitalWrite(boom_dir_PIN, HIGH);
          signalVal = -signalVal;
        }else{
          digitalWrite(boom_dir_PIN, LOW);
          }
          
      analogWrite(boom_pwm_PIN, signalVal);
      }
      
    // DIPPER
    if(strcmp(signalType, "RAY") == 0){
      if(signalVal < 0){
          digitalWrite(dipper_dir_PIN, HIGH);
          signalVal = -signalVal;
        }else{
          digitalWrite(dipper_dir_PIN, LOW);
        }  
      analogWrite(dipper_pwm_PIN, signalVal);
      }


    // ARM BASE
    // - ccw
    if(strcmp(signalType, "LB") == 0){
      digitalWrite(base_dir_PIN, HIGH);
      if(signalVal == 1){
          analogWrite(base_pwm_PIN, 100);
        }else{
          digitalWrite(base_pwm_PIN, LOW);
        }  
      }
    // -cw
    if(strcmp(signalType, "RB") == 0){
      digitalWrite(base_dir_PIN, LOW);
      if(signalVal == 1){
          analogWrite(base_pwm_PIN, 50);
        }else{
          digitalWrite(base_pwm_PIN, LOW);
        }  
      }
        
    if(strcmp(signalType, "LAX") == 0){
      steer_val_input = signalVal; 
      }

    // HAND BASE
    // - ccw
    if(strcmp(signalType, "X") == 0){
      if(signalVal == 1){
          sm.WriteSpe(1, 500);
        }else{
          sm.WriteSpe(1, 0);
        }  
      }
    // -cw
    if(strcmp(signalType, "B") == 0){
      if(signalVal == 1){
          sm.WriteSpe(1, -500);
        }else{
          sm.WriteSpe(1, 0);
        }  
      }
        
    if(strcmp(signalType, "LAX") == 0){
      steer_val_input = signalVal; 
      }

    /* 
     *  DRIVE
     */
     
    // - forward
    if(strcmp(signalType, "LAY") == 0){
      signalVal = signalVal/3;

      if(signalVal < 0){
        //FORWARD
        if(digitalRead(5) == HIGH){
          digitalWrite(5, LOW);
          delay(3000);
          }
          digitalWrite(29, HIGH);
          signalVal = -signalVal;
      } else if (signalVal >= 0){
        //BACKWARD
        if(digitalRead(29) == HIGH){
          digitalWrite(29, LOW);
          delay(3000);
          }
          digitalWrite(5, HIGH);
        }

      // Lower cap
      if(signalVal < 30){
          signalVal = 0;
          digitalWrite(29, LOW);
          digitalWrite(5, LOW);
        }

      // Upper cap
      if(signalVal > 150){
        signalVal = 150;
        }

      Serial.println(signalVal);

      analogWrite(motor_pwm_PIN, signalVal);

      }

    // close finger
    if(strcmp(signalType, "RT") == 0){
      if(signalVal < 50){
        sm.WriteSpe(6,  0);
        sm.WriteSpe(5,  0);
        sm.WriteSpe(4,  0);
        } else {
        claw_speed = claw_speedVal(signalVal);
        sm.WriteSpe(6, claw_speed);
        sm.WriteSpe(5, claw_speed);
        sm.WriteSpe(4, -claw_speed);
        }
      }

     // open finger
     if(strcmp(signalType, "LT") == 0){
      if(signalVal < 100){
        sm.WriteSpe(6,  0);
        sm.WriteSpe(5,  0);
        sm.WriteSpe(4,  0);
        } else {
        claw_speed = claw_speedVal(signalVal);
        sm.WriteSpe(6, -claw_speed);
        sm.WriteSpe(5, -claw_speed);
        sm.WriteSpe(4, claw_speed);
        }
      }

     if(strcmp(signalType, "DPX") == 0){
        if(signalVal == -1){
          sm.WriteSpe(2, -500);
        }else if(signalVal == 1){
          sm.WriteSpe(2, 500);
        }else{
          sm.WriteSpe(2, 0);
        }
      }

     if(strcmp(signalType, "DPY") == 0){
        if(signalVal == -1){
          sm.WriteSpe(3, -500);
        }else if(signalVal == 1){
          sm.WriteSpe(3, 500);
        }else{
          sm.WriteSpe(3, 0);
        }
      }

    if(strcmp(signalType, "Y") == 0){
      if(signalVal == 1){
          check_battery();
        }  
      }

      
    // -----------------
    // End of cycle & start new
    newData = false;
    }  
    //showNewData();

    //----------------------------
  }

void steer(){
      //| -------- |//
    //| STEERING |
    //| -------- |//
    // update the left and right value
    leftAnalog.update();
    rightAnalog.update();

    // reading
    left_analog_0  = leftAnalog.getValue();
    right_analog_0 = rightAnalog.getValue();

    // steering LEFT

    left_error   = left_steerVal(-steer_val_input) - left_analog_0;
    steer_control_left = 5*abs(left_error);
    
    if(steer_control_left < 30){steer_control_left = 0;}
    if(steer_control_left > 255){steer_control_left = 255;}

    Serial.print("L: ");
    Serial.print(left_error);
    Serial.print("  Control: ");
    Serial.println(steer_control_left);
    
     if(abs(left_error) < 10) {
      digitalWrite(left_pwm_PIN, LOW);
      } else if(left_error < 0){
      digitalWrite(left_dir_PIN, HIGH);
      analogWrite(left_pwm_PIN, steer_control_left);
      } else {
        digitalWrite(left_dir_PIN, LOW);
        analogWrite(left_pwm_PIN, steer_control_left);
        }
    
    // steering RIGHT
    right_error   =  right_analog_0 - right_steerVal(steer_val_input);
    steer_control_right = 5*abs(right_error);
    
    if(steer_control_right < 30){steer_control_right = 0;}
    if(steer_control_right > 255){steer_control_right = 255;}

//    Serial.print("R: ");
//    Serial.print(right_error);
//    Serial.print("  Control: ");
//    Serial.println(steer_control_right);
    
     if(abs(right_error) < 10) {
      digitalWrite(right_pwm_PIN, LOW);
      } else if(right_error < 0){
      digitalWrite(right_dir_PIN, HIGH);
      analogWrite(right_pwm_PIN, steer_control_right);
      } else {
        digitalWrite(right_dir_PIN, LOW);
        analogWrite(right_pwm_PIN, steer_control_right);
        }
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


//| ---- |//
//| MISC |
//| ---- |//

int left_steerVal(int data){
    
    int val = (430.0/510.0)*data + left_center_VAL;
    return val;
  }



int right_steerVal(int data){
    
    int val = (430.0/510.0)*data + right_center_VAL;
    return val;
  }


int claw_speedVal(int data){
    int val = (1500.0/225.0)*data;
    return val;
  }

  
//| --- |//
//| END |
//| --- |//
