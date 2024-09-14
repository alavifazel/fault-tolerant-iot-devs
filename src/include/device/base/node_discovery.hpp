#ifndef Node_DISCOVERY_HPP
#define Node_DISCOVERY_HPP

#ifndef SIMULATION
#include "cadmium/modeling/devs/atomic.hpp"
#else
#include <cadmium/core/modeling/atomic.hpp>
// #include "esp_log.h"
#endif

#include <iostream>
#include <string>
#include "../message.hpp"
#include "mqtt/mqtt_message.hpp"
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace cadmium::iot
{
    enum class NodeDiscoveryPhase
    {
        IDLE,
        ZONE_SEND_REPORT,
        ZONE_GET_REPORT
    };

    struct NodeDiscoveryState
    {
        double clock;
        double sigma;
        NodeDiscoveryPhase phase;
        vector<string> neighbouringNodes;
        NodeDiscoveryState()
            : clock(0),
              phase(NodeDiscoveryPhase::IDLE),
              sigma(std::numeric_limits<double>::max())
            {}
    };

    std::ostream &operator<<(std::ostream &out, const NodeDiscoveryState &s)
    {
        return out;
    }

    class NodeDiscovery : public Atomic<NodeDiscoveryState>
    {
    private:
        vector<string> zones;
    public:
        Port<vector<string>> inZones;
        Port<vector<string>> neighbouringNodes;
        Port<MQTTMessage> outMQTT;
        Port<MQTTMessage> inMQTT;
        
        // Port<MQTTMessage> outCommand;
        NodeDiscoveryState &getState() { return state; }


        NodeDiscovery(const std::string &id, vector<string> zones)
         : Atomic<NodeDiscoveryState>(id, NodeDiscoveryState()), zones(zones)
        {
            inZones = addInPort<vector<string>>("in_zones");
            neighbouringNodes = addOutPorts<vector<string>>("neighboringNodes");

            outMQTT = addOutPort<MQTTMessage>("outData");
            inMQTT = addInPort<MQTTMessage>("inMQTT");
        }

        void internalTransition(NodeDiscoveryState &s) const override
        {
            if(s.phase == NodeDiscoveryPhase::ZONE_GET_REPORT)
            
        }

        void externalTransition(NodeDiscoveryState &s, double e) const override
        {
            if(!inZones->empty())
            {
                auto zones = inZones->getBag().back();
                s.phase = NodeDiscoveryPhase::ZONE_GET_REPORT;
                s.sigma = 0;
            }
            
            if (!inMQTT->empty())
            {
                auto tmp = inMQTT->getBag().back();
            }
        }

        void output(const NodeDiscoveryState &s) const override {
            if(s.phase == ConnectionPhase::DATA_AVAILABLE_FOR_PUBLISH) {
                log("<NodeDiscovery> Sending message: " + jsonify(s.newMessage));
                
                outMQTT->addMessage(MQTTMessage("client",
                                                 jsonify(s.newMessage),
                                                 s.topicsToPublish.back(),
                                                 MQTTMessageType::PUBLISH, 2));

            } else if(s.phase ==  ConnectionPhase::DATA_AVAILABLE_FOR_SUBSCRIBE) {
                // outData->addMessage(MQTTMessage("client", "", "/topic/qos1", MQTTMessageType::SUBSCRIBE))
                log("<NodeDiscovery>" + id + " Sending SUBCRIBE message>" + s.listOfTopicsToSubscribe.back());

                outMQTT->addMessage(MQTTMessage("client", 
                                                "",
                                                s.listOfTopicsToSubscribe.back(),
                                                MQTTMessageType::SUBSCRIBE, 2));
            }
        }

        [[nodiscard]] double timeAdvance(const NodeDiscoveryState &s) const override
        {
            return s.sigma;
        }
    };
}

#endif
