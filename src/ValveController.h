#ifndef VALVE_CONTROLLER_H_
#define VALVE_CONTROLLER_H_

#include "application.h"
#include "Particle.h"
#include "chain-gang-protocol.h"

using namespace std;

class ValveController : public CGPDelegateInterface {
private:
  CGP* cgp;
  Datagram* lastDatagramReceived;
  void processDatagram(Datagram* datagram);
  int getStreamIndexForDatagram(Datagram* datagram);
  bool shouldForwardDatagram(Datagram* datagram);

public:
  bool assignValveId(int id);
  bool openValveWithId(int valveId);
  bool closeValveWithId(int valveId);
  vector<int>* identifyAllSlaves();
  ValveController(Stream *s);
};

#endif
