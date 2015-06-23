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

	
	Channel mRASbChannel;
	Channel mCASbChannel;
	Channel mWbChannel;
	Channel mOEbChannel;
	std::vector<Channel> mAddressChannel;
	std::vector<Channel> mDataChannel;

protected:
	std::shared_ptr<AnalyzerSettingInterfaceChannel>	mRASbChannelInterface;
	std::shared_ptr<AnalyzerSettingInterfaceChannel>	mCASbChannelInterface;
	std::shared_ptr<AnalyzerSettingInterfaceChannel>	mWbChannelInterface;
	std::shared_ptr<AnalyzerSettingInterfaceChannel>	mOEbChannelInterface;
	std::vector<std::shared_ptr<AnalyzerSettingInterfaceChannel> >	mAddressChannelInterface;
	std::vector<std::shared_ptr<AnalyzerSettingInterfaceChannel> >	mDataChannelInterface;

};

#endif //DRAM_ANALYZER_SETTINGS
