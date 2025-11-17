#pragma once

#include "dcomlib/dcom_utils/plugin_types.h"
#include "generated/TypeLib_h.h"
#include "plugin_name_mappings.h"


std::wstring CLSIDtoWstring(const CLSID clsid);

/////////////////////////////////////////////////////////////////////////////
// Module

#define LIB_GUID_1(b) LIBID_ ## b ##Lib
#define LIB_GUID(b) LIB_GUID_1(b)

class ModuleBridge :
    public IDispatchImpl<IDewePlugin, &IID_IDewePlugin, &LIB_GUID(_LIB_NAME)>,
    public CComObjectRoot,
    public CComCoClass<ModuleBridge, &CLSID_Module>
{
  public:
    ModuleBridge();

    BEGIN_COM_MAP(ModuleBridge)
        COM_INTERFACE_ENTRY(IDewePlugin)
    END_COM_MAP()

    static std::wstring getGuid();
    static HRESULT WINAPI UpdateRegistry(_In_ BOOL bRegister) throw();

    STDMETHOD(raw_OnMessage)(long eventID, VARIANT inParam, VARIANT* outParam);

  private:
    IAppPtr dewesoftApp;
    IMathContextPtr dewesoftContext;

    std::unique_ptr<Dewesoft::Processing::Api::Advanced::Module> module;

    STDMETHODIMP VersionCheck(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP Init(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP MountChannels(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP MountDynamicChannels(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP GetInputChannels(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP GetInputUsed(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP AcceptInputChannel(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP GetInputGroups(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP BeforePreInitiate(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP Initiate(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP Calculate(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP Reset(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP DeInit(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP ClearCalc(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP ClearInstances(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP SetProps(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP GetProps(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP ReadProjectSettings(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP UpdateXML(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP UpdateCalibrationXML(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP EstablishConnections(VARIANT inParam, VARIANT* outParam);
	STDMETHODIMP GetCustomError(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP HandleAlarm(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP HandleOleMsg(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP MessageReceived(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP InitMarker(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP MarkerInputChanged(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP GetWarning(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP SampleRateChanged(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP GetDescription(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP ChannelRemoved(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP MarkerSetupChanged(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP GetCurrentPass(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP IsMultipass(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP PassStarted(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP PassCompleted(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP UpdateDisplayTemplate(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP GetSupportsCalibration(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP GetCalibrationStatus(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP GetCalibrationDescription(VARIANT inParam, VARIANT* outParam);

    bool usingSupportedDewesoftVersion(const PluginProperties& props) const;
    bool usingSupportedDCOMVersion(const PluginProperties& props) const;
    Dewesoft::Utils::Serialization::NodePtr parseUpdateXMLParameters(VARIANT inParam) const;
    bool contextResamplerMode();
};


class ContextBridge :
    public IDispatchImpl<IDewePlugin, &IID_IDewePlugin, &LIB_GUID(_LIB_NAME)>,
    public CComObjectRoot,
    public CComCoClass<ContextBridge, &CLSID_Context>
{
public:
    ContextBridge();

    BEGIN_COM_MAP(ContextBridge)
        COM_INTERFACE_ENTRY(IDewePlugin)
    END_COM_MAP()

    static std::wstring getGuid();
    static HRESULT WINAPI UpdateRegistry(_In_ BOOL bRegister) throw();

    STDMETHOD(raw_OnMessage)(long eventID, VARIANT inParam, VARIANT* outParam);

private:
    IAppPtr dewesoftApp;
    IMathContextPtr dewesoftContext;

    std::unique_ptr<Dewesoft::Processing::Api::Advanced::SharedModule> sharedModule;

    STDMETHODIMP VersionCheck(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP Init(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP GetInputChannels(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP GetInputUsed(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP MountChannels(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP MountDynamicChannels(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP GetInputGroups(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP BeforePreInitiate(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP Initiate(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP Calculate(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP Reset(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP DeInit(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP ClearCalc(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP ClearInstances(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP GetProps(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP ReadProjectSettings(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP UpdateXML(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP EstablishConnections(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP HandleAlarm(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP HandleOleMsg(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP MessageReceived(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP GetCustomError(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP GetWarning(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP SampleRateChanged(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP GetCurrentPass(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP IsMultipass(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP PassStarted(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP PassCompleted(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP UpdateDisplayTemplate(VARIANT inParam, VARIANT* outParam);

    bool contextResamplerMode();
};

class SetupFrameBridge :
	public IDispatchImpl<IDewePlugin, &IID_IDewePlugin, &LIB_GUID(_LIB_NAME)>,
    public CComObjectRoot,
    public CComCoClass<SetupFrameBridge, &CLSID_SetupFrame>
{
  public:
    SetupFrameBridge();

    BEGIN_COM_MAP(SetupFrameBridge)
        COM_INTERFACE_ENTRY(IDewePlugin)
    END_COM_MAP()

    static std::wstring getGuid();
    static HRESULT WINAPI UpdateRegistry(_In_ BOOL bRegister) throw();

    STDMETHOD(raw_OnMessage)(long eventID, VARIANT inParam, VARIANT* outParam);

  private:
	IAppPtr dewesoftApp;
	IMathFrameContextPtr dewesoftContext;

    std::unique_ptr<Dewesoft::Processing::Api::Advanced::SetupWindow> setupWindow;

    Dewesoft::Processing::Api::Advanced::Module* module;

    STDMETHODIMP onShowSetupMUI(Dewesoft::MUI::IWindow* window);
    STDMETHODIMP onHideSetupMUI();

    STDMETHODIMP VersionCheck(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP ShowFrame(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP HideFrame(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP SetFrame(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP InitFrame(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP SetContext(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP SetModule(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP SetObject(VARIANT inParam, VARIANT* outParam);
};


class SettingsFrameBridge :
	public IDispatchImpl<IDewePlugin, &IID_IDewePlugin, &LIB_GUID(_LIB_NAME)>,
    public CComObjectRoot,
    public CComCoClass<SettingsFrameBridge, &CLSID_SettingsFrame>
{
  public:
    SettingsFrameBridge();

    BEGIN_COM_MAP(SettingsFrameBridge)
        COM_INTERFACE_ENTRY(IDewePlugin)
    END_COM_MAP()

    static std::wstring getGuid();
    static HRESULT WINAPI UpdateRegistry(_In_ BOOL bRegister) throw();

    STDMETHOD(raw_OnMessage)(long eventID, VARIANT inParam, VARIANT* outParam);

  private:
    IAppPtr dewesoftApp;

    std::unique_ptr<Dewesoft::Processing::Api::Advanced::SettingsWindow> settingsWindow;

    STDMETHODIMP onShowSettingsMUI(Dewesoft::MUI::IWindow* window);
    STDMETHODIMP onHideSettingsMUI();

    STDMETHODIMP VersionCheck(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP ShowFrame(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP HideFrame(VARIANT inParam, VARIANT* outParam);
    STDMETHODIMP UpdateXML(VARIANT inParam, VARIANT* outParam);
};
