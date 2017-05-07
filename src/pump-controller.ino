#include "Particle.h"
#include <SparkJson.h>
#include "Constants.h"
#include <vector>
#include "ValveController.h"
#include "MQTT.h"

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

int identify(String arg) {
  vector<int>* valveIds = valveController->identifyAllSlaves();
  Particle.publish("valves", generateJsonForIds(valveIds));

  return 1;
}

int execute(String json) {
    StaticJsonBuffer<250> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject((char*)json.c_str());
    JsonArray& valves = root["i"];
    int totalDuration = 0;
    for (int i = 0; i < valves.size(); i++) {
        int valveId = valves[i]["id"];
        int duration = valves[i]["d"];
        if (valveController->openValveWithId(valveId)) {
          totalDuration += duration;
          activatePump();
          delay(duration*1000);
          deactivatePump();
          if (!valveController->closeValveWithId(valveId))
            break;

          mqttClient.publish("dionysus/irrigation", "{device_id\": \"dozen_laser\", \"valve_id\":" + String(valveId) + ", \"value\":" + String(duration) + "}");
        }
    }
    return totalDuration;
 }

void mqttCallback(char* topic, byte* payload, unsigned int length) {
   char json[length + 1];
   memcpy(json, payload, length);
   json[length] = NULL;
   execute(json);
}


void setup() {
    Particle.function("identify", identify);
    Particle.function("execute", execute);
    pinMode(PUMP, OUTPUT);
    Serial.begin(9600);
    Serial1.begin(9600);

    valveController = new ValveController(&Serial1);

    vector<int>* valveIds = valveController->identifyAllSlaves();
    Particle.publish("valves", generateJsonForIds(valveIds));

    // connect to the server
    mqttClient.connect("sparkclient");

    // publish/subscribe
    if (mqttClient.isConnected()) {
        mqttClient.subscribe("dionysus/dozen_laser");
    }
}

void loop() {
  if (mqttClient.isConnected())
      mqttClient.loop();
}
