#include "ProtocolController.h"
#include <SparkJson.h>
#include "Constants.h"

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

void identifyAllSlaves() {
    Particle.publish("Trying to identify slaves");

    Datagram* msg = new Datagram(EVERYONE, IDENTIFY, NOOP);
    protocolController->sendDatagram(msg, MASTER);
}

void setup() {
    Particle.function("execute", execute);

    protocolController = new ProtocolController(&Serial1);
    pinMode(PUMP, OUTPUT);
    Serial.begin(9600);
    Serial1.begin(9600);

    // identifyAllSlaves();
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
