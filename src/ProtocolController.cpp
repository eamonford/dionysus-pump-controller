#include "ProtocolController.h"
#include "Constants.h"

ProtocolController::ProtocolController(Stream* comm) {
    this->comm = comm;
}

bool ProtocolController::sendDatagram(Datagram* msg, int identity) {
  if (sendSynAndWaitForAck(comm)) {
    comm->write(msg->destination);
    comm->write(msg->command);
    comm->write(msg->arg);
    return true;
  }
  return false;
}

Datagram* ProtocolController::getDatagram() {
    waitForSynAndSendAck(comm);
    int* datagramBytes = readBytes(comm, MAX_MSG_LEN);
    Datagram* datagram = Datagram::parse(datagramBytes);
    free(datagramBytes);
    return datagram;
}
