const byte numChars = 32; // max number of character
char receivedChars[numChars]; // holder for incomming data
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
    Serial.println(receivedChars);

    
    if(strcmp(receivedChars, "X,1") == 0){
        analogWrite(blue_PIN, 225);
        delay(10);
        }
        
    if(strcmp(receivedChars, "A,1") == 0){
        analogWrite(green_PIN, 225);
        delay(10); 
      }

    if(strcmp(receivedChars, "red") == 0){
        analogWrite(red_PIN, 225);
        delay(10); 
      }
      
     if(strcmp(receivedChars, "yellow") == 0){
        analogWrite(yellow_PIN, 225);
        delay(10); 
      }
      
     if(strcmp(receivedChars, "clear") == 0){
        digitalWrite(blue_PIN, LOW);
        digitalWrite(green_PIN, LOW);
        digitalWrite(red_PIN, LOW);
        digitalWrite(yellow_PIN, LOW);
        delay(10); 
      }

    // -----------------
    // End of cycle
    newData = false;
    }

    
    
//    showNewData();
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
