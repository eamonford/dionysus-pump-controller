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

Stream* ProtocolController::waitForSynAndSendAck() {
     while(true) {
        if (comm->available() > 0) {
            Serial.write(comm->peek());
        }

        if (comm->available() > 0 && comm->read() == SYN) {
            comm->write(ACK);
             // Eat up any extra SYNs that got sent during the delay
            while (comm->available() > 0 && comm->peek() == SYN) {
                comm->read();
            }
            return comm;
        }
        delay(30);
    }
}
