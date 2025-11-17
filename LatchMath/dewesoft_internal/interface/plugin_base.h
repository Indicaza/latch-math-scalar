#pragma once
#include <vector>
#include <string>
#include "dcomlib/dcom_display_template/display_template.h"
#include "interface/output_channels.h"
#include "registration/plugin_props.h"
#include "dcomlib/dcom_utils/plugin_types.h"
#include "interface/version.h"
#include "interface/input_slots.h"
#include "interface/macros.h"
#include "dcomlib/dcom_utils/dewesoft_dcom_node.h"

#ifdef PROCESSING_MARKERS
#include "interface/markers.h"
#else

class Marker
{
public:
    void setDCOMMarker(IProcessingMarkerPtr&){};
};

class MarkerDrawGroups
{
public:
    void setDCOMMarker(IProcessingMarkerPtr&){};
};

#endif

BEGIN_NAMESPACE_PROCESSING

class SetupWindow;
class SharedModule;

class Core
{
public:
    double acqSampleRate() const;
    double currentSampleRate() const;

private:
    IAppPtr dewesoftApp;

    friend class ModuleBridge;
    friend class ContextBridge;
};

#ifdef PROCESSING_MARKERS
struct MarkerSetupParams
{
    ComplexPresentation complexPresentation = cpDefault;
    bool interpolateBetweenData = true;
    bool findPeaks = true;
    bool useFFTPeaks = true;
    double peakSearchRange = 3;
};
#endif

class Module
{
public:
    using Setup = Dewesoft::Utils::Serialization::Node;
    using ProjectSettings = Dewesoft::Utils::Serialization::Node;

    Module();
    virtual ~Module();

    virtual void configure();
    virtual void start();
    virtual void calculate();
    virtual void clear();
    virtual void stop();

    virtual void copyProps(const Module* other);

    virtual void readProjectSettings(ProjectSettings& settings);
    virtual void updateSetup(Setup& setup);
    virtual void updateCalibration(Setup& setup);
    virtual void afterLoadSetup();

    virtual void connectInputChannels(InputChannelSlots& slots);
    virtual void mountChannels(OutputChannels& fixed, OutputChannels& dynamic);
    virtual void mountChannelGroups(ChannelGroups& groups);

    virtual std::string getError() const;
    virtual std::string getWarning() const;

    virtual std::string getDescription() const;

    virtual void updateDisplayTemplate(Dewesoft::Utils::Dcom::DisplayTemplate::DisplayTemplate& displayTemplate);

    virtual std::vector<std::wstring> getSupportedCalibrations();
    virtual CalibrationStatus calibrationStatus();
    virtual CalibrationDescription getCalibrationDescription(const std::wstring& id, CalibrationDescription desc);
    virtual void startControlChannelCalibration(const std::wstring& id);

    virtual void sampleRateChanged();

    virtual void handleAlarm(const int alarmIndex, const bool alarmOn, const double alarmTime, const int trigIndex);
    virtual void handleOleMsg(const int msg, const int param);
    virtual bool handleMessage(const std::wstring& msgHeader, const std::wstring& msgData, std::wstring& msgOut);

    virtual void connectMarkerChannels(Marker& marker);
    virtual void onMarkerInputChanged(int markerInputIndex);
    virtual void configureMarkerDrawGroups(MarkerDrawGroups& groups);

    virtual bool isMultiPass() const;
    virtual void passStarted();
    virtual void passCompleted();
    virtual MultipassType getCurrentPass() const;

	void inputChannelsCountChanged();

    void setMarker(IProcessingMarkerPtr processingMarker);

    std::wstring getRequestedCalibration() const;

    SharedModule* getSharedModule() const;
    size_t getModuleCount() const;
    Module* getModule(size_t i) const;
    std::vector<Module*> getModules() const;

    ChannelSlot getInputSlot(const std::string& slotName) const;

    IMathContextPtr getDewesoftContext();
    IMathContextPtr getDewesoftContext() const;
    IAppPtr getDewesoftApp();
    IAppPtr getDewesoftApp() const;

#ifdef PROCESSING_MARKERS
    const MarkerSetupParams& markerSetupParams = _markerSetupParams;
#endif

protected:
    Core core;
    Resampler resampler;
    CallInfo callInfo;
    InputChannelSlots inputSlots;
    Marker marker;
    MarkerDrawGroups groups;

#ifdef PROCESSING_MARKERS
    MarkerSetupParams _markerSetupParams;
#endif

private:
    IMathContextPtr dewesoftContext;
    IAppPtr dewesoftApp;
    OutputChannels fixedOutputChannels, dynamicOutputChannels;
    ChannelGroups channelGroups;
    DewesoftVersion currentVersion;

    friend class ModuleBridge;
    friend class ContextBridge;
    friend class InputChannelSlots;
    friend class SetupWindow;
};

class SharedModule : public Module
{
};

END_NAMESPACE_PROCESSING
