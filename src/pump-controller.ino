// #include "ProtocolController.h"
#include "Particle.h"
#include <SparkJson.h>
#include "Constants.h"
#include <vector>
#include "ValveController.h"

using namespace std;

int PUMP = A3;
// ProtocolController* protocolController;
ValveController* valveController;
CGP* cgp;

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
        }
    }
    return totalDuration;
 }

void setup() {
    Particle.function("identify", identify);
    Particle.function("execute", execute);
    pinMode(PUMP, OUTPUT);
    Serial.begin(9600);
    Serial1.begin(9600);

    // protocolController = new ProtocolController(&Serial1);
    valveController = new ValveController(&Serial1);

    vector<int>* valveIds = valveController->identifyAllSlaves();
    Particle.publish("valves", generateJsonForIds(valveIds));
}

void loop() {}
