// #include <Arduino.h>

// #define IR_RECEIVE_PIN          15  
// #define IR_SEND_PIN             23  
// #define TONE_PIN                27 
// #define APPLICATION_PIN         16

// #define DECODE_DENON
// #define DECODE_JVC
// #define DECODE_KASEIKYO
// #define DECODE_PANASONIC   
// #define DECODE_LG
// #define DECODE_NEC         
// #define DECODE_SAMSUNG
// #define DECODE_SONY
// #define DECODE_RC5
// #define DECODE_RC6

// #define DECODE_BOSEWAVE
// #define DECODE_LEGO_PF
// #define DECODE_MAGIQUEST
// #define DECODE_WHYNTER
// #define DECODE_FAST
// //

// #if !defined(RAW_BUFFER_LENGTH)
// // For air condition remotes it requires 750. Default is 200.
// #  if !((defined(RAMEND) && RAMEND <= 0x4FF) || (defined(RAMSIZE) && RAMSIZE < 0x4FF))
// #define RAW_BUFFER_LENGTH  750
// #  endif
// #endif

// //#define EXCLUDE_UNIVERSAL_PROTOCOLS // Saves up to 1000 bytes program memory.
// //#define EXCLUDE_EXOTIC_PROTOCOLS // saves around 650 bytes program memory if all other protocols are active
// //#define NO_LED_FEEDBACK_CODE      // saves 92 bytes program memory
// #define RECORD_GAP_MICROS 12000   // Default is 8000. Activate it for some LG air conditioner protocols
// //#define SEND_PWM_BY_TIMER         // Disable carrier PWM generation in software and use (restricted) hardware PWM.
// //#define USE_NO_SEND_PWM           // Use no carrier PWM, just simulate an active low receiver signal. Overrides SEND_PWM_BY_TIMER definition

// // MARK_EXCESS_MICROS is subtracted from all marks and added to all spaces before decoding,
// // to compensate for the signal forming of different IR receiver modules. See also IRremote.hpp line 142.
// #define MARK_EXCESS_MICROS    50    // Adapt it to your IR receiver module. 20 is recommended for the cheap VS1838 modules.

// // #define DEBUG // Activate this for lots of lovely debug output from the decoders.

// #include <IRremote.hpp>

// int SEND_BUTTON_PIN = APPLICATION_PIN;

// int DELAY_BETWEEN_REPEAT = 500;

// // Storage for the recorded code
// struct storedIRDataStruct {
//     IRData receivedIRData;
//     // extensions for sendRaw
//     uint8_t rawCode[RAW_BUFFER_LENGTH]; // The durations if raw
//     uint8_t rawCodeLength; // The length of the code
// } sStoredIRData;

// bool sSendButtonWasActive;

// void storeCode();
// void sendCode(storedIRDataStruct *aIRDataToSend);

// void setup() {
//     pinMode(SEND_BUTTON_PIN, INPUT_PULLUP);

//     Serial.begin(115200);
//     while (!Serial)
//         ; // Wait for Serial to become available. Is optimized away for some cores.

// #if defined(__AVR_ATmega32U4__) || defined(SERIAL_PORT_USBVIRTUAL) || defined(SERIAL_USB) /*stm32duino*/|| defined(USBCON) /*STM32_stm32*/ \
//     || defined(SERIALUSB_PID)  || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_attiny3217)
//     delay(4000); // To be able to connect Serial monitor after reset or power up and before first print out. Do not wait for an attached Serial Monitor!
// #endif
//     // Just to know which program is running on my Arduino
//     Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));

//     // Start the receiver and if not 3. parameter specified, take LED_BUILTIN pin from the internal boards definition as default feedback LED
//     IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
//     Serial.print(F("Ready to receive IR signals of protocols: "));
//     printActiveIRProtocols(&Serial);

//     IrSender.begin(); // Start with IR_SEND_PIN -which is defined in PinDefinitionsAndMore.h- as send pin and enable feedback LED at default feedback LED pin
//     Serial.println(SEND_BUTTON_PIN);
// }

// void loop() {

//     // If button pressed, send the code.
//     bool tSendButtonIsActive = (digitalRead(SEND_BUTTON_PIN) == LOW); // Button pin is active LOW

//     /*
//      * Check for current button state
//      */
//     if (tSendButtonIsActive) {
//         if (!sSendButtonWasActive) {
//             Serial.println(F("Stop receiving"));
//             IrReceiver.stop();
//         }
//         /*
//          * Button pressed -> send stored data
//          */
//         Serial.print(F("Button pressed, now sending "));
//         if (sSendButtonWasActive == tSendButtonIsActive) {
//             Serial.print(F("repeat "));
//             sStoredIRData.receivedIRData.flags = IRDATA_FLAGS_IS_REPEAT;
//         } else {
//             sStoredIRData.receivedIRData.flags = IRDATA_FLAGS_EMPTY;
//         }
//         Serial.flush(); // To avoid disturbing the software PWM generation by serial output interrupts
//         sendCode(&sStoredIRData);
//         delay(DELAY_BETWEEN_REPEAT); // Wait a bit between retransmissions

//     } else if (sSendButtonWasActive) {
//         /*
//          * Button is just released -> activate receiving
//          */
//         // Restart receiver
//         Serial.println(F("Button released -> start receiving"));
//         IrReceiver.start();

//     } else if (IrReceiver.decode()) {
//         /*
//          * Button is not pressed and data available -> store received data and resume
//          */
//         storeCode();
//         IrReceiver.resume(); // resume receiver
//     }

//     sSendButtonWasActive = tSendButtonIsActive;
//     delay(100);
// }

// // Stores the code for later playback in sStoredIRData
// // Most of this code is just logging
// void storeCode() {
//     if (IrReceiver.decodedIRData.rawDataPtr->rawlen < 4) {
//         // Serial.print(F("Ignore data with rawlen="));
//         // Serial.println(IrReceiver.decodedIRData.rawDataPtr->rawlen);
//         return;
//     }
//     if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
//         Serial.println(F("Ignore repeat"));
//         return;
//     }
//     if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_AUTO_REPEAT) {
//         Serial.println(F("Ignore autorepeat"));
//         return;
//     }
//     if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_PARITY_FAILED) {
//         Serial.println(F("Ignore parity error"));
//         return;
//     }
//     /*
//      * Copy decoded data
//      */
//     sStoredIRData.receivedIRData = IrReceiver.decodedIRData;

//     if (sStoredIRData.receivedIRData.protocol == UNKNOWN) {
//         Serial.print(F("Received unknown code and store "));
//         Serial.print(IrReceiver.decodedIRData.rawDataPtr->rawlen - 1);
//         Serial.println(F(" timing entries as raw "));
//         IrReceiver.printIRResultRawFormatted(&Serial, true); // Output the results in RAW format
//         sStoredIRData.rawCodeLength = IrReceiver.decodedIRData.rawDataPtr->rawlen - 1;
//         /*
//          * Store the current raw data in a dedicated array for later usage
//          */
//         IrReceiver.compensateAndStoreIRResultInArray(sStoredIRData.rawCode);
//     } else {
//         IrReceiver.printIRResultShort(&Serial);
//         IrReceiver.printIRSendUsage(&Serial);
//         sStoredIRData.receivedIRData.flags = 0; // clear flags -esp. repeat- for later sending
//         Serial.println();
//     }
// }

// void sendCode(storedIRDataStruct *aIRDataToSend) {
//     if (aIRDataToSend->receivedIRData.protocol == UNKNOWN /* i.e. raw */) {
//         // Assume 38 KHz
//         IrSender.sendRaw(aIRDataToSend->rawCode, aIRDataToSend->rawCodeLength, 38);

//         Serial.print(F("raw "));
//         Serial.print(aIRDataToSend->rawCodeLength);
//         Serial.println(F(" marks or spaces"));
//     } else {

//         IrSender.write(&aIRDataToSend->receivedIRData);
//         printIRResultShort(&Serial, &aIRDataToSend->receivedIRData, false);
//     }
// }


/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updated by chegewara

   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 4fafc201-1fb5-459e-8fcc-c5c9c331914b
   And has a characteristic of: beb5483e-36e1-4688-b7f5-ea07361b26a8

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   A connect handler associated with the server starts a background task that performs notification
   every couple of seconds.
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <BLE2901.h>

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
BLE2901 *descriptor_2901 = NULL;

bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String value = pCharacteristic->getValue();

    if (value.length() > 0) {
      Serial.println("*********");
      Serial.print("New value: ");
      for (int i = 0; i < value.length(); i++) {
        Serial.print(value[i]);
      }

      Serial.println();
      Serial.println("*********");
    }
  }
};

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
  }
};

void setup() {
  Serial.begin(115200);

  // Create the BLE Device
  BLEDevice::init("ESP32");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE
  );

  pCharacteristic->setCallbacks(new MyCallbacks());

  // Creates BLE Descriptor 0x2902: Client Characteristic Configuration Descriptor (CCCD)
  pCharacteristic->addDescriptor(new BLE2902());
  // Adds also the Characteristic User Description - 0x2901 descriptor
  descriptor_2901 = new BLE2901();
  descriptor_2901->setDescription("My own description for this characteristic.");
  descriptor_2901->setAccessPermissions(ESP_GATT_PERM_READ);  // enforce read only - default is Read|Write
  pCharacteristic->addDescriptor(descriptor_2901);

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
  // notify changed value
  if (deviceConnected) {
    pCharacteristic->setValue((uint8_t *)&value, 4);
    pCharacteristic->notify();
    value++;
    delay(500);
  }
  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    delay(500);                   // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising();  // restart advertising
    Serial.println("start advertising");
    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }
}
