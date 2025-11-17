#pragma once
#include "dcomlib/dcom_utils/plugin_types.h"
#include "interface/macros.h"
#include <mui/ds_window.h>
#include <mui/controls.h>
#include <mui/layout.h>
#include "dcomlib/dcom_utils/dewesoft_dcom_node.h"

class SettingsFrameBridge;

BEGIN_NAMESPACE_PROCESSING

class SettingsWindow : public Dewesoft::MUI::DSWindow
{
  public:
    using ProjectSettings = Dewesoft::Utils::Serialization::Node;

    SettingsWindow();

    virtual void connectComponents();
    virtual void initiate();
    virtual void bindEvents();
    virtual void updateProjectSettings(ProjectSettings& settings);

  protected:
    template <typename T>
    void connect(T& component, const std::string& componentName)
    {
        component = T::Connect(ui, componentName);
    }

	
    bool updatingGUI;
    Dewesoft::MUI::WindowPtr ui;

  private:
    virtual void connectWithUI(Dewesoft::MUI::WindowPtr aui);

    friend class SettingsFrameBridge;
};

END_NAMESPACE_PROCESSING
