#include "DRAMAnalyzerSettings.h"
#include <AnalyzerHelpers.h>


DRAMAnalyzerSettings::DRAMAnalyzerSettings()
:	mRASbChannel( UNDEFINED_CHANNEL ),
	mCASbChannel( UNDEFINED_CHANNEL ),
	mWbChannel( UNDEFINED_CHANNEL ),
	mOEbChannel( UNDEFINED_CHANNEL )
{
	mRASbChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
	mRASbChannelInterface->SetTitleAndTooltip( "RASb", "Standard Asynchoronous DRAM Analyzer" );
	mRASbChannelInterface->SetChannel( mRASbChannel );

	mCASbChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
	mCASbChannelInterface->SetTitleAndTooltip( "CASb", "Standard Asynchoronous DRAM Analyzer" );
	mCASbChannelInterface->SetChannel( mCASbChannel );

	mWbChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
	mWbChannelInterface->SetTitleAndTooltip( "Wb", "Standard Asynchoronous DRAM Analyzer" );
	mWbChannelInterface->SetChannel( mWbChannel );

	mOEbChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
	mOEbChannelInterface->SetTitleAndTooltip( "OEb", "Standard Asynchoronous DRAM Analyzer" );
	mOEbChannelInterface->SetChannel( mOEbChannel );

	AddInterface( mRASbChannelInterface.get() );
	AddInterface( mCASbChannelInterface.get() );
	AddInterface( mWbChannelInterface.get() );
	AddInterface( mOEbChannelInterface.get() );

	AddExportOption( 0, "Export as text/csv file" );
	AddExportExtension( 0, "text", "txt" );
	AddExportExtension( 0, "csv", "csv" );

	ClearChannels();
	AddChannel( mRASbChannel, "RASb", false );
	AddChannel( mCASbChannel, "CASb", false );
	AddChannel( mWbChannel, "Wb", false );
	AddChannel( mOEbChannel, "OEb", false );
}

DRAMAnalyzerSettings::~DRAMAnalyzerSettings()
{
}

bool DRAMAnalyzerSettings::SetSettingsFromInterfaces()
{
	mRASbChannel = mRASbChannelInterface->GetChannel();
	mCASbChannel = mCASbChannelInterface->GetChannel();
	mWbChannel = mWbChannelInterface->GetChannel();
	mOEbChannel = mOEbChannelInterface->GetChannel();

	ClearChannels();
	AddChannel( mRASbChannel, "Asynchoronous DRAM Analyzer", true );
	AddChannel( mCASbChannel, "Asynchoronous DRAM Analyzer", true );
	AddChannel( mWbChannel, "Asynchoronous DRAM Analyzer", true );
	AddChannel( mOEbChannel, "Asynchoronous DRAM Analyzer", true );

	return true;
}

void DRAMAnalyzerSettings::UpdateInterfacesFromSettings()
{
	mRASbChannelInterface->SetChannel( mRASbChannel );
	mCASbChannelInterface->SetChannel( mCASbChannel );
	mWbChannelInterface->SetChannel( mWbChannel );
	mOEbChannelInterface->SetChannel( mOEbChannel );

}

void DRAMAnalyzerSettings::LoadSettings( const char* settings )
{
	SimpleArchive text_archive;
	text_archive.SetString( settings );

	text_archive >> mRASbChannel;
	text_archive >> mCASbChannel;
	text_archive >> mWbChannel;
	text_archive >> mOEbChannel;

	ClearChannels();
	AddChannel( mRASbChannel, "Asynchoronous DRAM Analyzer", true );
	AddChannel( mCASbChannel, "Asynchoronous DRAM Analyzer", true );
	AddChannel( mWbChannel, "Asynchoronous DRAM Analyzer", true );
	AddChannel( mOEbChannel, "Asynchoronous DRAM Analyzer", true );

	UpdateInterfacesFromSettings();
}

const char* DRAMAnalyzerSettings::SaveSettings()
{
	SimpleArchive text_archive;

	text_archive << mRASbChannel;
	text_archive << mCASbChannel;
	text_archive << mWbChannel;
	text_archive << mOEbChannel;

	return SetReturnString( text_archive.GetString() );
}
