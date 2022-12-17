/*

*/

/********* ADAFRUIT DISPLAY SETTINGS *********/
  #include <Wire.h>
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>
  #define SCREEN_WIDTH 128 // OLED display width, in pixels
  #define SCREEN_HEIGHT 64 // OLED display height, in pixels

  #define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
  #define SCREEN_ADDRESS 0x3C // set address for the display
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/****************IRREMOTE SETTINGS************/
  #include <IRremote.hpp>
  #define USE_DEFAULT_FEEDBACK_LED_PIN LED_BUILTIN
  #define IR_RECEIVE_PIN 6
  #define IR_SEND_PIN 3

  #include <Arduino.h>

/************ SET GLOBAL VARIABLES ***********/
  // Storage for the recorded code
  struct storedIRDataStruct {
      IRData receivedIRData;
      // extensions for sendRaw
      uint8_t rawCode[RAW_BUFFER_LENGTH]; // The durations if raw
      uint8_t rawCodeLength; // The length of the code
  } sStoredIRData;

  // feedback pin
  int STATUS_PIN = LED_BUILTIN; // set the status led pin (used when receiving ir signals). In this case is the built-in led in any Arduino board.

  // send pins
  int DELAY_BETWEEN_REPEAT = 50; // delay in milliseconds between every time it sends the IR signal (a value from 25 to 75 is probably the safest)
  int DEFAULT_NUMBER_OF_REPEATS_TO_SEND = 2; // times to send the ir signal (it's safer to send the same signal 1/2 times to be sure it works)

  // set the pin numbers of every button used to navigate in the menu
  int Left = 9;
  int Up = 10;
  int Down = 11;
  int Right = 12;

  // set the number of "char(<number>)" for every arrow position (used for the menu)
  int Left_arrow = 27;
  int Up_arrow = 24;
  int Down_arrow = 25;
  int Right_arrow = 26;

  // menu settings (in pixels)
  int Top_margin = 5;
  int Line_space = 16;
  int Left_margin = 4;
  int Right_margin = 8;

  // state variable to switch state in the menu and other
  int State = 0;

/******************* SETUP *******************/
  void setup() {
    // begin a serial connection at 9600 baud
    Serial.begin(9600);
    
    // clear the display and set various things like the text color and other
    if (!display.begin( SSD1306_SWITCHCAPVCC, 0x3C)) {
      while (true);
      display.clearDisplay();
      display.display();
      display.setTextColor(WHITE);
      delay(1000);
    }

    // set pin modes for various buttons (and leds or other if you want to add)
    pinMode(Left, INPUT);
    pinMode(Up, INPUT);
    pinMode(Down, INPUT);
    pinMode(Right, INPUT);

    // tells at irremote how and where to receive and transmit ir signals
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK, USE_DEFAULT_FEEDBACK_LED_PIN); // Specify receive pin and enable feedback LED at default feedback LED pin
    IrSender.begin(IR_SEND_PIN, ENABLE_LED_FEEDBACK, USE_DEFAULT_FEEDBACK_LED_PIN); // Specify send pin and enable feedback LED at default feedback LED pin
  }

/***************** MAIN LOOP *****************/
  /*
  the main loop in this program basically check the state value and switch the case according to its value.
  Every case has its own function which just control the display and check for an input to do things.
  */
  void loop() {
    switch (State){
      case 0:
        main_menu_1();
        break;
      case 1:
        main_menu_2();
        break;
      case 2:
        main_menu_3();
        break;

      case 3:
        ir_signal_1();
        break;

      case 4:
        universal_remote_1();
        break;
      case 5:
        universal_remote_2();
        break;
      case 6:
        universal_remote_transmit();
        break;

      case 7:
        saved_remotes_1();
        break;

      case 8:
        learn_remote_1();
        break;
      case 9:    
        learn_remote_2_1();
        break;
      case 10:
        learn_remote_2_2();
        break;
      case 11:
        learn_remote_2_3();
        break;
    }
  }

/************ MAIN MENU FUNCTIONS ************/
  // first page of the main menu (for now it only works with IR signals)
  void main_menu_1() {
    // this first part of the function is somehow the same in any function:
    display.clearDisplay(); // clear the display from any previous texts
    display.setTextColor(WHITE); // set the colour of the text (important)
    display.setTextSize(1); // set the text size

    // print the first line
    display.setCursor(Left_margin, Top_margin); // setting the cursor means setting the coordinates where the text needs to start from
    display.println("IR Signal"); // just print IR signal
    display.setCursor((128 - Right_margin), Top_margin); // set the cursor to the right
    display.print(char(Up_arrow)); // print an arrow character

    // same thing as the first line but with second third and fourth
    display.setCursor(Left_margin, (Top_margin + Line_space));
    display.println("something 1");
    display.setCursor((128 - Right_margin), (Top_margin + Line_space));
    display.print(char(Left_arrow));

    display.setCursor(Left_margin, (Top_margin + Line_space * 2));
    display.println("something 2");
    display.setCursor((128 - Right_margin), (Top_margin + Line_space * 2));
    display.print(char(Right_arrow));

    display.setCursor(Left_margin, (Top_margin + Line_space * 3));
    display.println("Pag. down");
    display.setCursor((128 - Right_margin), (Top_margin + Line_space * 3));
    display.print(char(Down_arrow));

    display.display(); // thisplay all the things setted above
    
    // if you press the up arrow -> go to the ir signal page
    if (digitalRead(Up) == HIGH) {
      State = 3;
    }
    // else, if you press the the left arrow go to another page and so on
    else if (digitalRead(Left) == HIGH) {
      State = 0;
    }
    else if (digitalRead(Right) == HIGH) {
      State = 0;
    }
    else if (digitalRead(Down) == HIGH) {
      State = 1;
    }

    delay(100); // use a delay of 100 milliseconds between every "frame" so that the Arduino doesn't overload
    
  }

  // second page of the main menu (doesn't do anything)
  void main_menu_2() {
    display.clearDisplay();
    display.setTextColor( WHITE);
    display.setTextSize(1);

    display.setCursor(Left_margin, Top_margin);
    display.println("Pag. up");
    display.setCursor((128 - Right_margin), Top_margin);
    display.print(char(Up_arrow));

    display.setCursor(Left_margin, (Top_margin + Line_space));
    display.println("something 3");
    display.setCursor((128 - Right_margin), (Top_margin + Line_space));
    display.print(char(Left_arrow));

    display.setCursor(Left_margin, (Top_margin + Line_space * 2));
    display.println("something 4");
    display.setCursor((128 - Right_margin), (Top_margin + Line_space * 2));
    display.print(char(Right_arrow));

    display.setCursor(Left_margin, (Top_margin + Line_space * 3));
    display.println("Pag. down");
    display.setCursor((128 - Right_margin), (Top_margin + Line_space * 3));
    display.print(char(Down_arrow));

    display.display();

    if (digitalRead(Up) == HIGH) {
      State = 0;
    }
    else if (digitalRead(Left) == HIGH) {
      State = 1;
    }
    else if (digitalRead(Right) == HIGH) {
      State = 1;
    }
    else if (digitalRead(Down) == HIGH) {
      State = 2;
    }

    delay(100);
      
  }

  // third page of the main menu, doesn't do anything
  void main_menu_3() {
    display.clearDisplay();
    display.setTextColor( WHITE);
    display.setTextSize(1);

    display.setCursor(Left_margin, Top_margin);
    display.println("Pag. up");
    display.setCursor((128 - Right_margin), Top_margin);
    display.print(char(Up_arrow));

    display.setCursor(Left_margin, (Top_margin + Line_space));
    display.println("something 5");
    display.setCursor((128 - Right_margin), (Top_margin + Line_space));
    display.print(char(Left_arrow));

    display.setCursor(Left_margin, (Top_margin + Line_space * 2));
    display.println("Open SD");
    display.setCursor((128 - Right_margin), (Top_margin + Line_space * 2));
    display.print(char(Right_arrow));

    display.setCursor(Left_margin, (Top_margin + Line_space * 3));
    display.println("Info");
    display.setCursor((128 - Right_margin), (Top_margin + Line_space * 3));
    display.print(char(Down_arrow));

    display.display();

    if (digitalRead(Up) == HIGH) {
      State = 1;
    }
    else if (digitalRead(Left) == HIGH) {
      State = 2;
    }
    else if (digitalRead(Right) == HIGH) {
      State = 2;
    }
    else if (digitalRead(Down) == HIGH) {
      State = 2;
    }

    delay(100);
    
  }

/************** IR SIGNAL MENU ***************/
  void ir_signal_1() {
    display.clearDisplay();
    display.setTextColor( WHITE);
    display.setTextSize(1);

    display.setCursor(Left_margin, Top_margin);
    display.println("BACK");
    display.setCursor((128 - Right_margin), Top_margin);
    display.print(char(Up_arrow));

    display.setCursor(Left_margin, (Top_margin + Line_space));
    display.println("Univ. Remote");
    display.setCursor((128 - Right_margin), (Top_margin + Line_space));
    display.print(char(Left_arrow));

    display.setCursor(Left_margin, (Top_margin + Line_space * 2));
    display.println("Saved Remote");
    display.setCursor((128 - Right_margin), (Top_margin + Line_space * 2));
    display.print(char(Right_arrow));

    display.setCursor(Left_margin, (Top_margin + Line_space * 3));
    display.println("Learn Remote");
    display.setCursor((128 - Right_margin), (Top_margin + Line_space * 3));
    display.print(char(Down_arrow));

    display.display();

    if (digitalRead(Up) == HIGH) {
      State = 0;
    }
    else if (digitalRead(Left) == HIGH) {
      State = 4;
    }
    else if (digitalRead(Right) == HIGH) {
      State = 7;
    }
    else if (digitalRead(Down) == HIGH) {
      IrReceiver.resume(); // essential to start receiving a signal
      State = 8;
    }

    delay(100);

  }

  /************* UNIVERSAL REMOTES (Doesn't work) ************/
    void universal_remote_1() {
      display.clearDisplay();
      display.setTextColor( WHITE);
      display.setTextSize(1);

      display.setCursor(Left_margin, Top_margin);
      display.println("BACK");
      display.setCursor((128 - Right_margin), Top_margin);
      display.print(char(Up_arrow));

      display.setCursor(Left_margin, (Top_margin + Line_space));
      display.println("Vol. -");
      display.setCursor((128 - Right_margin), (Top_margin + Line_space));
      display.print(char(Left_arrow));

      display.setCursor(Left_margin, (Top_margin + Line_space * 2));
      display.println("Vol. +");
      display.setCursor((128 - Right_margin), (Top_margin + Line_space * 2));
      display.print(char(Right_arrow));

      display.setCursor(Left_margin, (Top_margin + Line_space * 3));
      display.println("Pag. Down");
      display.setCursor((128 - Right_margin), (Top_margin + Line_space * 3));
      display.print(char(Down_arrow));

      display.display();

      if (digitalRead(Up) == HIGH) {
        State = 3;
      }
      else if (digitalRead(Left) == HIGH) {
        // TBD (to be done): call the function for universal remotes
        State = 6;
      }
      else if (digitalRead(Right) == HIGH) {
        // TBD: call the function for universal remotes
        State = 6;
      }
      else if (digitalRead(Down) == HIGH) {
        State = 5;
      }

      delay(100);

    }

    void universal_remote_2() {
      display.clearDisplay();
      display.setTextColor( WHITE);
      display.setTextSize(1);

      display.setCursor(Left_margin, Top_margin);
      display.println("Pag. up");
      display.setCursor((128 - Right_margin), Top_margin);
      display.print(char(Up_arrow));

      display.setCursor(Left_margin, (Top_margin + Line_space));
      display.println("Cha. -");
      display.setCursor((128 - Right_margin), (Top_margin + Line_space));
      display.print(char(Left_arrow));

      display.setCursor(Left_margin, (Top_margin + Line_space * 2));
      display.println("Cha. +");
      display.setCursor((128 - Right_margin), (Top_margin + Line_space * 2));
      display.print(char(Right_arrow));

      display.setCursor(Left_margin, (Top_margin + Line_space * 3));
      display.println("Power");
      display.setCursor((128 - Right_margin), (Top_margin + Line_space * 3));
      display.print(char(Down_arrow));

      display.display();

      if (digitalRead(Up) == HIGH) {
        State = 4;
      }
      else if (digitalRead(Left) == HIGH) {
        // TBD: call the function for universal remotes
        State = 6;
      }
      else if (digitalRead(Right) == HIGH) {
        // TBD: call the function for universal remotes
        State = 6;
      }
      else if (digitalRead(Down) == HIGH) {
        // TBD: call the function for universal remotes
        State = 6;
      }

      delay(100);
      
    }

    void universal_remote_transmit() {
      int IR_signal = "<IR signal>";

      display.clearDisplay();
      display.setTextColor( WHITE);
      display.setTextSize(1);

      display.setCursor(10, 5);
      display.println("Sending IR signal:");

      display.setCursor(30, 25);
      display.println(IR_signal);

      display.setCursor(15, 55);
      display.println("STOP");
      display.setCursor(4, 55);
      display.print(char(Up_arrow));

      display.display();

      if (digitalRead(Up) == HIGH) {
        // interrompi funzione di ricerca segnale
        State = 4;
      }

      delay(100);
      
    }

  /************** SAVED REMOTES (Doesn't work) ***************/
    void saved_remotes_1() {
      display.clearDisplay();
      display.setTextColor( WHITE);
      display.setTextSize(1);

      display.setCursor(Left_margin, Top_margin);
      display.println("BACK");
      display.setCursor((128 - Right_margin), Top_margin);
      display.print(char(Up_arrow));

      display.setCursor(Left_margin, (Top_margin + Line_space));
      display.println("Imported Remotes");
      display.setCursor((128 - Right_margin), (Top_margin + Line_space));
      display.print(char(Left_arrow));

      display.setCursor(Left_margin, (Top_margin + Line_space * 2));
      display.println("Custom Remotes");
      display.setCursor((128 - Right_margin), (Top_margin + Line_space * 2));
      display.print(char(Right_arrow));

      display.display();

      if (digitalRead(Up) == HIGH) {
        State = 3;
      }
      else if (digitalRead(Left) == HIGH) {
        State = 7;
      }
      else if (digitalRead(Right) == HIGH) {
        State = 7;
      }

      delay(100);
      
    }

  /********** LEARN REMOTE (it works (hopefully)) ************/
    void learn_remote_1() {
      display.clearDisplay();
      display.setTextColor( WHITE);
      display.setTextSize(1);

      display.setCursor(20, 5);
      display.println("Waiting for");
      display.setCursor(20, 25);
      display.println("signal...");

      display.setCursor(15, 55);
      display.println("STOP");
      display.setCursor(4, 55);
      display.print(char(Up_arrow));

      display.display();

      if (digitalRead(Up) == HIGH) {
        State = 3;
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

          State = 9;
        }
        else {
          IrReceiver.resume(); //...else, resume to listen at signals
        }
      }
      delay(100);
    }

    // function to print in the display the informations about the signal received
    void learn_remote_2_1() {
      int IR_protocol = sStoredIRData.receivedIRData.protocol;
      int IR_address = sStoredIRData.receivedIRData.address;
      int IR_command = sStoredIRData.receivedIRData.command;
      int IR_raw = sStoredIRData.receivedIRData.decodedRawData;

      display.clearDisplay();
      display.setTextColor( WHITE);
      display.setTextSize(1);

      display.setCursor(15, Top_margin);
      display.println("Try again");
      display.setCursor(5, Top_margin);
      display.print(char(Up_arrow));

      display.setCursor(Left_margin, (Top_margin + Line_space));
      display.print("Protocol: ");
      display.println(getProtocolString(IR_protocol));
      display.setCursor(Left_margin, (Top_margin + Line_space * 2));
      display.print("Command: 0x");
      display.println(IR_command,HEX);

      display.setCursor(15, 55);
      display.println("Test");
      display.setCursor(4, 55);
      display.print(char(Left_arrow));
      
      display.setCursor(70, 55);
      display.println("Confirm");
      display.setCursor(120, 55);
      display.print(char(Right_arrow));

      display.display();

      if (digitalRead(Up) == HIGH) {
        IrReceiver.resume();
        State = 8;
      }
      else if (digitalRead(Left) == HIGH) {
        sendCode(&sStoredIRData);
        State = 10;
      }
      else if (digitalRead(Right) == HIGH) {
        State = 11;
      }

      delay(100);
      
    }

    // test the signal received by sending it
    void learn_remote_2_2() {
      display.clearDisplay();
      display.setTextColor( WHITE);
      display.setTextSize(1);

      display.setCursor(20, 5);
      display.println("Command send.");
      display.setCursor(20, 25);
      display.println("Did it worked?");

      display.setCursor(15, 55);
      display.println("No");
      display.setCursor(4, 55);
      display.print(char(Left_arrow));
      
      display.setCursor(100, 55);
      display.println("Yes");
      display.setCursor(120, 55);
      display.print(char(Right_arrow));

      display.display();
      
      if (digitalRead(Left) == HIGH) {
        State = 9;
      }
      else if (digitalRead(Right) == HIGH) {
        State = 11;
      }

      delay(100);
      
    }

    // save the command received in the SD (TBD: to be done)
    void learn_remote_2_3() {
      display.clearDisplay();
      display.setTextColor( WHITE);
      display.setTextSize(1);

      display.setCursor(20, 5);
      display.println("Command registered");
      display.setCursor(20, 25);
      display.println("correctly!");

      display.setCursor(15, 55);
      display.println("Menu");
      display.setCursor(4, 55);
      display.print(char(Left_arrow));
      
      display.setCursor(60, 55);
      display.println("Learn new");
      display.setCursor(120, 55);
      display.print(char(Right_arrow));

      display.display();
      
      if (digitalRead(Left) == HIGH) {
        State = 3;
      }
      else if (digitalRead(Right) == HIGH) {
        IrReceiver.resume();
        State = 8;
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


