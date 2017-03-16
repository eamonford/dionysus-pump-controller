#include "Particle.h"

#ifndef PROTOCOL_CONTROLLER
#define PROTOCOL_CONTROLLER

#include "chain-gang-protocol.h"

class ProtocolController : public CGPInterface {
    private:
        Stream* comm;
    public:
        ProtocolController(Stream* comm);

        bool sendDatagram(Datagram* msg, int identity);
        Datagram* getDatagram();
};

#endif
