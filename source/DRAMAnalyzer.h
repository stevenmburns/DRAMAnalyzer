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
	std::shared_ptr< DRAMAnalyzerSettings > mSettings;
	std::shared_ptr< DRAMAnalyzerResults > mResults;
	AnalyzerChannelData* mRASb;
	AnalyzerChannelData* mCASb;
	AnalyzerChannelData* mWb;
	AnalyzerChannelData* mOEb;

	DRAMSimulationDataGenerator mSimulationDataGenerator;
	bool mSimulationInitilized;
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //DRAM_ANALYZER_H
