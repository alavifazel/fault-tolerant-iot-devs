#ifndef CLIENT_INTERNAL_MESSAGE_HPP_
#define CLIENT_INTERNAL_MESSAGE_HPP_

#include "../mqtt_packet.hpp"
#include <vector>

namespace cadmium::iot {
    using std::vector;
    
    struct ClientInternalMessage {
        ClientInternalMessage() {}
        ClientInternalMessage(bool newPacket, MQTTPacket packet):
            newPacket(newPacket), packet(packet) {}

        bool newPacket;
        MQTTPacket packet;
    };

    std::ostream &operator<<(std::ostream &out, const ClientInternalMessage &s)
    {
        return out;
    }
    
    std::ostream &operator<<(std::ostream &out, const vector<ClientInternalMessage> &s)
    {
        return out;
    }
}

#endif