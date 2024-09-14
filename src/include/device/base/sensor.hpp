#ifndef SENSOR_HPP
#define SENSOR_HPP

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
#include <set>
#include <regex>
#include <map>
#include "../../../utilities/brooks_iyengar.hpp"

using std::string;
using std::vector;

using namespace std;

namespace cadmium::iot
{
    enum class ConnectionPhase
    {
        IDLE,
        DATA_AVAILABLE_FOR_PUBLISH,
        DATA_AVAILABLE_FOR_SUBSCRIBE,
        CONVERGING
    };

    struct SensorState
    {
        double clock;
        double sigma;
        ConnectionPhase phase;
        Message<double> newMessage;
        vector<string> listOfTopicsToSubscribe;
        string topicToPublish;
        set<string> neighbouringNodes;
        vector<double> receivedBuffer;
        map<string,double> m;
        int counter;
        SensorState(double sigma, string topicToPublish, vector<string> listOfTopicsToSubscribe = {})
            : clock(),
              sigma(sigma),
              newMessage(),
              topicToPublish(topicToPublish),
              listOfTopicsToSubscribe(listOfTopicsToSubscribe),
              counter(0)
        {
            if (listOfTopicsToSubscribe.size() > 0)
            {
                phase = ConnectionPhase::DATA_AVAILABLE_FOR_SUBSCRIBE;
                sigma = 0;
            }
        }
    };

    std::ostream &operator<<(std::ostream &out, const SensorState &s)
    {
        return out;
    }

    class Sensor : public Atomic<SensorState>
    {
    private:
        template <typename T>
        std::string jsonify(Message<T> message) const
        {
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
                return std::stof(matches[1].str());
            }
            else
            {
                std::cout << "Match not found\n";
                return 0;
            }
        }

        string extractOrigin(string origin) const
        {
            std::regex rgx("\"origin\":\"(.*)\",");
            std::smatch matches;

            if (std::regex_search(origin, matches, rgx))
            {
                return matches[1].str();
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

        SensorState &getState() { return state; }

        string zone;
        int numberOfNeighbours;

        Sensor(const std::string &id, const string zone, string topicToPublish, vector<string> listOfTopicsToSubscribe = {})
            : Atomic<SensorState>(id, SensorState(2, topicToPublish, listOfTopicsToSubscribe)),
              zone(zone),
              numberOfNeighbours(listOfTopicsToSubscribe.size())
        {
            for (size_t i = 0; i < listOfTopicsToSubscribe.size(); i++)
            {
                log(id + " " + listOfTopicsToSubscribe[i],2);
            }
            
            inData = addInPort<Message<double>>("inData");
            outMQTT = addOutPort<MQTTMessage>("outData");
            inMQTT = addInPort<MQTTMessage>("inMQTT");

        }

        void internalTransition(SensorState &s) const override
        {

            if (s.listOfTopicsToSubscribe.size() <= 0)
            {
                s.phase = ConnectionPhase::IDLE;
                s.sigma = std::numeric_limits<double>::max();
            }
            else
            {
                s.listOfTopicsToSubscribe.pop_back();
                if (s.listOfTopicsToSubscribe.size() != 0)
                {
                    s.phase = ConnectionPhase::DATA_AVAILABLE_FOR_SUBSCRIBE;
                    s.sigma = 0;
                    return;
                }
                else
                {
                    s.phase = ConnectionPhase::IDLE;
                    s.sigma = std::numeric_limits<double>::max();
                }
            }
            if(s.counter < 30)
            {
                s.phase = ConnectionPhase::DATA_AVAILABLE_FOR_PUBLISH;
                s.sigma = 50;
                s.counter++;
            } else
            {
                s.phase = ConnectionPhase::IDLE;
                s.sigma = std::numeric_limits<double>::max();                
            }

        }

        void externalTransition(SensorState &s, double e) const override
        {
            // s.clock += e;
            s.sigma -= e;
            if (!inData->empty())
            {
                s.newMessage = inData->getBag().back();
                s.newMessage.setOrigin(id);
                log("<Sensor> External Transition called. New MSG = " + id + " " + std::to_string(s.newMessage.getData()),2);
                s.sigma = 0;
                s.phase = ConnectionPhase::DATA_AVAILABLE_FOR_PUBLISH;
                s.counter = 0;

            }

            if (!inMQTT->empty())
            {
                auto tmp = inMQTT->getBag().back();
                s.receivedBuffer.push_back(extractData(tmp.getData()));

                cout << extractOrigin(tmp.getData()) << endl;
                s.m[extractOrigin(tmp.getData())] = extractData(tmp.getData());
                cout << s.m.size() << endl;

                log("My ID: " + id + " " + tmp.getData(), 2);


                if(s.counter > 30)
                {
                    s.phase = ConnectionPhase::IDLE;
                    s.sigma = std::numeric_limits<double>::max();
                    return;
                }
                if (s.m.size() >= numberOfNeighbours+1)
                {
                    s.phase = ConnectionPhase::DATA_AVAILABLE_FOR_PUBLISH;
                    s.sigma = 0;
                    s.counter++;

                    vector<pair<double, double>> tmp;
                    
                    for(const auto e: s.m)
                    {
                        tmp.push_back(make_pair(e.second - 2, e.second + 2));
                    }

                    s.newMessage.setData(brooksIyengar(tmp, 2));
                    s.newMessage.setOrigin(id);

                    log("My ID: " + id + " FUSED: " + to_string(brooksIyengar(tmp, 2)), 2);

                    s.m.clear();
                    s.m["self"] = s.newMessage.getData();
                }
            }
        }

        void output(const SensorState &s) const override
        {
            if (s.phase == ConnectionPhase::DATA_AVAILABLE_FOR_PUBLISH)
            {
                log("<Sensor> ID(" + id + ") Sending message: " + jsonify(s.newMessage) + " topic: " + s.topicToPublish, 2);
                outMQTT->addMessage(MQTTMessage("client",
                                                jsonify(s.newMessage),
                                                s.topicToPublish,
                                                MQTTMessageType::PUBLISH, 2));
            }
            else if (s.phase == ConnectionPhase::DATA_AVAILABLE_FOR_SUBSCRIBE)
            {
                log("<Sensor>" + id + " Sending SUBCRIBE message>" + s.listOfTopicsToSubscribe.back(), 2);
                outMQTT->addMessage(MQTTMessage("client",
                                                "",
                                                s.listOfTopicsToSubscribe.back(),
                                                MQTTMessageType::SUBSCRIBE, 2));
            }
        }

        [[nodiscard]] double timeAdvance(const SensorState &s) const override
        {
            return s.sigma;
        }
    };
}

#endif
