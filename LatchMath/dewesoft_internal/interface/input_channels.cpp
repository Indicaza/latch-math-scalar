#pragma once
#include "stdafx.h"
#include "interface/input_channels.h"

size_t InputChannelBase::valuesIndexForIthObject(size_t i) const
{
    return i * cache.arraySize;
}

size_t InputChannelBase::timesIndexForIthObject(size_t i) const
{
    return i;
}

void InputChannelBase::cacheChannelStructure()
{
    // Cache channel structure used for reading from input buffers at the end of Initiate()
    if (!dewesoftChannel)
        return;

    cache.arraySize = dewesoftChannel->ArraySize;
    cache.isComplexChannel = isComplexChannel();
    
    if (!(isScalarChannel() || isBinaryChannel()))
    {
        cache.dimensionSize[0] = dewesoftChannel->ArrayInfo->DimCount > 0 ? (size_t) dewesoftChannel->ArrayInfo->DimSizes[0] : 1;
        cache.dimensionSize[1] = dewesoftChannel->ArrayInfo->DimCount > 1 ? (size_t) dewesoftChannel->ArrayInfo->DimSizes[1] : 1;
    }
}

void InputChannelBase::cacheResampledBufferLocations(const DewesoftVersion& currentVersion)
{
    // Getting pointers to resampled buffers via DCOM is a bottleneck for input channels,
    // so we cache them before the calculate call.
    if (!dewesoftChannel || !dewesoftResampledChannel)
        return;

    cache.directData = reinterpret_cast<void*>(dewesoftResampledChannel->GetDirectDataBuffer().ullVal);
    IF_COMPATIBLE_VERSION(binaryChannelsVersion, {
        cache.byteValues = (unsigned char**) (dewesoftResampledChannel->GetBinaryPointersBuffer().ullVal);
        cache.sizeValues = (size_t*) (dewesoftResampledChannel->GetBinarySizesBuffer().ullVal);

        cache.binaryBufferSize = dewesoftChannel->BinBufSize;
        cache.binaryBuffer = (unsigned char*)(dewesoftChannel->GetBinAddress64());
    })
    cache.times = (double*)(dewesoftResampledChannel->GetTimesBuffer().ullVal);
}

InputChannelBase::InputChannelBase()
{
}

InputChannelBase::InputChannelBase(IChannelPtr chn)
    : Channel(chn)
{
}

void InputChannelBase::setInterpolation(Interpolation interpolation)
{
    this->interpolation = interpolation;
}

std::wstring InputChannelBase::getName() const
{
    _bstr_t name = dewesoftChannel->Name;
    if (!name)
        return L"";
    else
        return (wchar_t*)name;
}

std::wstring InputChannelBase::getUnit() const
{
    _bstr_t unit = dewesoftChannel->Unit_;
    if (!unit)
        return L"";
    else
        return (wchar_t*) unit;
}

std::wstring InputChannelBase::getDescription() const
{
    return (wchar_t*) dewesoftChannel->Description;
}
