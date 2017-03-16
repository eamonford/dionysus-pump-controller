#include "ProtocolController.h"
#include <SparkJson.h>
#include "Constants.h"
#include <vector>

using namespace std;

int PUMP = A3;
ProtocolController* protocolController;

void activatePump() {
    analogWrite(PUMP, 4095);
}

void deactivatePump() {
    analogWrite(PUMP, 0);
}

bool openValveWithId(int valveId) {
    Datagram* request = new Datagram(valveId, OPEN_VALVE, NOOP);
    if (!protocolController->sendDatagram(request, MASTER))
      return false;
    Datagram* response = getNextDatagram();
    // TODO: do some validation on the datagram
    delete response;
    return true;
}

bool closeValveWithId(int valveId) {
    Datagram* request = new Datagram(valveId, CLOSE_VALVE, NOOP);
    if (!protocolController->sendDatagram(request, MASTER))
      return false;
    Datagram* response = getNextDatagram();
    // TODO: do some validation on the datagram
    delete response;
    return true;
}

int execute(String json) {
    StaticJsonBuffer<250> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject((char*)json.c_str());
    JsonArray& valves = root["i"];
    int totalDuration = 0;
    for (int i = 0; i < valves.size(); i++) {
        int valveId = valves[i]["id"];
        int duration = valves[i]["d"];
        if (openValveWithId(valveId)) {
          totalDuration += duration;
          activatePump();
          delay(duration*1000);
          deactivatePump();
          if (!closeValveWithId(valveId))
            break;
        }
    }
    return totalDuration;
 }

bool assignValveId(int id) {
  Datagram* datagram = new Datagram(FIRST_UNIDENTIFIED, SET_ID, id);
  protocolController->sendDatagram(datagram, MASTER);
  Datagram* response = getNextDatagram();
  return response->command == SET_ID && response->arg == id;
}

String generateJsonForIds(vector<int>* ids) {
  String idsString = "[";
  for (int i = 0; i < ids->size(); i++) {
    idsString += (String((*ids)[i]) + (i < ids->size()-1 ? "," : ""));
  }
  idsString += "]";
  return idsString;
}

void identifyAllSlaves() {
    Datagram* msg = new Datagram(EVERYONE, IDENTIFY, NOOP);
    protocolController->sendDatagram(msg, MASTER);
    delete msg;

    vector<int>* valveIds = new vector<int>();
    Datagram* response;
    while ((response = getNextDatagram())->command != END_OF_CHAIN) {
      // if a valve is unidentified, mark it as needing to be assigned an ID
      if (response->command == IDENTIFY)
        valveIds->push_back(response->arg);
      delete response;
    }
    delete response;

    for (int i = 0; i < valveIds->size(); i++) {
      if ((*valveIds)[i] == 0) {
        int newId = rand() % 255;
        if (assignValveId(newId))
          (*valveIds)[i] = newId;
      }
    }
    Particle.publish("valves", generateJsonForIds(valveIds));
    delete valveIds;
}

void setup() {
    Particle.function("execute", execute);

    protocolController = new ProtocolController(&Serial1);
    pinMode(PUMP, OUTPUT);
    Serial.begin(9600);
    Serial1.begin(9600);

    identifyAllSlaves();
}

Datagram* getNextDatagram() {
    protocolController->waitForSynAndSendAck();
    int* datagramBytes = protocolController->readBytes(&Serial1, MAX_MSG_LEN);
    Datagram* datagram = Datagram::parse(datagramBytes);
    free(datagramBytes);
    return datagram;
}

void loop() {
    Datagram * datagram = getNextDatagram();
    delete datagram;
}
