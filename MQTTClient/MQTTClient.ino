/*
  Basic ESP8266 MQTT example

  // BASELINE DETAILS - THANKS TO PROVIDERS OF THIS EXAMPLE...
  //
  //  This sketch demonstrates the capabilities of the pubsub library in combination
  //  with the ESP8266 board/library.
  //
  //  It connects to an MQTT server then:
  //  - publishes "hello world" to the topic "outTopic" every two seconds
  //  - subscribes to the topic "inTopic", printing out any messages
  //    it receives. NB - it assumes the received payloads are strings not binary
  //  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
  //    else switch it off
  //
  //  It will reconnect to the server if the connection is lost using a blocking
  //  reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
  //  achieve the same result without blocking the main loop.
  //
  //  To install the ESP8266 board, (using Arduino 1.6.4+):
  //  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
  //       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  //  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  //  - Select your ESP8266 in "Tools -> Board"

  This Sketch Description:

  Developed by Stephen Sweeney from Example Sketch (PubSubClient mqtt_basic + JSON)
  Date: 22/08/2019

  This sketch utilises Serial port for communications to/from the NodeMCU.

  JSON strings are read through the serial port (from master Arduino) to:
  1) Setup WIFI connections (Not done through JSON yet!)
  a) {"topic":"SSID","value":"Your SSID"}
  b) {"topic":"SSIDPWD","value":"Your Wifi Password"}
  c) {"topic":"MQTT","value":"Your MQTT Server Name or address"}
  d) {"topic":"MQTTPORT","value":"1883"}
  2) Subscribe to an MQTT topic
  a) {"topic":"home/all/currentDate","value":"SUBSCRIBE"}
  3) Post a value (currently string only) to MQTT topic
  a) {"topic":"home/all/clockStatus","value":"true"}

  Data sent by MQTT server to NodeMCU (subscribed topics) will be receieved, parsed and forwarded back to master Arduino through Serial Port (print)
  {"topic":"home/office/ledColour","value":"Blue"}

  DEBUG
  =====
  There is a lot of debug messages created in here.  These are all prefixed by DEBUG NodeMCU and are sent over the Serial line as well.
  Switch them all off by commenting out the #define DEBUG true line below
  Recommend you leave them on.

  The Arduino master must:
  1) Setup Serial port connected to NodeMCU (Hardware or Software Serial)
  *** IMPORTANT - DONT FORGET 5V to 3.3V LOGIC LEVEL CONVERTER OTHERWISE YOU WILL BLOW UP THIS NODEMCU ***
  2) Read the Serial port for messages:
  a) Throw away anything from here starting with DEBUG, which it can redirect stright out its own Serial connection to provide all output messages in Serial Monitor.
  b) Parse the valid JSON string to interpret the results

  SETUP
  =====
  Install the NodeMCU board as per instrucions above.

  DOWNLOAD THIS SKETCH ONTO THE NODEMCU BOARD

  My NodeMCU Arduino IDE Tools Settings:

  Board: NodeMCU 1.0 (ESP-12E Module)
  Upload Speed: 115200
  CPU Frequency: 80MHz
  Flash Size: 4M (no SPIFFS)
  Debug port: Disabled
  Debug Level: None
  IwIP Varient: v2 Lower Memory
  VTables: Flash
  Exceptions: Disabled
  Erase Flash: Only Sketch
  SSL Support: All SSL ciphers (most compatible)
  Port: COMxxx

  Programmer: AVRISP mkII

*/
//#define DEBUG                           TRUE
//#define DEBUG_HIFREQ                    TRUE
//#define TRACE                           TRUE
//#define TRACE_HIFREQ                    TRUE
//#define INFO                            TRUE
//#define ERROR                           TRUE
//#define WARNING                         TRUE
#include "Debug.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Arduino_JSON.h>

char debug_stringBuf[DEBUG_BUFFER_SIZE];

// Update these with values suitable for your network.
// TODO: Replace these hardcoded values with JSON from master Arduino...
const char* ssid = "SKYE30B2";
const char* password = "BDTRVRBSXM";
// const char* mqtt_server = "piMQTT";  // THIS STOPPED WORKING????
const char* mqtt_server = "192.168.0.67";
const int   mqtt_server_port = 1883;
const char* mqtt_user = "piMQTT_user";
const char* mqtt_password = "IflmTT800";

#define SERIAL_BAUD 1200
// #define SERIAL_BAUD 57600

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[128];
int value = 0;

// Comment out to remove the debug, however the receiver of the messages will ignore anything starting "DEBUG NodeMCU"...
// -- Recommend turning DEBUG off, as the receiving end can get flooded...
// #define DEBUG             true

#define MAX_SUBSCRIPTIONS 10
#define MAX_JSONSTR_LEN  128
#define MAX_TOPIC_LEN     64
#define MAX_VALUE_LEN     32

// TODO: Replace these hardcoded values with JSON from master Arduino...
String subscriptions[MAX_SUBSCRIPTIONS] = {
  "home/#"                      // Should get them all...
  //   "home/curDateTime",           // Get timesync messages
  //   "home/MQTTServerStatus",      // Get messages when the server is going down
  //   "home/office/#"               // Get all messages to do with Office (Where the sirenOne is currently located e.g. colour/etc)
};

void setup_wifi()
{
  trace("");
  delay(10);

  // We start by connecting to a WiFi network
  info("Connecting to '%s'", ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    trace("Try to connect again...");
  }

  randomSeed(micros());

  info("WiFi connected: IP='%s'", WiFi.localIP().toString().c_str());
}

void callback(char* topic, byte* payload, unsigned int length)
{
  trace("");
  char value[MAX_VALUE_LEN];

  debug("Message arrived from MQTT Server...");
  debug("Topic: ['%s']", topic);

  for (unsigned int i = 0; i < length; i++) {
    value[i] = (char)payload[i];
    value[i + 1] = '\0';
  }

  debug("Value: ['%s']", value);

  char jsonStr[MAX_JSONSTR_LEN];
  createJSON(topic, value, jsonStr);

  // Heres where we send the JSON string to the receiver...
  Serial.println(jsonStr);
}

void createJSON( char *topic, char *value, char *jsonStr)
{
  // Create JSON Object to send to Arduino...
  JSONVar myObject;
  myObject["topic"] = topic;
  myObject["value"] = value;

  String jsonString = JSON.stringify(myObject);

  jsonString.toCharArray(jsonStr, MAX_JSONSTR_LEN);

  debug("JSON.stringify(myObject) = '%s'", jsonStr);
}

void parseJSON(String inputJSON, char *topic, char *value)
{
  trace("");

  char inputStr[MAX_JSONSTR_LEN];
  strcpy(inputStr, inputJSON.c_str());
  parseJSON(inputStr, topic, value);
}

void parseJSON(char *inputJSON, char *topic, char *value)
{
  trace("");

  bool msgOK = true;

  debug("Parse input JSON");

  JSONVar myObject = JSON.parse(inputJSON);

  // JSON.typeof(jsonVar) can be used to get the type of the var
  if (JSON.typeof(myObject) == "undefined")
    msgOK = false;;

  if (msgOK ==  true) {
    debug("JSON.typeof(myObject) = '%s'", JSON.typeof(myObject).c_str());

    // myObject.hasOwnProperty(key) checks if the object contains an entry for key
    if (myObject.hasOwnProperty("topic")) {
      debug("-- myObject[\"topic\"] = '%s'", (const char*) myObject["topic"]);
      strcpy(topic, (const char*) myObject["topic"]);
    }
    else
      msgOK = false;

    if (msgOK == true) {
      if (myObject.hasOwnProperty("value")) {
        debug("-- myObject[\"value\"] = '%s'", (const char*) myObject["value"]);
        strcpy(value, (const char*) myObject["value"]);
      }
      else
        msgOK = false;
    }
  }

  if (msgOK == false) {
    debug("Parsing input failed...");

    // Set the values to UNKNOWN so the message interpreter will ignore them...
    strcpy(topic, "UNKNOWN");
    strcpy(value, "UNKNOWN");
  }

  trace("<< Parse input JSON");
  return;
}

void addTopicToList(String topic)
{
  trace("Subscription message rx'd, is it already in the sublist?");

  bool itemInList = false;
  bool listFull = false;
  int idx = 0;

  // Scan through the list until you reach the end...
  while (subscriptions[idx].length() > 0) {
    debug("subscription[%d] = '%s'", idx, subscriptions[idx].c_str());
    
    if (topic == subscriptions[idx]) {
      debug("topic: '%s' already in list", topic.c_str());
      
      itemInList = true;
      break;
    }

    ++ idx;
    if (idx >= MAX_SUBSCRIPTIONS) {
      debug("List full, cannot add anymore subs to recovery list");
      listFull = true;
      break;
    }
  }

  if (!listFull) {
    if (itemInList) {
      debug("Exists already...");
    }
    else {
      debug("New Subscription, adding to list...");
      subscriptions[idx] = topic;

      //      int strSize = sizeof(char) * strlen(topic) + 1;
      //      char *newStr = (char *)malloc(strSize);
      //      strcpy(newStr, topic);
      //      subscriptions[idx] = newStr;
    }
  }
}

void reconnect()
{
  trace("");

  // Loop until we're reconnected
  while (!client.connected()) {
    debug("Attempting MQTT connection: ");

    // Create a random client ID
    String clientId = "NodeMCU-";
    clientId += String(random(0xffff), HEX);

    debug("Generating new clientId: '%s'", clientId.c_str());

    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      debug("connected...");
      debug("Resubscribing: ");

      // ... and resubscribe
      for (int i = 0; i < MAX_SUBSCRIPTIONS; i++) {
        if (subscriptions[i].length() > 0) {
          debug("-- '%s'", subscriptions[i].c_str());
          client.subscribe(subscriptions[i].c_str());
        }
      }
    }
    else {
      debug("failed, rc=%d, try again in 5 seconds...", client.state());
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(SERIAL_BAUD);
  trace("");

  setup_wifi();

  client.setServer(mqtt_server, mqtt_server_port);
  client.setCallback(callback);
}

long lastMQTTCheck = 0;
#ifdef DEBUG
int lineFeedCounter = 0;
#endif

void loop() {

  int connectStatus;

  connectStatus = client.connected();

  if (!(connectStatus)) {
    debug("MQTT Connection Lost");
    reconnect();
  }

  long now = millis();

  if (now - lastMQTTCheck > 100) {
    client.loop();
    lastMQTTCheck = now;

#ifdef DEBUG
    debug_nolf(".");
    ++lineFeedCounter;
    if (lineFeedCounter > 64) {
      debug_nolf("\n");
      lineFeedCounter = 0;
    }
#endif
  }

  if (now - lastMsg > 2000) {
    // Check the Serial port for any incoming messages...
    if (Serial.available() > 0) {
      String input;

      // Read the incoming stream
      input = Serial.readStringUntil('\n');

      char topic[MAX_TOPIC_LEN],
           value[MAX_VALUE_LEN];
      // Parse the JSON and ensure it is a valid JSON...
      parseJSON(input, topic, value);

      debug("parseJSON topic = '%s'", topic);
      debug("parseJSON value = '%s'", value);

      // Is it a valid message...
      if (strcmp(value, "UNKNOWN")) {
        // Is it a Subscription Message...
        if (!strcmp(value, "SUBSCRIBE")) {
          debug("Valid subscription message received: '%s'", input.c_str());

          // Add the topic to the list of subscriptions (for reconnect) if it doesn't exist already i.e. new subscription message from master
          addTopicToList(topic);
          client.subscribe(topic);
        }
        else {
          debug("Message to send value to MQTT received: '%s'", input.c_str());
          client.publish(topic, value);
        }
      }
#ifdef DEBUG
      else {
        debug("Invalid message received: '%s'", input.c_str());
      }
#endif
    }
    lastMsg = now;
  }
}
