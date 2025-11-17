#pragma once
#include "generated/ui/setup/plugin_setup_window.h"
#include "plugin.h"

class LatchMathSetupWindow : public LatchMathSetupWindowBase
{
public:
    virtual void bindEvents() override;
    virtual void initiate() override;

private:
    void onLatchCriteriaEditChanged(Dewesoft::MUI::TextBox& editBox, Dewesoft::MUI::EventArgs& args);

    void onEdgeTypeCBoxChanged(Dewesoft::MUI::ComboBox& cBox, Dewesoft::MUI::EventArgs& args);

    void onCriteriaChannelCBoxChanged(Dewesoft::MUI::ComboBox& cBox, Dewesoft::MUI::EventArgs& args);
};
