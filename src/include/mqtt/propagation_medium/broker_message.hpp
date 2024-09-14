#ifndef BROKER_MESSAGE_HPP
#define BROKER_MESSAGE_HPP

#include "../mqtt_packet.hpp"

namespace cadmium::iot {
    struct BrokerMessage
    {
        BrokerMessage() {}
        BrokerMessage(int portNumber, MQTTPacket packet):
            portNumber(portNumber), packet(packet) {}

        int portNumber;
        MQTTPacket packet;
    };

    std::ostream &operator<<(std::ostream &out, const BrokerMessage &s)
    {
        out << MQTTPacket::getMQTTPacketTypeAsString(s.packet);
        return out;
    }
    
    std::ostream &operator<<(std::ostream &out, const vector<BrokerMessage> &s)
    {
        return out;
    }
}

#endif