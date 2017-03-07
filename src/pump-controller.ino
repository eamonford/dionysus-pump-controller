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
        totalDuration += duration;
        // openValveWithId(valveId);
        activatePump();
        delay(duration*1000);
        deactivatePump();
        // closeValveWithId(valveId);
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
    protocolController->sendDatagram(msg, MASTER);
    getResponse();
    return 1;
}

int closeValveWithId(int valveId) {
    Datagram* msg = new Datagram(valveId, CLOSE_VALVE, NOOP);
    protocolController->sendDatagram(msg, MASTER);
    getResponse();
    return 1;
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
    Particle.publish("slaveIdentified", String(msg->arg));

    Serial.write(msg->destination);
    Serial.write(msg->command);
    Serial.write(msg->arg);
}

void getResponse() {
    Particle.publish("Listening for response");
    protocolController->waitForSynAndSendAck();
    Particle.publish("Recieved a SYN");
    int* datagramBytes = protocolController->readBytes(&Serial1, MAX_MSG_LEN);
    Datagram* datagram = Datagram::parse(datagramBytes);
    free(datagramBytes);
    processDatagram(datagram);
    delete datagram;
}
void loop() {
    getResponse();
}
