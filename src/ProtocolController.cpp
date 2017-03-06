#include "ProtocolController.h"
#include "Constants.h"

ProtocolController::ProtocolController(Stream* comm) {
    this->comm = comm;
}

bool ProtocolController::sendMessage(Message* msg, int identity) {
  if (sendSynAndWaitForAck(comm)) {
    comm->write(msg->destination);
    comm->write(msg->command);
    comm->write(msg->arg);
    return true;
  }
  return false;
}

bool ProtocolController::sendSynAndWaitForAck(Stream* stream) {
  int tryCount = 0;
  do {
    stream->write(SYN);
    Serial.println("Sending SYN");
    delay(100);
    if (tryCount++ == 30) {
      return false;
    }
  } while (comm->available() == 0 || comm->read() != ACK);
  Serial.println("Recieved ACK");
  return true;
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
        delay(300);
    }
}
