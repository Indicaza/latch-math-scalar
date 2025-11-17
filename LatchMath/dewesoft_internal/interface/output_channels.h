#pragma once
#include "interface/enums.h"
#include "dcomlib/dcom_utils/plugin_types.h"
#include "interface/macros.h"
#include "dcomlib/dcom_utils/colors.h"
#include "channels.h"
#include "output_channel.h"

BEGIN_NAMESPACE_PROCESSING class SetupWindow;
END_NAMESPACE_PROCESSING

class ChannelGroup
{
  public:
    void setProperty(const std::string& name, const std::string& value)
    {
        dewesoftGroup->Properties->Add(name.c_str(), value.c_str());
    };

    operator IInputGroupPtr()
    {
        return dewesoftGroup;
    };

  private:
    IInputGroupPtr dewesoftGroup;
    std::string name;

    friend class ChannelGroups;
};

class ChannelGroups
{
  public:
    ChannelGroup& mountGroup(const std::string& name, ChannelGroup* group = nullptr)
    {
        IInputGroupPtr dewesoftGroup = dewesoftContext->MountInputGroup(groupIndex(name));
        groups.emplace_back(ChannelGroup());
        groups.back().dewesoftGroup = dewesoftGroup;
        groups.back().name = name;
        dewesoftGroup->Name = name.c_str();
        *group = groups.back();
        return groups.back();
    }

  private:
    void setMetadata(IMathContextPtr dewesoftContext, IInputGroupsPtr inputGroups, bool allCh)
    {
        this->dewesoftContext = dewesoftContext;
        this->dewesoftGroups = inputGroups;
        this->allCh = allCh;
    }

    int groupIndex(const std::string& name)
    {
        for (size_t i = 0; i < groups.size(); ++i)
            if (groups[i].name == name)
                return static_cast<int>(i);
        return static_cast<int>(groups.size());
    }

    std::vector<ChannelGroup> groups;
    IMathContextPtr dewesoftContext;
    IInputGroupsPtr dewesoftGroups;
    bool allCh;

    friend class ModuleBridge;
    friend class ContextBridge;
};

struct OutputChannelConnection
{
    std::string name;
    int uniqueIndex;
    ChannelTimebase timebase;
    long datatype;
    Dimensions dimensions;
    Dewesoft::Utils::Dcom::Utils::Color color;

    OutputChannelBase* channel;
    IChannelPtr dewesoftChannel;
};

class OutputChannels
{
private:
    IMathContextPtr dewesoftContext;
    CallInfo* callInfo;

    std::vector<OutputChannelConnection> mounts;

    void setChannelTimebase(IChannelPtr& dewesoftChannel, ChannelTimebase timebase);
    void setChannelDimensions(IChannelPtr& dewesoftChannel, const Dimensions& dimensions);

    IChannelPtr mountOutputChannel(const std::string& name,
                                   int uniqueIndex,
                                   long datatype,
                                   ChannelTimebase timebase,
                                   const Dimensions& dimensions = {},
                                   long color = (long) Dewesoft::Utils::Dcom::Utils::Color::Lime);
    void applyMount(OutputChannelConnection& connection);

    void mountChannels();
    void clearMounts();

public:
    OutputChannelConnection& mountChannel(const std::string& name,
                                          int uniqueIndex,
                                          OutputChannelBase* channel,
                                          ChannelTimebase timebase = ChannelTimebase::SingleValue)
    {
        const Dimensions dimensions = [&]() -> Dimensions {
            switch (channel->channelDimension)
            {
                case ChannelDimension::Matrix:
                    return {1, 1};
                case ChannelDimension::Vector:
                    return {1};
                case ChannelDimension::Scalar:
                    return {};
                default:
                    return {};
            }
        }();
        OutputChannelConnection connection;
        connection.channel = channel;
        connection.name = name;
        connection.uniqueIndex = uniqueIndex;
        connection.timebase = timebase;
        connection.datatype = static_cast<long>(channel->getDataType());

        connection.dimensions = dimensions;
        connection.color = Dewesoft::Utils::Dcom::Utils::Color::Lime;
        mounts.push_back(connection);
        return mounts.back();
    }

    OutputChannelConnection& mountSyncChannel(const std::string& name, int uniqueIndex, OutputChannelBase* channel)
    {
        return mountChannel(name, uniqueIndex, channel, ChannelTimebase::Synchronous);
    }

    OutputChannelConnection& mountAsyncChannel(const std::string& name, int uniqueIndex, OutputChannelBase* channel)
    {
        return mountChannel(name, uniqueIndex, channel, ChannelTimebase::Asynchronous);
    }

    OutputChannelConnection& mountSingleValueChannel(const std::string& name, int uniqueIndex, OutputChannelBase* channel)
    {
        return mountChannel(name, uniqueIndex, channel, ChannelTimebase::SingleValue);
    }

    friend class ModuleBridge;
    friend class ContextBridge;
    friend class Dewesoft::Processing::Api::Advanced::SetupWindow;
};
