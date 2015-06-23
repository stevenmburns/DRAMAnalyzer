#include "DRAMSimulationDataGenerator.h"
#include "DRAMAnalyzerSettings.h"

#include <AnalyzerHelpers.h>

DRAMSimulationDataGenerator::DRAMSimulationDataGenerator()
{
}

DRAMSimulationDataGenerator::~DRAMSimulationDataGenerator()
{
}

void DRAMSimulationDataGenerator::Initialize( U32 simulation_sample_rate, DRAMAnalyzerSettings* settings )
{
	mSimulationSampleRateHz = simulation_sample_rate;
	mSettings = settings;

	mRASb = mSimulationData.Add( mSettings->mRASbChannel, mSimulationSampleRateHz, BIT_HIGH);
	mCASb = mSimulationData.Add( mSettings->mCASbChannel, mSimulationSampleRateHz, BIT_HIGH);
	mWb   = mSimulationData.Add( mSettings->mWbChannel, mSimulationSampleRateHz, BIT_HIGH);
	mOEb  = mSimulationData.Add( mSettings->mOEbChannel, mSimulationSampleRateHz, BIT_HIGH);
}

U32 DRAMSimulationDataGenerator::GenerateSimulationData( U64 largest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel )
{
	U64 adjusted_largest_sample_requested = AnalyzerHelpers::AdjustSimulationTargetSample( largest_sample_requested, sample_rate, mSimulationSampleRateHz );

	while( mRASb->GetCurrentSampleNumber() < adjusted_largest_sample_requested )
	{
	  mSimulationData.AdvanceAll( 1000);
	  mRASb->Transition();
	  mSimulationData.AdvanceAll( 1000);
	  mCASb->Transition();
	  mSimulationData.AdvanceAll( 1000);
	  mCASb->Transition();
	  mSimulationData.AdvanceAll( 1000);
	  mRASb->Transition();
	}

	*simulation_channel = mSimulationData.GetArray();
	return mSimulationData.GetCount();
}


