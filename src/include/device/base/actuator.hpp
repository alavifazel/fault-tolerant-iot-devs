 
#ifndef ACTUATOR_HPP
#define ACTUATOR_HPP

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
#include <regex>

using std::string;
using std::vector;
using namespace std;

namespace cadmium::iot
{
    enum class ActuatorPhase
    {
        IDLE,
        IRRIGATION_ON,
        IRRIGATING,
        IRRIGATION_OFF
    };

    struct ActuatorState
    {
        double clock;
        double sigma;
        ConnectionPhase connectionPhase;
        ActuatorPhase phase;
        Message<double> newMessage;
        vector<string> listOfTopicsToSubscribe;
        string topicToPublish;
        ActuatorState(string topicToPublish, vector<string> listOfTopicsToSubscribe = {})
            : clock(),
              sigma(std::numeric_limits<double>::max()), 
              newMessage(),
              topicToPublish(topicToPublish),
              listOfTopicsToSubscribe(listOfTopicsToSubscribe),
              phase(ActuatorPhase::IDLE)
            {
                if(listOfTopicsToSubscribe.size() > 0) {
                    connectionPhase = ConnectionPhase::DATA_AVAILABLE_FOR_SUBSCRIBE;
                    sigma = 0;
                }
            }
    };

    std::ostream &operator<<(std::ostream &out, const ActuatorState &s)
    {
        return out;
    }

    class Actuator : public Atomic<ActuatorState>
    {
    private:
         template<typename T>
         std::string jsonify(Message<T> message) const {
             std::string out;
             out += "{";
             out += "\"data\": " + std::to_string(message.getData()) + ", ";
             out += "\"origin\":\"" + message.getOrigin() + "\", ";
             out += "\"device_type\":\"" + message.getDeviceType() + "\"";
             out += "}";
             return out;
         }

        double extractData(string payload) const
        {
            std::regex rgx("\"data\": +([0-9]+\\.*[0-9]*)");
            std::smatch matches;

            if (std::regex_search(payload, matches, rgx))
            {
                return std::stoi(matches[1].str());
            }
            else
            {
                std::cout << "Match not found\n";
                return 0;
            }
        }         

    public:
        Port<Message<double>> inData;
        
        Port<MQTTMessage> outMQTT;
        Port<MQTTMessage> inMQTT;
        
        #ifndef DEPLOYMENT
        Port<bool> irrigationOff;
        Port<bool> irrigationOn;
        #endif

        // Port<MQTTMessage> outCommand;
        ActuatorState &getState() { return state; }


        Actuator(const std::string &id, string topicToPublish, vector<string> listOfTopicsToSubscribe = {})
         : Atomic<ActuatorState>(id, ActuatorState(topicToPublish, listOfTopicsToSubscribe))
        {
			inData = addInPort<Message<double>>("inData");
            outMQTT = addOutPort<MQTTMessage>("outData");
            inMQTT = addInPort<MQTTMessage>("inMQTT");

            #ifndef DEPLOYMENT
            irrigationOff = addOutPort<bool>("irrigation_off");
            irrigationOn = addOutPort<bool>("irrigation_on");
            #endif
        }

        void internalTransition(ActuatorState &s) const override
        {
            if(s.listOfTopicsToSubscribe.size() <= 0)
            {
                s.connectionPhase = ConnectionPhase::IDLE;
                s.sigma = std::numeric_limits<double>::max();
            }
            else
            {
                s.listOfTopicsToSubscribe.pop_back();
                if(s.listOfTopicsToSubscribe.size() != 0)
                {
                    s.connectionPhase = ConnectionPhase::DATA_AVAILABLE_FOR_SUBSCRIBE;
                    s.sigma = 0;
                }
                else
                {
                    s.connectionPhase = ConnectionPhase::IDLE;
                    s.sigma = std::numeric_limits<double>::max();
                }
            }
            if(s.phase == ActuatorPhase::IRRIGATION_ON)
            {
                s.phase = ActuatorPhase::IRRIGATING;
                s.sigma = 10;
            }
            else if(s.phase == ActuatorPhase::IRRIGATING)
            {
                s.phase = ActuatorPhase::IRRIGATION_OFF;
                s.sigma = std::numeric_limits<double>::max();
            }
        }

        void externalTransition(ActuatorState &s, double e) const override
        {
            if(!inData->empty())
            {
                s.newMessage = inData->getBag().back();
                log("<Actuator> External Transition called. New MSG");
                s.sigma = 0;
                s.connectionPhase = ConnectionPhase::DATA_AVAILABLE_FOR_PUBLISH;
            }
            
            if (!inMQTT->empty())
            {
                auto tmp = inMQTT->getBag().back();
                auto soilMoisture = extractData(tmp.getData());
                if(soilMoisture < 15)
                {
                    if(s.phase == ActuatorPhase::IDLE && s.phase != ActuatorPhase::IRRIGATING)
                    {
                        s.phase = ActuatorPhase::IRRIGATION_ON;
                        s.sigma = 0;
                    }
                }
            }
        }

        void output(const ActuatorState &s) const override {
            if(s.connectionPhase == ConnectionPhase::DATA_AVAILABLE_FOR_PUBLISH) {
                log("<Actuator> Sending message: " + jsonify(s.newMessage));

                outMQTT->addMessage(MQTTMessage("client",
                                                 jsonify(s.newMessage),
                                                 s.topicToPublish,
                                                 MQTTMessageType::PUBLISH, 2));

            } else if(s.connectionPhase ==  ConnectionPhase::DATA_AVAILABLE_FOR_SUBSCRIBE) {
                log("<Actuator>" + id + " Sending SUBCRIBE message");

                outMQTT->addMessage(MQTTMessage("client", 
                                                "",
                                                s.listOfTopicsToSubscribe.back(),
                                                MQTTMessageType::SUBSCRIBE, 2));
            }

            if(s.phase == ActuatorPhase::IRRIGATION_ON)
            {
                #ifndef DEPLOYMENT
                log("<ACTUATOR> SENDING IRRIGATION ON SIGNAL.");
                irrigationOn->addMessage(true);
                #endif
            }
            else if (s.phase == ActuatorPhase::IRRIGATING)
            {
                #ifndef DEPLOYMENT
                log("<ACTUATOR> SENDING IRRIGATION OFF SIGNAL.");
                irrigationOff->addMessage(true);
                #endif
            }
        }

        [[nodiscard]] double timeAdvance(const ActuatorState &s) const override
        {
            return s.sigma;
        }
    };
}

#endif
