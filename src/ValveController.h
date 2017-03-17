#ifndef VALVE_CONTROLLER_H_
#define VALVE_CONTROLLER_H

#include "ProtocolController.h"

using namespace std;

class ValveController {
private:
  ProtocolController* protocolController;

public:
  ValveController(ProtocolController* newProtocolController);

  bool assignValveId(int id);
  bool openValveWithId(int valveId);
  bool closeValveWithId(int valveId);

  vector<int>* identifyAllSlaves();
};

#endif
