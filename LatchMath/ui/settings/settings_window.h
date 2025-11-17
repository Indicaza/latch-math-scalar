#pragma once
#include "generated/ui/settings/plugin_settings_window.h"


class LatchMathSettingsWindow : public LatchMathSettingsWindowBase
{
  public:
    void bindEvents() override;
    void initiate() override;
    void updateProjectSettings(ProjectSettings& settings) override;

  private:
    int globalOffset;   

    void onGlobalOffsetEditChanged(Dewesoft::MUI::TextBox& editBox, Dewesoft::MUI::EventArgs& args);   
};