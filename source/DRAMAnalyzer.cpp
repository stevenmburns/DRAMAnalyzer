#include "DRAMAnalyzer.h"
#include "DRAMAnalyzerSettings.h"
#include <AnalyzerChannelData.h>
#include <iostream>

DRAMAnalyzer::DRAMAnalyzer()
:	Analyzer(),  
	mSettings( new DRAMAnalyzerSettings() ),
	mSimulationInitilized( false )
{
	SetAnalyzerSettings( mSettings.get() );
}

DRAMAnalyzer::~DRAMAnalyzer()
{
	KillThread();
}

void DRAMAnalyzer::WorkerThread()
{
	mResults.reset( new DRAMAnalyzerResults( this, mSettings.get() ) );
	SetAnalyzerResults( mResults.get() );
	mResults->AddChannelBubblesWillAppearOn( mSettings->mRASbChannel );

	mRASb = GetAnalyzerChannelData( mSettings->mRASbChannel );
	mCASb = GetAnalyzerChannelData( mSettings->mCASbChannel );
	mWb = GetAnalyzerChannelData( mSettings->mWbChannel );
	mOEb = GetAnalyzerChannelData( mSettings->mOEbChannel );

	bool valid_row_addr = false;

	for( ; ; ) {

	  U64 next_RAS = mRASb->GetSampleOfNextEdge();
	  U64 next_CAS = mCASb->GetSampleOfNextEdge();

	  std::cout << next_RAS << " " << next_CAS << std::endl;

	  bool prev_RASb_is_high = mRASb->GetBitState() == BIT_HIGH;
	  bool prev_CASb_is_high = mCASb->GetBitState() == BIT_HIGH;

	  if        ( next_RAS == next_CAS) {
	    mRASb->AdvanceToNextEdge();	    
	    mCASb->AdvanceToNextEdge();
	  } else if ( next_RAS < next_CAS) {
	    mRASb->AdvanceToNextEdge();	    

	    if ( prev_RASb_is_high && prev_CASb_is_high) {
	      valid_row_addr = true;
	    }

	  } else { /* next_RAS > next_CAS */
	    mCASb->AdvanceToNextEdge();

	    U64 starting_sample = mCASb->GetSampleNumber();

	    //we have a byte to save. 
	    Frame frame;
	    frame.mData1 = 1;
	    frame.mFlags = 0;
	    frame.mStartingSampleInclusive = starting_sample;
	    frame.mEndingSampleInclusive = starting_sample + 1;

	    mResults->AddFrame( frame );
	    mResults->CommitResults();
	    ReportProgress( frame.mEndingSampleInclusive );
	  }

          CheckIfThreadShouldExit();
	}
}

bool DRAMAnalyzer::NeedsRerun()
{
	return false;
}

U32 DRAMAnalyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	if( mSimulationInitilized == false )
	{
		mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), mSettings.get() );
		mSimulationInitilized = true;
	}

	return mSimulationDataGenerator.GenerateSimulationData( minimum_sample_index, device_sample_rate, simulation_channels );
}

U32 DRAMAnalyzer::GetMinimumSampleRateHz()
{
	return 100000;
}

const char* DRAMAnalyzer::GetAnalyzerName() const
{
	return "Asynchoronous DRAM Analyzer";
}

const char* GetAnalyzerName()
{
	return "Asynchoronous DRAM Analyzer";
}

Analyzer* CreateAnalyzer()
{
	return new DRAMAnalyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
	delete analyzer;
}
