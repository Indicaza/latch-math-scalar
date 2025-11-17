#include "StdAfx.h"
#include "dcomlib/dcom_display_template/display_template.h"
#include "interface/plugin_bridge.h"
#include "Windows.h"
#include "interface/enums.h"
#include "dcomlib/dcom_utils/variant_helper.h"
#include "dcomlib/dcom_utils/dewesoft_dcom_node.h"
#include <propvarutil.h>
#include <cmath>
#include "plugin_name_mappings.h"

using namespace Dewesoft::MUI;
using namespace Dewesoft::Utils::Serialization;
using namespace Dewesoft::Utils::Dcom::Utils;

std::wstring CLSIDtoWstring(const CLSID clsid)
{
    LPOLESTR lpolestrguid;
    StringFromCLSID(clsid, (LPOLESTR*) &lpolestrguid);
    std::wstring g = lpolestrguid;
    CoTaskMemFree(lpolestrguid);
    return g;
}

ModuleBridge::ModuleBridge()
    : dewesoftApp(nullptr)
{
    module = std::make_unique<Dewesoft::Processing::Api::Advanced::Internal::NameMappings::UserModuleClass>();
}

// static
HRESULT WINAPI ModuleBridge::UpdateRegistry(_In_ BOOL bRegister) throw()
{
    return ERROR_SUCCESS;
}

// static
std::wstring ModuleBridge::getGuid()
{
    return CLSIDtoWstring(CLSID_Module);
}

STDMETHODIMP ModuleBridge::raw_OnMessage(long eventID, VARIANT inParam, VARIANT* outParam)
{
    switch (eventID)
    {
        case mthVersionCheck:
            return VersionCheck(inParam, outParam);
        case mthInit:
            return Init(inParam, outParam);
        case mthMountChannels:
            return MountChannels(inParam, outParam);
        case mthMountDynamicChannels:
            return MountDynamicChannels(inParam, outParam);
        case mthGetInputChannels:
            return GetInputChannels(inParam, outParam);
        case mthGetInputUsed:
            return GetInputUsed(inParam, outParam);
        case mthAcceptInputChannel:
            return AcceptInputChannel(inParam, outParam);
        case mthGetInputGroups:
            return GetInputGroups(inParam, outParam);
        case mthCalculate:
            return Calculate(inParam, outParam);
        case mthBeforePreInitiate:
            return BeforePreInitiate(inParam, outParam);
        case mthInitiate:
            return Initiate(inParam, outParam);
        case mthReset:
            return Reset(inParam, outParam);
        case mthDeinit:
            return DeInit(inParam, outParam);
        case mthClearCalc:
            return ClearCalc(inParam, outParam);
        case mthClearInstances:
            return ClearInstances(inParam, outParam);
        case mthSetProps:
            return SetProps(inParam, outParam);
        case mthGetProps:
            return GetProps(inParam, outParam);
        case mthReadProjectSettings:
            return ReadProjectSettings(inParam, outParam);
        case mthUpdateXML:
            return UpdateXML(inParam, outParam);
        case mthUpdateCalibrationXML:
            return UpdateCalibrationXML(inParam, outParam);
        case mthEstablishConnections:
            return EstablishConnections(inParam, outParam);
        case mthGetPointer:
            return GetProps(inParam, outParam);
        case mthHandleAlarm:
            return HandleAlarm(inParam, outParam);
        case mthHandleOleMsg:
            return HandleOleMsg(inParam, outParam);
        case mthMessageReceived:
            return MessageReceived(inParam, outParam);
        case mthInitMarker:
            return InitMarker(inParam, outParam);
        case mthMarkerInputChanged:
            return MarkerInputChanged(inParam, outParam);
        case mthGetCustomError:
            return GetCustomError(inParam, outParam);
        case mthGetWarning:
            return GetWarning(inParam, outParam);
        case mthSampleRateChanged:
            return SampleRateChanged(inParam, outParam);
        case mthGetDescription:
            return GetDescription(inParam, outParam);
        case mthChannelRemoved:
            return ChannelRemoved(inParam, outParam);
        case mthMarkerSetupChanged:
            return MarkerSetupChanged(inParam, outParam);
        case mthIsMultipass:
            return IsMultipass(inParam, outParam);
        case mthGetCurrentPass:
            return GetCurrentPass(inParam, outParam);
        case mthPassCompleted:
            return PassCompleted(inParam, outParam);
        case mthPassStarted:
            return PassStarted(inParam, outParam);
        case mthUpdateDisplayTemplate:
            return UpdateDisplayTemplate(inParam, outParam);
        case mthCalibrationStatus:
            return GetCalibrationStatus(inParam, outParam);
        case mthSupportsCalibration:
            return GetSupportsCalibration(inParam, outParam);
        case mthGetCalibrationDescription:
            return GetCalibrationDescription(inParam, outParam);
    }
    return S_OK;
}

STDMETHODIMP ModuleBridge::VersionCheck(VARIANT inParam, VARIANT* outParam)
{
    outParam->vt = VT_I4;
    outParam->lVal = 1;
    return S_OK;
}

bool ModuleBridge::usingSupportedDewesoftVersion(const PluginProperties& props) const
{
    long super, xVersion, spVersion, buildVersion;
    unsigned char releaseChannelId;

    dewesoftApp->GetDewesoftVersion(&super, &xVersion, &spVersion, &buildVersion, (char*) &releaseChannelId);

    std::tuple<long, long, long> dewesoftVersion(xVersion, spVersion, buildVersion);
    std::tuple<long, long, long> minSupportedDewesoftVersion(
        props.minSupportedDewesoftVersion.major, props.minSupportedDewesoftVersion.minor, props.minSupportedDewesoftVersion.patch);

    if (dewesoftVersion < minSupportedDewesoftVersion)
    {
        std::stringstream ss;
        ss << props.name << " needs at least DEWESoft version " << props.minSupportedDewesoftVersion.toString() << ".";
        dewesoftApp->WriteErrorMessage(ss.str().c_str());
        return false;
    }
    else
        return true;
}

bool ModuleBridge::usingSupportedDCOMVersion(const PluginProperties& props) const
{
    DewesoftVersion dewesoftVersion;
    dewesoftApp->GetInterfaceVersion(&dewesoftVersion.major, &dewesoftVersion.minor, &dewesoftVersion.dcomVersion);

    if (dewesoftVersion.dcomVersion < props.minSupportedDCOMVersion)
    {
        std::stringstream ss;
        ss << props.name << " needs at least DCOM version " << props.minSupportedDCOMVersion << ", try updating DEWESoft to the latest version.";
        dewesoftApp->WriteErrorMessage(ss.str().c_str());
        return false;
    }
    else
        return true;
}

STDMETHODIMP ModuleBridge::Init(VARIANT inParam, VARIANT* outParam)
{
    CComSafeArray<VARIANT> sa(inParam.parray);
    module->dewesoftApp = dewesoftApp = reinterpret_cast<IApp*>(sa[0].pdispVal);
    module->dewesoftContext = dewesoftContext = reinterpret_cast<IMathContext*>(sa[1].pdispVal);

    module->core.dewesoftApp = dewesoftApp;

    PluginProperties props;
    Dewesoft::Processing::Api::Advanced::Internal::NameMappings::UserModuleClass::getPluginProperties(props);

    return (usingSupportedDewesoftVersion(props) && usingSupportedDCOMVersion(props)) ? S_OK : E_ABORT;
}

bool ModuleBridge::contextResamplerMode()
{
    return module->getSharedModule()->resampler.resamplerBase == ResamplerBase::SharedModule;
}

STDMETHODIMP ModuleBridge::MountChannels(VARIANT inParam, VARIANT* outParam)
{
    module->fixedOutputChannels.dewesoftContext = module->dewesoftContext;
    module->dynamicOutputChannels.dewesoftContext = module->dewesoftContext;

    module->fixedOutputChannels.clearMounts();
    module->dynamicOutputChannels.clearMounts();
    OutputChannels sink;
    module->mountChannels(module->fixedOutputChannels, sink);
    module->fixedOutputChannels.mountChannels();
    return S_OK;
}

STDMETHODIMP ModuleBridge::MountDynamicChannels(VARIANT inParam, VARIANT* outParam)
{
    OutputChannels sink;
    // module.dynamicOutputChannels.clearMounts();
    module->mountChannels(sink, module->dynamicOutputChannels);
    module->dynamicOutputChannels.mountChannels();
    return S_OK;
}

STDMETHODIMP ModuleBridge::GetInputChannels(VARIANT inParam, VARIANT* outParam)
{
    module->inputSlots = InputChannelSlots();
    module->inputSlots.dewesoftContext = module->getDewesoftContext();
    module->inputSlots.dewesoftApp = module->getDewesoftApp();
    module->connectInputChannels(module->inputSlots);

    PluginProperties props;
    Dewesoft::Processing::Api::Advanced::Internal::NameMappings::UserModuleClass::getPluginProperties(props);
    switch (props.inputSlotsMode)
    {
        case InputSlotsMode::none:
            assert(module->inputSlots.ownSlots.size() == 0 &&
                   "For InputChannelsMode::none modules you mustn't connect any connectInputChannels()");
            break;
        case InputSlotsMode::single:
            assert(module->inputSlots.ownSlots.size() == 1 &&
                   "For singleInput modules you must connect exactly one input channel in connectInputChannels()");
            break;
        case InputSlotsMode::multiple:
            assert(module->inputSlots.ownSlots.size() > 1 &&
                   "For multiInput modules you must connect more than one input channel in connectInputChannels()");
            break;
    }

    std::vector<std::string> channelNames;
    if (props.inputSlotsMode == InputSlotsMode::multiple || props.inputSlotsMode == InputSlotsMode::dynamic)
        for (size_t i = 0; i < module->inputSlots.ownSlots.size(); ++i)
            channelNames.push_back(module->inputSlots.ownSlots[i].name);
    toVariant(channelNames, outParam);
    return S_OK;
}

STDMETHODIMP ModuleBridge::GetInputUsed(VARIANT inParam, VARIANT* outParam)
{
    PluginProperties props;
    Dewesoft::Processing::Api::Advanced::Internal::NameMappings::UserModuleClass::getPluginProperties(props);
    if (props.inputSlotsMode == InputSlotsMode::custom)
        toVariant(true, outParam);
    else
    {
        InputChannelSlots sink;
        sink.dewesoftApp = dewesoftApp;
        sink.dewesoftContext = dewesoftContext;
        module->connectInputChannels(sink);

        if (sink.size() <= size_t(inParam.ullVal))
            toVariant(false, outParam);
        else
            toVariant(sink[size_t(inParam.ullVal)].used, outParam);
    }
    return S_OK;
}

STDMETHODIMP ModuleBridge::AcceptInputChannel(VARIANT inParam, VARIANT* outParam)
{
    PluginProperties props;
    Dewesoft::Processing::Api::Advanced::Internal::NameMappings::UserModuleClass::getPluginProperties(props);
    if (props.inputSlotsMode == InputSlotsMode::custom)
        return S_OK;
    else
    {
        CComSafeArray<VARIANT> sa(inParam.parray);
        size_t channelIndex = sa[0].lVal;
        IChannelPtr channel = reinterpret_cast<IChannel*>(sa[1].pdispVal);

        if (channelIndex < module->inputSlots.ownSlots.size() && module->inputSlots.ownSlots[channelIndex].satisfiesAllConditions(channel))
            return S_OK;
        else
            return S_FALSE;
    }
}

STDMETHODIMP ModuleBridge::GetInputGroups(VARIANT inParam, VARIANT* outParam)
{
    CComSafeArray<VARIANT> sa(inParam.parray);
    IInputGroupsPtr inputGroups = reinterpret_cast<IInputGroups*>(sa[0].pdispVal);
    bool allCh = sa[1].boolVal;
    if (dewesoftContext)
    {
        module->channelGroups.setMetadata(dewesoftContext, inputGroups, allCh);
        module->mountChannelGroups(module->channelGroups);
    }
    return S_OK;
}

STDMETHODIMP ModuleBridge::Initiate(VARIANT inParam, VARIANT* outParam)
{
    module->start();

    CComSafeArray<VARIANT> sa(inParam.parray);
    for (LONG i = sa.GetLowerBound(0); i <= sa.GetUpperBound(0); ++i)
    {
        if (i >= (LONG) module->inputSlots.size() || !module->inputSlots[i].channelHolder)
            break;

        module->inputSlots[i].channelHolder->dewesoftResampledChannel = reinterpret_cast<IResamplerChannel*>(sa[i].pdispVal);
    }

    for (size_t i = 0; i < module->inputSlots.size(); ++i)
    {
        module->inputSlots[i].channelHolder->cacheChannelStructure();
        module->inputSlots[i].channelHolder->callInfo = &module->callInfo;
    }

    for (auto& channel : module->fixedOutputChannels.mounts)
        channel.channel->cacheChannelProperties();

    for (auto& channel : module->dynamicOutputChannels.mounts)
        channel.channel->cacheChannelProperties();

    module->resampler.getCppResamplerProperties();

    return S_OK;
}

STDMETHODIMP ModuleBridge::BeforePreInitiate(VARIANT inParam, VARIANT* outParam)
{
    DewesoftVersion currentVersion;
    dewesoftApp->GetInterfaceVersion(&currentVersion.major, &currentVersion.minor, &currentVersion.dcomVersion);
    module->currentVersion = currentVersion;
    module->resampler.currentVersion = currentVersion;
    bool useOldVersion = true;
    IF_COMPATIBLE_VERSION(genericChannelsUpgradeVersion, { useOldVersion = false; })
    if (!useOldVersion)
    {
        CComSafeArray<VARIANT> sa(inParam.parray);
        module->resampler.cppResampler = reinterpret_cast<ICppResamplerEngine*>(sa[0].pdispVal);
        module->resampler.setResamplerOldMode(useOldVersion);
    }

    module->inputSlots.applyConnections();

#ifdef PROCESSING_MARKERS
    MarkerProperties markerProps;
    Dewesoft::Processing::Api::Advanced::Internal::NameMappings::UserModuleClass::getMarkerProperties(markerProps);
    module->marker.disconnectMarkerInputs();
    module->marker.disconnectMarkerOutputs();
    module->connectMarkerChannels(module->marker);

    module->marker.setMarkerProps(markerProps);
    module->marker.setOwnerChannelType(markerProps.supportedChannelType);

    if (module->inputSlots.size() > 0 && module->inputSlots[0].channelHolder->DCOMChannel() != nullptr)
        module->marker.setOwnerChannel(*(module->inputSlots[0].channelHolder));

    module->groups.clearDrawGroups();
    module->configureMarkerDrawGroups(module->groups);
    module->groups.applyDrawGroups();
#endif

    module->configure();

#ifdef PROCESSING_MARKERS
    module->marker.applyMarkerObjects();
#endif

    CallInfo* callInfo = contextResamplerMode() ? &module->getSharedModule()->callInfo : &module->callInfo;
    module->fixedOutputChannels.callInfo = callInfo;
    module->dynamicOutputChannels.callInfo = callInfo;

    for (int i = 0; i < module->dewesoftContext->OutputChannels->Count; ++i)
    {
        const auto name = module->dewesoftContext->OutputChannels->Item[i]->Name;
        if (module->dewesoftContext->OutputChannels->Item[i]->Async)
            assert(!std::isnan(module->dewesoftContext->OutputChannels->Item[i]->ExpectedAsyncRate) &&
                   "You must set expectedAsyncRate of all async output channels in configure method.");
    }

    size_t binaryCnt = 0;
    Resampler* r = contextResamplerMode() ? &module->getSharedModule()->resampler : &module->resampler;
    for (size_t i = 0; i < module->inputSlots.size(); ++i)
    {
        if (module->inputSlots[i].channelHolder->dewesoftChannel && module->inputSlots[i].channelHolder->isBinaryChannel())
            binaryCnt++;

        module->inputSlots[i].channelHolder->resampler = r;
        module->inputSlots[i].channelHolder->configureChannelDataConverter();
    }

    if (binaryCnt > 0 && currentVersion.dcomVersion < binaryChannelsVersion.dcomVersion)
        dewesoftApp->WriteErrorMessage(L"Need at least Dewesoft X version 2021.2 to use binary input channels.");

    assert(binaryCnt <= 1 && "Only one binary channel is allowed as input.");
    assert(!(binaryCnt > 0 && module->resampler.samplingRate != ResamplerSamplingRate::AsynchronousSingleMaster) &&
           "You must set samplingRate to AsynchronousSingleMaster when using binary input channel.");
    if (useOldVersion)
    {
        SAFEARRAY* sa = SafeArrayCreateVector(VT_VARIANT, 0, 7);
        outParam->vt = VT_ARRAY | VT_VARIANT;
        outParam->parray = sa;

        toVariant(int(module->resampler.samplingRate), (static_cast<VARIANT*>(sa->pvData) + 0));
        toVariant(module->resampler.blockSizeInSamples, (static_cast<VARIANT*>(sa->pvData) + 1));
        toVariant(module->resampler.pastSamplesRequiredForCalculation, (static_cast<VARIANT*>(sa->pvData) + 2));
        toVariant(module->resampler.futureSamplesRequiredForCalculation, (static_cast<VARIANT*>(sa->pvData) + 3));
        toVariant(int(module->resampler.resamplerBase), (static_cast<VARIANT*>(sa->pvData) + 4));
        toVariant(int(module->resampler.blockSizeType), (static_cast<VARIANT*>(sa->pvData) + 5));
        if (module->resampler.masterChannel && *module->resampler.masterChannel)
            toVariant(module->resampler.masterChannel->dewesoftChannel, (static_cast<VARIANT*>(sa->pvData) + 6));
        else if (module->inputSlots.size() > 0)
            toVariant(module->inputSlots[0].channelHolder->dewesoftChannel, (static_cast<VARIANT*>(sa->pvData) + 6));
    }
    else
        module->resampler.setCppResamplerProperties();
    return S_OK;
}

STDMETHODIMP ModuleBridge::Calculate(VARIANT inParam, VARIANT* outParam)
{
    if (contextResamplerMode())
        module->callInfo = module->getSharedModule()->callInfo;
    else
    {
        CComSafeArray<VARIANT> sa(inParam.parray);
        module->callInfo.startBlockTime = sa[0].dblVal;
        module->callInfo.endBlockTime = sa[1].dblVal;
        module->callInfo.newSamplesCount = sa[2].intVal;

        module->resampler.blockSizeInSamples = module->callInfo.newSamplesCount;
    }
    for (size_t i = 0; i < module->inputSlots.size(); ++i)
        module->inputSlots[i].channelHolder->cacheResampledBufferLocations(module->currentVersion);

    for (auto& channel : module->fixedOutputChannels.mounts)
        channel.channel->cacheBufferLocation();

    for (auto& channel : module->dynamicOutputChannels.mounts)
        channel.channel->cacheBufferLocation();

    module->calculate();

    if (module->getCurrentPass() == MultipassType::None)
    {
        for (auto& channel : module->fixedOutputChannels.mounts)
            channel.channel->updateDBPos();

        for (auto& channel : module->dynamicOutputChannels.mounts)
            channel.channel->updateDBPos();
    }

    return S_OK;
}

STDMETHODIMP ModuleBridge::Reset(VARIANT inParam, VARIANT* outParam)
{
    module->clear();
    return S_OK;
}

STDMETHODIMP ModuleBridge::DeInit(VARIANT inParam, VARIANT* outParam)
{
    module->stop();
    return S_OK;
}

STDMETHODIMP ModuleBridge::ClearCalc(VARIANT inParam, VARIANT* outParam)
{
    module->stop();
    module->start();
    module->clear();
    return S_OK;
}

STDMETHODIMP ModuleBridge::ClearInstances(VARIANT inParam, VARIANT* outParam)
{
    module->fixedOutputChannels.clearMounts();
    module->dynamicOutputChannels.clearMounts();
    module->inputSlots.clearInstances();
    return S_OK;
}

STDMETHODIMP ModuleBridge::SetProps(VARIANT inParam, VARIANT* outParam)
{
    Dewesoft::Processing::Api::Advanced::Module* latestPlugin = (Dewesoft::Processing::Api::Advanced::Module*) inParam.byref;
    if (latestPlugin)
    {
        LibXmlNodePtr latestPluginSetup = std::make_shared<LibXmlNode>();
        latestPluginSetup->updateOperation(true);
        latestPlugin->updateSetup(*latestPluginSetup);
        latestPluginSetup->updateOperation(false);
        module->updateSetup(*latestPluginSetup);

        module->copyProps(latestPlugin);
    }
    return S_OK;
}

STDMETHODIMP ModuleBridge::GetProps(VARIANT inParam, VARIANT* outParam)
{
    outParam->vt = VT_BYREF;
    outParam->byref = (void*) (module.get());

    return S_OK;
}

Dewesoft::Utils::Serialization::NodePtr ModuleBridge::parseUpdateXMLParameters(VARIANT inParam) const
{
    CComSafeArray<VARIANT> sa(inParam.parray);
    IDwXMLDocumentPtr doc = reinterpret_cast<IDwXMLDocument*>(sa[0].pdispVal);
    IDwXMLNodePtr node = reinterpret_cast<IDwXMLNode*>(sa[1].pdispVal);
    bool write = (sa[2].boolVal == VARIANT_TRUE);

    if (!doc || !node)
        return nullptr;

    NodePtr xml = std::make_shared<DewesoftDCOMNode>(doc, node);
    xml->updateOperation(write);
    return xml;
}

STDMETHODIMP ModuleBridge::ReadProjectSettings(VARIANT inParam, VARIANT* outParam)
{
    NodePtr xml = parseUpdateXMLParameters(inParam);
    if (xml)
        module->readProjectSettings(*xml);

    return S_OK;
}

STDMETHODIMP ModuleBridge::UpdateXML(VARIANT inParam, VARIANT* outParam)
{
    NodePtr xml = parseUpdateXMLParameters(inParam);
    if (xml)
        module->updateSetup(*xml);

    return S_OK;
}

STDMETHODIMP_(HRESULT __stdcall) ModuleBridge::UpdateCalibrationXML(VARIANT inParam, VARIANT* outParam)
{
    NodePtr xml = parseUpdateXMLParameters(inParam);
    if (xml)
        module->updateCalibration(*xml);
    return S_OK;
}

STDMETHODIMP ModuleBridge::EstablishConnections(VARIANT inParam, VARIANT* outParam)
{
    module->inputSlots.applyConnections();
#ifdef PROCESSING_MARKERS
    MarkerProperties markerProps;
    Dewesoft::Processing::Api::Advanced::Internal::NameMappings::UserModuleClass::getMarkerProperties(markerProps);
    module->marker.disconnectMarkerInputs();
    module->marker.disconnectMarkerOutputs();
    module->connectMarkerChannels(module->marker);
    module->marker.applyMarkerObjects();

    module->marker.setMarkerProps(markerProps);
    module->marker.setOwnerChannelType(markerProps.supportedChannelType);
    if (module->inputSlots.size() > 0 && module->inputSlots[0].channelHolder->DCOMChannel() != nullptr)
        module->marker.setOwnerChannel(*(module->inputSlots[0].channelHolder));

    module->groups.clearDrawGroups();
    module->configureMarkerDrawGroups(module->groups);
    module->groups.applyDrawGroups();
#endif
    module->afterLoadSetup();

    Resampler* r = contextResamplerMode() ? &module->getSharedModule()->resampler : &module->resampler;
    r->useOldResamplerMode = false;
#ifdef PROCESSING_MARKERS
    r->samplingRate = ResamplerSamplingRate::SingleValue;
#endif
    for (size_t i = 0; i < module->inputSlots.size(); ++i)
    {
        module->inputSlots[i].channelHolder->resampler = r;
        module->inputSlots[i].channelHolder->configureChannelDataConverter();
    }

    CallInfo* callInfo = contextResamplerMode() ? &module->getSharedModule()->callInfo : &module->callInfo;
    module->fixedOutputChannels.callInfo = callInfo;
    module->dynamicOutputChannels.callInfo = callInfo;

    DewesoftVersion currentVersion;
    dewesoftApp->GetInterfaceVersion(&currentVersion.major, &currentVersion.minor, &currentVersion.dcomVersion);
    module->resampler.currentVersion = currentVersion;

    IF_COMPATIBLE_VERSION(basicSRDivSupport,
    {
        CComSafeArray<VARIANT> sa(inParam.parray);
        module->resampler.cppResampler = reinterpret_cast<ICppResamplerEngine*>(sa[0].pdispVal);
        module->resampler.setResamplerOldMode(false);
    });

    return S_OK;
}

STDMETHODIMP ModuleBridge::GetCustomError(VARIANT inParam, VARIANT* outParam)
{
    const std::string error = module->getError();
    if (!error.empty())
        toVariant(error, outParam);
    return S_OK;
}

STDMETHODIMP ModuleBridge::HandleAlarm(VARIANT inParam, VARIANT* outParam)
{
    CComSafeArray<VARIANT> sa(inParam.parray);
    int alarmIndex = sa[0].intVal;
    bool alarmOn = sa[1].boolVal;
    double alarmTime = sa[2].dblVal;
    int trigIndex = -1;
    if (sa.GetCount() > 3)
        trigIndex = sa[3].intVal;

    module->handleAlarm(alarmIndex, alarmOn, alarmTime, trigIndex);

    return S_OK;
}

STDMETHODIMP_(HRESULT __stdcall) ModuleBridge::HandleOleMsg(VARIANT inParam, VARIANT* outParam)
{
    CComSafeArray<VARIANT> sa(inParam.parray);
    int msg = sa[0].intVal;
    int param = sa[1].intVal;

    module->handleOleMsg(msg, param);

    return S_OK;
}

STDMETHODIMP ModuleBridge::MessageReceived(VARIANT inParam, VARIANT* outParam)
{
    CComSafeArray<VARIANT> saIn(inParam.parray);
    std::wstring msgHeader(saIn[0].bstrVal);
    std::wstring msgData(saIn[1].bstrVal);
    std::wstring msgOut(saIn[2].bstrVal);

    bool handled = module->handleMessage(msgHeader, msgData, msgOut);

    SAFEARRAY* saOut = SafeArrayCreateVector(VT_VARIANT, 0, 2);
    outParam->vt = VT_ARRAY | VT_VARIANT;
    outParam->parray = saOut;
    toVariant(handled, (static_cast<VARIANT*>(saOut->pvData) + 0));
    toVariant(std::string(msgOut.begin(), msgOut.end()), (static_cast<VARIANT*>(saOut->pvData) + 1));
    return S_OK;
}

STDMETHODIMP ModuleBridge::InitMarker(VARIANT inParam, VARIANT* outParam)
{
#ifdef PROCESSING_MARKERS
    CComSafeArray<VARIANT> sa(inParam.parray);
    IProcessingMarkerPtr marker = reinterpret_cast<IProcessingMarker*>(sa[0].pdispVal);

    module->setMarker(marker);
#endif

    return S_OK;
}

STDMETHODIMP_(HRESULT __stdcall) ModuleBridge::MarkerInputChanged(VARIANT inParam, VARIANT* outParam)
{
    CComSafeArray<VARIANT> sa(inParam.parray);
    int markerInputIndex = sa[0].intVal;

    module->onMarkerInputChanged(markerInputIndex);

    return S_OK;
}

STDMETHODIMP_(HRESULT __stdcall) ModuleBridge::MarkerSetupChanged(VARIANT inParam, VARIANT* outParam)
{
    CComSafeArray<VARIANT> sa(inParam.parray);

#ifdef PROCESSING_MARKERS
    module->_markerSetupParams.complexPresentation = sa[0].intVal < 7 ? ComplexPresentation(sa[0].intVal) : ComplexPresentation::cpRe;
    module->_markerSetupParams.findPeaks = sa[1].boolVal;
    module->_markerSetupParams.peakSearchRange = sa[2].dblVal;
    module->_markerSetupParams.useFFTPeaks = sa[3].boolVal;
    module->_markerSetupParams.interpolateBetweenData = !sa[4].boolVal;
#endif

    return S_OK;
}

STDMETHODIMP ModuleBridge::GetWarning(VARIANT inParam, VARIANT* outParam)
{
    const std::string warning = module->getWarning();
    if (!warning.empty())
        toVariant(warning, outParam);
    return S_OK;
}

STDMETHODIMP ModuleBridge::SampleRateChanged(VARIANT inParam, VARIANT* outParam)
{
    module->sampleRateChanged();

    return S_OK;
}

STDMETHODIMP ModuleBridge::GetDescription(VARIANT inParam, VARIANT* outParam)
{
    const std::string description = module->getDescription();
    if (!description.empty())
        toVariant(description, outParam);

    return S_OK;
}

STDMETHODIMP ModuleBridge::ChannelRemoved(VARIANT inParam, VARIANT* outParam)
{
    IChannelPtr channel = reinterpret_cast<IChannel*>(inParam.pdispVal);
    module->inputSlots.clearChannel(channel);
    return S_OK;
}

STDMETHODIMP ModuleBridge::GetCurrentPass(VARIANT inParam, VARIANT* outParam)
{
    toVariant(static_cast<long>(module->getCurrentPass()), outParam);
    return S_OK;
}

STDMETHODIMP ModuleBridge::IsMultipass(VARIANT inParam, VARIANT* outParam)
{
    toVariant(module->isMultiPass(), outParam);
    return S_OK;
}

STDMETHODIMP ModuleBridge::PassStarted(VARIANT inParam, VARIANT* outParam)
{
    module->passStarted();
    return S_OK;
}

STDMETHODIMP ModuleBridge::PassCompleted(VARIANT inParam, VARIANT* outParam)
{
    module->passCompleted();
    return S_OK;
}

STDMETHODIMP ModuleBridge::UpdateDisplayTemplate(VARIANT inParam, VARIANT* outParam)
{
    const IDisplayTemplatePtr displayTemplatePtr = reinterpret_cast<IDisplayTemplate*>(inParam.pdispVal);
    Dewesoft::Utils::Dcom::DisplayTemplate::DisplayTemplate displayTemplate =
        Dewesoft::Utils::Dcom::DisplayTemplate::DisplayTemplate(displayTemplatePtr);
    displayTemplate.clear();
    const auto moduleName = dewesoftContext->GetName();
    displayTemplate.setGroupName(std::string(moduleName));

    module->updateDisplayTemplate(displayTemplate);

    return S_OK;
}

STDMETHODIMP ModuleBridge::GetSupportsCalibration(VARIANT inParam, VARIANT* outParam)
{
    std::vector<std::wstring> strings = module->getSupportedCalibrations();
    SAFEARRAY* sa = SafeArrayCreateVector(VT_VARIANT, 0, (ULONG) strings.size());
    outParam->vt = VT_ARRAY | VT_VARIANT;
    outParam->parray = sa;
    for (size_t i = 0; i < strings.size(); ++i)
    {
        (static_cast<VARIANT*>(sa->pvData) + i)->vt = VT_BSTR;
        (static_cast<VARIANT*>(sa->pvData) + i)->bstrVal = SysAllocString(_bstr_t(strings[i].c_str()));
    }

    return S_OK;
}

STDMETHODIMP ModuleBridge::GetCalibrationStatus(VARIANT inParam, VARIANT* outParam)
{
    CalibrationStatus status = module->calibrationStatus();
    SAFEARRAY* sa = SafeArrayCreateVector(VT_VARIANT, 0, 3);
    outParam->vt = VT_ARRAY | VT_VARIANT;
    outParam->parray = sa;
    toVariant(static_cast<long>(status.status), (static_cast<VARIANT*>(sa->pvData) + 0));
    toVariant(status.percentDone, (static_cast<VARIANT*>(sa->pvData) + 1));
    toVariant(status.additionalInfo, (static_cast<VARIANT*>(sa->pvData) + 2));

    return S_OK;
}

STDMETHODIMP_(HRESULT __stdcall) ModuleBridge::GetCalibrationDescription(VARIANT inParam, VARIANT* outParam)
{
    CComSafeArray<VARIANT> saIn(inParam.parray);
    CalibrationDescription desc;

    std::wstring id = (!saIn[0].bstrVal) ? L"" : static_cast<wchar_t*>(saIn[0].bstrVal);
    desc.description = (!saIn[1].bstrVal) ? L"" : static_cast<wchar_t*>(saIn[1].bstrVal);
    desc.additionalCalibrationInfo = (!saIn[2].bstrVal) ? L"" : static_cast<wchar_t*>(saIn[2].bstrVal);
    desc.lastCalibrated = saIn[3].dblVal;

    desc = module->getCalibrationDescription(id, desc);
    SAFEARRAY* saOut = SafeArrayCreateVector(VT_VARIANT, 0, 5);
    outParam->vt = VT_ARRAY | VT_VARIANT;
    outParam->parray = saOut;

    (static_cast<VARIANT*>(saOut->pvData) + 0)->vt = VT_BSTR;
    (static_cast<VARIANT*>(saOut->pvData) + 0)->bstrVal = SysAllocString(_bstr_t(desc.description.c_str()));
    (static_cast<VARIANT*>(saOut->pvData) + 1)->vt = VT_BSTR;
    (static_cast<VARIANT*>(saOut->pvData) + 1)->bstrVal = SysAllocString(_bstr_t(desc.additionalCalibrationInfo.c_str()));
    (static_cast<VARIANT*>(saOut->pvData) + 2)->vt = VT_R8;
    (static_cast<VARIANT*>(saOut->pvData) + 2)->dblVal = desc.lastCalibrated;
    (static_cast<VARIANT*>(saOut->pvData) + 3)->vt = VT_BOOL;
    (static_cast<VARIANT*>(saOut->pvData) + 3)->boolVal = desc.mandatoryCalibration;
    (static_cast<VARIANT*>(saOut->pvData) + 4)->vt = VT_BSTR;
    (static_cast<VARIANT*>(saOut->pvData) + 4)->bstrVal = SysAllocString(_bstr_t(desc.displayName.c_str()));

    return S_OK;
}

ContextBridge::ContextBridge()
    : dewesoftApp(nullptr)
{
    sharedModule = std::make_unique<Dewesoft::Processing::Api::Advanced::Internal::NameMappings::UserSharedModuleClass>();
}

// static
HRESULT WINAPI ContextBridge::UpdateRegistry(_In_ BOOL bRegister) throw()
{
    return ERROR_SUCCESS;
}

// static
std::wstring ContextBridge::getGuid()
{
    return CLSIDtoWstring(CLSID_Context);
}

STDMETHODIMP ContextBridge::raw_OnMessage(long eventID, VARIANT inParam, VARIANT* outParam)
{
    switch (eventID)
    {
        case mthVersionCheck:
            return VersionCheck(inParam, outParam);
        case mthInit:
            return Init(inParam, outParam);
        case mthGetInputChannels:
            return GetInputChannels(inParam, outParam);
        case mthGetInputUsed:
            return GetInputUsed(inParam, outParam);
        case mthMountChannels:
            return MountChannels(inParam, outParam);
        case mthMountDynamicChannels:
            return MountDynamicChannels(inParam, outParam);
        case mthGetInputGroups:
            return GetInputGroups(inParam, outParam);
        case mthCalculate:
            return Calculate(inParam, outParam);
        case mthReset:
            return Reset(inParam, outParam);
        case mthDeinit:
            return DeInit(inParam, outParam);
        case mthClearCalc:
            return ClearCalc(inParam, outParam);
        case mthClearInstances:
            return ClearInstances(inParam, outParam);
        case mthBeforePreInitiate:
            return BeforePreInitiate(inParam, outParam);
        case mthInitiate:
            return Initiate(inParam, outParam);
        case mthUpdateXML:
            return UpdateXML(inParam, outParam);
        case mthEstablishConnections:
            return EstablishConnections(inParam, outParam);
        case mthGetProps:
            return GetProps(inParam, outParam);
        case mthGetPointer:
            return GetProps(inParam, outParam);
        case mthHandleAlarm:
            return HandleAlarm(inParam, outParam);
        case mthHandleOleMsg:
            return HandleOleMsg(inParam, outParam);
        case mthMessageReceived:
            return MessageReceived(inParam, outParam);
        case mthGetCustomError:
            return GetCustomError(inParam, outParam);
        case mthGetWarning:
            return GetWarning(inParam, outParam);
        case mthSampleRateChanged:
            return SampleRateChanged(inParam, outParam);
        case mthIsMultipass:
            return IsMultipass(inParam, outParam);
        case mthGetCurrentPass:
            return GetCurrentPass(inParam, outParam);
        case mthPassCompleted:
            return PassCompleted(inParam, outParam);
        case mthPassStarted:
            return PassStarted(inParam, outParam);
        case mthUpdateDisplayTemplate:
            return UpdateDisplayTemplate(inParam, outParam);
    }
    return S_OK;
}

STDMETHODIMP ContextBridge::VersionCheck(VARIANT inParam, VARIANT* outParam)
{
    outParam->vt = VT_I4;
    outParam->lVal = 1;
    return S_OK;
}

STDMETHODIMP ContextBridge::Init(VARIANT inParam, VARIANT* outParam)
{
    CComSafeArray<VARIANT> sa(inParam.parray);
    sharedModule->dewesoftApp = dewesoftApp = reinterpret_cast<IApp*>(sa[0].pdispVal);
    sharedModule->dewesoftContext = dewesoftContext = reinterpret_cast<IMathContext*>(sa[1].pdispVal);

    // TODO: do we really need this here?
    sharedModule->inputSlots = InputChannelSlots();
    sharedModule->inputSlots.dewesoftContext = dewesoftContext;
    sharedModule->inputSlots.dewesoftApp = dewesoftApp;
    sharedModule->connectInputChannels(sharedModule->inputSlots);

    sharedModule->core.dewesoftApp = dewesoftApp;

    return S_OK;
}

STDMETHODIMP ContextBridge::MountChannels(VARIANT inParam, VARIANT* outParam)
{
    sharedModule->fixedOutputChannels.dewesoftContext = sharedModule->dewesoftContext;
    sharedModule->dynamicOutputChannels.dewesoftContext = sharedModule->dewesoftContext;

    sharedModule->fixedOutputChannels.clearMounts();
    sharedModule->dynamicOutputChannels.clearMounts();
    OutputChannels sink;
    sharedModule->mountChannels(sharedModule->fixedOutputChannels, sink);
    sharedModule->fixedOutputChannels.mountChannels();
    return S_OK;
}

STDMETHODIMP ContextBridge::MountDynamicChannels(VARIANT inParam, VARIANT* outParam)
{
    OutputChannels sink;
    sharedModule->dynamicOutputChannels.clearMounts();
    sharedModule->mountChannels(sink, sharedModule->dynamicOutputChannels);
    sharedModule->dynamicOutputChannels.mountChannels();
    return S_OK;
}

STDMETHODIMP ContextBridge::GetInputChannels(VARIANT inParam, VARIANT* outParam)
{
    sharedModule->inputSlots = InputChannelSlots();
    sharedModule->inputSlots.dewesoftContext = sharedModule->getDewesoftContext();
    sharedModule->inputSlots.dewesoftApp = sharedModule->getDewesoftApp();
    sharedModule->connectInputChannels(sharedModule->inputSlots);

    std::vector<std::string> channelNames;
    for (size_t i = 0; i < sharedModule->inputSlots.size(); ++i)
        channelNames.push_back(sharedModule->inputSlots[i].name);
    toVariant(channelNames, outParam);
    return S_OK;
}

STDMETHODIMP ContextBridge::GetInputUsed(VARIANT inParam, VARIANT* outParam)
{
    PluginProperties props;
    Dewesoft::Processing::Api::Advanced::Internal::NameMappings::UserModuleClass::getPluginProperties(props);
    if (props.inputSlotsMode == InputSlotsMode::custom)
    {
        toVariant(true, outParam);
        return S_OK;
    }
    else
    {
        InputChannelSlots sink;
        sink.dewesoftApp = dewesoftApp;
        sink.dewesoftContext = dewesoftContext;
        sharedModule->connectInputChannels(sink);
        if (sink.size() <= size_t(inParam.ullVal))
            toVariant(false, outParam);
        else
            toVariant(sink[size_t(inParam.ullVal)].used, outParam);
        return S_OK;
    }
}

STDMETHODIMP ContextBridge::GetInputGroups(VARIANT inParam, VARIANT* outParam)
{
    return S_OK;
}

STDMETHODIMP ContextBridge::Initiate(VARIANT inParam, VARIANT* outParam)
{
    sharedModule->start();
    if (!contextResamplerMode())
        return S_OK;

    CComSafeArray<VARIANT> sa(inParam.parray);
    for (LONG i = sa.GetLowerBound(0); i <= sa.GetUpperBound(0); ++i)
    {
        if (i >= (LONG) sharedModule->inputSlots.size() || !sharedModule->inputSlots[i].channelHolder)
            break;

        sharedModule->inputSlots[i].channelHolder->dewesoftResampledChannel = reinterpret_cast<IResamplerChannel*>(sa[i].pdispVal);
    }

    for (size_t i = 0; i < sharedModule->inputSlots.size(); ++i)
    {
        sharedModule->inputSlots[i].channelHolder->cacheChannelStructure();
        sharedModule->inputSlots[i].channelHolder->callInfo = &sharedModule->callInfo;
        sharedModule->inputSlots[i].channelHolder->currentVersion = sharedModule->currentVersion;
    }

    sharedModule->resampler.getCppResamplerProperties();

    for (const auto& module : sharedModule->getModules())
    {
        for (size_t i = 0; i < module->inputSlots.size(); ++i)
        {
            module->inputSlots[i].channelHolder->cacheChannelStructure();
            module->inputSlots[i].channelHolder->currentVersion = module->currentVersion;
            module->inputSlots[i].channelHolder->callInfo = &module->callInfo;
        }
        module->resampler.getCppResamplerProperties();
    }

    for (auto& channel : sharedModule->fixedOutputChannels.mounts)
        channel.channel->cacheChannelProperties();

    for (auto& channel : sharedModule->dynamicOutputChannels.mounts)
        channel.channel->cacheChannelProperties();

    for (const auto& module : sharedModule->getModules())
    {
        for (auto& channel : module->fixedOutputChannels.mounts)
            channel.channel->cacheChannelProperties();

        for (auto& channel : module->dynamicOutputChannels.mounts)
            channel.channel->cacheChannelProperties();
    }

    return S_OK;
}

STDMETHODIMP ContextBridge::BeforePreInitiate(VARIANT inParam, VARIANT* outParam)
{
    DewesoftVersion currentVersion;
    dewesoftApp->GetInterfaceVersion(&currentVersion.major, &currentVersion.minor, &currentVersion.dcomVersion);
    sharedModule->resampler.currentVersion = currentVersion;

    bool useOldVersion = true;
    IF_COMPATIBLE_VERSION(genericChannelsUpgradeVersion, { useOldVersion = false; })
    if (!useOldVersion)
    {
        CComSafeArray<VARIANT> sa(inParam.parray);
        sharedModule->resampler.cppResampler = reinterpret_cast<ICppResamplerEngine*>(sa[0].pdispVal);
        sharedModule->resampler.setResamplerOldMode(useOldVersion);
    }

    sharedModule->inputSlots.applyConnections();
    sharedModule->configure();
    sharedModule->currentVersion = currentVersion;

    if (contextResamplerMode())
    {
        sharedModule->fixedOutputChannels.callInfo = &sharedModule->callInfo;
        sharedModule->dynamicOutputChannels.callInfo = &sharedModule->callInfo;
    }

    for (int i = 0; i < sharedModule->dewesoftContext->OutputChannels->Count; ++i)
        if (sharedModule->dewesoftContext->OutputChannels->Item[i]->Async)
            assert(!std::isnan(sharedModule->dewesoftContext->OutputChannels->Item[i]->ExpectedAsyncRate) &&
                   "You must set expectedAsyncRate of all async output channels in configure method.");

    for (size_t i = 0; i < sharedModule->inputSlots.size(); ++i)
    {
        sharedModule->inputSlots[i].channelHolder->resampler = &sharedModule->resampler;
        sharedModule->inputSlots[i].channelHolder->configureChannelDataConverter();

    }

    if (useOldVersion)
    {
        SAFEARRAY* sa = SafeArrayCreateVector(VT_VARIANT, 0, 7);
        outParam->vt = VT_ARRAY | VT_VARIANT;
        outParam->parray = sa;

        toVariant(int(sharedModule->resampler.samplingRate), (static_cast<VARIANT*>(sa->pvData) + 0));
        toVariant(sharedModule->resampler.blockSizeInSamples, (static_cast<VARIANT*>(sa->pvData) + 1));
        toVariant(sharedModule->resampler.pastSamplesRequiredForCalculation, (static_cast<VARIANT*>(sa->pvData) + 2));
        toVariant(sharedModule->resampler.futureSamplesRequiredForCalculation, (static_cast<VARIANT*>(sa->pvData) + 3));
        toVariant(int(sharedModule->resampler.resamplerBase), (static_cast<VARIANT*>(sa->pvData) + 4));
        toVariant(int(sharedModule->resampler.blockSizeType), (static_cast<VARIANT*>(sa->pvData) + 5));
        if (sharedModule->resampler.masterChannel && *sharedModule->resampler.masterChannel)
            toVariant(sharedModule->resampler.masterChannel->dewesoftChannel, (static_cast<VARIANT*>(sa->pvData) + 6));
        else if (sharedModule->inputSlots.size() > 0)
            toVariant(sharedModule->inputSlots[0].channelHolder->dewesoftChannel, (static_cast<VARIANT*>(sa->pvData) + 6));
    }
    else
        sharedModule->resampler.setCppResamplerProperties();
    return S_OK;
}

bool ContextBridge::contextResamplerMode()
{
    return sharedModule->resampler.resamplerBase == ResamplerBase::SharedModule;
}

STDMETHODIMP ContextBridge::Calculate(VARIANT inParam, VARIANT* outParam)
{
    if (contextResamplerMode())
    {
        CComSafeArray<VARIANT> sa(inParam.parray);
        sharedModule->callInfo.startBlockTime = sa[0].dblVal;
        sharedModule->callInfo.endBlockTime = sa[1].dblVal;
        sharedModule->callInfo.newSamplesCount = sa[2].intVal;

        sharedModule->resampler.blockSizeInSamples = sharedModule->callInfo.newSamplesCount;
    }
    for (size_t i = 0; i < sharedModule->inputSlots.size(); ++i)
        sharedModule->inputSlots[i].channelHolder->cacheResampledBufferLocations(sharedModule->currentVersion);

    if (contextResamplerMode())
    {
        for (auto& module : sharedModule->getModules())
        {
            for (size_t i = 0; i < module->inputSlots.size(); ++i)
                module->inputSlots[i].channelHolder->cacheResampledBufferLocations(sharedModule->currentVersion);

            for (auto& channel : module->fixedOutputChannels.mounts)
                channel.channel->cacheBufferLocation();

            for (auto& channel : module->dynamicOutputChannels.mounts)
                channel.channel->cacheBufferLocation();
        }
    }

    for (auto& channel : sharedModule->fixedOutputChannels.mounts)
        channel.channel->cacheBufferLocation();

    for (auto& channel : sharedModule->dynamicOutputChannels.mounts)
        channel.channel->cacheBufferLocation();

    sharedModule->calculate();

    for (auto& channel : sharedModule->fixedOutputChannels.mounts)
        channel.channel->updateDBPos();

    for (auto& channel : sharedModule->dynamicOutputChannels.mounts)
        channel.channel->updateDBPos();

    if (contextResamplerMode())
    {
        for (auto& module : sharedModule->getModules())
        {
            for (auto& channel : module->fixedOutputChannels.mounts)
                channel.channel->updateDBPos();

            for (auto& channel : module->dynamicOutputChannels.mounts)
                channel.channel->updateDBPos();
        }
    }
    return S_OK;
}

STDMETHODIMP ContextBridge::Reset(VARIANT inParam, VARIANT* outParam)
{
    sharedModule->clear();
    return S_OK;
}

STDMETHODIMP ContextBridge::DeInit(VARIANT inParam, VARIANT* outParam)
{
    sharedModule->stop();
    return S_OK;
}

STDMETHODIMP ContextBridge::ClearCalc(VARIANT inParam, VARIANT* outParam)
{
    sharedModule->stop();
    sharedModule->start();
    sharedModule->clear();
    return S_OK;
}

STDMETHODIMP ContextBridge::ClearInstances(VARIANT inParam, VARIANT* outParam)
{
    sharedModule->fixedOutputChannels.clearMounts();
    sharedModule->dynamicOutputChannels.clearMounts();
    sharedModule->inputSlots.clearInstances();
    return S_OK;
}

STDMETHODIMP ContextBridge::GetProps(VARIANT inParam, VARIANT* outParam)
{
    outParam->vt = VT_BYREF;
    outParam->byref = (void*) (sharedModule.get());

    return S_OK;
}

STDMETHODIMP ContextBridge::UpdateXML(VARIANT inParam, VARIANT* outParam)
{
    CComSafeArray<VARIANT> sa(inParam.parray);
    IDwXMLDocumentPtr doc = reinterpret_cast<IDwXMLDocument*>(sa[0].pdispVal);
    IDwXMLNodePtr node = reinterpret_cast<IDwXMLNode*>(sa[1].pdispVal);
    bool write = (sa[2].boolVal == VARIANT_TRUE);

    if (!doc || !node)
        return S_OK;

    NodePtr xml = std::make_shared<DewesoftDCOMNode>(doc, node);
    xml->updateOperation(write);
    sharedModule->updateSetup(*xml);

    return S_OK;
}

STDMETHODIMP ContextBridge::EstablishConnections(VARIANT inParam, VARIANT* outParam)
{
    sharedModule->inputSlots.applyConnections();
    sharedModule->afterLoadSetup();

    return S_OK;
}

STDMETHODIMP ContextBridge::HandleAlarm(VARIANT inParam, VARIANT* outParam)
{
    CComSafeArray<VARIANT> sa(inParam.parray);
    int alarmIndex = sa[0].intVal;
    bool alarmOn = sa[1].boolVal;
    double alarmTime = sa[2].dblVal;
    int trigIndex = -1;
    if (sa.GetCount() > 3)
        trigIndex = sa[3].intVal;

    sharedModule->handleAlarm(alarmIndex, alarmOn, alarmTime, trigIndex);

    return S_OK;
}

STDMETHODIMP_(HRESULT __stdcall) ContextBridge::HandleOleMsg(VARIANT inParam, VARIANT* outParam)
{
    CComSafeArray<VARIANT> sa(inParam.parray);
    int msg = sa[0].intVal;
    int param = sa[1].intVal;

    sharedModule->handleOleMsg(msg, param);

    return S_OK;
}

STDMETHODIMP ContextBridge::MessageReceived(VARIANT inParam, VARIANT* outParam)
{
    CComSafeArray<VARIANT> saIn(inParam.parray);
    std::wstring msgHeader(saIn[0].bstrVal);
    std::wstring msgData(saIn[1].bstrVal);
    std::wstring msgOut(saIn[2].bstrVal);

    bool handled = sharedModule->handleMessage(msgHeader, msgData, msgOut);

    SAFEARRAY* saOut = SafeArrayCreateVector(VT_VARIANT, 0, 2);
    outParam->vt = VT_ARRAY | VT_VARIANT;
    outParam->parray = saOut;
    toVariant(handled, (static_cast<VARIANT*>(saOut->pvData) + 0));
    toVariant(std::string(msgOut.begin(), msgOut.end()), (static_cast<VARIANT*>(saOut->pvData) + 1));
    return S_OK;
}

STDMETHODIMP ContextBridge::GetCustomError(VARIANT inParam, VARIANT* outParam)
{
    const std::string error = sharedModule->getError();
    if (!error.empty())
        toVariant(error, outParam);
    return S_OK;
}

STDMETHODIMP ContextBridge::GetWarning(VARIANT inParam, VARIANT* outParam)
{
    const std::string warning = sharedModule->getWarning();
    if (!warning.empty())
        toVariant(warning, outParam);
    return S_OK;
}

STDMETHODIMP ContextBridge::SampleRateChanged(VARIANT inParam, VARIANT* outParam)
{
    sharedModule->sampleRateChanged();

    return S_OK;
}


STDMETHODIMP ContextBridge::GetCurrentPass(VARIANT inParam, VARIANT* outParam)
{
    toVariant(static_cast<long>(sharedModule->getCurrentPass()), outParam);
    return S_OK;
}

STDMETHODIMP ContextBridge::IsMultipass(VARIANT inParam, VARIANT* outParam)
{
    toVariant(sharedModule->isMultiPass(), outParam);
    return S_OK;
}

STDMETHODIMP ContextBridge::PassStarted(VARIANT inParam, VARIANT* outParam)
{
    sharedModule->passStarted();
    return S_OK;
}

STDMETHODIMP ContextBridge::PassCompleted(VARIANT inParam, VARIANT* outParam)
{
    sharedModule->passCompleted();
    return S_OK;
}


STDMETHODIMP ContextBridge::UpdateDisplayTemplate(VARIANT inParam, VARIANT* outParam)
{
    const IDisplayTemplatePtr displayTemplatePtr = reinterpret_cast<IDisplayTemplate*>(inParam.pdispVal);
    Dewesoft::Utils::Dcom::DisplayTemplate::DisplayTemplate displayTemplate =
        Dewesoft::Utils::Dcom::DisplayTemplate::DisplayTemplate(displayTemplatePtr);
    displayTemplate.clear();
    const auto moduleName = dewesoftContext->GetName();
    displayTemplate.setGroupName(std::string(moduleName));

    sharedModule->updateDisplayTemplate(displayTemplate);

    return S_OK;
}

SetupFrameBridge::SetupFrameBridge()
{
}

// static
std::wstring SetupFrameBridge::getGuid()
{
    return CLSIDtoWstring(CLSID_SetupFrame);
}

// static
HRESULT WINAPI SetupFrameBridge::UpdateRegistry(_In_ BOOL bRegister) throw()
{
    return ERROR_SUCCESS;
}

STDMETHODIMP SetupFrameBridge::raw_OnMessage(long eventID, VARIANT inParam, VARIANT* outParam)
{
    switch (eventID)
    {
        case mfrVersionCheck:
            return VersionCheck(inParam, outParam);
        case mfrShowFrame:
            return ShowFrame(inParam, outParam);
        case mfrHideFrame:
            return HideFrame(inParam, outParam);
        case mfrSetFrame:
            return SetFrame(inParam, outParam);
        case mfrInitFrame:
            return InitFrame(inParam, outParam);
        case mfrSetContext:
            return SetContext(inParam, outParam);
        case mfrSetModule:
            return SetModule(inParam, outParam);
        case mfrSetObject:
            return SetObject(inParam, outParam);
            // mfrResizeFrame:    Result := ResizeFrame(InParam, OutParam);
    }
    return S_OK;
}

STDMETHODIMP SetupFrameBridge::VersionCheck(VARIANT inParam, VARIANT* outParam)
{
    outParam->vt = VT_I4;
    outParam->lVal = 1;
    return S_OK;
}

STDMETHODIMP SetupFrameBridge::ShowFrame(VARIANT inParam, VARIANT* outParam)
{
    // Set GUI based on stored params
    CComSafeArray<VARIANT> sa(inParam.parray);
    IWindow* win = reinterpret_cast<IWindow*>(sa[1].punkVal);
    onShowSetupMUI(win);
    setupWindow->initiateWithModule(module);
    return S_OK;
}

STDMETHODIMP SetupFrameBridge::HideFrame(VARIANT inParam, VARIANT* outParam)
{
    onHideSetupMUI();
    return S_OK;
}

STDMETHODIMP SetupFrameBridge::SetFrame(VARIANT inParam, VARIANT* outParam)
{
    module = (Dewesoft::Processing::Api::Advanced::Module*) inParam.byref;
    if (setupWindow)
        setupWindow->initiateWithModule(module);
    return S_OK;
}

STDMETHODIMP SetupFrameBridge::InitFrame(VARIANT inParam, VARIANT* outParam)
{
    CComSafeArray<VARIANT> sa(inParam.parray);
    dewesoftApp = reinterpret_cast<IApp*>(sa[0].pdispVal);
    return S_OK;
}

STDMETHODIMP SetupFrameBridge::SetContext(VARIANT inParam, VARIANT* outParam)
{
    CComSafeArray<VARIANT> sa(inParam.parray);
    dewesoftContext = reinterpret_cast<IMathFrameContext*>(sa[0].pdispVal);
    return S_OK;
}

STDMETHODIMP SetupFrameBridge::SetModule(VARIANT inParam, VARIANT* outParam)
{
    Dewesoft::Processing::Api::Advanced::Module* newModule = (Dewesoft::Processing::Api::Advanced::Module*) inParam.byref;
    if (!newModule || newModule == module)
        return S_OK;

    module = newModule;
    if (setupWindow)
        setupWindow->initiateWithModule(module);
    return S_OK;
}

STDMETHODIMP SetupFrameBridge::SetObject(VARIANT inParam, VARIANT* outParam)
{
    Dewesoft::Processing::Api::Advanced::Module* newModule = (Dewesoft::Processing::Api::Advanced::Module*) inParam.byref;
    if (!newModule)
        return S_OK;

    module = newModule;
    if (setupWindow)
        setupWindow->initiateWithModule(module);
    return S_OK;
}

STDMETHODIMP SetupFrameBridge::onShowSetupMUI(IWindow* window)
{
    try
    {
        setupWindow = std::make_unique<Dewesoft::Processing::Api::Advanced::Internal::NameMappings::UserSetupWindowClass>();
        setupWindow->connectWithUI(window);
        setupWindow->dewesoftContext = dewesoftContext;
        setupWindow->connectComponents();
        setupWindow->bindEvents();
        if (module)
            setupWindow->initiateWithModule(module);
    }
    catch (const std::exception& e)
    {
        std::string msg = e.what();
        dewesoftApp->WriteErrorMessage(e.what());
        return E_FAIL;
    }
    return S_OK;
}

STDMETHODIMP SetupFrameBridge::onHideSetupMUI()
{
    setupWindow.reset(nullptr);
    return S_OK;
}

SettingsFrameBridge::SettingsFrameBridge()
{
}

// static
std::wstring SettingsFrameBridge::getGuid()
{
    return CLSIDtoWstring(CLSID_SettingsFrame);
}

// static
HRESULT WINAPI SettingsFrameBridge::UpdateRegistry(_In_ BOOL bRegister) throw()
{
    return ERROR_SUCCESS;
}

STDMETHODIMP SettingsFrameBridge::raw_OnMessage(long eventID, VARIANT inParam, VARIANT* outParam)
{
    switch (eventID)
    {
        case mfrVersionCheck:
            return VersionCheck(inParam, outParam);
        case mfrShowFrame:
            return ShowFrame(inParam, outParam);
        case mfrUpdateXML:
            return UpdateXML(inParam, outParam);
        case mfrHideFrame:
            return HideFrame(inParam, outParam);
    }
    return S_OK;
}

STDMETHODIMP SettingsFrameBridge::VersionCheck(VARIANT inParam, VARIANT* outParam)
{
    outParam->vt = VT_I4;
    outParam->lVal = 1;
    return S_OK;
}

STDMETHODIMP SettingsFrameBridge::ShowFrame(VARIANT inParam, VARIANT* outParam)
{
    // Set GUI based on stored params
    CComSafeArray<VARIANT> sa(inParam.parray);
    IWindow* win = reinterpret_cast<IWindow*>(sa[1].punkVal);
    onShowSettingsMUI(win);
    return S_OK;
}

STDMETHODIMP SettingsFrameBridge::HideFrame(VARIANT inParam, VARIANT* outParam)
{
    onHideSettingsMUI();
    return S_OK;
}

STDMETHODIMP SettingsFrameBridge::onShowSettingsMUI(IWindow* window)
{
    try
    {
        settingsWindow = std::make_unique<Dewesoft::Processing::Api::Advanced::Internal::NameMappings::UserSettingsWindowClass>();
        settingsWindow->connectWithUI(window);
        settingsWindow->connectComponents();
        settingsWindow->bindEvents();
    }
    catch (const std::exception& e)
    {
        std::string msg = e.what();
        dewesoftApp->WriteErrorMessage(e.what());
        return E_FAIL;
    }
    return S_OK;
}

STDMETHODIMP SettingsFrameBridge::onHideSettingsMUI()
{
    settingsWindow.reset(nullptr);
    return S_OK;
}

STDMETHODIMP SettingsFrameBridge::UpdateXML(VARIANT inParam, VARIANT* outParam)
{
    if (!settingsWindow)
        return S_OK;

    CComSafeArray<VARIANT> sa(inParam.parray);
    IDwXMLDocumentPtr doc = reinterpret_cast<IDwXMLDocument*>(sa[0].pdispVal);
    IDwXMLNodePtr node = reinterpret_cast<IDwXMLNode*>(sa[1].pdispVal);
    bool write = (sa[2].boolVal == VARIANT_TRUE);

    if (!doc || !node)
        return S_OK;

    NodePtr xml = std::make_shared<DewesoftDCOMNode>(doc, node);
    xml->updateOperation(write);
    settingsWindow->updateProjectSettings(*xml);
    if (!write)
        settingsWindow->initiate();

    return S_OK;
}
