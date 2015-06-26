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

	  bool curr_RASb_is_high = mRASb->GetBitState() == BIT_HIGH;
	  bool curr_CASb_is_high = mCASb->GetBitState() == BIT_HIGH;


	  if ( prev_RASb_is_high && prev_CASb_is_high) {
	    if        (  curr_RASb_is_high &&  curr_CASb_is_high) {
	      /* no transition */
	    } else if (  curr_RASb_is_high && !curr_CASb_is_high) {
	      /* CAS */
	    } else if ( !curr_RASb_is_high &&  curr_CASb_is_high) {
	      /* RAS */
	      valid_row_addr = true;
	      U64 starting_sample = mRASb->GetSampleNumber();
	      mResults->AddMarker( starting_sample, AnalyzerResults::DownArrow, mSettings->mRASbChannel);
              mResults->AddResultString( "Valid Addr Required");
	      mResults->CommitResults();
	      ReportProgress( starting_sample);
	    } else if ( !curr_RASb_is_high && !curr_CASb_is_high) {
	      /* simultaneous flip */
	    }
	  } else if (  prev_RASb_is_high && !prev_CASb_is_high) {
	    if        (  curr_RASb_is_high &&  curr_CASb_is_high) {
	      valid_row_addr = false;
	    } else if (  curr_RASb_is_high && !curr_CASb_is_high) {
	      /* no transition */
	    } else if ( !curr_RASb_is_high &&  curr_CASb_is_high) {
	      /* simultaneous flip */
	    } else if ( !curr_RASb_is_high && !curr_CASb_is_high) {
	      /* RAS down after CAS down (hidden refresh) */
	      U64 starting_sample = mRASb->GetSampleNumber();
	      mResults->AddMarker( starting_sample, AnalyzerResults::Square, mSettings->mRASbChannel);
	      mResults->CommitResults();
	      ReportProgress( starting_sample);
	    }
	  } else if ( !prev_RASb_is_high &&  prev_CASb_is_high) {
	    if        (  curr_RASb_is_high &&  curr_CASb_is_high) {
	      /* RASb up */
	    } else if (  curr_RASb_is_high && !curr_CASb_is_high) {
	      /* simult */
	    } else if ( !curr_RASb_is_high &&  curr_CASb_is_high) {
	      /* no transition  */
	    } else if ( !curr_RASb_is_high && !curr_CASb_is_high) {
	      /* write/read */
	      U64 starting_sample = mCASb->GetSampleNumber();
	      mResults->AddMarker( starting_sample, AnalyzerResults::DownArrow, mSettings->mCASbChannel);
	      mResults->CommitResults();
	      ReportProgress( starting_sample);

#if 0
	      //we have a byte to save. 
	      Frame frame;
	      frame.mData1 = 1;
	      frame.mFlags = 0;
	      frame.mStartingSampleInclusive = starting_sample;
	      frame.mEndingSampleInclusive = starting_sample + 100;
	      mResults->AddFrame( frame );
	      mResults->CommitResults();
	      ReportProgress( frame.mEndingSampleInclusive );
#endif

	    }
	  } else if ( !prev_RASb_is_high && !prev_CASb_is_high) {
	    if        (  curr_RASb_is_high &&  curr_CASb_is_high) {
	      /* simultaneous flip */
	    } else if (  curr_RASb_is_high && !curr_CASb_is_high) {
	      /* RAS up */
	      /* write/read */
	      U64 starting_sample = mRASb->GetSampleNumber();
	      mResults->AddMarker( starting_sample, AnalyzerResults::UpArrow, mSettings->mRASbChannel);
	      mResults->CommitResults();
	      ReportProgress( starting_sample);

	    } else if ( !curr_RASb_is_high &&  curr_CASb_is_high) {
	      /* CAS up */
	    } else if ( !curr_RASb_is_high && !curr_CASb_is_high) {
	      /* no transition */
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
