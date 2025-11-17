#include "StdAfx.h"
#include "ui/settings/settings_window.h"

using namespace Dewesoft::MUI;

void LatchMathSettingsWindow::bindEvents()
{
    // Procedure used to bind events to event handlers. Always use event()
    // wrapper.

    globalOffsetEdit.OnTextChanged += event(&LatchMathSettingsWindow::onGlobalOffsetEditChanged);    
}

void LatchMathSettingsWindow::updateProjectSettings(ProjectSettings& settings)
{
    // Procedure used to save and load settings into Dewesoft's settings file.

    settings.update("globalOffset", globalOffset);
}

void LatchMathSettingsWindow::initiate()
{
    // Procedure used to set up the UI. Called after the first call to
    // updateProjectSettings() so you can use the stored variables to correctly
    // set the UI.

    globalOffsetEdit.setText(std::to_string(globalOffset));
}

void LatchMathSettingsWindow::onGlobalOffsetEditChanged(TextBox& editBox, EventArgs& args)
{
    globalOffset = std::stol((std::wstring) globalOffsetEdit.getText());
}
