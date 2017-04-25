// #include "ProtocolController.h"
// #include "Constants.h"
//
// ProtocolController::ProtocolController(Stream* comm) : CGPInterface() {
//     this->comm = comm;
// }
//
// void ProtocolController::sendDatagram(Datagram* datagram) {
//   CGPInterface::sendDatagram(comm, datagram);
// }
//
// Datagram* ProtocolController::getDatagram(int timeout) {
//     return CGPInterface::getDatagram(comm, timeout);
// }
//
// int ProtocolController::readUntil(int* buffer, Stream* stream, int terminal, int timeout) {
//   int i = 0;
//   int startTime = millis();
//   while (true) {
//     if (timeout != 0 && millis() - startTime > timeout)
//       break;
//     if (stream->available() > 0) {
//       int newByte = stream->read();
//       if (newByte != terminal)
//         buffer[i++] = newByte;
//       else
//         break;
//     }
//   }
//   return i;
// }
//
// Datagram* ProtocolController::getDatagram(Stream* stream, int timeout) {
//   while (true) {
//     int* bytes = (int*)malloc(sizeof(int)*128);
//     int numBytes = readUntil(bytes, stream, END_OF_DATAGRAM, timeout);
//     if (numBytes == 4) {
//       Datagram* newDatagram = Datagram::parse(bytes);
//       if (lastDatagramId == newDatagram->id) {
//         free(bytes);
//         delete newDatagram;
//       } else {
//         free(bytes);
//         lastDatagramId = newDatagram->id;
//         return newDatagram;
//       }
//     } else if (numBytes == 0) {
//       free(bytes);
//       return NULL;
//     } else {
//       free(bytes);
//     }
//   }
// }
