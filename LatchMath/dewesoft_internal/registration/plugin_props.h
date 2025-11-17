#pragma once
#include "StdAfx.h"
#include <string>
#include <numeric>
#include "interface/enums.h"

enum class InputSlotsMode
{
    none = 0,
    single = 1,
    multiple = 2,
    custom = 3,
    dynamic = 4
};

enum class OutputChannelsMode
{
    none = 0,
    default = 1,
    custom = 2
};

enum class PluginType
{
    math = 0,
    application = 1,
    marker = 2
};


enum class AddMathPermissions
{
    amNever = 0,
    amAnalysisOnly = 1,
    amAnalysisAndMeasure = 2,
    amAnalysisMeasureAndStore = 3
};


struct Version
{
    int major, minor, patch;
    std::string toString(const std::string& prefix = "v") const
    {
        return prefix + std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
    }
};

struct MarkerProperties
{
    struct SupportedSetupParams
    {
        bool interpolateBetweenData = false;
        bool findPeaks = false;
        bool useFFTPeaks = false;
    };

    bool showSetupFrame = false;
    std::vector<MarkerValueType> supportedValueTypes;
    MarkerChannelType supportedChannelType;
    bool updateInputsAfterMove = false;
    SupportedSetupParams supportedSetupParams;

    std::string supportedTypesToString() const
    {
        std::string delimiter = " ";
        return std::accumulate(
            std::begin(supportedValueTypes), std::end(supportedValueTypes), std::string(), [](std::string& ss, MarkerValueType val) {
                std::string s = std::to_string(static_cast<int>(val));
                return ss.empty() ? s : ss + " " + s;
            });
    }
};

struct PluginProperties
{
    InputSlotsMode inputSlotsMode = InputSlotsMode::single;
    OutputChannelsMode outputChannelsMode = OutputChannelsMode::default;
    PluginType pluginType = PluginType::math;
    AddMathPermissions addMathPermissions = AddMathPermissions::amNever;
    std::string name, shortName, vendor, description, icon, groupName;

    [[deprecated("Change plugin version in PluginConfig.props.")]] Version version = {1, 0, 0};
    Version minSupportedDewesoftVersion = {3, 8, 19051300};
    int minSupportedDCOMVersion = 0;
    bool hasProjectSettings = false;
    bool showChannelsPanel = true;
    bool hasModuleTemplate = false;
    bool hasContextTemplate = false;
    bool supportsSetupTemplates = true;
};
