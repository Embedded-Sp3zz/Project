/*
publish_digital_input.cpp

publish switch data on GPIO46 to an indicator feed on AdafruitIO
*/


#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include "MQTTClient.h"
using namespace std;

//Please replace the username, feed and token with your values
#define ADDRESS    "tcp://io.adafruit.com"
#define CLIENTID   "Beagle1"
#define TOPIC      "EmbeddedSp3zz/feeds/digitalinput"
#define AUTHMETHOD "EmbeddedSp3zz"
#define AUTHTOKEN  "aio_LOwT73HBfu9ZTICznJHfwSm4gM94"
#define QOS        1
#define TIMEOUT    10000L
#define GPIO_PATH "/sys/class/gpio/gpio46"

string readGPIO(string path)
{
  stringstream ss;
  ss << path << "/value";
  fstream fs;
  fs.open(ss.str().c_str(), fstream::in);
  string input;
  getline(fs, input);
  fs.close();
  cout << "input is " << input << endl;
  return input;
}

int main(int argc, char* argv[]) {
   char str_payload[100];          // Set your max message size here
   MQTTClient client;
   cout << "Starting Beagle board MQTT Adafruit Publish Example" << endl;
   MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
   MQTTClient_message pubmsg = MQTTClient_message_initializer;
   MQTTClient_deliveryToken token;
   MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
   opts.keepAliveInterval = 20;
   opts.cleansession = 1;
   opts.username = AUTHMETHOD;
   opts.password = AUTHTOKEN;
   int rc;
   if ((rc = MQTTClient_connect(client, &opts)) != MQTTCLIENT_SUCCESS) {
      cout << "Failed to connect, return code " << rc << endl;
      return -1;
   }
   sprintf(str_payload, "%f", stof(readGPIO(GPIO_PATH)));
   pubmsg.payload = str_payload;
   pubmsg.payloadlen = strlen(str_payload);
   pubmsg.qos = QOS;
   pubmsg.retained = 0;
   MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
   cout << "Waiting for up to " << (int)(TIMEOUT/1000) <<
        " seconds for publication of " << str_payload <<
        " \non topic " << TOPIC << " for ClientID: " << CLIENTID << endl;
   rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
   if(rc == MQTTCLIENT_SUCCESS) {
     cout << "Message with token " << (int)token << " delivered." << endl;
   }
   else {
     cout << "Did not complete with error code: " << rc << endl;
     // MQTTCLIENT_SUCCESS 0           MQTTCLIENT_FAILURE -1
     // MQTTCLIENT_DISCONNECTED -3     MQTTCLIENT_MAX_MESSAGES_INFLIGHT -4
     // MQTTCLIENT_BAD_UTF8_STRING -5  MQTTCLIENT_NULL_PARAMETER -6
     // MQTTCLIENT_TOPICNAME_TRUNCATED -7   MQTTCLIENT_BAD_STRUCTURE -8
     // MQTTCLIENT_BAD_QOS   -9        MQTTCLIENT_SSL_NOT_SUPPORTED   -10
   }
   MQTTClient_disconnect(client, 10000);
   MQTTClient_destroy(&client);
   return rc;
}

