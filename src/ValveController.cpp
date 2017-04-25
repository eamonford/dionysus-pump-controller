#include "ValveController.h"
#include "Constants.h"


void ValveController::processDatagram(Datagram* datagram) {
  lastDatagramReceived = datagram;
}

int ValveController::getStreamIndexForDatagram(Datagram* datagram) {
  return 0;
}

bool ValveController::shouldForwardDatagram(Datagram* datagram) {
 return false;
}

ValveController::ValveController(Stream* serial) {
  Stream** serialList = (Stream**)malloc(sizeof(Stream*));
  serialList[0] = serial;
  this->lastDatagramReceived = NULL;
  this->cgp = new CGP(1, serialList, this);
}

bool ValveController::openValveWithId(int valveId) {
    Datagram* request = new Datagram(valveId, OPEN_VALVE, NOOP);
    cgp->sendDatagram(request);
    delete request;
    cgp->getAndProcessDatagram();
    bool success = lastDatagramReceived != NULL && lastDatagramReceived->command == OPEN_VALVE;
    delete lastDatagramReceived;
    lastDatagramReceived = NULL;
    return success;
}

bool ValveController::closeValveWithId(int valveId) {
    Datagram* request = new Datagram(valveId, CLOSE_VALVE, NOOP);
    cgp->sendDatagram(request);
    delete request;
    cgp->getAndProcessDatagram();
    bool success = lastDatagramReceived != NULL && lastDatagramReceived->command == CLOSE_VALVE;
    delete lastDatagramReceived;
    lastDatagramReceived = NULL;
    return success;
}

bool ValveController::assignValveId(int id) {
  Datagram* request = new Datagram(FIRST_UNIDENTIFIED, SET_ID, id);
  cgp->sendDatagram(request);
  delete request;
  cgp->getAndProcessDatagram();
  bool success = lastDatagramReceived != NULL && lastDatagramReceived->command == SET_ID;
  delete lastDatagramReceived;
  lastDatagramReceived = NULL;
  return success;
}

vector<int>* ValveController::identifyAllSlaves() {
    Datagram* request = new Datagram(EVERYONE, IDENTIFY, NOOP);
    cgp->sendDatagram(request);
    delete request;

    vector<int>* valveIds = new vector<int>();
    while (true) {
      cgp->getAndProcessDatagram(1000);
      if (lastDatagramReceived == NULL || lastDatagramReceived->command == END_OF_CHAIN) {
        delete lastDatagramReceived;
        lastDatagramReceived = NULL;
        break;
      } else if (lastDatagramReceived->command == IDENTIFY) {
        // if a valve is unidentified, mark it as needing to be assigned an ID
        valveIds->push_back(lastDatagramReceived->arg);
      }
      delete lastDatagramReceived;
      lastDatagramReceived = NULL;
    }

    for (int i = 0; i < valveIds->size(); i++) {
      if ((*valveIds)[i] == 0) {
        int newId = random(1, 254);
        if (assignValveId(newId))
          (*valveIds)[i] = newId;
      }
    }
    return valveIds;
}
