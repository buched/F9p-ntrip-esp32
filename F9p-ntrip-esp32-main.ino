#include <WiFi.h>
#include "NTRIPClient.h"

#include <WiFiMulti.h>
#include <WiFiUdp.h>
#include <HardwareSerial.h>

HardwareSerial MySerial(2);
HardwareSerial Serialrx(1);

char* host = "";//enter caster server adress
int httpPort = ;//enter caster server port
char* mntpnt = "";//enter mountpoint
char* user = "";//enter caster username
char* passwd = "";//enter caster password
NTRIPClient ntrip_c;

const char* udpAddress = "192.168.1.255";
const int udpPort = 9999;

int trans = 0;  //0 = serial, 1 = udp, 2 = bt, 3 = serialrx, 4 = myserial Choose wich out you want use. for rs232 set 0 et connect tx f9p directly to rs232 module

WiFiUDP udp;
WiFiMulti wifiMulti;
// uncomment this to use bluetooth but you need set the good partition scheme like picture
//#include "BluetoothSerial.h"
//
//#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
//#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
//#endif
//
//#if !defined(CONFIG_BT_SPP_ENABLED)
//#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
//#endif
//
//BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  delay(500);
  MySerial.begin(115200, SERIAL_8N1, 16, 17);//serial port to send rtcm to f9p
  delay(100);
  Serialrx.begin(115200, SERIAL_8N1, 23, 22);
  delay(100);
//  if (trans == 2) {
//    SerialBT.begin("f9hpbt");  //Bluetooth device name
//    Serial.println("Data out via bluetooth");
//  }

  wifiMulti.addAP("ssid", "password");
  wifiMulti.addAP("CATS41", "");

  Serial.println("Connection au Wifi...");

  if (wifiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connecté");
    Serial.println("addresse IP : ");
    Serial.println(WiFi.localIP());
  }

  Serial.println("Requete SourceTable.");

  if (ntrip_c.reqSrcTbl(host, httpPort)) {
    char buffer[512];
    delay(5);
    while (ntrip_c.available()) {
      ntrip_c.readLine(buffer, sizeof(buffer));
      Serial.print(buffer);
    }
  } else {
    Serial.println("Erreur lecture SourceTable");
  }
  Serial.print("Requete sourcetable OK\n");
  ntrip_c.stop();  //Need to call "stop" function for next request.

  Serial.println("Requete données mountpoint");
  if (!ntrip_c.reqRaw(host, httpPort, mntpnt, user, passwd)) {
    delay(15000);
    ESP.restart();
  }
  Serial.println("MountPoint OK");
}


void loop() {

  while (ntrip_c.available()) {
    char ch = ntrip_c.read();
    MySerial.print(ch);
  }
  while (Serialrx.available()) {
    String s = Serialrx.readStringUntil('\n');
    switch (trans) {
      case 0:  //serial out
        Serial.println(s);
        break;
      case 1:  //udp out
        udp.beginPacket(udpAddress, udpPort);
        udp.print(s);
        udp.endPacket();
        break;
//      case 2:  //bluetooth out
//        SerialBT.println(s);
//        break;
      case 3:  //serialrx out
        Serialrx.println(s);
        break;
      case 4:  //MySerial out
        MySerial.println(s);
        break;
      default:  //mauvaise config
        Serial.println("mauvais choix ou oubli de configuration");
        break;
    }
  }
}
