#pragma once

#include "StdAfx.h"
#include <string>

class PluginRegistrar
{
public:
    static std::wstring pluginRegPath;
    static std::wstring regCOMPathModuleClass;
    static std::wstring regCOMPathContextClass;
    static std::wstring regCOMPathSetupFrameClass;
    static std::wstring regCOMPathSettingsFrameClass;
    static std::wstring keyName;
    static std::wstring name;
    static std::wstring version;
    static std::wstring vendor;
    static std::wstring description;
    static std::wstring moduleGuid;
    static std::wstring contextGuid;
    static std::wstring setupFrameGuid;
    static std::wstring settingsFrameGuid;
    static std::wstring icon;
    static bool perUser;
    static HINSTANCE hInst;
    static HKEY regRoot;

    static HRESULT registerPlugin();
    static HRESULT unregisterPlugin();
private:
    static void init();
    static void registerClass(const std::wstring& className, const TCHAR* classDescription);
};
