#ifndef PROPAGATION_MEDIUM_HPP
#define PROPAGATION_MEDIUM_HPP

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
#include "../broker_internal_message.hpp"
#include "../mqtt_packet.hpp"
#include "receiving_buffer.hpp"
#include "sending_buffer.hpp"

namespace cadmium::iot {
    using std::vector;

	struct PropagationMedium: public Coupled {
        int numOfIOPorts;
        double processingTimeExpMean;
        
        vector<Port<MQTTPacket>> receivingBufferIn;
        Port<BrokerInternalMessage> receivingBufferOut;
        
        Port<vector<BrokerInternalMessage>> sendingBufferIn;
        vector<Port<MQTTPacket>> sendingBufferOut;

        PropagationMedium(const std::string &id, double processingTimeExpMean, int numOfIOPorts):
                            Coupled(id),
                            numOfIOPorts(numOfIOPorts),   
                            processingTimeExpMean(processingTimeExpMean),
                            receivingBufferIn(numOfIOPorts),
                            sendingBufferOut(numOfIOPorts)

        {
            receivingBufferOut = addOutPort<BrokerInternalMessage>("receiving_buffer_out");
            sendingBufferIn = addInPort<vector<BrokerInternalMessage>>("in");

            for(int i = 0; i < numOfIOPorts; ++i) {
                receivingBufferIn[i]  = addInPort<MQTTPacket>("receiving_buffer_in" + std::to_string(i));
                sendingBufferOut[i] = addOutPort<MQTTPacket>("sending_buffer_out" + std::to_string(i));    
            }

			auto receivingBuffer = addComponent<ReceivingBuffer>(
                                    "receiving_buffer",
                                    processingTimeExpMean,
                                    numOfIOPorts);

			auto sendingBuffer = addComponent<SendingBuffer>(
                                    "sending_buffer",
                                    processingTimeExpMean,
                                    numOfIOPorts);

            addCoupling(receivingBuffer->out, receivingBufferOut);
            addCoupling(sendingBufferIn, sendingBuffer->in);

            // Couplings
            for(int i = 0; i < numOfIOPorts; ++i) {
			    addCoupling(receivingBufferIn[i], receivingBuffer->in[i]);
                addCoupling(sendingBuffer->out[i], sendingBufferOut[i]);
            }
		}
	};
}

#endif