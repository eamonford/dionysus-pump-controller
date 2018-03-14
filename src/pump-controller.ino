#include "Particle.h"
#include <SparkJson.h>
#include "Constants.h"
#include <vector>
#include "ValveController.h"
#include "MQTT.h"

#define DEBUG false
// If DEGUG is enabled, no commands will be sent to valves
// but the pump will otherwise act as normal

int PUMP = A3;
ValveController* valveController;
CGP* cgp;

byte mqttHost[] = { 192,168,86,100 };
MQTT mqttClient(mqttHost, 1883, mqttCallback);

void activatePump() {
    analogWrite(PUMP, 4095);
}

void deactivatePump() {
    analogWrite(PUMP, 0);
}

String generateJsonForIds(vector<int>* ids) {
  String idsString = "[";
  for (int i = 0; i < ids->size(); i++) {
    idsString += (String((*ids)[i]) + (i < ids->size()-1 ? "," : ""));
  }
  idsString += "]";
  return idsString;
}

void identify() {
  vector<int>* valveIds = valveController->identifyAllSlaves();
  mqttClient.publish("dionysus/valves", generateJsonForIds(valveIds));
}

int execute(String json) {
    StaticJsonBuffer<250> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject((char*)json.c_str());
    JsonArray& valves = root["i"];
    int totalDuration = 0;
    for (int i = 0; i < valves.size(); i++) {
        int valveId = valves[i]["id"];
        int duration = valves[i]["d"];
        if (DEBUG || valveController->openValveWithId(valveId)) {
          totalDuration += duration;
          activatePump();
          delay(duration*1000);
          deactivatePump();
          if (!DEBUG && !valveController->closeValveWithId(valveId))
            break;

          mqttClient.publish("dionysus/events", "{\"device_id\": \"dozen_laser\", \"valveId\":" + String(valveId) + ", \"value\":" + String(duration) + "}");
        }
    }
    return totalDuration;
 }

void mqttCallback(char* topic, byte* payload, unsigned int length) {
   char json[length + 1];
   memcpy(json, payload, length);
   json[length] = NULL;
   mqttClient.publish("dionysus/debug", "Received command");

   execute(json);
}


void setup() {
    Particle.function("execute", execute);
    pinMode(PUMP, OUTPUT);
    Serial.begin(9600);
    Serial1.begin(9600);

    valveController = new ValveController(&Serial1);
    // connect to the server
    mqttClient.connect("sparkclient");

    // publish/subscribe
    if (mqttClient.isConnected()) {
        mqttClient.subscribe("dionysus/dozen_laser");
    }

    if (DEBUG == true) {
      mqttClient.publish("dionysus/debug", "Debug mode is ON");
    } else {
      vector<int>* valveIds = valveController->identifyAllSlaves();
      mqttClient.publish("dionysus/debug", generateJsonForIds(valveIds));
    }
}

void loop() {
  if (mqttClient.isConnected()) {
    mqttClient.loop();
  } else {
      mqttClient.connect("sparkclient");
      if (mqttClient.isConnected()) {
          mqttClient.subscribe("dionysus/dozen_laser");
      }
  }
  delay(1000);
}
