#include "StdAfx.h"
#include "Resource.h"
#include <initguid.h>

#include "generated/TypeLib_h.h"
#include "interface/plugin_bridge.h"
#include "registration/plugin_reg.h"

ATL::CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
    OBJECT_ENTRY(CLSID_Module, ModuleBridge)
    OBJECT_ENTRY(CLSID_Context, ContextBridge)
    OBJECT_ENTRY(CLSID_SetupFrame, SetupFrameBridge)
    OBJECT_ENTRY(CLSID_SettingsFrame, SettingsFrameBridge)
END_OBJECT_MAP()

// Common Controls initialization helper
inline BOOL AtlInitCommonControls(DWORD dwFlags)
{
    INITCOMMONCONTROLSEX iccx = { sizeof(INITCOMMONCONTROLSEX), dwFlags };
    BOOL bRet = ::InitCommonControlsEx(&iccx);
    ATLASSERT(bRet);
    return bRet;
}

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        PluginRegistrar::hInst = hInstance;
        _Module.Init(ObjectMap, hInstance, &LIB_GUID(_LIB_NAME));
        AtlInitCommonControls(ICC_WIN95_CLASSES);
        DisableThreadLibraryCalls(hInstance);

        //DewesoftBridge::DLLModule = hInstance;
    }
    else if (dwReason == DLL_PROCESS_DETACH)
        _Module.Term();
    return TRUE;    // ok
}

STDAPI DllCanUnloadNow(void)
{
    return (_Module.GetLockCount() == 0) ? S_OK : S_FALSE;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

STDAPI DllRegisterServer(void)
{
    PluginRegistrar::moduleGuid = ModuleBridge::getGuid();
    PluginRegistrar::contextGuid = ContextBridge::getGuid();
    PluginRegistrar::setupFrameGuid = SetupFrameBridge::getGuid();
    PluginRegistrar::settingsFrameGuid = SettingsFrameBridge::getGuid();
	PluginRegistrar::perUser = _AtlRegisterPerUser;
	PluginRegistrar::registerPlugin();

	// registers object, typelib and all interfaces in typelib
	return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	PluginRegistrar reg;
	reg.moduleGuid = ModuleBridge::getGuid();

	reg.unregisterPlugin();

	return _Module.UnregisterServer(TRUE);
}

STDAPI DllInstall(BOOL bInstall, _In_opt_  LPCWSTR pszCmdLine)
{
	HRESULT hr = E_FAIL;
	static const wchar_t szUserSwitch[] = L"user";
	if (pszCmdLine != NULL)
	{
		if (_wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0)
		{
			ATL::AtlSetPerUserRegistration(true);
		}
	}
	if (bInstall)
	{
		hr = DllRegisterServer();
		if (FAILED(hr))
		{
			DllUnregisterServer();
		}
	}
	else
	{
		hr = DllUnregisterServer();
	}
	return hr;
}
