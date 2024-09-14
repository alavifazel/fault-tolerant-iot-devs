#ifndef MQTT_BROKER_HPP_
#define MQTT_BROKER_HPP_

#ifndef SIMULATION
#include "cadmium/modeling/devs/atomic.hpp"
#else
#include <cadmium/core/modeling/atomic.hpp>
#endif

#include <cadmium/core/modeling/atomic.hpp>
#include <iostream>
#include <vector>
#include <queue>
#include <random>
#include <set>
#include <map>
#include "../mqtt_packet.hpp"
// #include "ack_buffer.hpp"
#include "base.hpp"

namespace cadmium::iot {
    using std::vector;

	struct MQTTBroker: public Coupled {
        double processingTimeExpMean;
        Port<BrokerInternalMessage> inBroker;
        Port<vector<BrokerInternalMessage>> outBroker;
        
        MQTTBroker(const std::string &id, double processingTimeExpMean, int numOfIOPorts):
                            Coupled(id),
                            processingTimeExpMean(processingTimeExpMean)

        {
            inBroker = addInPort<BrokerInternalMessage>("in");
            outBroker = addOutPort<vector<BrokerInternalMessage>>("out");
            auto base = addComponent<mqtt_broker::Base>("mqtt_broker_base");
			
            // auto ackBuffer = addComponent<mqtt_broker::AckBuffer>("mqtt_broker_ackbuffer", numOfIOPorts);
            // addCoupling(base->outAckBuffer, ackBuffer->inPacket);
            // addCoupling(ackBuffer->outPacket, base->inAckBuffer);
            
            addCoupling(inBroker, base->in);
            addCoupling(base->out, outBroker);
		}
	};
}

#endif