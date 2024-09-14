#ifndef MQTT_BUFFER_PACKET_HPP
#define MQTT_BUFFER_PACKET_HPP

#include "../mqtt_packet.hpp"

namespace cadmium::iot
{
    struct BufferPacket
    {
        const MQTTPacket packet;
        const int portNumber;
        BufferPacket(MQTTPacket packet, int portNumber)
            : packet(packet), portNumber(portNumber) {}
    };
}

#endif