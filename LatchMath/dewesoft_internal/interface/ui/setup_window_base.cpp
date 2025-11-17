#include "StdAfx.h"
#include "interface/ui/setup_window_base.h"

using namespace Dewesoft::MUI;

BEGIN_NAMESPACE_PROCESSING


SetupWindow::SetupWindow()
  : updatingGUI(false)
{
}

void SetupWindow::connectComponents(){};

void SetupWindow::initiate(){};

void SetupWindow::bindEvents(){};

void SetupWindow::connectWithUI(Dewesoft::MUI::WindowPtr aui){};

void SetupWindow::assignModule(Module* m){};

void SetupWindow::initiateWithModule(Module* m)
{
    baseModule = m;
    baseSharedModule = m->getSharedModule();
    assignModule(m);
    updatingGUI = true;
    initiate();
    updatingGUI = false;
}

void SetupWindow::apply()
{
    VARIANT props;
    VariantInit(&props);
    props.vt = VT_BYREF;
    props.byref = (void *)baseModule;
    dewesoftContext->raw_Apply(props);
}

void SetupWindow::refreshDynamicChannelsFor(Module* m)
{
    unmountDynamicChannelsFor(m);

    OutputChannels sink;
    m->mountChannels(sink, m->dynamicOutputChannels);
    m->dynamicOutputChannels.mountChannels();
    m->getDewesoftContext()->SetDefaultChProps(VARIANT_TRUE);
}

void SetupWindow::refreshDynamicChannels()
{
    apply();
    refreshDynamicChannelsFor(baseSharedModule);
    for (auto& m : baseModule->getModules())
        refreshDynamicChannelsFor(m);
}

void SetupWindow::unmountDynamicChannelsFor(Module* m)
{
    std::vector<IChannel*> dynamicChannels;
    for (auto& mm : m->dynamicOutputChannels.mounts)
        dynamicChannels.push_back(mm.dewesoftChannel);

    m->dynamicOutputChannels.clearMounts();
    
    for (size_t i = 0; i < dynamicChannels.size(); ++i)
        if (dynamicChannels[i])
            m->getDewesoftContext()->UnmountChannel(&dynamicChannels[i]);
}

void SetupWindow::unmountDynamicChannels()
{
    unmountDynamicChannelsFor(baseSharedModule);
    for (auto& m : baseModule->getModules())
        unmountDynamicChannelsFor(m);
}

bool SetupWindow::isMyChannel(IChannelPtr channel, const std::vector<IChannelPtr>& outputChannelList) const
{
    return std::find(std::begin(outputChannelList), std::end(outputChannelList), channel) != std::end(outputChannelList);
}

void SetupWindow::buildOutputChannelListFor(IMathContextPtr dewesoftContext, std::vector<IChannelPtr>& outputChannelList) const
{
    auto outputs = dewesoftContext->OutputChannels;
    for (int i = 0; i < outputs->Count; ++i)
        outputChannelList.push_back(outputs->Item[i]);
}

std::vector<IChannelPtr> SetupWindow::buildOutputChannelList() const
{
    std::vector<IChannelPtr> channelList;
    buildOutputChannelListFor(baseSharedModule->getDewesoftContext(), channelList);
    for (auto& m : baseModule->getModules())
        buildOutputChannelListFor(m->getDewesoftContext(), channelList);
    return channelList;
}

std::vector<std::string> SetupWindow::availableChannelsFor(const ChannelSlot& slot)
{
    std::vector<std::string> result;
    baseModule->getDewesoftApp()->Data->BuildChannelList();
    auto channels = baseModule->getDewesoftApp()->Data->UsedChannels;

    std::vector<IChannelPtr> pluginOutputChannels = buildOutputChannelList();

    for (int i = 0; i < channels->Count; i++)
    {
        IChannelPtr channel = channels->GetItem(i);
        if (!isMyChannel(channel, pluginOutputChannels) && slot.satisfiesAllConditions(channel))
            result.push_back((std::string)channel->Name);
    }
    return result;
}

END_NAMESPACE_PROCESSING
