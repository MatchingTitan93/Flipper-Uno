/*
PLEASE NOTE:
this program needs a lot of flash memory: 140.000 bytes which is something like 90% on Arduino Nano Every and 0% on Nano RP2040 Connect.
So run this only if you have a powerful hardware. Also, if you are using the RP2040 it could give you an error like:

C:\Users\Sborra\Documents\Arduino\libraries\GEM-1.4.0\src\GEM_u8g2.cpp: In member function 'void GEM_u8g2::enterEditValueMode()':
C:\Users\Sborra\Documents\Arduino\libraries\GEM-1.4.0\src\GEM_u8g2.cpp:558:7: error: 'dtostrf' was not declared in this scope
       dtostrf(*(float*)menuItemTmp->linkedVariable, menuItemTmp->precision + 1, menuItemTmp->precision, _valueString);
       ^~~~~~~
C:\Users\Sborra\Documents\Arduino\libraries\GEM-1.4.0\src\GEM_u8g2.cpp:558:7: note: suggested alternative: 'strstr'
       dtostrf(*(float*)menuItemTmp->linkedVariable, menuItemTmp->precision + 1, menuItemTmp->precision, _valueString);
       ^~~~~~~
       strstr

exit status 1

Compilation error: exit status 1

In this case, go to C:\Users\<YourName>\Documents\Arduino\libraries\GEM-1.4.0\src\GEM_u8g2.cpp and edit lines 40-45 from:
// AVR-based Arduinos have suppoort for dtostrf, some others may require manual inclusion (e.g. SAMD),
// see https://github.com/plotly/arduino-api/issues/38#issuecomment-108987647
#if defined(GEM_SUPPORT_FLOAT_EDIT) && (defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_SAM))
#include <avr/dtostrf.h>
#endif

to:
#include <avr/dtostrf.h>

visit: https://github.com/MatchingTitan93/Flipper-Uno and ask for help if you encounter any issue with the program or the hardware, every advise is well accepted. 
*/

/*************** MENU SETTINGS ***************/
void FunctionToBeReplaced();
void SwitchToLearnRemote();

#include <GEM_u8g2.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// GEM pages
GEMPage menuPage_Main("Main Menu");
  GEMPage menuPage_Main_IRsignal("IR Signal");
    GEMPage menuPage_Main_IRsignal_UniversalRemotes("Universal Remotes");
    GEMPage menuPage_Main_IRsignal_SavedRemotes("Saved Remotes");
      GEMPage menuPage_Main_IRsignal_SavedRemotes_Imported("Imported Remotes");
      GEMPage menuPage_Main_IRsignal_SavedRemotes_Custom("Custom Remotes");
    GEMPage menuPage_Main_IRsignal_LearnRemote("Learn Remote");
  GEMPage menuPage_Main_Info("Info");

// GEM items
GEMItem menuItem_Main_IRsignal("IR Signal", menuPage_Main_IRsignal);
  GEMItem menuItem_Main_IRsignal_UniversalRemote("Universal Remotes", menuPage_Main_IRsignal_UniversalRemotes);
    GEMItem menuItem_Main_IRsignal_UniversalRemote_CommandPower("Power", FunctionToBeReplaced);
    GEMItem menuItem_Main_IRsignal_UniversalRemote_CommandChaIncrease("Cha +", FunctionToBeReplaced);
    GEMItem menuItem_Main_IRsignal_UniversalRemote_CommandChaDecrease("Cha -", FunctionToBeReplaced);
    GEMItem menuItem_Main_IRsignal_UniversalRemote_CommandVolIncrease("Vol +", FunctionToBeReplaced);
    GEMItem menuItem_Main_IRsignal_UniversalRemote_CommandVolDecrease("Vol -", FunctionToBeReplaced);
  GEMItem menuItem_Main_IRsignal_SavedRemotes("Saved Remotes", menuPage_Main_IRsignal_SavedRemotes);
    GEMItem menuItem_Main_IRsignal_SavedRemotes_Imported("Imported Remotes", menuPage_Main_IRsignal_SavedRemotes_Imported);
    GEMItem menuItem_Main_IRsignal_SavedRemotes_Custom("Saved Remotes", menuPage_Main_IRsignal_SavedRemotes_Custom);
  GEMItem menuItem_Main_IRsignal_LearnRemote("Learn Remote", SwitchToLearnRemote);
GEMItem menuItem_Main_Something1("Something1", menuPage_Main);
GEMItem menuItem_Main_Something2("Something2", menuPage_Main);
GEMItem menuItem_Main_Something3("Something3", menuPage_Main);
GEMItem menuItem_Main_Info("Info", menuPage_Main_Info);

GEM_u8g2 menu(u8g2);

/*************** IRREMOTE SETTINGS ***********/
  #include <IRremote.hpp>
  #define USE_DEFAULT_FEEDBACK_LED_PIN LED_BUILTIN
  #define IR_RECEIVE_PIN 6
  #define IR_SEND_PIN 3

  #include <Arduino.h>

/************ SET GLOBAL VARIABLES ***********/
  // set the pin numbers of every button used to navigate in the menu
  int ButtonConfirm = 7;
  int ButtonBack = 8;
  int ButtonLeft = 9;
  int ButtonUp = 10;
  int ButtonRight = 11;
  int ButtonDown = 12;

  // Storage for the recorded code
  struct storedIRDataStruct {
      IRData receivedIRData;
      // extensions for sendRaw
      uint8_t rawCode[RAW_BUFFER_LENGTH]; // The durations if raw
      uint8_t rawCodeLength; // The length of the code
  } sStoredIRData;

  // feedback pin
  int STATUS_PIN = LED_BUILTIN; // set the status led pin (used when receiving ir signals). In this case is the built-in led in any Arduino board.

  // send variables
  int DELAY_BETWEEN_REPEAT = 50; // delay in milliseconds between every time it sends the IR signal (a value from 25 to 75 is probably the safest)
  int DEFAULT_NUMBER_OF_REPEATS_TO_SEND = 2; // times to send the ir signal (it's safer to send the same signal 1/2 times to be sure it works)
  
  int State = 0;  

  void storeCode(IRData *aIRReceivedData);
  void sendCode(storedIRDataStruct *aIRDataToSend);

void FunctionToBeReplaced(){}
/******************* SETUP *******************/
  void setup() {
    // begin a serial connection at 9600 baud
    Serial.begin(9600);

    // U8g2 library init. Pass pin numbers the buttons are connected to.
    // The push-buttons should be wired with pullup resistors (so the LOW means that the button is pressed)
    u8g2.begin(/*Select/OK=*/ ButtonConfirm, /*Right/Next=*/ ButtonRight, /*Left/Prev=*/ ButtonLeft, /*Up=*/ ButtonUp, /*Down=*/ ButtonDown, /*Home/Cancel=*/ ButtonBack);

    // Menu init, setup and draw
    menu.init();
    setupMenu();
    menu.drawMenu();
    
    // tells at irremote how and where to receive and transmit ir signals
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK, USE_DEFAULT_FEEDBACK_LED_PIN); // Specify receive pin and enable feedback LED at default feedback LED pin
    IrSender.begin(IR_SEND_PIN, ENABLE_LED_FEEDBACK, USE_DEFAULT_FEEDBACK_LED_PIN); // Specify send pin and enable feedback LED at default feedback LED pin
  }

void setupMenu() {
  // Add menu pages
  menuPage_Main_IRsignal.setParentMenuPage(menuPage_Main);
    menuPage_Main_IRsignal_UniversalRemotes.setParentMenuPage(menuPage_Main_IRsignal);
    menuPage_Main_IRsignal_SavedRemotes.setParentMenuPage(menuPage_Main_IRsignal);
      menuPage_Main_IRsignal_SavedRemotes_Imported.setParentMenuPage(menuPage_Main_IRsignal_SavedRemotes);
      menuPage_Main_IRsignal_SavedRemotes_Custom.setParentMenuPage(menuPage_Main_IRsignal_SavedRemotes);
    menuPage_Main_IRsignal_LearnRemote.setParentMenuPage(menuPage_Main_IRsignal);
  menuPage_Main_Info.setParentMenuPage(menuPage_Main);
  
  // Add mennu items  
  menuPage_Main.addMenuItem(menuItem_Main_IRsignal);
    menuPage_Main_IRsignal.addMenuItem(menuItem_Main_IRsignal_UniversalRemote);
      menuPage_Main_IRsignal_UniversalRemotes.addMenuItem(menuItem_Main_IRsignal_UniversalRemote_CommandPower);
      menuPage_Main_IRsignal_UniversalRemotes.addMenuItem(menuItem_Main_IRsignal_UniversalRemote_CommandChaIncrease);
      menuPage_Main_IRsignal_UniversalRemotes.addMenuItem(menuItem_Main_IRsignal_UniversalRemote_CommandChaDecrease);
      menuPage_Main_IRsignal_UniversalRemotes.addMenuItem(menuItem_Main_IRsignal_UniversalRemote_CommandVolIncrease);
      menuPage_Main_IRsignal_UniversalRemotes.addMenuItem(menuItem_Main_IRsignal_UniversalRemote_CommandVolDecrease);
    menuPage_Main_IRsignal.addMenuItem(menuItem_Main_IRsignal_SavedRemotes);
      menuPage_Main_IRsignal_SavedRemotes.addMenuItem(menuItem_Main_IRsignal_SavedRemotes_Imported);
      menuPage_Main_IRsignal_SavedRemotes.addMenuItem(menuItem_Main_IRsignal_SavedRemotes_Custom);
    menuPage_Main_IRsignal.addMenuItem(menuItem_Main_IRsignal_LearnRemote);
  menuPage_Main.addMenuItem(menuItem_Main_Something1);
  menuPage_Main.addMenuItem(menuItem_Main_Something2);
  menuPage_Main.addMenuItem(menuItem_Main_Something3);
  menuPage_Main.addMenuItem(menuItem_Main_Info);

  

  // Add menu page to menu and set it as current
  menu.setMenuPageCurrent(menuPage_Main);
}

/***************** MAIN LOOP *****************/
  /*
  the main loop in this program basically check the state value and switch the case according to its value.
  Every case has its own function which just control the display and check for an input to do things.
  */
  void loop() {
    switch (State){
      case 0:
        MenuLoop();
        break;
      case 1:
        LearnRemote();
        break;
      case 2:
        LearnRemoteResults();
        break;
      case 3:
        LearnRemoteTest();
        break;
      case 4:
        LearnRemoteRegisteredCorrectly();
        break;
    }
  }

void MenuLoop() {
  // If menu is ready to accept button press...
  if (menu.readyForKey()) {
    // ...detect key press using U8g2 library
    // and pass pressed button to menu
    menu.registerKeyPress(u8g2.getMenuEvent());
  }    
}

void SwitchToLearnRemote() {
  State = 1;
  u8g2.clear();

  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_6x12_t_symbols);
    u8g2.drawStr(20,5, "Waiting for");
    u8g2.drawStr(20,25, "signal...");
    u8g2.drawStr(15,55, "STOP");
    u8g2.drawUTF8(4,55, "←");
  } while (u8g2.nextPage());

  IrReceiver.resume();  
}

void LearnRemote() {  
  if (digitalRead(ButtonLeft) == LOW) {
      // Draw menu back on screen and clear context
      menu.reInit();
      menu.drawMenu();
      menu.clearContext();

      Serial.println("Exit Learn remote");
      State = 0;
  }
  else if (IrReceiver.decode()) { // if the sensor detect a signal...
    if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) { // if the data is repeated it ignores it
      IrReceiver.resume();
      return;
    }
    
    int temp_protocol = IrReceiver.decodedIRData.protocol; // temporary variable to check if the data is useful

    if (temp_protocol != 0) {    // if the received data has a meaning, then...
      storeCode(IrReceiver.read()); //... store the data in the structure defined in the beginning

      // print in the serial some usefull informations
      Serial.print("Protocol: ");
      Serial.println(getProtocolString(sStoredIRData.receivedIRData.protocol));
      Serial.print("Address: 0x");
      Serial.println(sStoredIRData.receivedIRData.address,HEX);
      Serial.print("Command: 0x");
      Serial.println(sStoredIRData.receivedIRData.command,HEX);
      Serial.print("Decoded Raw: ");
      Serial.println(sStoredIRData.receivedIRData.decodedRawData,HEX);
      Serial.println();
      
      u8g2.clear();
      State = 2;
    }
    else {
      IrReceiver.resume(); //...else, resume to listen at signals
    }
  }
  delay(100);
}

void LearnRemoteResults() {
    int IR_protocol = sStoredIRData.receivedIRData.protocol;
    int IR_address = sStoredIRData.receivedIRData.address;
    int IR_command = sStoredIRData.receivedIRData.command;
    int IR_raw = sStoredIRData.receivedIRData.decodedRawData;
    u8g2.firstPage();
    
    do {
      u8g2.setFont(u8g2_font_6x12_t_symbols);
      
      u8g2.drawUTF8(5, 5, "↑");
      u8g2.drawStr(15, 5, "Try again");

      u8g2.setCursor(5,20);
      u8g2.print("Protocol: ");
      u8g2.print(getProtocolString((decode_type_t)IR_protocol));
      
      u8g2.setCursor(5,35);
      u8g2.print("Command: 0x");
      u8g2.print(IR_command,HEX);
      
      u8g2.drawUTF8(5, 50, "←");
      u8g2.drawStr(15, 50, "Test");

      u8g2.drawStr(70, 50, "Confirm");
      u8g2.drawUTF8(120, 50, "→");
      
    } while (u8g2.nextPage());

    if (digitalRead(ButtonUp) == LOW) {
      SwitchToLearnRemote();
    }
    else if (digitalRead(ButtonLeft) == LOW) {
      sendCode(&sStoredIRData);
      u8g2.clear();
      State = 3;
    }
    else if (digitalRead(ButtonRight) == LOW) {
      u8g2.clear();
      State = 4;
    }

    delay(100);
}

// test the signal received by sending it
void LearnRemoteTest() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_6x12_t_symbols);
    
    u8g2.drawStr(20, 5, "Command sent.");
    u8g2.drawStr(20, 25, "Did it worked?");
    
    u8g2.drawUTF8(5, 50, "←");
    u8g2.drawStr(15, 50, "No");

    u8g2.drawStr(100, 50, "Yes");
    u8g2.drawUTF8(120, 50, "→");
    
  } while (u8g2.nextPage());
  
  if (digitalRead(ButtonLeft) == LOW) {
    u8g2.clear();
    State = 2;
  }
  else if (digitalRead(ButtonRight) == LOW) {
    u8g2.clear();
    State = 4;
  }

  delay(100);
  
}

// save the command received in the SD (TBD: to be done)
void LearnRemoteRegisteredCorrectly() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_6x12_t_symbols);
    
    u8g2.drawStr(20, 5, "Command registered");
    u8g2.drawStr(20, 25, "correctly!");
    
    u8g2.drawUTF8(5, 50, "←");
    u8g2.drawStr(15, 50, "Menu");

    u8g2.drawStr(60, 50, "Learn new");
    u8g2.drawUTF8(120, 50, "→");
    
  } while (u8g2.nextPage());

  if (digitalRead(ButtonLeft) == LOW) {
    // Draw menu back on screen and clear context
    menu.reInit();
    menu.drawMenu();
    menu.clearContext();

    Serial.println("Exit Learn remote");
    State = 0;
  }
  else if (digitalRead(ButtonRight) == LOW) {
    SwitchToLearnRemote();
  }

  delay(100);
  
}

/************ STORE AND SEND IR SIGNAL FUNCTIONS **********/
  // function to store code in a structure for using it in different ways: sending it, nothing more.
  void storeCode(IRData *aIRReceivedData) {
    if (aIRReceivedData->flags & IRDATA_FLAGS_IS_REPEAT) {
        Serial.println(F("Ignore repeat"));
        return;
    }
    if (aIRReceivedData->flags & IRDATA_FLAGS_IS_AUTO_REPEAT) {
        Serial.println(F("Ignore autorepeat"));
        return;
    }
    if (aIRReceivedData->flags & IRDATA_FLAGS_PARITY_FAILED) {
        Serial.println(F("Ignore parity error"));
        return;
    }
    /*
      * Copy decoded data
      */
    sStoredIRData.receivedIRData = *aIRReceivedData;

    if (sStoredIRData.receivedIRData.protocol == UNKNOWN) {
        Serial.print(F("Received unknown code and store "));
        Serial.print(IrReceiver.decodedIRData.rawDataPtr->rawlen - 1);
        Serial.println(F(" timing entries as raw "));
        IrReceiver.printIRResultRawFormatted(&Serial, true); // Output the results in RAW format
        sStoredIRData.rawCodeLength = IrReceiver.decodedIRData.rawDataPtr->rawlen - 1;
        /*
          * Store the current raw data in a dedicated array for later usage
          */
        IrReceiver.compensateAndStoreIRResultInArray(sStoredIRData.rawCode);
    } else {
        IrReceiver.printIRResultShort(&Serial);
        IrReceiver.printIRSendUsage(&Serial);
        sStoredIRData.receivedIRData.flags = 0; // clear flags -esp. repeat- for later sending
        Serial.println();
    }
  }

  // function to transmit code using the data given in a structure.
  void sendCode(storedIRDataStruct *aIRDataToSend) {
    if (aIRDataToSend->receivedIRData.protocol == UNKNOWN /* i.e. raw */) {
        // Assume 38 KHz
        IrSender.sendRaw(aIRDataToSend->rawCode, aIRDataToSend->rawCodeLength, 38);

        Serial.print(F("Sent raw "));
        Serial.print(aIRDataToSend->rawCodeLength);
        Serial.println(F(" marks or spaces"));
    } else {

        /*
          * Use the write function, which does the switch for different protocols
          */
        IrSender.write(&aIRDataToSend->receivedIRData, DEFAULT_NUMBER_OF_REPEATS_TO_SEND);

        Serial.print(F("Sent: "));
        printIRResultShort(&Serial, &aIRDataToSend->receivedIRData, false);
    }
  }
//


