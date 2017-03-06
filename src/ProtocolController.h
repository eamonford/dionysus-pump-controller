#ifndef PROTOCOL_CONTROLLER
#define PROTOCOL_CONTROLLER

class ProtocolController : public CGPInterface {
    private:
        Stream* comm;
    public:
        ProtocolController(Stream* comm);

        bool sendMessage(Message* msg, int identity);
        bool sendSynAndWaitForAck(Stream*);
        Stream* waitForSynAndSendAck();
};

#endif
