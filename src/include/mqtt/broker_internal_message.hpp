#ifndef BROKER_INTERNAL_MESSAGE_HPP_
#define BROKER_INTERNAL_MESSAGE_HPP_

#include "mqtt_packet.hpp"

namespace cadmium::iot {
    struct BrokerInternalMessage
    {
        BrokerInternalMessage() {}
        BrokerInternalMessage(int portNumber, MQTTPacket packet):
            portNumber(portNumber), packet(packet) {}

        int portNumber;
        MQTTPacket packet;
    };

    std::ostream &operator<<(std::ostream &out, const BrokerInternalMessage &s)
    {
        out << MQTTPacket::getMQTTPacketTypeAsString(s.packet);
        return out;
    }
    
    std::ostream &operator<<(std::ostream &out, const vector<BrokerInternalMessage> &s)
    {
        return out;
    }
}

#endif