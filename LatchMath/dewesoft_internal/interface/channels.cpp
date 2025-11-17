#pragma once
#include "stdafx.h"
#include <vector> 
#include <functional>
#include <algorithm>
#include <complex>
#include "version.h"
#include "interface/input_channels.h"
#include "interface/enums.h"

using namespace Dewesoft::Processing::Api::Advanced;


int Resampler::size() const
{ 
    return pastSamplesRequiredForCalculation + blockSizeInSamples + futureSamplesRequiredForCalculation;
}

void Resampler::setMasterChannel(InputChannelBase* master)
{
    masterChannel = master;
}

double Resampler::acqSampleRate() const
{
    if (cppResampler)
    {
        IF_COMPATIBLE_VERSION(basicSRDivSupport,
        {
            return cppResampler->AcqSampleRate();
        });
        throw "resampler.acqSampleRate() is not supported in this version of Dewesoft X.";
    }
    return 0;
}

double Resampler::currentSampleRate() const
{
    if (cppResampler)
    {
        IF_COMPATIBLE_VERSION(basicSRDivSupport,
        {
            return cppResampler->CurrentSampleRate();
        });
        throw "resampler.currentSampleRate() is not supported in this version of Dewesoft X.";
    }
    return 0;
}

void Resampler::setCppResamplerProperties()
{
    if (cppResampler)
    {
        cppResampler->SamplingRate = static_cast<long>(samplingRate);
        cppResampler->BlockSizeInSamples = static_cast<long>(blockSizeInSamples);
        cppResampler->PastSamplesRequiredForCalculation = static_cast<long>(pastSamplesRequiredForCalculation);
        cppResampler->FutureSamplesRequiredForCalculation = static_cast<long>(futureSamplesRequiredForCalculation);
        cppResampler->ResamplerBase = static_cast<long>(resamplerBase);
        cppResampler->ResamplerBlockType = static_cast<long>(blockSizeType);
        if (masterChannel)
            cppResampler->SetMasterChannel(masterChannel->DCOMChannel());
        IF_COMPATIBLE_VERSION(basicSRDivSupport,
        {
            cppResampler->SupportsSRDivChannels = static_cast<long>(supportsSRDivChannels);
        });
    }
}

void Resampler::getCppResamplerProperties()
{
    if (cppResampler)
    {
        samplingRate = static_cast<ResamplerSamplingRate>(cppResampler->SamplingRate);
        blockSizeInSamples = cppResampler->BlockSizeInSamples;
        pastSamplesRequiredForCalculation = cppResampler->PastSamplesRequiredForCalculation;
        futureSamplesRequiredForCalculation = cppResampler->FutureSamplesRequiredForCalculation;
        resamplerBase = static_cast<ResamplerBase>(cppResampler->ResamplerBase);
        blockSizeType = static_cast<ResamplerBlockSizeType>(cppResampler->ResamplerBlockType);
        IF_COMPATIBLE_VERSION(basicSRDivSupport,
        {
            supportsSRDivChannels = static_cast<ResamplerSupportsSRDivChannels>(cppResampler->SupportsSRDivChannels);
        });
    }
}

void Resampler::setResamplerOldMode(bool isOld)
{
    cppResampler->IsOldMode = isOld;
    useOldResamplerMode = isOld;
}

Axis::Axis(IChannelPtr channel, int axisIndex)
    : dewesoftChannel(channel)
    , axisIndex(axisIndex)
{
}

Axis::~Axis()
{
}

void Axis::buildAxisValues(const Dewesoft::Processing::Api::Advanced::Vector& values)
{
    if (values.size() == 0)
    {
        buildAxisValues(1);
    } else
    {
        dewesoftChannel->ArrayInfo->DimSizes[(long)axisIndex] = (long)values.size();
        dewesoftChannel->ArrayInfo->Init();
        auto axisDef = dewesoftChannel->ArrayInfo->AxisDef[(long)axisIndex];
        axisDef->AxisType = atFloat;
        for (long i = 0; i < (long)values.size(); ++i)
            axisDef->FloatValues[i] = values[i];
    }
}

void Axis::buildAxisValues(double start, int count, double step)
{
    dewesoftChannel->ArrayInfo->DimSizes[axisIndex] = count;
    dewesoftChannel->ArrayInfo->Init();
    auto axisDef = dewesoftChannel->ArrayInfo->AxisDef[axisIndex];
    axisDef->AxisType = atFloatLinearFunc;
    axisDef->StartValue = start;
    axisDef->StepValue = step;
}

void Axis::buildAxisValues(int count)
{
    buildAxisValues(0, count);
}

void Axis::buildAxisValues(const std::vector<std::string>& values)
{
    std::vector<std::wstring> wideValues(values.size());

    for (size_t i = 0; i < values.size(); i++)
        wideValues[i] = std::wstring(values[i].begin(), values[i].end());
		
	buildAxisValues(wideValues);
}

void Axis::buildAxisValues(const std::vector<std::wstring>& values)
{
    dewesoftChannel->ArrayInfo->DimSizes[(long) axisIndex] = (long) values.size();
    dewesoftChannel->ArrayInfo->Init();
    auto axisDef = dewesoftChannel->ArrayInfo->AxisDef[(long) axisIndex];
    axisDef->AxisType = atString;
    for (long i = 0; i < (long) values.size(); ++i)
        axisDef->StringValues[i] = values[i].c_str();
}

void Axis::copyAxis(const Axis& axis)
{
    auto otherAxis = axis.dewesoftChannel->ArrayInfo->AxisDef[axis.axisIndex];
    dewesoftChannel->ArrayInfo->DimSizes[axisIndex] = axis.dewesoftChannel->ArrayInfo->DimSizes[axis.axisIndex];
    dewesoftChannel->ArrayInfo->Init();
    auto axisDef = dewesoftChannel->ArrayInfo->AxisDef[axisIndex];
    axisDef->AxisType = otherAxis->AxisType;

    switch (otherAxis->AxisType)
    {
    case atFloat:
        for (int i = 0; i < axis.dewesoftChannel->ArrayInfo->DimSizes[axis.axisIndex]; ++i)
            axisDef->FloatValues[i] = otherAxis->FloatValues[i];
        break;
    case atFloatLinearFunc:
        axisDef->StartValue = otherAxis->StartValue;
        axisDef->StepValue = otherAxis->StepValue;
        break;
    case atString:
        for (int i = 0; i < axis.dewesoftChannel->ArrayInfo->DimSizes[axis.axisIndex]; ++i)
            axisDef->StringValues[i] = otherAxis->StringValues[i];
        break;
    }

    auto name = otherAxis->Name;
    auto unit = otherAxis->_Unit;
    setName(!name ? L"" : std::wstring(name));
    setUnit(!unit ? L"" : std::wstring(unit));

    setPrecision(otherAxis->Precision);
}

void Axis::setUnit(const std::string& unit)
{
    dewesoftChannel->ArrayInfo->AxisDef[axisIndex]->_Unit = unit.c_str();
}

void Axis::setName(const std::string& name)
{
    dewesoftChannel->ArrayInfo->AxisDef[axisIndex]->Name = name.c_str();
}

void Axis::setUnit(const std::wstring& unit)
{
    dewesoftChannel->ArrayInfo->AxisDef[axisIndex]->_Unit = unit.c_str();
}

void Axis::setName(const std::wstring& name)
{
    dewesoftChannel->ArrayInfo->AxisDef[axisIndex]->Name = name.c_str();
}

void Axis::setPrecision(int precision)
{
    dewesoftChannel->ArrayInfo->AxisDef[axisIndex]->Precision = precision;
}

std::wstring Axis::unit() const
{
     _bstr_t unit = dewesoftChannel->ArrayInfo->AxisDef[axisIndex]->_Unit;
    if (!unit)
        return L"";
    else
        return (wchar_t*) unit;
}

std::wstring Axis::name() const
{
    _bstr_t name = dewesoftChannel->ArrayInfo->AxisDef[axisIndex]->Name;
    if (!name)
        return L"";
    else
        return (wchar_t*) name;
}

int Axis::precision() const
{
    return dewesoftChannel->ArrayInfo->AxisDef[axisIndex]->Precision;
}

size_t Axis::size() const
{
    return static_cast<size_t>(dewesoftChannel->ArrayInfo->AxisDef[axisIndex]->Size);
}

double Axis::value(size_t index) const
{
    IAxisDefPtr axisDef = dewesoftChannel->ArrayInfo->AxisDef[axisIndex];
    switch (axisDef->AxisType)
    {
        case TAxisType::atFloat:
            return axisDef->GetFloatValues(static_cast<long>(index));
        case TAxisType::atFloatLinearFunc:
            return axisDef->StartValue + static_cast<long>(index) * axisDef->StepValue;
        case TAxisType::atString:
            return static_cast<double>(index);
        default:
            return 0.0;
    }
}

double Axis::minValue() const
{
    return value(0);
}

double Axis::maxValue() const
{
    return value(size() - 1);
}


Channel::Channel(IChannelPtr dewesoftChannel)
  : dewesoftChannel(dewesoftChannel)
{
}

Channel::operator bool() const
{
    return dewesoftChannel;
}

IChannelPtr Channel::DCOMChannel() const
{
    return dewesoftChannel;
}

Channel::operator IChannelPtr() const
{
    return dewesoftChannel;
}

void Channel::clearDewesoftChannel()
{
    dewesoftChannel = nullptr;
    for (size_t a = 0; a < axes.size(); ++a)
        axes[a].dewesoftChannel = nullptr;
}

void Channel::setDewesoftChannel(IChannelPtr newChannel)
{
    dewesoftChannel = newChannel;
    for (size_t a = 0; a < axes.size(); ++a)
        axes[a].dewesoftChannel = dewesoftChannel;
}

bool Channel::isScalarChannel() const
{
    return !dewesoftChannel->ArrayChannel && !isBinaryChannel();
}

bool Channel::isBinaryChannel() const
{
    return dewesoftChannel->DataType == (long) ChannelDataType::Binary;
}

bool Channel::isVectorChannel() const
{
    return dewesoftChannel->ArrayInfo
        && dewesoftChannel->ArrayInfo->DimCount == 1;
}
bool Channel::isMatrixChannel() const
{
    return dewesoftChannel->ArrayInfo
        && dewesoftChannel->ArrayInfo->DimCount == 2;
}
bool Channel::isComplexChannel() const
{
    return dewesoftChannel->DataType == (long)ChannelDataType::ComplexSingle
        || dewesoftChannel->DataType == (long)ChannelDataType::ComplexDouble;
}

ChannelDataType Channel::getChannelDataType() const
{
    return static_cast<ChannelDataType>(dewesoftChannel->DataType);
}

std::wstring Channel::name() const
{
    _bstr_t name = dewesoftChannel->Name;
    if (!name)
        return L"";
    else
        return (wchar_t*)name;
}

std::wstring Channel::unit() const
{
    _bstr_t unit = dewesoftChannel->Unit_;
    if (!unit)
        return L"";
    else
        return (wchar_t*)unit;
}

ExpectedAsyncRate Channel::expectedAsyncRate() const
{
    return {
               (float)dewesoftChannel->ExpectedAsyncRate,
               (ExpectedAsyncRateType)dewesoftChannel->ExpectedAsyncRateType
           };
}

bool Channel::isEnabled() const
{
    return dewesoftChannel->Used;
}

ChannelTimebase Channel::timebase() const
{
    if (!dewesoftChannel) return ChannelTimebase::Unknown;

    if (dewesoftChannel->Async) return ChannelTimebase::Asynchronous;
    if (dewesoftChannel->IsSingleValue) return ChannelTimebase::SingleValue;
    return ChannelTimebase::Synchronous;
}
