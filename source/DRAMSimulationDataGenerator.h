#ifndef DRAM_SIMULATION_DATA_GENERATOR
#define DRAM_SIMULATION_DATA_GENERATOR

#include <SimulationChannelDescriptor.h>
#include <string>
class DRAMAnalyzerSettings;

class DRAMSimulationDataGenerator
{
public:
	DRAMSimulationDataGenerator();
	~DRAMSimulationDataGenerator();

	void Initialize( U32 simulation_sample_rate, DRAMAnalyzerSettings* settings );
	U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel );

protected:
	DRAMAnalyzerSettings* mSettings;
	U32 mSimulationSampleRateHz;

protected:
	void CreateSerialByte();
	std::string mSerialText;
	U32 mStringIndex;

	SimulationChannelDescriptor mSerialSimulationData;

};
#endif //DRAM_SIMULATION_DATA_GENERATOR