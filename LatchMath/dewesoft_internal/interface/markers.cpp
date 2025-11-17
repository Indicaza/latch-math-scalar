#include "stdafx.h"
#include "markers.h"
#include "dcomlib/dcom_utils/variant_helper.h"

using namespace Dewesoft::Utils::Dcom::Utils;

Marker::Marker()
{
    dewesoftMarker = nullptr;
}

Marker::Marker(IProcessingMarkerPtr marker)
{
    dewesoftMarker = marker;
}

void Marker::setOwnerChannel(InputChannelBase& owner)
{
    dewesoftMarker->MarkerOwner->Owner = owner.DCOMChannel();
}

void Marker::setOwnerChannelType(MarkerChannelType channelType)
{
    dewesoftMarker->MarkerOwner->ChannelType = static_cast<IMarkerChannelType>(channelType);
}

void Marker::connectInput(MarkerInput& input)
{
    for (MarkerObject* object : markerInputs)
    {
        if (input.markerObject == object->markerObject)
            return;
    }

    input.markerObject = dewesoftMarker->ConnectInputEx(static_cast<IMarkerValueType>(input.getMarkerValueType()));
    markerInputs.push_back(&input);
}

void Marker::applyMarkerObjects()
{
    for (MarkerObject* input : markerInputs)
    {
        if (input->objectUnit != "")
            input->markerObject->ValueUnit = input->objectUnit.c_str();
        for (int i = 0; i < static_cast<int>(input->axisTypes.size()); ++i)
            input->markerObject->AddAxisType(static_cast<IMarkerValueType>(input->axisTypes[i]));
    }

    for (MarkerObject* output : markerOutputs)
    {
        if (output->objectUnit != "")
             output->markerObject->ValueUnit = output->objectUnit.c_str();
        for (int i = 0; i < static_cast<int>(output->axisTypes.size()); ++i)
            output->markerObject->AddAxisType(static_cast<IMarkerValueType>(output->axisTypes[i]));
    }
}

void Marker::connectOutput(MarkerOutput& output, OutputChannelBase& channel)
{
    for (MarkerObject* object : markerOutputs)
    {
        if (output.markerObject == object->markerObject)
            return;
    }
    output.markerObject = dewesoftMarker->ConnectOutputEx(static_cast<IMarkerValueType>(output.getMarkerValueType()));
    output.setOutputChannel(channel);
    if (output.objectUnit != "")
        output.markerObject->ValueUnit = output.objectUnit.c_str();
    else
        output.markerObject->ValueUnit = channel.unit().c_str();

    markerOutputs.push_back(&output);
}

void Marker::disconnectMarkerInputs()
{
    if (dewesoftMarker)
    {
        for (MarkerObject* object : markerInputs)
            object->markerObject = nullptr;
        dewesoftMarker->DisconnectInputs();
        markerInputs.clear();
    }
}

void Marker::disconnectMarkerOutputs()
{
    if (dewesoftMarker)
    {
        for (MarkerObject* object : markerOutputs)
            object->markerObject = nullptr;
        dewesoftMarker->DisconnectOutputs();
        markerOutputs.clear();
    }
}

void Marker::setMarkerProps(MarkerProperties& props)
{
    VARIANT propsOut;

    SAFEARRAY* saProps = SafeArrayCreateVector(VT_VARIANT, 0, 4);
    propsOut.vt = VT_ARRAY | VT_VARIANT;
    propsOut.parray = saProps;
    toVariant(std::to_string(props.updateInputsAfterMove), (static_cast<VARIANT*>(saProps->pvData) + 0));
    toVariant(std::to_string(props.supportedSetupParams.interpolateBetweenData), (static_cast<VARIANT*>(saProps->pvData) + 1));
    toVariant(std::to_string(props.supportedSetupParams.findPeaks), (static_cast<VARIANT*>(saProps->pvData) + 2));
    toVariant(std::to_string(props.supportedSetupParams.useFFTPeaks), (static_cast<VARIANT*>(saProps->pvData) + 3));

    dewesoftMarker->SetMarkerProps(propsOut);
}

IProcessingMarkerPtr Marker::DCOMMarker()
{
    return dewesoftMarker;
}

void Marker::setDCOMMarker(IProcessingMarkerPtr processingMarker)
{
    dewesoftMarker = processingMarker;
}

MarkerDrawGroup::MarkerDrawGroup()
{
}

void MarkerDrawGroup::addStyle(MarkerStyle style)
{
    styles.push_back(style);
}

void MarkerDrawGroup::addMarkerOutput(MarkerObject& markerOutput)
{
    markerOutputs.push_back(&markerOutput);
}

void MarkerDrawGroup::addMarkerInput(MarkerObject& markerInput)
{
    markerInputs.push_back(&markerInput);
}

void MarkerDrawGroup::setColor(dcom::Color color)
{
    processingColor = color;
    colorSet = true;
}

void MarkerDrawGroup::addName(const std::string& name)
{
    names.push_back(name);
}

MarkerDrawGroups::MarkerDrawGroups()
{
}

MarkerDrawGroup& MarkerDrawGroups::addDrawGroup()
{
    groups.push_back(MarkerDrawGroup());
    groups.back().index = static_cast<int>(groups.size() - 1);
    return groups.back();
}

void MarkerDrawGroups::clearDrawGroups()
{
    if (dewesoftMarker)
    {
        groups.clear();
        dewesoftMarker->ClearGroups();
    }
}

void MarkerDrawGroups::applyDrawGroups()
{
    if (dewesoftMarker)
    {
        for (MarkerDrawGroup& group : groups)
        {
            dewesoftMarker->CreateGroup();

            if (group.colorSet)
                dewesoftMarker->PutGroupColor(group.index, static_cast<long>(group.processingColor));
            for (const std::string& name : group.names)
                dewesoftMarker->AddGroupName(group.index, name.c_str());
            dewesoftMarker->PutGroupVisible(group.index, (group.visible ? VARIANT_TRUE : VARIANT_FALSE));
            dewesoftMarker->PutGroupAlwaysDrawInputs(group.index, (group.alwaysDrawInputs ? VARIANT_TRUE : VARIANT_FALSE));
            dewesoftMarker->PutGroupWaitForMouseInput(group.index, (group.waitForMouseInput ? VARIANT_TRUE : VARIANT_FALSE));
            dewesoftMarker->PutGroupHasOwnColor(group.index, (group.setColorIndividually ? VARIANT_TRUE : VARIANT_FALSE));

            for (MarkerStyle style : group.styles)
                dewesoftMarker->AddGroupStyle(group.index, static_cast<int>(style));
            for (MarkerObject* markerOutput : group.markerOutputs)
                dewesoftMarker->AddGroupMarkerOutput(group.index, IMarkerOutputPtr(markerOutput->markerObject));
            for (MarkerObject* markerInput : group.markerInputs)
                dewesoftMarker->AddGroupMarkerInput(group.index, IMarkerInputPtr(markerInput->markerObject));
        }
    }
}

void MarkerDrawGroups::setDCOMMarker(IProcessingMarkerPtr processingMarker)
{
    dewesoftMarker = processingMarker;
}

MarkerObject::MarkerObject()
{
}

MarkerObject::~MarkerObject()
{
}

std::vector<double> MarkerObject::toDoubleVec(const _variant_t var) const
{
    const size_t size = var.parray->rgsabound->cElements;
    std::vector<double> result(size);
    memcpy(result.data(), var.parray->pvData, size * sizeof(double));
    return result;
}

void MarkerObject::setUnit(const std::string& unit)
{
    objectUnit = unit;
}

std::string MarkerObject::getUnit() const
{
    BSTR unitBstr = markerObject->ValueUnit.GetBSTR();
    if (SysStringLen(unitBstr) == 0)
        return "";
    else
        return std::string(_bstr_t(unitBstr, false));
}

void MarkerObject::addAxisType(MarkerValueType axisType)
{
    for (int i = 0; i < static_cast<int>(axisTypes.size()); ++i)
        if (axisTypes[i] == axisType)
            return;

    axisTypes.push_back(axisType);
}

MarkerValueType MarkerObject::getMarkerValueType() const
{
    return valueType;
}

void MarkerObject::setMarkerValueType(MarkerValueType valueType)
{
    this->valueType = valueType;
}

MarkerInput::MarkerInput()
{
}

double MarkerInput::getValue() const
{
    return IMarkerInputPtr(markerObject)->ValueEx;
}

void MarkerInput::setValue(double value)
{
    IMarkerInputPtr(markerObject)->ValueEx = value;
}

void MarkerInput::setUsingChannelInput(bool usingChannelInput)
{
    IMarkerInputPtr(markerObject)->UsingChannelInput = usingChannelInput;
}

MarkerOutput::MarkerOutput()
{
}

MarkerOutput::MarkerOutput(OutputChannelBase& channel)
{
    setOutputChannel(channel);
}

MarkerOutput::~MarkerOutput()
{
}

void MarkerOutput::setOutputChannel(OutputChannelBase& channel)
{
    IMarkerOutputPtr(markerObject)->Channel = channel.DCOMChannel();
}
