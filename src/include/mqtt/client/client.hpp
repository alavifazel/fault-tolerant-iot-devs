#ifndef MQTT_CLIENT_HPP_
#define MQTT_CLIENT_HPP_


#ifndef SIMULATION
#include "cadmium/modeling/devs/atomic.hpp"
#else
#include <cadmium/core/modeling/atomic.hpp>
#endif

#include "../mqtt_message.hpp"
#include "../mqtt_packet.hpp"
#include "../mqtt_client_command.hpp"
#include "base.hpp"
// #include "ack_buffer.hpp"

namespace cadmium::iot {
	struct MQTTClient: public Coupled {
        Port<MQTTMessage> inData;
        Port<MQTTMessage> outData;
        Port<MQTTClientCommand> inCommand;
        Port<MQTTPacket> outBroker;
        Port<MQTTPacket> inBroker;

		MQTTClient(const std::string& id): Coupled(id)
        {
            inData = addInPort<MQTTMessage>("in");
            outData = addOutPort<MQTTMessage>("out");
            outBroker = addOutPort<MQTTPacket>("out_broker");
            inBroker = addInPort<MQTTPacket>("in_broker");
            inCommand = addInPort<MQTTClientCommand>("in_command");

            auto base = addComponent<mqtt_client::Base>("mqtt_client_base" + id);
			// auto ackBuffer = addComponent<mqtt_client::AckBuffer>("ack_buffer" + id);
            
            // Couplings
			addCoupling(inData, base->inData);
            addCoupling(base->outData, outData);
            addCoupling(inCommand, base->inCommand);

            // addCoupling(base->outAckBuffer, ackBuffer->inPacket);
            // addCoupling(ackBuffer->outPacket, base->inAckBuffer);

            addCoupling(base->outBroker, outBroker);
            addCoupling(inBroker, base->inBroker);

		}
	};
}


#endif