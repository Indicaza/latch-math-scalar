#pragma once
#include "interface/plugin_base.h"

enum edgeTypes
{
    RisingEdge = 0,
    FallingEdge = 1
};

class LatchMathModule : public Dewesoft::Processing::Api::Advanced::Module
{
public:
    static void getPluginProperties(PluginProperties& props);

    void connectInputChannels(InputChannelSlots& slots) override;
    void mountChannels(OutputChannels& fixed, OutputChannels& dynamic) override;
    void configure() override;
    void calculate() override;
    void updateSetup(Setup& setup) override;

    double criteriaLimit = 0;
    edgeTypes edgeType = RisingEdge;

    ScalarInputChannel inputChannelIn;
    ScalarInputChannel criteriaChannelIn;
    ScalarOutputChannel outputChannel;
};

class LatchMathSharedModule : public Dewesoft::Processing::Api::Advanced::SharedModule
{
public:
    void connectInputChannels(InputChannelSlots& slots) override;
    ScalarInputChannel criteriaChannelShared;
};
