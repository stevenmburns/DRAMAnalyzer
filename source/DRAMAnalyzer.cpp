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
	mResults->AddChannelBubblesWillAppearOn( mSettings->mCASbChannel );

	mRASb = GetAnalyzerChannelData( mSettings->mRASbChannel );
	mCASb = GetAnalyzerChannelData( mSettings->mCASbChannel );
	mWb = GetAnalyzerChannelData( mSettings->mWbChannel );
	mOEb = GetAnalyzerChannelData( mSettings->mOEbChannel );

	bool valid_row_addr = false;

	for( ; ; ) {

	  U64 next_RAS = mRASb->GetSampleOfNextEdge();
	  U64 next_CAS = mCASb->GetSampleOfNextEdge();

	  //	  std::cout << next_RAS << " " << next_CAS << std::endl;

	  bool prev_RASb_is_high = mRASb->GetBitState() == BIT_HIGH;
	  bool prev_CASb_is_high = mCASb->GetBitState() == BIT_HIGH;

	  if ( next_RAS <= next_CAS) {
	    mRASb->AdvanceToNextEdge();	    
	    mCASb->AdvanceToAbsPosition( mRASb->GetSampleNumber());
          } else {
	    mCASb->AdvanceToNextEdge();	    
	    mRASb->AdvanceToAbsPosition( mCASb->GetSampleNumber());
	  }

	  if ( prev_RASb_is_high && prev_CASb_is_high) {
	    if ( mRASb->GetBitState() == BIT_HIGH) {
	      if ( mCASb->GetBitState() == BIT_HIGH) {
		/* no transition */
	      } else {
		/* CAS */
	      }
	    } else {
	      if ( mCASb->GetBitState() == BIT_HIGH) {
		/* RAS */
		valid_row_addr = true;
	      } else {
		/* simultaneous flip */
	      }
	    }
	  } else if ( !prev_RASb_is_high &&  prev_CASb_is_high) {
	    if ( mRASb->GetBitState() == BIT_HIGH) {
	      if ( mCASb->GetBitState() == BIT_HIGH) {
		/* RASb up */
	      } else {
		/* simult */
	      }
	    } else {
	      if ( mCASb->GetBitState() == BIT_HIGH) {
		/* no transition  */
	      } else {
		/* write/read */
		U64 starting_sample = mCASb->GetSampleNumber();

		std::cout << "write/read: " << starting_sample << std::endl;

		//we have a byte to save. 
		Frame frame;
		frame.mData1 = 1;
		frame.mFlags = 0;
		frame.mStartingSampleInclusive = starting_sample;
		frame.mEndingSampleInclusive = starting_sample + 100;

		mResults->AddMarker( starting_sample, AnalyzerResults::DownArrow, mSettings->mCASbChannel);
		mResults->AddFrame( frame );
		mResults->CommitResults();
		ReportProgress( frame.mEndingSampleInclusive );
	      }
	    }
	  } else if (  prev_RASb_is_high && !prev_CASb_is_high) {
	    if ( mRASb->GetBitState() == BIT_HIGH) {
	      if ( mCASb->GetBitState() == BIT_HIGH) {
		valid_row_addr = false;
	      } else {
		/* no transition */
	      }
	    } else {
	      if ( mCASb->GetBitState() == BIT_HIGH) {
		/* simultaneous flip */
	      } else {
		/* RAS down after CAS down */
	      }
	    }
	  } else if ( !prev_RASb_is_high && !prev_CASb_is_high) {
	    if ( mRASb->GetBitState() == BIT_HIGH) {
	      if ( mCASb->GetBitState() == BIT_HIGH) {
		/* simultaneous flip */
	      } else {
		/* RAS up */
	      }
	    } else {
	      if ( mCASb->GetBitState() == BIT_HIGH) {
		/* CAS up */
	      } else {
		/* no transition */
	      }
	    }
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
