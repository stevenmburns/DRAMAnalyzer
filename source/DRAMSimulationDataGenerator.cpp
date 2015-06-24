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
	mOEb  = mSimulationData.Add( mSettings->mOEbChannel, mSimulationSampleRateHz, BIT_LOW);
}

U32 DRAMSimulationDataGenerator::GenerateSimulationData( U64 largest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel )
{
	U64 adjusted_largest_sample_requested = AnalyzerHelpers::AdjustSimulationTargetSample( largest_sample_requested, sample_rate, mSimulationSampleRateHz );

	//
	// Eight RAS only to get started
	//

	for (int i=0; i<8; ++i) {
	  mSimulationData.AdvanceAll( 1000);   mRASb->Transition();
	  mSimulationData.AdvanceAll( 1000);   mRASb->Transition();
	}

	const int nwrite_rows = 16;
	const int nread_rows = 16;
	const int cols_per_row = 8;

	while( mRASb->GetCurrentSampleNumber() < adjusted_largest_sample_requested )
	{
	  mSimulationData.AdvanceAll( 1000);   mWb->Transition(); mOEb->Transition();	  

	  for (int i=0; i<nwrite_rows; ++i) {
	    // Row addr
	    mSimulationData.AdvanceAll( 1000);   mRASb->Transition();
	    for (int j=0; j<cols_per_row; ++j) {
	      // Col addr
	      mSimulationData.AdvanceAll( 1000); mCASb->Transition();
	      if ( j+1==cols_per_row) {
		// Hidden refresh
		mSimulationData.AdvanceAll( 1000);   mRASb->Transition();
		mSimulationData.AdvanceAll( 1000);   mRASb->Transition();
	      }
	      mSimulationData.AdvanceAll( 1000); mCASb->Transition();
	    }
	    mSimulationData.AdvanceAll( 1000);   mRASb->Transition();
	  }

	  mSimulationData.AdvanceAll( 1000);   mWb->Transition(); mOEb->Transition();	  

	  for (int i=0; i<nread_rows; ++i) {
	    // Row addr
	    mSimulationData.AdvanceAll( 1000);   mRASb->Transition();
	    for (int j=0; j<cols_per_row; ++j) {
	      // Col addr
	      mSimulationData.AdvanceAll( 1000); mCASb->Transition();
	      if ( j+1==cols_per_row) {
		// Hidden refresh
		mSimulationData.AdvanceAll( 1000);   mRASb->Transition();
		mSimulationData.AdvanceAll( 1000);   mRASb->Transition();
	      }
	      mSimulationData.AdvanceAll( 1000); mCASb->Transition();
	    }
	    mSimulationData.AdvanceAll( 1000);   mRASb->Transition();
	  }
	}

	*simulation_channel = mSimulationData.GetArray();
	return mSimulationData.GetCount();
}


