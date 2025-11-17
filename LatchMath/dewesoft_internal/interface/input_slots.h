#pragma once
#include <functional>
#include <map>
#include <set>
#include <vector>
#include "dcomlib/dcom_utils/plugin_types.h"
#include "interface/input_channels.h"
#include "interface/macros.h"

class ModuleBridge;
class ContextBridge;
class InputChannelSlots;
BEGIN_NAMESPACE_PROCESSING class SetupWindow;
END_NAMESPACE_PROCESSING

using ChannelPredicate = std::function<bool(const InputChannelBase&)>;

const std::map<InputChannelDataType, std::set<ChannelDataType>> dataTypeTree = {
    {InputChannelDataType::Byte, {ChannelDataType::Byte}},
    {InputChannelDataType::ShortInt, {ChannelDataType::ShortInt}},
    {InputChannelDataType::SmallInt, {ChannelDataType::SmallInt}},
    {InputChannelDataType::Word, {ChannelDataType::Word}},
    {InputChannelDataType::Integer, {ChannelDataType::Integer}},
    {InputChannelDataType::Single, {ChannelDataType::Single}},
    {InputChannelDataType::Int64, {ChannelDataType::Int64}},
    {InputChannelDataType::Double, {ChannelDataType::Double}},
    {InputChannelDataType::Longword, {ChannelDataType::Longword}},
    {InputChannelDataType::ComplexSingle, {ChannelDataType::ComplexSingle}},
    {InputChannelDataType::ComplexDouble, {ChannelDataType::ComplexDouble}},
    {InputChannelDataType::Text, {ChannelDataType::Text}},
    {InputChannelDataType::Binary, {ChannelDataType::Binary}},
    {InputChannelDataType::CANMessage, {ChannelDataType::CANMessage}},
    {InputChannelDataType::CANFDMessage, {ChannelDataType::CANFDMessage}},
    {InputChannelDataType::Any,
     {
         ChannelDataType::ComplexSingle, ChannelDataType::ComplexDouble, ChannelDataType::Single,       ChannelDataType::Double,
         ChannelDataType::Byte,          ChannelDataType::ShortInt,      ChannelDataType::SmallInt,     ChannelDataType::Word,
         ChannelDataType::Integer,       ChannelDataType::Int64,         ChannelDataType::Longword,     ChannelDataType::Text,
         ChannelDataType::Binary,        ChannelDataType::CANMessage,    ChannelDataType::CANFDMessage, ChannelDataType::Bytes8,
         ChannelDataType::Bytes16,       ChannelDataType::Bytes16,       ChannelDataType::Bytes32,      ChannelDataType::Bytes64,
     }},
    {InputChannelDataType::Complex, {ChannelDataType::ComplexSingle, ChannelDataType::ComplexDouble}},
    {InputChannelDataType::ComplexNumber,
     {
         ChannelDataType::ComplexSingle,
         ChannelDataType::ComplexDouble,
         ChannelDataType::Single,
         ChannelDataType::Double,
         ChannelDataType::Byte,
         ChannelDataType::ShortInt,
         ChannelDataType::SmallInt,
         ChannelDataType::Word,
         ChannelDataType::Integer,
         ChannelDataType::Int64,
         ChannelDataType::Longword,
     }},
    {InputChannelDataType::Number,
     {
         ChannelDataType::Single,
         ChannelDataType::Double,
         ChannelDataType::Byte,
         ChannelDataType::ShortInt,
         ChannelDataType::SmallInt,
         ChannelDataType::Word,
         ChannelDataType::Integer,
         ChannelDataType::Int64,
         ChannelDataType::Longword,
     }},
    {InputChannelDataType::Float, {ChannelDataType::Single, ChannelDataType::Double}},
    {InputChannelDataType::Ordinal,
     {
         ChannelDataType::Byte,
         ChannelDataType::ShortInt,
         ChannelDataType::SmallInt,
         ChannelDataType::Word,
         ChannelDataType::Integer,
         ChannelDataType::Int64,
         ChannelDataType::Longword,
     }},
};

class ChannelSlot
{
public:
    std::string name;
    InputChannelBase* channelHolder;
    ChannelPredicate acceptsChannel = [](const InputChannelBase&) { return true; };
    InputChannelBase::Interpolation interpolation = InputChannelBase::Interpolation::Linear;
    bool used;

    void assignChannel(const std::wstring& channelName);
    InputChannelBase getAssignedChannel() const;
    InputChannelDataType dataType;
    ChannelDimension dimension;

private:
    bool isCorrectDatatype(const InputChannelBase& inputChannel) const
    {
        const bool isDoublePrecision = inputChannel.DCOMChannel()->IsDoublePrecision;
        const bool isScaled = InputChannelBase::isChannelScaled(inputChannel.DCOMChannel());

        const ChannelDataType type = [&]() {
            if (isScaled)
            {
                if (isDoublePrecision)
                    return ChannelDataType::Double;
                else
                    return ChannelDataType::Single;
            }
            else
                return static_cast<ChannelDataType>(inputChannel.DCOMChannel()->DataType);
        }();

        return dataTypeTree.at(dataType).end() != dataTypeTree.at(dataType).find(type);
    };

    bool isCorrectDimension(const InputChannelBase& inputChannel) const
    {
        if (channelHolder->slotDataType == InputChannelDataType::Any)
            return true;

        const long axisCount = [&]() -> long {
            if (inputChannel.DCOMChannel()->ArrayInfo)
                return inputChannel.DCOMChannel()->ArrayInfo->DimCount;
            else
                return 0;
        }();

        return static_cast<ChannelDimension>(axisCount) == dimension;
    }
    ChannelPredicate isCorrectTimebase = [](const InputChannelBase&) { return true; };

    bool satisfiesAllConditions(const InputChannelBase& channel) const;

    IMathContextPtr dewesoftContext;
    IAppPtr dewesoftApp;
    size_t index;

    friend class InputChannelSlots;
    friend class ModuleBridge;
    friend class ContextBridge;
    friend class Dewesoft::Processing::Api::Advanced::SetupWindow;
};

class InputChannelSlots
{
private:
    IMathContextPtr dewesoftContext;
    IAppPtr dewesoftApp;
    std::vector<ChannelSlot> ownSlots;
    std::vector<ChannelSlot> externalSlots;

    ChannelSlot& addSlot(const std::string& name, InputChannelBase* channel, ChannelTimebase timebase);
    void applyConnections();

    void clearInstances();
    void clearChannel(const IChannelPtr& channel);

public:
    template <typename T>
    ChannelSlot& connectChannel(const std::string& name, InputChannel<T>* channel, ChannelTimebase timebase = ChannelTimebase::Any)
    {
        ChannelSlot& conn = addSlot(name, channel, timebase);
        conn.dataType = channel->slotDataType;
        conn.dimension = channel->channelDimension;

        return conn;
    }

    void useSharedModuleChannel(const std::string& sharedModuleChannelSlotName, InputChannelBase* in);

    size_t size() const;
    ChannelSlot& operator[](size_t i);
    ChannelSlot operator[](size_t i) const;

    friend class ModuleBridge;
    friend class ContextBridge;
};
