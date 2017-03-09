#include "Particle.h"

#pragma once

#ifndef PROTOCOL_CONTROLLER
#define PROTOCOL_CONTROLLER

#include "chain-gang-protocol.h"

class ProtocolController : public CGPInterface {
    private:
        Stream* comm;
    public:
        ProtocolController(Stream* comm);
        bool sendDatagram(Datagram* msg, int identity);
        Stream* waitForSynAndSendAck();
};

#endif
