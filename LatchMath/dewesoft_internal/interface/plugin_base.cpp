#include "StdAfx.h"
#include <cassert>
#include "interface/plugin_base.h"

BEGIN_NAMESPACE_PROCESSING

double Core::acqSampleRate() const
{
    return dewesoftApp->MeasureSampleRateEx;
}

double Core::currentSampleRate() const
{
    if (dewesoftApp->IsSetupMode)
        return dewesoftApp->SetupSampleRate;
    else
        return dewesoftApp->MeasureSampleRateEx;
}

Module::Module()
    : dewesoftContext(nullptr)
    , dewesoftApp(nullptr)
{
    resampler.samplingRate = ResamplerSamplingRate::Synchronous;
    resampler.blockSizeType = ResamplerBlockSizeType::Constant;
    resampler.blockSizeInSamples = 1;
    resampler.pastSamplesRequiredForCalculation = 0;
    resampler.futureSamplesRequiredForCalculation = 0;
    resampler.resamplerBase = ResamplerBase::PerModule;
}

Module::~Module()
{
}

void Module::readProjectSettings(ProjectSettings& settings)
{
}

void Module::updateSetup(Setup& setup)
{
}

void Module::updateCalibration(Setup& setup)
{
}

void Module::afterLoadSetup()
{
}

void Module::connectInputChannels(InputChannelSlots& slots)
{
}

void Module::mountChannels(OutputChannels& fixed, OutputChannels& dynamic)
{
}

void Module::mountChannelGroups(ChannelGroups& groups)
{
}

std::string Module::getError() const
{
    return "";
}

std::string Module::getWarning() const
{
    return "";
}

std::string Module::getDescription() const
{
    return "";
}

void Module::updateDisplayTemplate(Dewesoft::Utils::Dcom::DisplayTemplate::DisplayTemplate& displayTemplate)
{
}

std::vector<std::wstring> Module::getSupportedCalibrations()
{
    return std::vector<std::wstring>();
}

CalibrationStatus Module::calibrationStatus()
{
    return CalibrationStatus::none();
}

CalibrationDescription Module::getCalibrationDescription(const std::wstring& id, CalibrationDescription desc)
{
    return CalibrationDescription{};
}

void Module::startControlChannelCalibration(const std::wstring& id)
{
    IF_COMPATIBLE_VERSION(calibrationUpgradeVersion, {
        _bstr_t bstr = SysAllocStringLen(id.data(), id.length());
        dewesoftContext->StartControlChannelCalibration(bstr);
    });
}

void Module::sampleRateChanged()
{
}

void Module::handleAlarm(const int alarmIndex, const bool alarmOn, const double alarmTime, const int trigIndex)
{
}

void Module::handleOleMsg(const int msg, const int param)
{
}

bool Module::handleMessage(const std::wstring& msgHeader, const std::wstring& msgData, std::wstring& msgOut)
{
    return false;
}

void Module::configureMarkerDrawGroups(MarkerDrawGroups& groups)
{
}

void Module::connectMarkerChannels(Marker& marker)
{
}

void Module::onMarkerInputChanged(int markerInputIndex)
{
}

void Module::setMarker(IProcessingMarkerPtr processingMarker)
{
    marker.setDCOMMarker(processingMarker);
    groups.setDCOMMarker(processingMarker);
}

std::wstring Module::getRequestedCalibration() const
{
    IF_COMPATIBLE_VERSION(calibrationUpgradeVersion, {
        _bstr_t bstr = dewesoftContext->GetRequestedCalibration();
        return (!bstr) ? L"" : static_cast<wchar_t*>(bstr);
    });
    return L"";
}

bool Module::isMultiPass() const
{
    return false;
}

void Module::passStarted()
{
}

void Module::passCompleted()
{
}

MultipassType Module::getCurrentPass() const
{
    return MultipassType::None;
}

void Module::configure()
{
}

void Module::start()
{
}

void Module::calculate()
{
}

void Module::clear()
{
}

void Module::stop()
{
}

void Module::copyProps(const Module* other)
{
}

SharedModule* Module::getSharedModule() const
{
    return (SharedModule*) dewesoftContext->MathContextPointer().byref;
}

size_t Module::getModuleCount() const
{
    return ((Module*) (dewesoftContext->MathContextPointer().byref))->getDewesoftContext()->SubItemCount;
}

Module* Module::getModule(size_t i) const
{
    return (Module*) dewesoftContext->MathModulePointer((long) i).byref;
}

std::vector<Module*> Module::getModules() const
{
    std::vector<Module*> modules(getModuleCount());
    for (size_t i = 0; i < modules.size(); ++i)
        modules[i] = getModule(i);
    return modules;
}

ChannelSlot Module::getInputSlot(const std::string& slotName) const
{
    for (size_t i = 0; i < inputSlots.size(); ++i)
        if (inputSlots[i].name == slotName)
            return inputSlots[i];
    assert(("Invalid slotName in getInputSlot() call. Re-check the slot names in connectInputChannels() function.", false));
    return ChannelSlot();
}

IMathContextPtr Module::getDewesoftContext()
{
    return dewesoftContext;
}

IMathContextPtr Module::getDewesoftContext() const
{
    return dewesoftContext;
}

IAppPtr Module::getDewesoftApp()
{
    return dewesoftApp;
}

IAppPtr Module::getDewesoftApp() const
{
    return dewesoftApp;
}

void Module::inputChannelsCountChanged()
{
    dewesoftContext->InputChannelsCountChanged();
}

END_NAMESPACE_PROCESSING
