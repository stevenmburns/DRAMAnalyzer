#ifndef DRAM_ANALYZER_H
#define DRAM_ANALYZER_H

#include <Analyzer.h>
#include "DRAMAnalyzerResults.h"
#include "DRAMSimulationDataGenerator.h"

class DRAMAnalyzerSettings;
class ANALYZER_EXPORT DRAMAnalyzer : public Analyzer
{
public:
	DRAMAnalyzer();
	virtual ~DRAMAnalyzer();
	virtual void WorkerThread();

	virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
	virtual U32 GetMinimumSampleRateHz();

	virtual const char* GetAnalyzerName() const;
	virtual bool NeedsRerun();

protected: //vars
	std::auto_ptr< DRAMAnalyzerSettings > mSettings;
	std::auto_ptr< DRAMAnalyzerResults > mResults;
	AnalyzerChannelData* mSerial;

	DRAMSimulationDataGenerator mSimulationDataGenerator;
	bool mSimulationInitilized;

	//Serial analysis vars:
	U32 mSampleRateHz;
	U32 mStartOfStopBitOffset;
	U32 mEndOfStopBitOffset;
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //DRAM_ANALYZER_H
