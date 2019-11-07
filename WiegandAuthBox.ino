/*
Pin D2 - DATA-0 (Green)
Pin D3 - DATA-1 (White)
*/

#include <RCSwitch.h>
#include <Wiegand.h>
#include <EEPROM.h>     // We are going to read and write PICC's UIDs from/to EEPROM
WIEGAND wg;
RCSwitch mySwitch = RCSwitch();

#define relay 4      // Set Relay Pin

boolean match = false;          // initialize card match to false
boolean programMode = false;  // initialize programming mode to false

int successRead;    // Variable integer to keep if we have Successful Read from Reader

byte storedCard[4];   // Stores an ID read from EEPROM
byte readCard[4];   // Stores scanned ID read from RFID Module
byte masterCard[4];   // Stores master card's ID read from EEPROM
byte openCard[4];               // Карта которая держит постоянно дверь открытой..

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(13, OUTPUT);
//  pinMode(5, OUTPUT);
//  pinMode(6, OUTPUT); 
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);    // Make sure door is locked
  digitalWrite(5, LOW); //indication stuff
  digitalWrite(6, LOW);

  //Protocol Configuration
  
  //SPI.begin();           // MFRC522 Hardware uses SPI protocol
  wg.begin();
  mySwitch.enableTransmit(9);
  Serial.println(F("Access Control v1.1"));   // For debugging purposes
  // ShowReaderDetails(); // Show details of PCD - MFRC522 Card Reader details

  // Check if master card defined, if not let user choose a master card
  // This also useful to just redefine Master Card
  // You can keep other EEPROM records just write other than 143 to EEPROM address 1
  // EEPROM address 1 should hold magical number which is '143'

  if (EEPROM.read(1) != 143) {

    Serial.println(F("No Master Card Defined"));
    Serial.println(F("Scan A Card/FP to Define as Master ID"));
    do {
      successRead = getID();            // sets successRead to 1 when we get read from reader otherwise 0
      //    digitalWrite(blueLed, LED_ON);    // Visualize Master Card need to be defined
      delay(200);
      //     digitalWrite(blueLed, LED_OFF);
      delay(200);
    }
    while (!successRead);                  // Program will not go further while you not get a successful read
    for ( int j = 0; j < 4; j++ ) {        // Loop 4 times
      EEPROM.write( 2 + j, readCard[j] );  // Write scanned PICC's UID to EEPROM, start from address 3
    }
    EEPROM.write(1, 143);                  // Write to EEPROM we defined Master Card.
    Serial.println(F("Master ID Defined"));
  }
  Serial.println(F("-------------------"));
  Serial.println(F("Master UID"));
  for ( int i = 0; i < 4; i++ ) {          // Read Master Card's UID from EEPROM
    masterCard[i] = EEPROM.read(2 + i);    // Write it to masterCard
    Serial.print(masterCard[i]);
  }
  Serial.println("");
  Serial.println(F("-------------------"));
  Serial.println(F("Everything Ready"));
  Serial.println(F("Waiting ID's to be scanned"));
  digitalWrite(5, 0);
  digitalWrite(6, 1);
  // cycleLeds();    // Everything ready lets give user some feedback by cycling leds
  
}



///////////////////////////////////////// Get PICC's UID ///////////////////////////////////
int getID() {
  if (wg.available()){
    Serial.println(F("Received UID:"));
    for (int i = 0; i < 4; i++) {  //
      readCard[i] = wg.getCode();
      Serial.print(readCard[i]);
    }
    Serial.println("");
    return 1;
  }
  return 0; 
}

void loop() {

  do {
    successRead = getID();  // sets successRead to 1 when we get read from reader otherwise 0
    if (programMode) {
      // cycleLeds();              // Program Mode cycles through RGB waiting to read a new card
  digitalWrite(5, 1);
  digitalWrite(6, 0);
  delay(100);
  digitalWrite(5, 0);
  digitalWrite(6, 1);
  delay(100);
  digitalWrite(5, 1);
  digitalWrite(6, 0);
  delay(100);
  digitalWrite(5, 0);
  digitalWrite(6, 1);
  delay(100);
  digitalWrite(5, 0);
  digitalWrite(6, 0);
    }
    else {
      //  normalModeOn();     // Normal mode, blue Power LED is on, all others are off
  digitalWrite(5, 0);
  digitalWrite(6, 0);
      if (digitalRead(10) == 1) clearKeys();
    }
  }
  while (!successRead);   //the program will not go further while you not get a successful read
    
  if (programMode) {
    if ( isMaster(readCard) ) { //If master card scanned again exit program mode
      Serial.println(F("Master ID Detected"));
      Serial.println(F("Exiting Program Mode"));
      Serial.println(F("-----------------------------"));
      programMode = false;
      return;
    }
    else {
      if ( findID(readCard) ) { // If scanned card is known delete it
        Serial.println(F("I know this ID, removing..."));
        deleteID(readCard);
        Serial.println("-----------------------------");
      }
      else {                    // If scanned card is not known add it
        Serial.println(F("I do not know this ID, adding..."));
        writeID(readCard);  
        Serial.println(F("-----------------------------"));
      }
      for(int i = 0; i<4; i++){readCard[i] = 0;} //clear for normal use, otherwise it go loop cause it's think this card scanned again!
      successRead = getID();
    }
  }
  else {
    if ( isMaster(readCard) ) {   // If scanned card's ID matches Master Card's ID enter program mode
      programMode = true;
      Serial.println(F("Hello Master - Entered Program Mode"));
      int count = EEPROM.read(0);   // Read the first Byte of EEPROM that
      Serial.print(F("I have "));     // stores the number of ID's in EEPROM
      Serial.print(count);
      Serial.print(F(" record(s) on EEPROM"));
      Serial.println("");
      Serial.println(F("Scan a ID to ADD or REMOVE"));
      Serial.println(F("-----------------------------"));
    }
    else {
      if ( findID(readCard) ) { // If not, see if the card is in the EEPROM
        Serial.println(F("Welcome, You shall pass"));
        granted(500);          // Open the door lock 
        mySwitch.send(readCard, 24);
      }
      else {      // If not, show that the ID was not valid
        Serial.println(F("You shall not pass"));
        denied();
      }
    }
  }
  for(int i = 0; i<4; i++){readCard[i] = 0;} //clear for normal use, otherwise it go loop cause it's think this card scanned again!


}

/////////////////////////////////////////  Access Granted    ///////////////////////////////////
void granted (int setDelay) {
  //  digitalWrite(blueLed, LED_OFF);   // Turn off blue LED
  //  digitalWrite(redLed, LED_OFF);  // Turn off red LED
  //  digitalWrite(greenLed, LED_ON);   // Turn on green LED
  digitalWrite(5, 1);
  digitalWrite(6, 0);
  digitalWrite(relay, HIGH);     // Unlock door!
  delay(setDelay);          // Hold door lock open for given seconds
  digitalWrite(relay, LOW);    // Relock door
  delay(1000);            // Hold green LED on for a second
}

///////////////////////////////////////// Access Denied  ///////////////////////////////////
void denied() {
  //  digitalWrite(greenLed, LED_OFF);  // Make sure green LED is off
  // digitalWrite(blueLed, LED_OFF);  // Make sure blue LED is off
  //  digitalWrite(redLed, LED_ON);   // Turn on red LED
  digitalWrite(5, 0);
  digitalWrite(6, 1);
  delay(1000);
}
//////////////////////////////////////// Read an ID from EEPROM //////////////////////////////
void readID( int number ) {
  int start = (number * 4 ) + 2;    // Figure out starting position
  for ( int i = 0; i < 4; i++ ) {     // Loop 4 times to get the 4 Bytes
    storedCard[i] = EEPROM.read(start + i);   // Assign values read from EEPROM to array
  }
}

///////////////////////////////////////// Add ID to EEPROM   ///////////////////////////////////
void writeID( byte a[] ) {
  if ( !findID( a ) ) {     // Before we write to the EEPROM, check to see if we have seen this card before!
    int num = EEPROM.read(0);     // Get the numer of used spaces, position 0 stores the number of ID cards
    int start = ( num * 4 ) + 6;  // Figure out where the next slot starts
    num++;                // Increment the counter by one
    EEPROM.write( 0, num );     // Write the new count to the counter
    for ( int j = 0; j < 4; j++ ) {   // Loop 4 times
      EEPROM.write( start + j, a[j] );  // Write the array values to EEPROM in the right position
    }

    Serial.println(F("Succesfully added ID record to EEPROM"));
  }
  else {

    Serial.println(F("Failed! There is something wrong with ID or bad EEPROM"));
  }
}

///////////////////////////////////////// Remove ID from EEPROM   ///////////////////////////////////
void deleteID( byte a[] ) {
  if ( !findID( a ) ) {     // Before we delete from the EEPROM, check to see if we have this card!

    Serial.println(F("Failed! There is something wrong with ID or bad EEPROM"));
  }
  else {
    int num = EEPROM.read(0);   // Get the numer of used spaces, position 0 stores the number of ID cards
    int slot;       // Figure out the slot number of the card
    int start;      // = ( num * 4 ) + 6; // Figure out where the next slot starts
    int looping;    // The number of times the loop repeats
    int j;
    int count = EEPROM.read(0); // Read the first Byte of EEPROM that stores number of cards
    slot = findIDSLOT( a );   // Figure out the slot number of the card to delete
    start = (slot * 4) + 2;
    looping = ((num - slot) * 4);
    num--;      // Decrement the counter by one
    EEPROM.write( 0, num );   // Write the new count to the counter
    for ( j = 0; j < looping; j++ ) {         // Loop the card shift times
      EEPROM.write( start + j, EEPROM.read(start + 4 + j));   // Shift the array values to 4 places earlier in the EEPROM
    }
    for ( int k = 0; k < 4; k++ ) {         // Shifting loop
      EEPROM.write( start + j + k, 0);
    }

    Serial.println(F("Succesfully removed ID record from EEPROM"));
  }
}

///////////////////////////////////////// Check Bytes   ///////////////////////////////////
boolean checkTwo ( byte a[], byte b[] ) {
  if ( a[0] != NULL )       // Make sure there is something in the array first
    match = true;       // Assume they match at first
  for ( int k = 0; k < 4; k++ ) {   // Loop 4 times
    if ( a[k] != b[k] )     // IF a != b then set match = false, one fails, all fail
      match = false;
  }
  if ( match ) {      // Check to see if if match is still true
    return true;      // Return true
  }
  else  {
    return false;       // Return false
  }
}

///////////////////////////////////////// Find Slot   ///////////////////////////////////
int findIDSLOT( byte find[] ) {
  int count = EEPROM.read(0);       // Read the first Byte of EEPROM that
  for ( int i = 1; i <= count; i++ ) {    // Loop once for each EEPROM entry
    readID(i);                // Read an ID from EEPROM, it is stored in storedCard[4]
    if ( checkTwo( find, storedCard ) ) {   // Check to see if the storedCard read from EEPROM
      // is the same as the find[] ID card passed
      return i;         // The slot number of the card
      break;          // Stop looking we found it
    }
  }
}

///////////////////////////////////////// Find ID From EEPROM   ///////////////////////////////////
boolean findID( byte find[] ) {
  int count = EEPROM.read(0);     // Read the first Byte of EEPROM that
  for ( int i = 1; i <= count; i++ ) {    // Loop once for each EEPROM entry
    readID(i);          // Read an ID from EEPROM, it is stored in storedCard[4]
    if ( checkTwo( find, storedCard ) ) {   // Check to see if the storedCard read from EEPROM
      return true;
      break;  // Stop looking we found it
    }
    else {    // If not, return false
    }
  }
  return false;
}

////////////////////// Check readCard IF is masterCard   ///////////////////////////////////
// Check to see if the ID passed is the master programing card
boolean isMaster( byte test[] ) {
  if ( checkTwo( test, masterCard ) )
    return true;
  else
    return false;
}

void clearKeys(){
  Serial.println(F("Emergency button! Clearing ID's"));
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.write(1, 143); //this add magic number. Device think it's have master record but it's not. This prevent add any ID as Master.
  // you need clear EEPROM fully for change master ID or use 000 as master ID (possible)
  // turn the LED on when we're done
  digitalWrite(13, HIGH);
  while(1){
        digitalWrite(5, 1);
        digitalWrite(6, 0);
        delay(2165);
        digitalWrite(5, 0);
        digitalWrite(6, 1);
    };
  }
