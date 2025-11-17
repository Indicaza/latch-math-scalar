#include "StdAfx.h"
#include <atlstr.h>
#include <atlconv.h>
#include <codecvt>
#include <locale>
#include "registration/plugin_reg.h"
#include "registration/plugin_props.h"
#include "plugin_name_mappings.h"

#define CONC(a, b) a##b
#define STRINGIFY_TEMP_W(s) CONC(L, #s)
#define STRINGIFY_W(s) STRINGIFY_TEMP_W(s)

#define MAKE_VERSION_DOT(major, minor, release) major.minor.release

std::wstring PluginRegistrar::name = STRINGIFY_W(_PLUGIN_DISPLAY_NAME);
std::wstring PluginRegistrar::vendor = STRINGIFY_W(_PLUGIN_VENDOR);
std::wstring PluginRegistrar::version = STRINGIFY_W(MAKE_VERSION_DOT(_PLUGIN_VERSION_MAJOR, _PLUGIN_VERSION_MINOR, _PLUGIN_VERSION_RELEASE));
std::wstring PluginRegistrar::description = STRINGIFY_W(_PLUGIN_DESCRIPTION);
std::wstring PluginRegistrar::icon = L"Icon";
std::wstring PluginRegistrar::keyName = STRINGIFY_W(_PLUGIN_NAME);
std::wstring PluginRegistrar::pluginRegPath;
std::wstring PluginRegistrar::regCOMPathModuleClass;
std::wstring PluginRegistrar::regCOMPathContextClass;
std::wstring PluginRegistrar::regCOMPathSetupFrameClass;
std::wstring PluginRegistrar::regCOMPathSettingsFrameClass;
std::wstring PluginRegistrar::moduleGuid;
std::wstring PluginRegistrar::contextGuid;
std::wstring PluginRegistrar::setupFrameGuid;
std::wstring PluginRegistrar::settingsFrameGuid;
bool PluginRegistrar::perUser = true;
HINSTANCE PluginRegistrar::hInst = nullptr;
HKEY PluginRegistrar::regRoot = nullptr;

inline std::wstring toWstring(const std::string& in)
{
#ifdef UNICODE
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(in);
#else
    return in;
#endif
}

inline std::string toString(const std::wstring& in)
{
    return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(in);
}

void PluginRegistrar::registerClass(const std::wstring& className, const TCHAR* classDescription)
{
    HKEY hKey;
    DWORD disposition;

    if (RegCreateKeyEx(regRoot, className.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &hKey, &disposition) == ERROR_SUCCESS)
    {
        RegSetValueEx(hKey, nullptr, 0, REG_SZ, (BYTE*)classDescription, (DWORD)_tcslen(classDescription) * sizeof(TCHAR));

        HKEY hSubKey;
        if (RegCreateKeyEx(hKey, _T("InprocServer32"), 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &hSubKey, &disposition) == ERROR_SUCCESS)
        {
            TCHAR moduleFileName[2048];
            GetModuleFileNameW(hInst, moduleFileName, 2048);

            RegSetValueEx(hSubKey, nullptr, 0, REG_SZ, (BYTE*)moduleFileName, (DWORD)_tcslen(moduleFileName) * sizeof(TCHAR));
            static std::wstring ThreadingModelApartment = _T("Apartment");
            RegSetValueEx(hSubKey, _T("ThreadingModel"), 0, REG_SZ, (BYTE*)ThreadingModelApartment.c_str(), (DWORD)_tcslen(ThreadingModelApartment.c_str()) * sizeof(TCHAR));

            RegCloseKey(hSubKey);
        }

        RegCloseKey(hKey);
    }
}

HRESULT PluginRegistrar::registerPlugin()
{
    HKEY hKey;
    DWORD disposition;

    init();

    if (RegCreateKeyEx(regRoot, pluginRegPath.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &hKey, &disposition) == ERROR_SUCCESS)
    {
        PluginProperties props;
        props.name = toString(name);
        props.vendor = toString(vendor);
        Dewesoft::Processing::Api::Advanced::Internal::NameMappings::UserModuleClass::getPluginProperties(props);
        if (props.shortName.empty())
            props.shortName = props.name;

        std::wstring name = toWstring(props.name);           const TCHAR* RegName = name.c_str();
        std::wstring shortName = toWstring(props.shortName); const TCHAR* RegShortName = shortName.c_str();
        std::wstring groupName = toWstring(props.groupName); const TCHAR* RegGroupName = groupName.c_str();
        std::wstring vendor = toWstring(props.vendor);       const TCHAR* RegVendor = vendor.c_str();
        const TCHAR* RegVersion = version.c_str();
        std::wstring description = toWstring(props.description);      const TCHAR* RegDescription = description.c_str();
        const TCHAR* RegIcon = icon.c_str();
        std::wstring dewesoftIcon = toWstring(props.icon); const TCHAR* RegDewesoftIcon = dewesoftIcon.c_str();
        const TCHAR* RegGuid = moduleGuid.c_str();
        const TCHAR* RegContextGuid = contextGuid.c_str();
        const TCHAR* RegSetupFrameGuid = setupFrameGuid.c_str();
        const TCHAR* RegSettingsFrameGuid = settingsFrameGuid.c_str();
        const int RegHasTabsheet = props.pluginType == PluginType::application;
        const int RegHasSettingsFrame = props.hasProjectSettings;
        const int RegInputSlotsMode = (int)props.inputSlotsMode;
        const int RegOutputChannelsMode = (int)props.outputChannelsMode;
        const int AddMathPermissions = (int)props.addMathPermissions;
        const int RegHasModuleTemplate = props.hasModuleTemplate;
        const int RegHasContextTemplate = props.hasContextTemplate;
        const int RegSupportsSetupTemplates = props.supportsSetupTemplates;

        RegSetValueEx(hKey, _T("Name"), 0, REG_SZ, (BYTE*)RegName, (DWORD)_tcslen(RegName) * sizeof(TCHAR));
        RegSetValueEx(hKey, _T("ShortName"), 0, REG_SZ, (BYTE*)RegShortName, (DWORD)_tcslen(RegShortName) * sizeof(TCHAR));
        if (!props.groupName.empty())
            RegSetValueEx(hKey, _T("GroupName"), 0, REG_SZ, (BYTE*)RegGroupName, (DWORD)_tcslen(RegGroupName) * sizeof(TCHAR));
        RegSetValueEx(hKey, _T("Vendor"), 0, REG_SZ, (BYTE*)RegVendor, (DWORD)_tcslen(RegVendor) * sizeof(TCHAR));
        RegSetValueEx(hKey, _T("Version"), 0, REG_SZ, (BYTE*)RegVersion, (DWORD)_tcslen(RegVersion) * sizeof(TCHAR));
        RegSetValueEx(hKey, _T("Description"), 0, REG_SZ, (BYTE*)RegDescription, (DWORD)_tcslen(RegDescription) * sizeof(TCHAR));
        RegSetValueEx(hKey, _T("GUID"), 0, REG_SZ, (BYTE*)RegGuid, (DWORD)_tcslen(RegGuid) * sizeof(TCHAR));
        RegSetValueEx(hKey, _T("ContextGUID"), 0, REG_SZ, (BYTE*)RegContextGuid, (DWORD)_tcslen(RegContextGuid) * sizeof(TCHAR));
        RegSetValueEx(hKey, _T("SetupFrameGUID"), 0, REG_SZ, (BYTE*)RegSetupFrameGuid, (DWORD)_tcslen(RegSetupFrameGuid) * sizeof(TCHAR));
        RegSetValueEx(hKey, _T("SettingsFrameGUID"), 0, REG_SZ, (BYTE*)RegSettingsFrameGuid, (DWORD)_tcslen(RegSettingsFrameGuid) * sizeof(TCHAR));
        if (props.icon.empty())
        {
            RegSetValueEx(hKey, _T("Icon"), 0, REG_SZ, (BYTE*)RegIcon, (DWORD)_tcslen(RegIcon) * sizeof(TCHAR));
            RegDeleteValue(hKey, _T("DewesoftIcon"));
        }
        else
        {
            RegDeleteValue(hKey, _T("Icon"));
            RegSetValueEx(hKey, _T("DewesoftIcon"), 0, REG_SZ, (BYTE*)RegDewesoftIcon, (DWORD)_tcslen(RegDewesoftIcon) * sizeof(TCHAR));
        }
        RegSetValueEx(hKey, _T("HasTabSheet"), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&RegHasTabsheet), sizeof(RegHasTabsheet));
        RegSetValueEx(hKey, _T("SupportsSetupTemplates"), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&RegSupportsSetupTemplates), sizeof(RegSupportsSetupTemplates));
        RegSetValueEx(hKey, _T("HasSettingsFrame"), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&RegHasSettingsFrame), sizeof(RegHasSettingsFrame));
        RegSetValueEx(hKey, _T("InputSlotsMode"), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&RegInputSlotsMode), sizeof(RegInputSlotsMode));
        RegSetValueEx(hKey, _T("OutputChannelsMode"), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&RegOutputChannelsMode), sizeof(RegOutputChannelsMode));
        RegSetValueEx(hKey, _T("AddMathPermissions"), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&AddMathPermissions), sizeof(AddMathPermissions));

#ifdef PROCESSING_MARKERS
        MarkerProperties markerProps;
        Dewesoft::Processing::Api::Advanced::Internal::NameMappings::UserModuleClass::getMarkerProperties(markerProps);

        const int RegIsMarker = props.pluginType == PluginType::marker;
        const int RegMarkerChannelType = (int)markerProps.supportedChannelType;
        const int RegMarkerShowSetupFrame = markerProps.showSetupFrame;
        std::wstring markerSupportedTypes = toWstring(markerProps.supportedTypesToString());
        const TCHAR* RegMarkerSupportedTypes = markerSupportedTypes.c_str();

        RegSetValueEx(hKey, _T("IsMarker"), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&RegIsMarker), sizeof(RegIsMarker));
        RegSetValueEx(hKey, _T("MarkerShowSetupFrame"), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&RegMarkerShowSetupFrame), sizeof(RegMarkerShowSetupFrame));
        RegSetValueEx(hKey, _T("MarkerSupportedChannelType"), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&RegMarkerChannelType), sizeof(RegMarkerChannelType));
        RegSetValueEx(hKey, _T("MarkerSupportedValueTypes"), 0, REG_SZ, (BYTE*)RegMarkerSupportedTypes, (DWORD)_tcslen(RegMarkerSupportedTypes) * sizeof(TCHAR));
#endif

        RegSetValueEx(hKey, _T("HasModuleTemplate"), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&RegHasModuleTemplate), sizeof(RegHasModuleTemplate));
        RegSetValueEx(hKey, _T("HasContextTemplate"), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&RegHasContextTemplate), sizeof(RegHasContextTemplate));
        
        // delete deprecated values we once set
        RegDeleteValue(hKey, _T("ModuleType"));

        RegCloseKey(hKey);
    }

    registerClass(regCOMPathModuleClass, _T("Module class"));
    registerClass(regCOMPathContextClass, _T("Context class"));
    registerClass(regCOMPathSetupFrameClass, _T("Setup frame class"));
    registerClass(regCOMPathSettingsFrameClass, _T("Settings frame class"));

    return ERROR_SUCCESS;
}

HRESULT PluginRegistrar::unregisterPlugin()
{
    init();

    RegDeleteKey(regRoot, pluginRegPath.c_str());
    RegDeleteKey(regRoot, regCOMPathModuleClass.c_str());
    RegDeleteKey(regRoot, regCOMPathContextClass.c_str());
    RegDeleteKey(regRoot, regCOMPathSetupFrameClass.c_str());
    RegDeleteKey(regRoot, regCOMPathSettingsFrameClass.c_str());

    return ERROR_SUCCESS;
}

void PluginRegistrar::init()
{
    pluginRegPath = _T("SOFTWARE\\Dewesoft\\Maths\\") + keyName;

    if (perUser)
        regRoot = HKEY_CURRENT_USER;
    else
        regRoot = HKEY_LOCAL_MACHINE;

    regCOMPathModuleClass = _T("SOFTWARE\\Classes\\CLSID\\") + moduleGuid;
    regCOMPathContextClass = _T("SOFTWARE\\Classes\\CLSID\\") + contextGuid;
    regCOMPathSetupFrameClass = _T("SOFTWARE\\Classes\\CLSID\\") + setupFrameGuid;
    regCOMPathSettingsFrameClass = _T("SOFTWARE\\Classes\\CLSID\\") + settingsFrameGuid;
}
