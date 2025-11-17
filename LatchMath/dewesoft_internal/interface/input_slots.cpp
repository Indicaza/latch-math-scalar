#pragma once
#include "stdafx.h"
#include "interface/input_slots.h"
#include "interface/plugin_base.h"

bool ChannelSlot::satisfiesAllConditions(const InputChannelBase& channel) const
{
    return isCorrectTimebase(channel) && isCorrectDatatype(channel) && acceptsChannel(channel) && isCorrectDimension(channel);
}

InputChannelBase ChannelSlot::getAssignedChannel() const
{
    IChannelPtr channel = (dewesoftContext->ObjectContext->CurrentContext)->GetInputChannels()->Item[(long) index];
    return InputChannelBase(channel);
}

void ChannelSlot::assignChannel(const std::wstring& channelName)
{
    IChannelPtr channel = dewesoftApp->Data->FindChannel(channelName.c_str());
    IChannelListExPtr(dewesoftContext->ObjectContext->CurrentContext->GetInputChannels())->SetCh((long) index, channel);
}

ChannelSlot& InputChannelSlots::addSlot(const std::string& name, InputChannelBase* channel, ChannelTimebase timebase)
{
    ChannelSlot conn;
    conn.name = name;
    conn.channelHolder = channel;
    conn.index = ownSlots.size();
    conn.dewesoftContext = dewesoftContext;
    conn.dewesoftApp = dewesoftApp;
    conn.used = true;
    if (timebase != ChannelTimebase::Any)
        conn.isCorrectTimebase = [timebase](const InputChannelBase& ch) { return (bool) (ch.timebase() & timebase); };
    ownSlots.push_back(conn);
    return ownSlots[ownSlots.size() - 1];
}

void InputChannelSlots::useSharedModuleChannel(const std::string& sharedModuleChannelSlotName, InputChannelBase* in)
{
    auto mathContext = (Dewesoft::Processing::Api::Advanced::SharedModule*) (dewesoftContext->MathContextPointer().byref);
    if (!mathContext)
        return;

    InputChannelSlots& sharedModuleConnections = mathContext->inputSlots;
    for (size_t i = 0; i < sharedModuleConnections.size(); ++i)
    {
        if (sharedModuleConnections[i].name == sharedModuleChannelSlotName)
        {
            externalSlots.push_back(sharedModuleConnections[i]);
            externalSlots.back().channelHolder = in;
            return;
        }
    }
}

size_t InputChannelSlots::size() const
{
    return ownSlots.size() + externalSlots.size();
}

ChannelSlot& InputChannelSlots::operator[](size_t i)
{
    if (i < ownSlots.size())
        return ownSlots.at(i);
    else
        return externalSlots.at(i - ownSlots.size());
}

ChannelSlot InputChannelSlots::operator[](size_t i) const
{
    if (i < ownSlots.size())
        return ownSlots.at(i);
    else
        return externalSlots.at(i - ownSlots.size());
}

void InputChannelSlots::applyConnections()
{
    if (!dewesoftContext)
        return;

    IChannelListPtr moduleChannels = dewesoftContext->GetInputChannels();
    size_t validConnectionCount = std::min<size_t>(moduleChannels->Count, ownSlots.size());
    for (size_t i = 0; i < validConnectionCount; ++i)
    {
        if (moduleChannels->Item[(long)i])
            ownSlots[i].channelHolder->setDewesoftChannel(moduleChannels->Item[(long)i]);
        else
            ownSlots[i].channelHolder->setDewesoftChannel(nullptr);
    }

    IChannelListPtr sharedModuleChannels = dewesoftContext->ObjectContext->CurrentContext->GetInputChannels();
    validConnectionCount = std::min<size_t>(sharedModuleChannels->Count, externalSlots.size());
    for (size_t i = 0; i < validConnectionCount; ++i)
    {
        if (sharedModuleChannels->Item[(long)i])
            externalSlots[i].channelHolder->setDewesoftChannel(sharedModuleChannels->Item[(long)i]);
        else
            externalSlots[i].channelHolder->setDewesoftChannel(nullptr);
    }
}

void InputChannelSlots::clearInstances()
{
    for (size_t i = 0; i < size(); ++i)
        operator[](i).channelHolder->clearDewesoftChannel();
}

void InputChannelSlots::clearChannel(const IChannelPtr& channel)
{
    for (size_t i = 0; i < size(); ++i)
    {
        const auto currentChannel = operator[](i).channelHolder->dewesoftChannel;
        if (channel == currentChannel)
            operator[](i).channelHolder->clearDewesoftChannel();
    }
}
