#pragma once
#include <string>
#include <vector>
#include <mui/ds_window.h>
#include <mui/controls.h>
#include <mui/layout.h>
#include "../plugin_base.h"
#include "../input_slots.h"
#include "../macros.h"

class SetupFrameBridge;

BEGIN_NAMESPACE_PROCESSING

class Module;
class SharedModule;

class SetupWindow : public Dewesoft::MUI::DSWindow
{
  public:
    SetupWindow();

    virtual void initiate();
    virtual void bindEvents();

  protected:
    void refreshDynamicChannels();
    void unmountDynamicChannels();
    std::vector<std::string> availableChannelsFor(const ChannelSlot& slot);
    void apply();

    template <typename TReturn, typename TInstance, typename... TArgs>
    auto mathEvent(TReturn(TInstance::*func)(TArgs... args))
    {
        return delegate<TReturn(TArgs...)>([func, this](TArgs... args2) -> TReturn
        {
            if (updatingGUI) return TReturn();

            TReturn r = (static_cast<TInstance* const>(this)->*func)(args2...);
            apply();
            return r;
        });
    }
    template <typename TInstance, typename... TArgs>
    auto mathEvent(void(TInstance::*func)(TArgs... args))
    {
        return delegate<void(TArgs...)>([func, this](TArgs... args2) -> void
        {
            if (updatingGUI) return;

            (static_cast<TInstance* const>(this)->*func)(args2...);
            apply();
        });
    }
    template <typename TReturn, typename TInstance, typename... TArgs>
    auto mathEvent(TReturn(TInstance::*func)(TArgs... args) const)
    {
        return delegate<TReturn(TArgs...)>([func, this](TArgs... args2) -> TReturn
        {
            if (updatingGUI) return TReturn();

            TReturn r = (static_cast<TInstance* const>(this)->*func)(args2...);
            apply();
            return r;
        });
    }
    template <typename TInstance, typename... TArgs>
    auto mathEvent(void(TInstance::*func)(TArgs... args) const)
    {
        return delegate<void(TArgs...)>([func, this](TArgs... args2) -> void
        {
            if (updatingGUI) return;

            (static_cast<TInstance* const>(this)->*func)(args2...);
            apply();
        });
    }

    template <typename T>
    void connect(T& component, const std::string& componentName)
    {
        component = T::Connect(ui, componentName);
    }


    bool updatingGUI;
    Dewesoft::MUI::WindowPtr ui;

  private:
    virtual void connectComponents();
    void initiateWithModule(Module* m);
    virtual void assignModule(Module* m);
    virtual void connectWithUI(Dewesoft::MUI::WindowPtr aui);
    void refreshDynamicChannelsFor(Module* p);
    void unmountDynamicChannelsFor(Module* p);
    std::vector<IChannelPtr> buildOutputChannelList() const;
    void buildOutputChannelListFor(IMathContextPtr dewesoftContext, std::vector<IChannelPtr>& outputChannelList) const;
    bool isMyChannel(IChannelPtr channel, const std::vector<IChannelPtr>& outputChannelList) const;

    Module* baseModule;
    SharedModule* baseSharedModule;
    IMathFrameContextPtr dewesoftContext;

    friend class SetupFrameBridge;
};

END_NAMESPACE_PROCESSING
