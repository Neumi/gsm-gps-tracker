#include <TinyGPS++.h>
#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>
#include <SoftwareSerial.h>

const char apn[]  = "YourAPN"; // you can leave this as it is
const char user[] = ""; // you can leave this as it is
const char pass[] = ""; // you can leave this as it is


SoftwareSerial SerialGSM(10, 11);

static const int RXPinGPS = 8, TXPinGPS = 9;

TinyGPSPlus gps;
SoftwareSerial SerialGPS(RXPinGPS, TXPinGPS);

TinyGsm modem(SerialGSM);
TinyGsmClient client(modem);

const char server[] = "www.MYDOMAIN.com"; // change this to your domain. 
const char resource[] = "/logger.php";

int payload = 0;

void setup() {
  Serial.begin(9600);
  delay(10);

  SerialGSM.begin(9600);
  SerialGPS.begin(9600);
  
  //SerialGSM.listen();
  delay(3000);

  Serial.println("Modem restart...");
  modem.restart();

  // Unlock your SIM card with a PIN if needed
  //modem.simUnlock("1234");
}

void loop() {
  SerialGPS.listen();
  while (SerialGPS.available() > 0) {
    if (gps.encode(SerialGPS.read())) {
      Serial.println("Found true GPS position.");
      getGPSAndSend();
    }
  }
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while (true);
  }

}

void sendDataToServer(String getRequest) {
  SerialGSM.listen();
  Serial.print("Waiting for network...");
  if (!modem.waitForNetwork()) {
    Serial.println(" fail");
    delay(10000);
    return;
  }
  Serial.println(" OK");

  Serial.print("Connecting to ");
  Serial.print(apn);
  if (!modem.gprsConnect(apn, user, pass)) {
    Serial.println(" fail");
    delay(10000);
    return;
  }
  Serial.println(" OK");

  Serial.print("Connecting to ");
  Serial.print(server);
  if (!client.connect(server, 80)) {
    Serial.println(" fail");
    delay(10000);
    return;
  }
  Serial.println(" OK");

  // Make a HTTP GET request:
  client.print(String("GET ") + resource + getRequest + " HTTP/1.0\r\n");
  client.print(String("Host: ") + server + "\r\n");
  client.print("Connection: close\r\n\r\n");

  unsigned long timeout = millis();
  while (client.connected() && millis() - timeout < 10000L) {
    while (client.available()) {
      char c = client.read();
      Serial.print(c);
      timeout = millis();
    }
  }
  Serial.println();

  client.stop();
  Serial.println("Server disconnected");

  modem.gprsDisconnect();
  Serial.println("GPRS disconnected");

  
  delay(4000);
}

void getPayload() {
  // put anything here ... like temperature, humidity, light, air quality, whatever sensors, 
  payload = analogRead(A0); 
}


void getGPSAndSend() {
  Serial.print(F("Location: "));
  if (gps.location.isValid())
  {
    String locationString = "?lat=" + String(gps.location.lat(), 6) + "&lng=" + String(gps.location.lng(), 6) + "&deg=" + String(gps.course.deg(), 1) + "&speed=" + String(gps.speed.kmph(), 2) + "&payload=" + String(payload);
    locationString += "&type=gps_gsm&name=GPS_GSM_sensor";
    Serial.print("Trying to send data to Server: ");
    Serial.println(locationString);
    sendDataToServer(locationString);
  }
  else
  {
    Serial.print("Invalid GPS position...");
  }
}
