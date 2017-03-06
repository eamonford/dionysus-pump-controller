#include "ProtocolController.h"
#include <SparkJson.h>
#include "Constants.h"

int PUMP = A3;
ProtocolController* protocolController;

void activatePump() {
    analogWrite(PUMP, 4096);
}

int execute(String json) {
    StaticJsonBuffer<250> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject((char*)json.c_str());
    JsonArray& valves = root["i"];
    for (int i = 0; i < valves.size(); i++) {
        int valveId = valves[i]["id"];
        int duration = valves[i]["d"];
        // openValveWithId(valveId);
        activatePump();
        delay(duration*1000);
        deactivatePump();
        // closeValveWithId(valveId);
    }

    return 1;
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
    Message* msg = new Message(valveId, OPEN_VALVE, NOOP);
    protocolController->sendMessage(msg, MASTER);
    getResponse();
    return 1;
}

int closeValveWithId(int valveId) {
    Message* msg = new Message(valveId, CLOSE_VALVE, NOOP);
    protocolController->sendMessage(msg, MASTER);
    getResponse();
    return 1;
}

void identifyAllSlaves() {
    Particle.publish("Trying to identify slaves");

    Message* msg = new Message(EVERYONE, IDENTIFY, NOOP);
    protocolController->sendMessage(msg, MASTER);
}

void setup() {
    Particle.function("execute", execute);
    Particle.function("openValve", openValve);
    Particle.function("closeValve", closeValve);

    protocolController = new ProtocolController(&Serial1);
    pinMode(PUMP, OUTPUT);
    Serial.begin(9600);
    Serial1.begin(9600);

    identifyAllSlaves();
}

void processMessage(Message* msg) {
    Particle.publish("slaveIdentified", String(msg->arg));

    Serial.write(msg->destination);
    Serial.write(msg->command);
    Serial.write(msg->arg);
}

void getResponse() {
    Particle.publish("Listening for response");
    protocolController->waitForSynAndSendAck();
    Particle.publish("Recieved a SYN");
    int* messageBytes = protocolController->readBytes(&Serial1, MAX_MSG_LEN);
    Message* message = Message::parse(messageBytes);
    free(messageBytes);
    processMessage(message);
    delete message;
}
void loop() {
    getResponse();
}
