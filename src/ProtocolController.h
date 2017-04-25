// #include "Particle.h"
//
// #ifndef PROTOCOL_CONTROLLER
// #define PROTOCOL_CONTROLLER
//
// #include "chain-gang-protocol.h"
//
// class ProtocolController : public CGPInterface {
//     private:
//         Stream* comm;
//     public:
//         ProtocolController(Stream* comm);
//
//         void sendDatagram(Datagram* datagram);
//         Datagram* getDatagram(int timeout);
//         Datagram* getDatagram(Stream* stream, int timeout);
//         int readUntil(int* buffer, Stream* stream, int terminal, int timeout);
// };
//
// #endif
