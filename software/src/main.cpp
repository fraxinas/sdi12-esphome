/**
   @file RAK13010_SDI_12_Check_All_Addresses.ino
   @author rakwireless.com
   @brief  Check all Addresses for Active Sensors and Print Status.
           It discovers the address of all sensors active and attached to the board.
           THIS CAN BE *REALLY* SLOW TO RUN!!!
           Each sensor should have a unique address already - if not, multiple sensors may
           respond simultaenously to the same request and the output will not be readable
           by the Arduino.
   @version 0.1
   @date 2022-03-11
   @copyright Copyright (c) 2022
**/
#include "RAK13010_SDI12.h" // Click to install library: // Click to install library: http://librarymanager/All#RAK13010

#define RX_PIN 25 // The pin of the SDI-12 data bus.
#define TX_PIN 26 // The pin of the SDI-12 data bus.

#define OE 27    // Output enable pin, active low.

RAK_SDI12 mySDI12(RX_PIN, TX_PIN, OE);

/**
 * @brief Gets identification information from a sensor, and prints it to the serial port expects.
 * @param i A character between '0'-'9', 'a'-'z', or 'A'-'Z'
 */
void printInfo(char i)
{
  String command = "";
  command += (char)i;
  command += "I!";
  mySDI12.sendCommand(command);
  mySDI12.clearBuffer();
  delay(30);

  Serial.print("  --");
  Serial.print(i);
  Serial.print("--  ");

  while (mySDI12.available())
  {
    Serial.write(mySDI12.read());
    delay(10); // 1 character ~ 7.5ms.
  }
  Serial.print("\r\n");
}

/**
 * @brief this checks for activity at a particular address.
 * @param i A character between '0'-'9', 'a'-'z', or 'A'-'Z'
 */
boolean checkActive(char i)
{
  String myCommand = "";
  myCommand += (char)i; // Sends basic 'acknowledge' command [address][!].
  myCommand += "!";

  for (int j = 0; j < 3; j++)
  {
    mySDI12.sendCommand(myCommand, 20);
    mySDI12.clearBuffer();
    delay(20);
    if (mySDI12.available())
    {
      return true;
    }
  }
  mySDI12.clearBuffer();
  return false;
}

void scanAddressSpace(void)
{
  // while (1)
  {
    for (char i = '0'; i <= '9'; i++) // Scan address space 0-9.
    {
      Serial.printf("Scan Address Space = %c\r\n", i);
      if (checkActive(i))
      {
        printInfo(i);
      }
    }

    for (char i = 'a'; i <= 'z'; i++) // Scan address space a-z.
    {
      Serial.printf("Scan Address Space = %c\r\n", i);
      if (checkActive(i))
      {
        printInfo(i);
      }
    }

    for (char i = 'A'; i <= 'Z'; i++) // Scan address space A-Z.
    {
      Serial.printf("Scan Address Space = %c\r\n", i);
      if (checkActive(i))
      {
        printInfo(i);
      }
    }
  }
}

void addressQueryFindDelay()
{
  String command = "?!";

  Serial.print("  address query find delay: ");

  for (int d = 0; d < 150; d += 2)
  {
    mySDI12.sendCommand(command, d);
    mySDI12.clearBuffer();
    // delay(d);
    mySDI12.sendCommand(command);
    mySDI12.clearBuffer();
    delay(d);
    if (mySDI12.available())
    {
      Serial.printf("available after %dms: ", d);
      {
        Serial.write(mySDI12.read());
        delay(10); // 1 character ~ 7.5ms.
      }
      Serial.printf("\r\n");
      return;
    } else {
      Serial.printf("%d ", d);
    }
  }
  Serial.write("unavailable!\r\n");
}

void addressQueryFindDelay2()
{
  String command = "?!";

  for (int d = 5; d<105; d+=5)
  {
    Serial.printf("Address query with %d ms delay: ", d);

    // for (int j = 0; j < 3; j++)
    {
      mySDI12.sendCommand(command, d);
      mySDI12.clearBuffer();
      mySDI12.sendCommand(command);
      mySDI12.clearBuffer();
      delay(d);
      // Serial.printf("#%d ",j);
      while (mySDI12.available())
      {
        Serial.write(mySDI12.read());
        delay(10); // 1 character ~ 7.5ms.
      }
    }
    mySDI12.clearBuffer();
    Serial.write("\r\n");
  }
}

void addressQuery(int d)
{
  String command = "?!";

  mySDI12.sendCommand(command, d);
  mySDI12.clearBuffer();

  mySDI12.sendCommand(command);
  mySDI12.clearBuffer();

  mySDI12.sendCommand(command);
  mySDI12.clearBuffer();

  delay(d);

  Serial.print("  address query: ");

  if (mySDI12.available())
  {
    Serial.printf("available after %dms: ", d);
    {
      Serial.write(mySDI12.read());
      delay(10); // 1 character ~ 7.5ms.
    }
    Serial.printf("\r\n");
    return;
  }
  Serial.printf("No reply to address query with delay=%dms\r\n", d);
}

void requestMeasurement(char i)
{
  String command = "";
  command += (char)i;
  command += "M!";
  mySDI12.sendCommand(command,100);
  mySDI12.clearBuffer();

  delay(30);

  Serial.printf("  requestMeasurement with command '%s': ", command.c_str());

  while (mySDI12.available())
  {
    Serial.write(mySDI12.read());
    delay(10); // 1 character ~ 7.5ms.
  }
}

void sendData(char i)
{
  String command = "";
  command += (char)i;
  command += "D0!";
  mySDI12.sendCommand(command,100);
  mySDI12.clearBuffer();
  delay(30);

  Serial.print("  sendData --");
  Serial.print(i);
  Serial.print("--  ");

  while (mySDI12.available())
  {
    Serial.write(mySDI12.read());
    delay(10); // 1 character ~ 7.5ms.
  }
}

void toggleRecorder()
{
  String command = "";
  command += "R!";
  mySDI12.sendCommand(command);
  mySDI12.clearBuffer();
  delay(30);

  Serial.print("  toggleRecorder --");
  Serial.print("--  ");

  while (1) {
    while (mySDI12.available())
    {
      Serial.write(mySDI12.read());
      delay(10); // 1 character ~ 7.5ms.
    }
    delay(500);
  }
}


void setup()
{
  // pinMode(WB_IO2, OUTPUT);
  // digitalWrite(WB_IO2, HIGH); // Power the sensors.

  // Initialize Serial for debug output.
  time_t timeout = millis();
  Serial.begin(115200);
  while (!Serial)
  {
    if ((millis() - timeout) < 5000)
    {
      delay(100);
    }
    else
    {
      break;
    }
  }
  Serial.println("Start Search for SDI-12 Devices.");

  mySDI12.begin();
  delay(4000);
  // addressQuery(20);
  // delay(4000);
  requestMeasurement('0');
  delay(2000);
  sendData('0');
  delay(2000);
  addressQueryFindDelay();
  delay(2500);
  addressQueryFindDelay2();

  // addressQuery();
  delay(2500);
  scanAddressSpace();
  // delay(2500);
  // printInfo('0');
  // delay(2500);
  // requestMeasurement('0');
  // delay(2500);
  // sendData('0');
  // delay(2500);
  // toggleRecorder();

  mySDI12.end();

  Serial.println("End Search for SDI-12 Devices.");

  // digitalWrite(WB_IO2, LOW); // Cut power.
}

void loop()
{
  delay(100);
}