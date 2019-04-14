const byte numChars = 32; // max number of character
char receivedChars[numChars]; // holder for incomming data
char signalType[numChars] = {0};
int signalVal = 0;
boolean newData = false; // boolean to keep track if new data has been read


// Declare PIN var
const int blue_PIN = 3;
const int green_PIN = 5;
const int red_PIN = 6;
const int yellow_PIN = 9;

void setup() {
  // Declare PIN mode
  pinMode(blue_PIN, OUTPUT);
  pinMode(green_PIN, OUTPUT);
  pinMode(red_PIN, OUTPUT);
  pinMode(yellow_PIN, OUTPUT);
  
  Serial.begin(57600);
  Serial.println("<Arduino is ready>");
}

void loop() {
    
//  analogWrite(blue_PIN, 10);
//  delay(1000);
//  analogWrite(green_PIN, 10);
//  delay(1000);
//  analogWrite(red_PIN, 200);
//  delay(1000);
//  analogWrite(yellow_PIN, 255);
//  delay(1000);
//  
//  digitalWrite(blue_PIN, LOW);
//  delay(1000);
//  digitalWrite(green_PIN, LOW);
//  delay(1000);
//  digitalWrite(red_PIN, LOW);
//  delay(1000);
//  digitalWrite(yellow_PIN, LOW);
//  delay(1000);

  
  recvWithStartEndMarkers();

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

    // Doing stuff
      analogSignal("LAX", blue_PIN, signalType, signalVal);

      analogSignal("LAY", green_PIN, signalType, signalVal);
      
      analogSignal("RAX", red_PIN, signalType, signalVal);

      analogSignal("RAY", yellow_PIN, signalType, signalVal);
      
//    if(strcmp(receivedChars, "A,1") == 0){
//        analogWrite(green_PIN, 225);
//        delay(10); 
//      }
//
//    if(strcmp(receivedChars, "red") == 0){
//        analogWrite(red_PIN, 225);
//        delay(10); 
//      }
//      
//     if(strcmp(receivedChars, "yellow") == 0){
//        analogWrite(yellow_PIN, 225);
//        delay(10); 
//      }
      
     if(strcmp(receivedChars, "clear") == 0){
        digitalWrite(blue_PIN, LOW);
        digitalWrite(green_PIN, LOW);
        digitalWrite(red_PIN, LOW);
        digitalWrite(yellow_PIN, LOW);
        delay(10); 
      }

    // -----------------
    // End of cycle & start new
    newData = false;
    }  
    //showNewData();
  }

void digitalSignal(char *btn, int pin, char *typ, int val){
  // digital signal
  
  if(strcmp(typ, btn) == 0){
    if(val == 0){
      digitalWrite(pin, LOW);
      delay(10);
      }else{
        digitalWrite(pin, HIGH);
        }
      }
  }


void analogSignal(char *btn, int pin, char *typ, int val){
  // digital signal
  
  if(strcmp(typ, btn) == 0){
    if(val == 0){
      digitalWrite(pin, LOW);
      delay(10);
      }else{
        analogWrite(pin, val);
        }
      }
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
