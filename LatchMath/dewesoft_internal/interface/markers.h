#pragma once
#include "interface/input_channels.h"
#include "interface/output_channels.h"
#include "interface/enums.h"
#include "registration/plugin_props.h"

namespace dcom = Dewesoft::Utils::Dcom::Utils;
namespace adv = Dewesoft::Processing::Api::Advanced;

class MarkerObject
{
public:
    MarkerObject();
    ~MarkerObject();

    std::string getUnit() const;
    void setUnit(const std::string& unit);
    void addAxisType(MarkerValueType axisType);

protected:
    MarkerValueType valueType;
    IMarkerObjectPtr markerObject;
    std::string objectUnit = "";
    std::vector<MarkerValueType> axisTypes;

    std::vector<double> toDoubleVec(const _variant_t var) const;

    MarkerValueType getMarkerValueType() const;
    void setMarkerValueType(MarkerValueType valueType);

    friend class MarkerInput;
    friend class MarkerOutput;
    friend class Marker;
    friend class MarkerDrawGroup;
    friend class MarkerDrawGroups;
};

class MarkerInput : public MarkerObject
{
public:
    MarkerInput();

    void setUsingChannelInput(bool usingChannelInput);
    double getValue() const;
    void setValue(double value);
};

class MarkerOutput : public MarkerObject
{
public:
    MarkerOutput();
    MarkerOutput(OutputChannelBase& channel);
    ~MarkerOutput();

    void setOutputChannel(OutputChannelBase& channel);
};

#define DEFINE_MARKER_INPUT(T)          \
    class T##MarkerInput : public MarkerInput       \
    {                                               \
    public:                                         \
        T##MarkerInput()                            \
        {                                           \
            setMarkerValueType(MarkerValueType::T); \
        }                                           \
    };

#define DEFINE_MARKER_OUTPUT(T)                     \
    class T##MarkerOutput : public MarkerOutput     \
    {                                               \
    public:                                         \
        T##MarkerOutput()                           \
        {                                           \
            setMarkerValueType(MarkerValueType::T); \
        }                                           \
    };

DEFINE_MARKER_INPUT(Time);
DEFINE_MARKER_INPUT(Value);
DEFINE_MARKER_INPUT(Axis0);
DEFINE_MARKER_INPUT(Axis1);

DEFINE_MARKER_OUTPUT(Time);
DEFINE_MARKER_OUTPUT(Value);
DEFINE_MARKER_OUTPUT(Axis0);
DEFINE_MARKER_OUTPUT(Axis1);
DEFINE_MARKER_OUTPUT(Result);

class MarkerDrawGroup
{
public:
    MarkerDrawGroup();

    void addStyle(MarkerStyle style);
    void addMarkerInput(MarkerObject& markerInput);
    void addMarkerOutput(MarkerObject& markerOutput);
    void setColor(dcom::Color color);
    void addName(const std::string& name);

    bool visible = true;
    bool alwaysDrawInputs = false;
    bool waitForMouseInput = false;
    bool setColorIndividually = false;

protected:
    dcom::Color processingColor;
    bool colorSet = false;
    int index;
    std::vector<std::string> names;
    std::vector<MarkerStyle> styles;
    std::vector<MarkerObject*> markerOutputs;
    std::vector<MarkerObject*> markerInputs;

    friend class MarkerDrawGroups;
};

class MarkerDrawGroups
{
public:
    MarkerDrawGroups();
    MarkerDrawGroup& addDrawGroup();
    void setDCOMMarker(IProcessingMarkerPtr processingMarker);

protected:
    void applyDrawGroups();
    void clearDrawGroups();
    IProcessingMarkerPtr dewesoftMarker = nullptr;

private:
    std::vector<MarkerDrawGroup> groups;

    friend class ModuleBridge;
    friend class adv::SetupWindow;
    friend class SetupWindow;
};

class Marker
{
public:
    Marker();
    Marker(IProcessingMarkerPtr marker);
    void setOwnerChannel(InputChannelBase& owner);
    void setOwnerChannelType(MarkerChannelType channelType);

    void connectInput(MarkerInput& input);
    void connectOutput(MarkerOutput& output, OutputChannelBase& channel);

    IProcessingMarkerPtr DCOMMarker();
    void setDCOMMarker(IProcessingMarkerPtr processingMarker);
protected:
    IProcessingMarkerPtr dewesoftMarker = nullptr;
    void disconnectMarkerInputs();
    void disconnectMarkerOutputs();
    void applyMarkerObjects();
    void setMarkerProps(MarkerProperties& props);

private:
    std::vector<MarkerObject*> markerInputs;
    std::vector<MarkerObject*> markerOutputs;
    InputChannelBase markerOwner;

    friend class SetupWindow;
    friend class ModuleBridge;
    friend class adv::SetupWindow;
    friend class MarkerDrawGroups;
};
