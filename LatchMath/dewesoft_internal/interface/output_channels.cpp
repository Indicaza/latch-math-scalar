#pragma once
#include "StdAfx.h"
#include "interface/output_channels.h"
#include "interface/input_channels.h"
#include "interface/enums.h"
#include "dcomlib/dcom_utils/variant_helper.h"
#include <cmath>


using namespace Dewesoft::Utils::Dcom::Utils;
using namespace Dewesoft::Processing::Api::Advanced;

void OutputChannels::setChannelTimebase(IChannelPtr& dewesoftChannel, ChannelTimebase timebase)
{
    switch (timebase)
    {
        case ChannelTimebase::Asynchronous:
            dewesoftChannel->SetAsync(true);
            dewesoftChannel->ExpectedAsyncRate = NAN;
            break;
        case ChannelTimebase::SingleValue:
            dewesoftChannel->SetIsSingleValue(true);
            break;
    }
}
void OutputChannels::setChannelDimensions(IChannelPtr& dewesoftChannel, const Dimensions& dimensions)
{
    if (!dimensions.empty())
    {
        dewesoftChannel->ArrayChannel = true;
        dewesoftChannel->ArrayInfo->DimCount = (long)dimensions.size();
        for (long d = 0; d < (long)dimensions.size(); ++d)
            dewesoftChannel->ArrayInfo->DimSizes[d] = dimensions[d];

        dewesoftChannel->ArrayInfo->Init();

        for (long d = 0; d < (long)dimensions.size(); ++d)
        {
            dewesoftChannel->ArrayInfo->AxisDef[d]->AxisType = atFloatLinearFunc;
            dewesoftChannel->ArrayInfo->AxisDef[d]->StartValue = 0;
            dewesoftChannel->ArrayInfo->AxisDef[d]->StepValue = 1;
        }
    }
}

IChannelPtr OutputChannels::mountOutputChannel(const std::string& name, int uniqueIndex, long datatype, ChannelTimebase timebase, const Dimensions& dimensions, long color)
{
    VARIANT_BOOL created;
    IChannelPtr dewesoftChannel = dewesoftContext->MountChannelEx(&created, uniqueIndex, std::to_string(uniqueIndex).c_str(), "", name.c_str(), color);
    if (datatype == (int) ChannelDataType::Text)
    {
        // Text
        dewesoftChannel->SetDataType((int) ChannelDataType::Binary);
        dewesoftChannel->MType = 5;
        dewesoftChannel->BinAvgSampleSize = 512;
    }
    else
    {
        dewesoftChannel->SetDataType(datatype);
    }
    setChannelTimebase(dewesoftChannel, timebase);
    setChannelDimensions(dewesoftChannel, dimensions);

    dewesoftChannel->Used = true;
    return dewesoftChannel;
}

void OutputChannels::applyMount(OutputChannelConnection& connection)
{
    connection.dewesoftChannel = mountOutputChannel(connection.name,
                                                    connection.uniqueIndex,
                                                    connection.datatype,
                                                    connection.timebase,
                                                    connection.dimensions,
                                                    (long)Dewesoft::Utils::Dcom::Utils::RGBtoBGR(connection.color));
    connection.channel->setDewesoftChannel(connection.dewesoftChannel);
    connection.channel->callInfo = &callInfo;
    connection.channel->outputTimebase = connection.channel->timebase();
}

void OutputChannels::mountChannels()
{
    for (auto& mount : mounts)
        applyMount(mount);
}

void OutputChannels::clearMounts()
{
    for (auto& mount : mounts)
    {
        mount.dewesoftChannel = nullptr;
        mount.channel->clearDewesoftChannel();
    }
    mounts.clear();
}

#undef DEFINE_MOUNT_CHANNEL

