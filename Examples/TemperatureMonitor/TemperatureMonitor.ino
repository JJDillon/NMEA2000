// Demo: NMEA2000 library. Send main cabin temperature to the bus.

#include <Arduino.h>
#include <NMEA2000_CAN.h>  // This will automatically choose right CAN library and create suitable NMEA2000 object
#include <N2kMessages.h>

void setup() {
  // Set Product information
  NMEA2000.SetProductInformation("00000001", // Manufacturer's Model serial code
                                 100, // Manufacturer's product code
                                 "Simple temp monitor",  // Manufacturer's Model ID
                                 "1.0.0.11 (2015-08-03)",  // Manufacturer's Software version code
                                 "1.0.0.0 (2015-08-03)" // Manufacturer's Model version
                                 );
  // Det device information
  NMEA2000.SetDeviceInformation(1, // Unique number. Use e.g. Serial number.
                                130, // Device function=Temperature. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                75, // Device class=Sensor Communication Interface. See codes on  http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                2046 // Just choosen free from code list on http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf                               
                               );
  // Uncomment 3 rows below to see, what device will send to bus                           
   Serial.begin(115200);
  // NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text. Leave uncommented for default Actisense format.
   // NMEA2000.SetForwardOwnMessages();

  // If you also want to see all traffic on the bus use N2km_ListenAndNode instead of N2km_NodeOnly below
  NMEA2000.SetMode(tNMEA2000::N2km_NodeOnly,22);
  //NMEA2000.SetDebugMode(tNMEA2000::dm_Actisense); // Uncomment this, so you can test code without CAN bus chips on Arduino Mega
  NMEA2000.EnableForward(false); // Disable all msg forwarding to USB (=Serial)
  NMEA2000.Open();
}


void loop() {
  SendN2kTemperature();
  NMEA2000.ParseMessages();
}

double ReadCabinTemp() {
  return CToKelvin(21.0); // Read here the true temperature e.g. from analog input
}

double ReadWaterTemp() {
  return CToKelvin(15.5); // Read here the true temperature e.g. from analog input
}

#define TempUpdatePeriod 2500

void SendN2kTemperature() {
  static unsigned long TempUpdated=millis();
  tN2kMsg N2kMsg;

  if ( TempUpdated+TempUpdatePeriod<millis() ) {
    TempUpdated=millis();
    SetN2kTemperature(N2kMsg, 1, 1, N2kts_MainCabinTemperature, ReadCabinTemp());
    NMEA2000.SendMsg(N2kMsg);
    SetN2kEnvironmentalParameters(N2kMsg, 1, N2kts_MainCabinTemperature, ReadCabinTemp());
    NMEA2000.SendMsg(N2kMsg);
    SetN2kOutsideEnvironmentalParameters(N2kMsg, 1, ReadWaterTemp());
    NMEA2000.SendMsg(N2kMsg);
    // Serial.print(millis()); Serial.println(", Temperature send ready");
  }
}

