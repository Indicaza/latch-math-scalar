#include "StdAfx.h"
#include "plugin.h"

using namespace Dewesoft::Processing::Api::Advanced;

void LatchMathModule::getPluginProperties(PluginProperties& props)
{
    props.name = "Latch math - scalar";
    props.description = "Pro Tutorial example.";
    props.pluginType = PluginType::application;
    props.hasProjectSettings = false;
    props.inputSlotsMode = InputSlotsMode::multiple;
}

void LatchMathModule::connectInputChannels(InputChannelSlots& slots)
{
    slots.connectChannel("Input channel", &inputChannelIn, ChannelTimebase::Synchronous);
    slots.connectChannel("Criteria channel", &criteriaChannelIn, ChannelTimebase::Synchronous);
}

void LatchMathModule::mountChannels(OutputChannels& fixed, OutputChannels& dynamic)
{
    fixed.mountAsyncChannel("Latch", 0, &outputChannel);
}

void LatchMathModule::configure()
{
    resampler.blockSizeInSamples = 1;
    resampler.samplingRate = ResamplerSamplingRate::Synchronous;
    resampler.futureSamplesRequiredForCalculation = 1;

    outputChannel.setExpectedAsyncRate(5);
}

void LatchMathModule::calculate()
{
    float current = criteriaChannelIn.getScalar(0);
    float next = criteriaChannelIn.getScalar(1);

    bool rising = (current <= criteriaLimit && next >= criteriaLimit);
    bool falling = (current >= criteriaLimit && next <= criteriaLimit);

    if ((rising && edgeType == RisingEdge) || (falling && edgeType == FallingEdge))
    {
        float value = inputChannelIn.getScalar(1);
        outputChannel.addScalar(value, inputChannelIn.getTime(1));
    }
}

void LatchMathModule::updateSetup(Setup& setup)
{
    setup.update("criteriaLimit", criteriaLimit);
    setup.update("edgeType", (int&) edgeType);
}
