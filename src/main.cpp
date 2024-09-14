#include <cadmium/core/logger/csv.hpp>
#include <cadmium/core/simulation/root_coordinator.hpp>
#include <cadmium/lib/iestream.hpp>
#include <limits>
#include "include/generator_model.hpp"
#include "include/generator_n_shots.hpp"
#include "include/generator_n_shots_bool.hpp"
#include "include/rand.hpp"
#include "include/environment/soil.hpp"
#include "include/device/wrapper_model.hpp"
#include "include/device/base/sensor.hpp"
#include "include/device/base/actuator.hpp"
#include "include/mqtt/client/client.hpp"
#include "include/mqtt/broker/broker.hpp"
#include "include/device/daq.hpp"
#include "include/mqtt/propagation_medium/propagation_medium.hpp"
#include "include/utility.hpp"

using namespace std;

namespace cadmium::iot
{

	struct TopLevelModel : public Coupled
	{
		TopLevelModel(const std::string &id) : Coupled(id)
		{
			const int numOfBrokerPorts = 10;

			vector<std::function<double()>> f_intervalGeneration;

			f_intervalGeneration.push_back([=]()
										   {
			static bool firstRun = true;
			if(firstRun)
			{
				firstRun = false;
				return 1000 + 500.0;
			}

			return  1000.0; });

			f_intervalGeneration.push_back([=]()
										   {
			static bool firstRun2 = true;
			if(firstRun2)
			{
				firstRun2 = false;
				return 1000 + 500.0;

			}
			return  1000.0; });

			f_intervalGeneration.push_back([=]()
										   {
			static bool firstRun3 = true;
			if(firstRun3)
			{
				return 1000 + 400.0;

			}
			return  1000.0; });

			f_intervalGeneration.push_back([=]()
										   {
			static bool firstRun4 = true;
			if(firstRun4)
			{
				firstRun4 = false;
				return 1000 + 400.0;

			}
			return  1000.0; });

			f_intervalGeneration.push_back([=]()
										   {
			static bool firstRun5 = true;
			if(firstRun5)
			{
				firstRun5 = false;
				return 1000 + 300.0;

			}
			return  1000.0; });

			f_intervalGeneration.push_back([=]()
										   {
			static bool firstRun6 = true;
			if(firstRun6)
			{
				firstRun6 = false;
				return 1000 + 300.0;

			}
			return  1000.0; });

			f_intervalGeneration.push_back([=]()
										   {
			static bool firstRun7 = true;
			if(firstRun7)
			{
				firstRun7 = false;
				return 1000 + 4.0;

			}
			return  1000.0; });

			f_intervalGeneration.push_back([=]()
										   {
			static bool firstRun8 = true;
			if(firstRun8)
			{
				firstRun8 = false;
				return 1000 + 4.0;

			}
			return  1000.0; });

			f_intervalGeneration.push_back([=]()
										   {
			static bool firstRun9 = true;
			if(firstRun9)
			{
				firstRun9 = false;
				return 1000 + 5.0;

			}
			return  1000.0; });

			f_intervalGeneration.push_back([=]()
										   {
			static bool firstRun10 = true;
			if(firstRun10)
			{
				firstRun10 = false;
				return 1000 + 5.0;

			}
			return  1000.0; });

			f_intervalGeneration.push_back([=]()
										   {
			static bool firstRun11 = true;
			if(firstRun11)
			{
				firstRun11 = false;
				return 1000 + 6.0;

			}
			return  1000.0; });

			f_intervalGeneration.push_back([=]()
										   {
			static bool firstRun12 = true;
			if(firstRun12)
			{
				firstRun12 = false;
				return 1000 + 6.0;
			}
			return  1000.0; });

			f_intervalGeneration.push_back([=]()
										   {
			static bool firstRun13 = true;
			if(firstRun13)
			{
				firstRun13 = false;
				return 1000 + 7.0;
			}
			return  1000.0; });

			f_intervalGeneration.push_back([=]()
										   {
			static bool firstRun14 = true;
			if(firstRun14)
			{
				firstRun14 = false;
				return 1000 + 7.0;

			}
			return  1000.0; });

			std::function<double(std::string id, int x)> f_dataGeneration = [](std::string id, int x)
			{
				static std::default_random_engine generator(std::random_device{}());
				static std::normal_distribution<double> distribution(5.0, 0.1);
				return distribution(generator);
			};

			std::function<double(std::string id, int x)> f_dataGeneration2 = [](std::string id, int x)
			{

				static std::default_random_engine generator(std::random_device{}());
				static std::normal_distribution<double> distribution(20.0, 0.4);
				return distribution(generator);
			};

			vector<std::shared_ptr<cadmium::iot::GeneratorNShots>> generators;
			generators.push_back(addComponent<GeneratorNShots>("generator_n_shots1", f_intervalGeneration[0], f_dataGeneration2, 7));
			generators.push_back(addComponent<GeneratorNShots>("generator_n_shots2", f_intervalGeneration[1], f_dataGeneration2, 7));
			generators.push_back(addComponent<GeneratorNShots>("generator_n_shots3", f_intervalGeneration[2], f_dataGeneration2, 7));
			generators.push_back(addComponent<GeneratorNShots>("generator_n_shots4", f_intervalGeneration[3], f_dataGeneration2, 7));
			generators.push_back(addComponent<GeneratorNShots>("generator_n_shots5", f_intervalGeneration[4], f_dataGeneration, 7));
			generators.push_back(addComponent<GeneratorNShots>("generator_n_shots6", f_intervalGeneration[5], f_dataGeneration, 7));
			generators.push_back(addComponent<GeneratorNShots>("generator_n_shots7", f_intervalGeneration[6], f_dataGeneration, 7));
			generators.push_back(addComponent<GeneratorNShots>("generator_n_shots8", f_intervalGeneration[7], f_dataGeneration, 7));
			generators.push_back(addComponent<GeneratorNShots>("generator_n_shots9", f_intervalGeneration[8], f_dataGeneration, 7));
			generators.push_back(addComponent<GeneratorNShots>("generator_n_shots10", f_intervalGeneration[9], f_dataGeneration, 7));
			generators.push_back(addComponent<GeneratorNShots>("generator_n_shots11", f_intervalGeneration[10], f_dataGeneration, 7));
			generators.push_back(addComponent<GeneratorNShots>("generator_n_shots12", f_intervalGeneration[11], f_dataGeneration, 7));
			generators.push_back(addComponent<GeneratorNShots>("generator_n_shots13", f_intervalGeneration[12], f_dataGeneration, 7));
			generators.push_back(addComponent<GeneratorNShots>("generator_n_shots14", f_intervalGeneration[13], f_dataGeneration, 7));

			vector<std::shared_ptr<cadmium::iot::DAQ>> daqs;
			daqs.push_back(addComponent<DAQ>("daq1"));
			daqs.push_back(addComponent<DAQ>("daq2"));
			daqs.push_back(addComponent<DAQ>("daq3"));
			daqs.push_back(addComponent<DAQ>("daq4"));
			daqs.push_back(addComponent<DAQ>("daq5"));
			daqs.push_back(addComponent<DAQ>("daq6"));
			daqs.push_back(addComponent<DAQ>("daq7"));

			vector<std::shared_ptr<cadmium::iot::Wrapper>> wrappers;
			wrappers.push_back(addComponent<Wrapper>("wrapper1", "generator1", "simulation_model"));
			wrappers.push_back(addComponent<Wrapper>("wrapper2", "generator2", "simulation_model"));
			wrappers.push_back(addComponent<Wrapper>("wrapper3", "generator3", "simulation_model"));
			wrappers.push_back(addComponent<Wrapper>("wrapper4", "generator4", "simulation_model"));
			wrappers.push_back(addComponent<Wrapper>("wrapper5", "generator5", "simulation_model"));
			wrappers.push_back(addComponent<Wrapper>("wrapper6", "generator6", "simulation_model"));
			wrappers.push_back(addComponent<Wrapper>("wrapper7", "generator7", "simulation_model"));
			wrappers.push_back(addComponent<Wrapper>("wrapper8", "generator8", "simulation_model"));
			wrappers.push_back(addComponent<Wrapper>("wrapper9", "generator9", "simulation_model"));
			wrappers.push_back(addComponent<Wrapper>("wrapper10", "generator10", "simulation_model"));
			wrappers.push_back(addComponent<Wrapper>("wrapper11", "generator11", "simulation_model"));
			wrappers.push_back(addComponent<Wrapper>("wrapper12", "generator12", "simulation_model"));
			wrappers.push_back(addComponent<Wrapper>("wrapper13", "generator13", "simulation_model"));
			wrappers.push_back(addComponent<Wrapper>("wrapper14", "generator14", "simulation_model"));
			vector<std::shared_ptr<cadmium::iot::Sensor>> sensors;
			double r = 1.42;
			vector<pair<double, double>> v;
			v.push_back(make_pair(0, 0));
			v.push_back(make_pair(0, 0));
			v.push_back(make_pair(0, 0));
			v.push_back(make_pair(0, 0));
			v.push_back(make_pair(0, 0));
			v.push_back(make_pair(0, 0));
			v.push_back(make_pair(0, 0));

			auto m = createPubMap(getNeighbors(v, r));

			sensors.push_back(addComponent<Sensor>("sensor1",
												   "zone1",
												   "/zone/0",
												   m[0]));

			sensors.push_back(addComponent<Sensor>("sensor2",
												   "zone1",
												   "/zone/1",
												   m[1]));

			sensors.push_back(addComponent<Sensor>("sensor3",
												   "zone1",
												   "/zone/2",
												   m[2]));

			sensors.push_back(addComponent<Sensor>("sensor4",
												   "zone1",
												   "/zone/3",
												   m[3]));

			sensors.push_back(addComponent<Sensor>("sensor5",
												   "zone1",
												   "/zone/4",
												   m[4]));

			sensors.push_back(addComponent<Sensor>("sensor6",
												   "zone1",
												   "/zone/5",
												   m[5]));

			sensors.push_back(addComponent<Sensor>("sensor7",
												   "zone1",
												   "/zone/6",
												   m[6]));

			vector<std::shared_ptr<cadmium::iot::MQTTClient>> mqttClients;
			mqttClients.push_back(addComponent<MQTTClient>("mqtt_client1"));
			mqttClients.push_back(addComponent<MQTTClient>("mqtt_client2"));
			mqttClients.push_back(addComponent<MQTTClient>("mqtt_client3"));
			mqttClients.push_back(addComponent<MQTTClient>("mqtt_client4"));
			mqttClients.push_back(addComponent<MQTTClient>("mqtt_client5"));
			mqttClients.push_back(addComponent<MQTTClient>("mqtt_client6"));
			mqttClients.push_back(addComponent<MQTTClient>("mqtt_client7"));

			auto mqttBroker = addComponent<MQTTBroker>("mqtt_broker", 0.1, numOfBrokerPorts);
			auto propagationMedium = addComponent<PropagationMedium>("propagation_medium", 0.1, numOfBrokerPorts);
			addCoupling(propagationMedium->receivingBufferOut, mqttBroker->inBroker);
			addCoupling(mqttBroker->outBroker, propagationMedium->sendingBufferIn);

			for (int i = 0; i < sensors.size(); i++)
			{
				addCoupling(generators[2 * i]->outData, wrappers[2 * i]->inData);
				addCoupling(wrappers[2 * i]->outData, daqs[i]->inData);
				addCoupling(generators[2 * i + 1]->outData, wrappers[2 * i + 1]->inData);
				addCoupling(wrappers[2 * i + 1]->outData, daqs[i]->inData);
				addCoupling(daqs[i]->outData, sensors[i]->inData);
				addCoupling(sensors[i]->outMQTT, mqttClients[i]->inData);
				addCoupling(mqttClients[i]->outData, sensors[i]->inMQTT);
				assignFreePort(mqttClients[i], mqttBroker, propagationMedium, numOfBrokerPorts);
			}

		}

		void assignFreePort(std::shared_ptr<cadmium::iot::MQTTClient> mqttClient,
							std::shared_ptr<cadmium::iot::MQTTBroker> mqttBroker,
							std::shared_ptr<cadmium::iot::PropagationMedium> propagationMedium,
							int numOfBrokerPorts)
		{
			static bool firstRun = false;
			static vector<bool> freePorts(numOfBrokerPorts, true);
			if (firstRun)
			{
				firstRun = false;
			}
			else
			{
				for (size_t i = 0; i < freePorts.size(); i++)
				{
					if (freePorts[i])
					{
						addCoupling(mqttClient->outBroker, propagationMedium->receivingBufferIn[i]);
						addCoupling(propagationMedium->sendingBufferOut[i], mqttClient->inBroker);
						freePorts[i] = false;
						break;
					}
				}
			}
		}
	};
}

using namespace cadmium::iot;

int main()
{
	auto model = make_shared<TopLevelModel>("top");
	auto rootCoordinator = cadmium::RootCoordinator(model);
	auto logger = make_shared<cadmium::CSVLogger>("log.csv", ";");
	rootCoordinator.setLogger(logger);
	rootCoordinator.start();
	rootCoordinator.simulate(std::numeric_limits<double>::infinity());
	rootCoordinator.stop();
	return 0;
}
