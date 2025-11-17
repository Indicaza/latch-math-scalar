#pragma once
#include "interface/macros.h"
#include "plugin.h"
#include "ui/setup/setup_window.h"
#include "ui/settings/settings_window.h"
#include "generated/ui/setup/plugin_setup_window.h"
#include "generated/ui/settings/plugin_settings_window.h"


BEGIN_NAMESPACE_PROCESSING
namespace Internal 
{
namespace NameMappings
{

using UserModuleClass = LatchMathModule;
using UserSharedModuleClass = LatchMathSharedModule;
using UserSetupWindowClass = LatchMathSetupWindow;
using UserSettingsWindowClass = LatchMathSettingsWindow;
using UserSetupWindowBaseClass = LatchMathSetupWindowBase;
using UserSettingsWindowBaseClass = LatchMathSettingsWindowBase;

} 
}
END_NAMESPACE_PROCESSING