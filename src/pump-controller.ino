#include "ProtocolController.h"
#include <SparkJson.h>
#include "Constants.h"

int PUMP = A3;
ProtocolController* protocolController;

void activatePump() {
    analogWrite(PUMP, 4095);
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
          closeValveWithId(valveId);
        }
    }

    return totalDuration;
 }

int openValve(String idString) {
    return openValveWithId(idString.toInt());
}

int closeValve(String idString) {
    return closeValveWithId(idString.toInt());
}

int deactivatePump() {
    analogWrite(PUMP, 0);
}

int openValveWithId(int valveId) {
    Datagram* msg = new Datagram(valveId, OPEN_VALVE, NOOP);
    bool result = protocolController->sendDatagram(msg, MASTER);
    // getResponse();
    if (result) return 1; else return 0;
}

int closeValveWithId(int valveId) {
    Datagram* msg = new Datagram(valveId, CLOSE_VALVE, NOOP);
    bool result = protocolController->sendDatagram(msg, MASTER);
    // getResponse();
    if (result) return 1; else return 0;
}

void identifyAllSlaves() {
    Particle.publish("Trying to identify slaves");

    Datagram* msg = new Datagram(EVERYONE, IDENTIFY, NOOP);
    protocolController->sendDatagram(msg, MASTER);
}

void setup() {
    Particle.function("execute", execute);
    Particle.function("openValve", openValve);
    Particle.function("closeValve", closeValve);

    protocolController = new ProtocolController(&Serial1);
    pinMode(PUMP, OUTPUT);
    Serial.begin(9600);
    Serial1.begin(9600);

    // identifyAllSlaves();
}

void processDatagram(Datagram* msg) {
    Serial.write(msg->destination);
    Serial.write(msg->command);
    Serial.write(msg->arg);
}

void getResponse() {
    protocolController->waitForSynAndSendAck();
    int* datagramBytes = protocolController->readBytes(&Serial1, MAX_MSG_LEN);
    Datagram* datagram = Datagram::parse(datagramBytes);
    free(datagramBytes);
    processDatagram(datagram);
    delete datagram;
}
void loop() {
    getResponse();
}
