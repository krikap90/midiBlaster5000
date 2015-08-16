// IMPORTS
//------------------------------------------------------------------------------

// loads the lcd api an configures the screen
#include <LiquidCrystal.h>
LiquidCrystal lcd(10, 2, 3, 4, 5, 17);

// loads the EEPROM API to store the presets
#include <Wire.h>

// GLOBALS
//------------------------------------------------------------------------------

//preset Editor input
String presetInputString; // Data received from the serial port


int displayMode = 0;
boolean secondScreen = false;

//preset Speicher
int presetIndex = 0;

String preset[] = {
"none",
"none",
"none",
"none",
"none",
"none",
"none",
"none",
"none"};

String presetName[] = {"none","none","none","none","none","none","none","none","none"};
String potiName[] = {"","","","","","","","","","","","","","","",""};

//Menu
int menuIndex = 0;
String menu1[] = {"set Poti", "set Midichannel", "select preset", "connect 2 editor", "delete presets"};

// selected parts in menu
int selectedPoti = 1;
int selectedMidiChannel = 1;
int selectedCC = 0;
int selectedToggleMin = 0;
int selectedToggleMax = 0;

//MIDI WRITE CC
int controlChange = 176;

//Multiplexer:
int potiInputsOben = 0;
int potiInputsUnten = 0;
int potiInputsObenSmooth = 0;
int potiInputsUntenSmooth = 0;

//Multiplex Poti Counter:
int potiIndex = 0;
//Rotary Encoder:
int clkPin = 6;
int dtPin = 7;
boolean encoderChange = false;

//LEDs:
int leds[] = {22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37};
//Toggles:
int toggles[] = {38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53};

//Toggle values:
int toggleMin[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int toggleMax[] = {127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127};

//Buttons:
int button1 = 12;
int button2 = 19;

//Poti Values:
int potiValueOld[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int potiValue[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int ccValue[] = {30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45};


int bit1 = 0;
int bit2 = 0;
int bit3 = 0;


String editorCombined;
int editorCounter = 0;
String editorInput[10];



// SETUP
//------------------------------------------------------------------------------

void setup() {
  //16x2 LCD
  lcd.begin(16, 2);
  lcd.clear();

  for(int i = 0; i < 16; i++){
    //leds
    pinMode(leds[i], OUTPUT); 
    //toggles
    pinMode(toggles[i], INPUT);
  }

  //Menue Button:
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);

  //Encoder:
  pinMode(clkPin, INPUT);
  pinMode(dtPin, INPUT);

  //Multiplex:
  pinMode(14, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(16, OUTPUT);


  //EEPROM
  Wire.begin();
  //USB:
  Serial.begin(9600);
  //MIDI:
  Serial1.begin(31250);

  //deleteEEPROM();

  // Startscreen
  showHomescreen();
  showPotiText();

}

// LOOP
//------------------------------------------------------------------------------

void loop () {
       
    boolean activeToggles[16];

    toggleController(activeToggles);
    normalProgramMode(activeToggles); 
    
    buttonController();      // loads the correct menu for the pressed button
    displayModeController(); // displays the right menu for te pressed button
 
}

// All Programm Modes
//------------------------------------------------------------------------------

void normalProgramMode(boolean activeToggles[]) {
  
  for ( potiIndex = 0 ; potiIndex <= 7; potiIndex++ ) {
    bit1 = bitRead(potiIndex, 0);
    bit2 = bitRead(potiIndex, 1);
    bit3 = bitRead(potiIndex, 2);

    digitalWrite(14, bit1);
    digitalWrite(15, bit2);
    digitalWrite(16, bit3);

    potiInputsOben = analogRead(A0);
    potiInputsUnten = analogRead(A1);

    for(int i = 0; i < 8; i++){
      
      if (potiIndex == i) {
        
        if(activeToggles[i] == true){
          potiValue[i] = map(potiInputsOben, 0, 1023, toggleMin[i], toggleMax[i]);
        }
        else{
          potiValue[i] = map(potiInputsOben, 0, 1023, 0, 128); //128, da durch die Glättung der Werte die 127 nicht mehr erreicht wird
        }
        potiValue[i] = 0.6 * potiValue[i] + 0.4 * potiValueOld[i];
        
        if(activeToggles[i+8] == true){
          potiValue[i+8] = map(potiInputsUnten, 0, 1023, toggleMin[i+8], toggleMax[i+8]);
        }
        else{
          potiValue[i+8] = map(potiInputsUnten, 0, 1023, 0, 128);
        }
        potiValue[i+8] = 0.6 * potiValue[i+8] + 0.4 * potiValueOld[i+8];
              
      
        if (potiValue[i] != potiValueOld[i]) {
          
          midiWrite(controlChange, ccValue[i], potiValue[i]);
          int potiIndexOben = i+1;
          if(displayMode == 0){
              showPotiValue(potiIndexOben, potiValue[i]);
            }

        }
      
        if (potiValue[i+8] != potiValueOld[i+8]) {
          
          midiWrite(controlChange, ccValue[i+8], potiValue[i+8]);
          int potiIndexUnten = i+9;
          
          if(displayMode == 0){
              showPotiValue(potiIndexUnten, potiValue[i+8]);
            }
            
        }
      
        potiValueOld[i] = potiValue[i];
        potiValueOld[i+8] = potiValue[i+8];

      }
   }
 }
}

// Alle LCD Funktionen
//------------------------------------------------------------------------------
void showHomescreen(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WELCOME TO THE");
  lcd.setCursor(0, 1);
  lcd.print("MIDIBLASTER 5000");
  
  for (int i = 0; i < 16; i++) {
      digitalWrite(leds[i], HIGH);
      delay(100);
  }

  checkForPresets();
  
  for (int i = 0; i < 16; i++) {
      digitalWrite(leds[i], LOW);
      delay(100);
  }
}

void showPotiText(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Poti: ");
  lcd.setCursor(0, 1);
  lcd.print("Value: ");
}

void showLoadPreset(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Waiting for");
  lcd.setCursor(0, 1);
  lcd.print("connection...");
}

void showSetPreset(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("select preset");
}

void showPotiValue(int poti, int value){
  
  if(potiName[poti-1].equals("")){
    lcd.setCursor(0, 0);
    lcd.print("Poti:           ");
    lcd.setCursor(6, 0);
    lcd.print(poti, DEC);
  }
  else{
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 0);
    lcd.print(potiName[poti-1]);
  }
  
  lcd.setCursor(7, 1);
  lcd.print("   ");
  lcd.setCursor(7, 1);
  lcd.print(value, DEC);
}

void showMenu(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MENU");
}

void showSelectPoti(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Select Poti");
}

void showSelectMidiChannel(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sel. Midichannel");
}

void showSelectCC(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Select CC");
}

void showSelectToggleMin(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("set toggle min");
}

void showSelectToggleMax(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("set toggle max");
}

void showIntInLine2(int index){
    
  if(encoderChange == true){
    lcd.setCursor(0, 1);
    lcd.print("        ");
    } 
    
    lcd.setCursor(0, 1);
    lcd.print(index);
  }
  
void showStringInLine2(String input){
    if(encoderChange == true){
    lcd.setCursor(0, 1);
    lcd.print("                ");
    }
    lcd.setCursor(0, 1);
    lcd.print(input);
    encoderChange = false;
  }

// Alle Encoder Funktionen
//------------------------------------------------------------------------------

int getEncoderTurn()
{
  static int oldA = HIGH;
  static int oldB = HIGH;
  int result = 0;
  int newA = digitalRead(clkPin);
  int newB = digitalRead(dtPin);
  if (newA != oldA || newB != oldB)
  {
    // something has changed
    if (oldA == HIGH && newA == LOW)
    {
      result = (oldB * 2 - 1);
    }
  }
  oldA = newA;
  oldB = newB;
  return result;
}

String selectMenu(){
    
     int temp = menuIndex;
      
     menuIndex += getEncoderTurn();
     if(temp != menuIndex){
      encoderChange = true;
    }
    
    if(menuIndex > 4){
        menuIndex = 4;
        
      }
    if(menuIndex < 0){
        menuIndex = 0;
      }
  
    return menu1[menuIndex];
  }
  
String selectPreset(){
  
    int temp = presetIndex;
    
    presetIndex += getEncoderTurn();
    
    if(temp != presetIndex){
      encoderChange = true;
    }
    if(presetIndex > 8){
        presetIndex = 0;
      }
    if(presetIndex < 0){
        presetIndex = 8;
      }
  
    return presetName[presetIndex];
  
  }

int selectPoti(){
    int temp = selectedPoti;
    selectedPoti += getEncoderTurn();
    
    if(temp != selectedPoti){
      encoderChange = true;
    }
    
    if(selectedPoti > 16){
        selectedPoti = 1;
      }
    if(selectedPoti < 1){
        selectedPoti = 16;
      }
  
    return selectedPoti;
  
  }

int selectMidiChannel(){
    int temp = selectedMidiChannel;
    selectedMidiChannel += getEncoderTurn();
    
    if(temp != selectedMidiChannel){
      encoderChange = true;
    }
    
    if(selectedMidiChannel > 16){
        selectedMidiChannel = 1;
      }
    if(selectedMidiChannel < 1){
        selectedMidiChannel = 16;
      }
  
    return selectedMidiChannel;
  
  }
  
int selectCC(){
    int temp = selectedCC;
    selectedCC += getEncoderTurn();
    
    if(temp != selectedCC){
      encoderChange = true;
    }
    
    if(selectedCC > 127){
        selectedCC = 0;
      }
    if(selectedCC < 0){
        selectedCC = 127;
      }
  
    return selectedCC;
  
  }
  
int selectToggleMin(){
    int temp = selectedToggleMin;
    selectedToggleMin += getEncoderTurn();
    
    if(temp != selectedToggleMin){
      encoderChange = true;
    }
    
    if(selectedToggleMin > 127){
        selectedToggleMin = 0;
      }
    if(selectedToggleMin < 0){
        selectedToggleMin = 127;
      }
  
    return selectedToggleMin;
  
  }
  
int selectToggleMax(){
    int temp = selectedToggleMax;
    selectedToggleMax += getEncoderTurn();
    
    if(temp != selectedToggleMax){
      encoderChange = true;
    }
    
    if(selectedToggleMax > 127){
        selectedToggleMax = 0;
      }
    if(selectedToggleMax < 0){
        selectedToggleMax = 127;
      }
  
    return selectedToggleMax;
  
  }

void setPoti(){
     
    potiName[selectedPoti-1] = "";
    ccValue[selectedPoti-1] = selectedCC;
    if(selectedToggleMin <= selectedToggleMax){
      toggleMin[selectedPoti-1] = selectedToggleMin;
      toggleMax[selectedPoti-1] = selectedToggleMax + 1; //+1, da durch die Glättung der Werte die Spitze nicht erreicht wird. Rundung nach unten
    }
    else{
      toggleMin[selectedPoti-1] = selectedToggleMax;
      toggleMax[selectedPoti-1] = selectedToggleMin + 1; //+1, da durch die Glättung der Werte die Spitze nicht erreicht wird. Rundung nach unten
    }
  }
  
void setMidiChannel(){
    controlChange = selectedMidiChannel -1 + 176;
  }



// Alle toggle Funktionen
//------------------------------------------------------------------------------

// fills the array of activeToggles and switches the LEDS on
void toggleController(boolean *activeToggles) { 
  
  for (int i = 0; i < 16; i++) {
    if (digitalRead(toggles[i]) == HIGH) {
      digitalWrite(leds[i], HIGH);
      activeToggles[i] = 1;
    }
    else {
      digitalWrite(leds[i], LOW);
      activeToggles[i] = 0;
    }
  }
}

// sets a new toggle value
void setToggleMinValue(int index, int value){
    toggleMin[index] = value;
  }
  
void setToggleMaxValue(int index, int value){
    toggleMax[index] = value;
  }
 
// preset String Verarbeitung
//------------------------------------------------------------------------------

void savePresets(String transmittedString){
      
        if(transmittedString.substring(0,2) == "$!"){
          
            int tempIndex = (transmittedString.substring( 2, 3).toInt());

            presetName[tempIndex] =  transmittedString.substring( 4, 20);
            preset[tempIndex] = transmittedString;

            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("saving...       ");

            writeStringOnEEPROM(transmittedString, 0, (byte) tempIndex * 2);
            
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("saved preset:   ");
            lcd.setCursor(0,1);
            lcd.print(presetName[tempIndex]);
            delay(3000);
            lcd.clear();

        }
        else {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("ERROR!          ");
            lcd.setCursor(0,1);
            lcd.print("SOMETHINGS WRONG");
            delay(3000);
            lcd.clear();
            
        }
  }

void checkForPresets(){
    
    for(int i = 0; i < 9; i++){
        
       String loadedString = getStringFromEEPROM(3,0,(byte)(i*2));
      
        if(loadedString.substring(0,2) == "$!"){
            loadedString = getStringFromEEPROM(485,0,(byte)(i*2));
            
            int tempIndex = (loadedString.substring( 2, 3).toInt());
    
            presetName[tempIndex] =  loadedString.substring( 4, 20);
            
            preset[tempIndex] = loadedString;
        }
      }
  }

void loadPresetFromString(String str){
  
    if(str.substring( 0, 2).equals("$!")){
    
      presetIndex = str.substring( 2, 3).toInt();
    
      presetName[presetIndex] =  str.substring( 4, 20);
    
      for(int i = 0; i < 16; i++){ 
        potiName[i] = str.substring( 21+i*17, 37+i*17);
      }
    
      for(int i = 0; i < 16; i++){ 
        ccValue[i] = str.substring( 293+i*4, 297+i*4).toInt();
      }
    
      for(int i = 0; i < 16; i++){ 
        toggleMin[i] = str.substring( 357+i*4, 361+i*4).toInt();
      }
    
      for(int i = 0; i < 16; i++){ 
        toggleMax[i] = str.substring( 421+i*4, 425+i*4).toInt();
      }
    }
     
  }
  
  
  
// Write and Read the EEPROM
//------------------------------------------------------------------------------
void writeStringOnEEPROM(String input, byte low, byte high){
    
    byte dataArray[input.length()+1];
   
    input.getBytes(dataArray, input.length()+1);
  
    for(int i = 0; i <= input.length(); i++){
      Wire.beginTransmission(0x50);
      Wire.write(high);
      Wire.write(low);
      Wire.write(dataArray[i]);
      Wire.endTransmission();
      delay(5);

      if(low == 255) high++;
      
      low++;
    }  
  }

String getStringFromEEPROM(int dataLength, byte low, byte high){
    
    char result[dataLength]; 
  
    for(int i = 0; i < dataLength; i++){
      
      Wire.beginTransmission(0x50);
      Wire.write(high);
      Wire.write(low);
      Wire.endTransmission();
      Wire.requestFrom(0x50,1);
      delay(5);
      
      result[i] = Wire.read();
      low++;
      
      if(low == 255){
        high++;
      }
      
    }
    
    String resultString = result;
    return resultString;
  }

void deleteEEPROM(){
  
    for(int i = 0; i <= 18; i++){
      for(int j = 0; j <= 3; j++){
        
       Wire.beginTransmission(0x50);
       Wire.write(i);
       Wire.write(j);
       Wire.write(0);
       Wire.endTransmission();
       delay(5);
       
      }
    } 

    for(int i = 0; i < 9; i++){

        presetName[i] = "none";
        preset[i] = "none";
      
    }
    
  }

// MIDI Output Funktionen
//------------------------------------------------------------------------------
void midiWrite(int type, int db1, int db2) {
  Serial1.write(type);
  Serial1.write(db1);
  Serial1.write(db2);
}

// communication between preset editor and midi controller
void establishContact() {
  while (Serial.available() <= 0) {
  Serial.println("A");   // send a capital A
  delay(300);
  
  if(digitalRead(button2) == HIGH){
      displayMode = 0;
      showPotiText();
      break;
    }
  
  }
}


// Preset Verbindung 
//------------------------------------------------------------------------------
void establishContactFull(){

//     while (Serial.available() <= 0) {
//        Serial.println("A");   // send a capital A
//        delay(300);
//  
//        if(digitalRead(button2) == HIGH){
//          displayMode = 0;
//          showPotiText();
//          break;
//        }
//  
//      }
     establishContact();

     String temp = Serial.readString();

     if (!(temp.equals("A"))){

       lcd.clear();
       lcd.setCursor(0,0);
       lcd.print("receiving data..");

       lcd.setCursor(0,1);
       lcd.print("                ");
       lcd.setCursor(0,1);

       switch (editorCounter) {
        case 0:
          lcd.print("              0%");
          break;
        case 1:
          lcd.print(".            13%");
          break;
        case 2:
          lcd.print("..           25%");
          break;
        case 3:
          lcd.print("...          38%");
          break;
         case 4:
          lcd.print("....         50%");
          break;
        case 5:
          lcd.print(".....        63%");
          break;
        case 6:
          lcd.print("......       75%");
          break;
        case 7:
          lcd.print(".......      88%");
          break;
        case 8:
          lcd.print("........    100%");
          break;
        }

       editorInput[editorCounter] = temp;
       editorCounter++;
     }
     
     if (editorCounter == 10){     

       for(int i = 0; i < 10; i++){
          editorCombined += editorInput[i];
       }
      
       savePresets(editorCombined);

       editorCombined = "";
       editorCounter = 0;

       displayMode = 0;
       showPotiText();
     }

}

void buttonController(){
    //Enter main menu
    if (digitalRead(button1) == HIGH && displayMode == 0) {
      displayMode = 1;
      showMenu();
      delay(300);
    }
    if (digitalRead(button2) == HIGH && displayMode == 1) {
      displayMode = 0;
      showPotiText();
      delay(300);
    }
    // Main Menu
    if (digitalRead(button1) == HIGH && displayMode == 1 && menuIndex == 0) {
      displayMode = 2;
      showSelectPoti();
      delay(300);
     }
    if (digitalRead(button2) == HIGH && displayMode == 2) {
      displayMode = 1;
      showMenu();
      delay(300);
    }
    if (digitalRead(button1) == HIGH && displayMode == 1 && menuIndex == 1) {
      displayMode = 40;
      showSelectMidiChannel();
      delay(300);
     }  
     
    if (digitalRead(button2) == HIGH && displayMode == 40) {
      displayMode = 1;
      showMenu();
      delay(300);
     }
    if (digitalRead(button1) == HIGH && displayMode == 40) {
      setMidiChannel();
      displayMode = 0;
      showPotiText();
      delay(300);
     }
     
    if (digitalRead(button1) == HIGH && displayMode == 1 && menuIndex == 3) {// select preset
      displayMode = 10;
      showLoadPreset();
      delay(300);
     }
    if (digitalRead(button1) == HIGH && displayMode == 1 && menuIndex == 2) {// Connect
      displayMode = 20;
      showSetPreset();
      delay(300);
     }

    if (digitalRead(button1) == HIGH && displayMode == 1 && menuIndex == 4) {//clear EEPROM
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("deleting EEPROM ");
      
      deleteEEPROM();

      lcd.setCursor(0,1);
      lcd.print("done!");
      delay(2000);

      displayMode = 0;
      showPotiText();
      delay(300);
    }
     if (digitalRead(button2) == HIGH && displayMode == 20) {//set preset
      displayMode = 1;
      showMenu();
      delay(300);
     }
     
     if (digitalRead(button1) == HIGH && displayMode == 20) {//set preset
      loadPresetFromString(preset[presetIndex]);
      displayMode = 0;
      showPotiText();
      delay(300);
     }
    
    //select CC
     if (digitalRead(button1) == HIGH && displayMode == 2) {
      displayMode = 3;
      showSelectCC();
      delay(300);
    }
    if (digitalRead(button2) == HIGH && displayMode == 3) {
      displayMode = 2;
      showSelectPoti();
      delay(300);
    }
    
    //select Toggle Min
     if (digitalRead(button1) == HIGH && displayMode == 3) {
      displayMode = 4;
      showSelectToggleMin();
      delay(300);
    }
    if (digitalRead(button2) == HIGH && displayMode == 4) {
      displayMode = 3;
      showSelectCC();
      delay(300);
    }
    
    //select Toggle max
     if (digitalRead(button1) == HIGH && displayMode == 4) {
      displayMode = 5;
      showSelectToggleMax();
      delay(300);
    }
    if (digitalRead(button2) == HIGH && displayMode == 5) {
      displayMode = 4;
      showSelectToggleMin();
      delay(300);
    }
    
    //set the values
    if (digitalRead(button1) == HIGH && displayMode == 5) {
      setPoti();
      displayMode = 0;
      showPotiText();
      delay(300);
    }
}

void displayModeController(){
  if(displayMode == 1){
      showStringInLine2(selectMenu());
    }
    if(displayMode == 2){
      showIntInLine2(selectPoti());
    }
    if(displayMode == 3){
      showIntInLine2(selectCC());
    }
    if(displayMode == 4){
      showIntInLine2(selectToggleMin());
    }
    if(displayMode == 5){
      showIntInLine2(selectToggleMax());
    }
    if(displayMode == 40){
      showIntInLine2(selectMidiChannel());
    }
    
    if(displayMode == 20){
      showStringInLine2(selectPreset());
    }
    if(displayMode == 10){
      establishContactFull();
    }
}



