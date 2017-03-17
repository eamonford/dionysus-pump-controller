#include "ValveController.h"
#include "Constants.h"


ValveController::ValveController(ProtocolController* newProtocolController) {
    protocolController = newProtocolController;
}

bool ValveController::openValveWithId(int valveId) {
    Datagram* request = new Datagram(valveId, OPEN_VALVE, NOOP);
    if (!protocolController->sendDatagram(request, MASTER))
      return false;
    Datagram* response = protocolController->getDatagram();
    bool success = response->command == OPEN_VALVE;
    delete response;
    return success;
}

bool ValveController::closeValveWithId(int valveId) {
    Datagram* request = new Datagram(valveId, CLOSE_VALVE, NOOP);
    if (!protocolController->sendDatagram(request, MASTER))
      return false;
    Datagram* response = protocolController->getDatagram();
    bool success = response->command == CLOSE_VALVE;
    delete response;
    return success;
}

bool ValveController::assignValveId(int id) {
  Datagram* datagram = new Datagram(FIRST_UNIDENTIFIED, SET_ID, id);
  protocolController->sendDatagram(datagram, MASTER);
  Datagram* response = protocolController->getDatagram();
  Particle.publish("valves", "Response was " + String(response->command) + " " + String(response->arg));

  return response->command == SET_ID;
}

vector<int>* ValveController::identifyAllSlaves() {
    Datagram* msg = new Datagram(EVERYONE, IDENTIFY, NOOP);
    protocolController->sendDatagram(msg, MASTER);
    delete msg;

    vector<int>* valveIds = new vector<int>();
    Datagram* response;
    while ((response = protocolController->getDatagram())->command != END_OF_CHAIN) {
      // if a valve is unidentified, mark it as needing to be assigned an ID
      if (response->command == IDENTIFY)
        valveIds->push_back(response->arg);
      delete response;
    }
    delete response;

    for (int i = 0; i < valveIds->size(); i++) {
      if ((*valveIds)[i] == 0) {
        int newId = (rand() % 254) + 1;
        if (assignValveId(newId))
          (*valveIds)[i] = newId;
      }
    }
    return valveIds;
}
