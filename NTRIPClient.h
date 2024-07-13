#ifndef NTRIP_CLIENT
#define NTRIP_CLIENT

#include <WiFiClient.h>
#include <Arduino.h>
#include<base64.h>

class NTRIPClient : public WiFiClient{
  public :
  bool reqSrcTbl(char* host,int &port);   //request MountPoints List serviced the NTRIP Caster 
  bool reqRaw(char* host,int &port,char* mntpnt,char* user,char* psw);      //request RAW data from Caster 
  bool reqRaw(char* host,int &port,char* mntpnt); //non user
  int readLine(char* buffer,int size);
  void sendGGA(const char* ggaMessage, const char* host, int port, const char* user, const char* passwd, const char* mntpnt);
  void enqueueGGA(String message);
  String dequeueGGA();
  bool isQueueEmpty();

private:
  static const int queueSize = 10;
  String ggaQueue[queueSize];
  int queueFront = 0;
  int queueRear = 0;
  int queueCount = 0;
};
#endif
