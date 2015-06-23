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

	for( ; ; ) {


	  if( mRASb->GetBitState() == BIT_HIGH ) {
	    U64 starting_sample = mRASb->GetSampleNumber();
	    mRASb->AdvanceToNextEdge();

	    //we have a byte to save. 
	    Frame frame;
	    frame.mData1 = 7; // Hacked
	    frame.mFlags = 0;
	    frame.mStartingSampleInclusive = starting_sample;
	    frame.mEndingSampleInclusive = mRASb->GetSampleNumber();

	    mResults->AddFrame( frame );
	    mResults->CommitResults();
	    ReportProgress( frame.mEndingSampleInclusive );
	  } else {
	    mRASb->AdvanceToNextEdge();
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
