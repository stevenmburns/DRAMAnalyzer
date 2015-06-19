#ifndef DRAM_ANALYZER_SETTINGS
#define DRAM_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

class DRAMAnalyzerSettings : public AnalyzerSettings
{
public:
	DRAMAnalyzerSettings();
	virtual ~DRAMAnalyzerSettings();

	virtual bool SetSettingsFromInterfaces();
	void UpdateInterfacesFromSettings();
	virtual void LoadSettings( const char* settings );
	virtual const char* SaveSettings();

	
	Channel mInputChannel;
	U32 mBitRate;

protected:
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mInputChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceInteger >	mBitRateInterface;
};

#endif //DRAM_ANALYZER_SETTINGS
