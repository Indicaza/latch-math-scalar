#pragma once
#include "interface/version.h"
#include "interface/channels.h"
#include "dcomlib/dcom_utils/variant_helper.h"
#include "basic_types.h"
#include <array>

struct ChannelCache
{
    void* directData = nullptr;
    double* doubleValues = nullptr;
    unsigned char** byteValues = nullptr;
    size_t* sizeValues = nullptr;
    double* times = nullptr;

    int arraySize = 1;
	unsigned char* binaryBuffer = nullptr;
	long binaryBufferSize = 1;
    bool isComplexChannel = false;
    std::array<size_t, 2> dimensionSize;
    bool isScaled = true;
};

class InputChannelBase : public Channel
{
public:
    enum class Interpolation
    {
        None,
        Linear
    };

    InputChannelBase();
    InputChannelBase(IChannelPtr channel);

    void setInterpolation(Interpolation interpolation);

    template <typename T>
    constexpr void determineDataType()
    {
        using namespace Dewesoft::Processing::Api::Advanced;
        if constexpr (!std::is_void_v<T>)
        {
            if constexpr (is_instance<T, DSVector>::value)
                channelDimension = ChannelDimension::Vector;
            else if constexpr (is_instance<T, DSMatrix>::value)
                channelDimension = ChannelDimension::Matrix;
            else
                channelDimension = ChannelDimension::Scalar;
        }

        if constexpr (std::is_same_v<innermost_t<T>, Byte>)
            slotDataType = InputChannelDataType::Byte;
        else if constexpr (std::is_same_v<innermost_t<T>, ShortInt>)
            slotDataType = InputChannelDataType::ShortInt;
        else if constexpr (std::is_same_v<innermost_t<T>, SmallInt>)
            slotDataType = InputChannelDataType::SmallInt;
        else if constexpr (std::is_same_v<innermost_t<T>, Word>)
            slotDataType = InputChannelDataType::Word;
        else if constexpr (std::is_same_v<innermost_t<T>, Integer>)
            slotDataType = InputChannelDataType::Integer;
        else if constexpr (std::is_same_v<innermost_t<T>, Single>)
            slotDataType = InputChannelDataType::Single;
        else if constexpr (std::is_same_v<innermost_t<T>, Int64>)
            slotDataType = InputChannelDataType::Int64;
        else if constexpr (std::is_same_v<innermost_t<T>, Double>)
            slotDataType = InputChannelDataType::Double;
        else if constexpr (std::is_same_v<innermost_t<T>, LongWord>)
            slotDataType = InputChannelDataType::Longword;
        else if constexpr (std::is_same_v<innermost_t<T>, Bytes>)
            slotDataType = InputChannelDataType::Binary;
        else if constexpr (std::is_same_v<innermost_t<T>, ComplexSingle>)
            slotDataType = InputChannelDataType::ComplexSingle;
        else if constexpr (std::is_same_v<innermost_t<T>, ComplexDouble>)
            slotDataType = InputChannelDataType::ComplexDouble;
        else if constexpr (std::is_same_v<T, Text>)
            slotDataType = InputChannelDataType::Text;
        else if constexpr (std::is_void_v<T>)
            slotDataType = InputChannelDataType::Any;
        else if constexpr (std::is_same_v<innermost_t<T>, DSComplexNumber>)
            slotDataType = InputChannelDataType::ComplexNumber;
        else if constexpr (std::is_same_v<innermost_t<T>, DSComplex>)
            slotDataType = InputChannelDataType::Complex;
        else if constexpr (std::is_same_v<innermost_t<T>, DSNumber>)
            slotDataType = InputChannelDataType::Number;
        else if constexpr (std::is_same_v<innermost_t<T>, DSOrdinal>)
            slotDataType = InputChannelDataType::Ordinal;
        else if constexpr (std::is_same_v<innermost_t<T>, CANMessage>)
            slotDataType = InputChannelDataType::CANMessage;
        else if constexpr (std::is_same_v<innermost_t<T>, CANFDMessage>)
            slotDataType = InputChannelDataType::CANFDMessage;
        else
            static_assert(false, "Data type not supported");  // Static assert should fail if the type does not match
    }

    static bool isChannelScaled(IChannelPtr dewesoftChannel)
    {
        double offset, scale;
        const bool isDoublePrecision = dewesoftChannel->IsDoublePrecision;
        if (isDoublePrecision)
        {
            offset = dewesoftChannel->ScaleValueDouble(0);
            scale = dewesoftChannel->ScaleValueDouble(1) - offset;
        }
        else
        {
            offset = dewesoftChannel->ScaleValue(0);
            scale = dewesoftChannel->ScaleValue(1) - offset;
        }

        return offset != 0.0 || scale != 1.0;
    }

    std::wstring getName() const;
    std::wstring getUnit() const;
    std::wstring getDescription() const;

    InputChannelDataType slotDataType = InputChannelDataType::Single;
    ChannelDimension channelDimension = ChannelDimension::Scalar;

protected:
    Resampler* resampler;
    CallInfo* callInfo;
    IResamplerChannelPtr dewesoftResampledChannel;
    Interpolation interpolation;
    ChannelCache cache;
    bool useOldResamplerMode() const
    {
        return resampler->useOldResamplerMode;
    }

    virtual void cacheChannelStructure();
    virtual void cacheResampledBufferLocations(const DewesoftVersion& currentVersion);
    virtual void configureChannelDataConverter()
    {
		// this is definitely not needed here, but for some reason it doesn't compile (link) if
		// we just access IndexEx in the overloaded configureChannelDataConverter without first
		// accessing it here...
		dewesoftChannel->IndexEx;
    };

    size_t valuesIndexForIthObject(size_t i) const;
    size_t timesIndexForIthObject(size_t i) const;

    friend class ModuleBridge;
    friend class ContextBridge;
    friend class InputChannelSlots;

private:
    DewesoftVersion currentVersion;
};

template <typename T>
class Converter
{
public:
    T getScalar(int pos) const
    {
        const auto value = getValue(pos);
        if (scaleData)
            return value * scale + offset;
        else
            return value;
    }

    template <typename U = T>
    std::enable_if_t<is_instance<U, Dewesoft::Processing::Api::Advanced::DSVector>::value, T> getVector(int pos) const
    {
        using namespace Dewesoft::Processing::Api::Advanced;
        switch (channelDataType)
        {
            case ChannelDataType::Byte:
            {
                const auto* resampledData = static_cast<DSDataType_t<ChannelDataType::Byte>*>(data);
                return T(&resampledData[pos], &resampledData[pos + step]);
            }
            case ChannelDataType::ShortInt:
            {
                const auto* resampledData = static_cast<DSDataType_t<ChannelDataType::ShortInt>*>(data);
                return T(&resampledData[pos], &resampledData[pos + step]);
            }
            case ChannelDataType::SmallInt:
            {
                const auto* resampledData = static_cast<DSDataType_t<ChannelDataType::SmallInt>*>(data);
                return T(&resampledData[pos], &resampledData[pos + step]);
            }
            case ChannelDataType::Word:
            {
                const auto* resampledData = static_cast<DSDataType_t<ChannelDataType::Word>*>(data);
                return T(&resampledData[pos], &resampledData[pos + step]);
            }
            case ChannelDataType::Integer:
            {
                const auto* resampledData = static_cast<DSDataType_t<ChannelDataType::Integer>*>(data);
                return T(&resampledData[pos], &resampledData[pos + step]);
            }
            case ChannelDataType::Single:
            {
                const auto* resampledData = static_cast<DSDataType_t<ChannelDataType::Single>*>(data);
                return T(&resampledData[pos], &resampledData[pos + step]);
            }
            case ChannelDataType::Int64:
            {
                const auto* resampledData = static_cast<DSDataType_t<ChannelDataType::Int64>*>(data);
                return T(&resampledData[pos], &resampledData[pos + step]);
            }
            case ChannelDataType::Double:
            {
                const auto* resampledData = static_cast<DSDataType_t<ChannelDataType::Double>*>(data);
                return T(&resampledData[pos], &resampledData[pos + step]);
            }
            case ChannelDataType::Longword:
            {
                const auto* resampledData = static_cast<DSDataType_t<ChannelDataType::Longword>*>(data);
                return T(&resampledData[pos], &resampledData[pos + step]);
            }
            case ChannelDataType::ComplexSingle:
            {
                if constexpr (std::is_same_v<T, DSVector<ComplexSingle>> || std::is_same_v<T, DSVector<ComplexDouble>>)
                {
                    const auto* resampledData = static_cast<DSDataType_t<ChannelDataType::ComplexSingle>*>(data);
                    return T(&resampledData[pos], &resampledData[pos + step]);
                }
            }
            break;
            case ChannelDataType::ComplexDouble:
            {
                if constexpr (std::is_same_v<T, DSVector<ComplexSingle>> || std::is_same_v<T, DSVector<ComplexDouble>>)
                {
                    const auto* resampledData = static_cast<DSDataType_t<ChannelDataType::ComplexDouble>*>(data);
                    return T(&resampledData[pos], &resampledData[pos + step]);
                }
            }
            break;
        }
    }

    template <typename U = T>
    std::enable_if_t<is_instance<U, Dewesoft::Processing::Api::Advanced::DSMatrix>::value, T> getMatrix(int pos) const
    {
        using namespace Dewesoft::Processing::Api::Advanced;
        switch (channelDataType)
        {
            case ChannelDataType::Byte:
            {
                const auto* resampledData = static_cast<DSDataType_t<ChannelDataType::Byte>*>(data);
                return T(&resampledData[pos], &resampledData[pos + step], dimensionSize[0], dimensionSize[1]);
            }
            case ChannelDataType::ShortInt:
            {
                const auto* resampledData = static_cast<DSDataType_t<ChannelDataType::ShortInt>*>(data);
                return T(&resampledData[pos], &resampledData[pos + step], dimensionSize[0], dimensionSize[1]);
            }
            case ChannelDataType::SmallInt:
            {
                const auto* resampledData = static_cast<DSDataType_t<ChannelDataType::SmallInt>*>(data);
                return T(&resampledData[pos], &resampledData[pos + step], dimensionSize[0], dimensionSize[1]);
            }
            case ChannelDataType::Word:
            {
                const auto* resampledData = static_cast<DSDataType_t<ChannelDataType::Word>*>(data);
                return T(&resampledData[pos], &resampledData[pos + step], dimensionSize[0], dimensionSize[1]);
            }
            case ChannelDataType::Integer:
            {
                const auto* resampledData = static_cast<DSDataType_t<ChannelDataType::Integer>*>(data);
                return T(&resampledData[pos], &resampledData[pos + step], dimensionSize[0], dimensionSize[1]);
            }
            case ChannelDataType::Single:
            {
                const auto* resampledData = static_cast<DSDataType_t<ChannelDataType::Single>*>(data);
                return T(&resampledData[pos], &resampledData[pos + step], dimensionSize[0], dimensionSize[1]);
            }
            case ChannelDataType::Int64:
            {
                const auto* resampledData = static_cast<DSDataType_t<ChannelDataType::Int64>*>(data);
                return T(&resampledData[pos], &resampledData[pos + step], dimensionSize[0], dimensionSize[1]);
            }
            case ChannelDataType::Double:
            {
                const auto* resampledData = static_cast<DSDataType_t<ChannelDataType::Double>*>(data);
                return T(&resampledData[pos], &resampledData[pos + step], dimensionSize[0], dimensionSize[1]);
            }
            case ChannelDataType::Longword:
            {
                const auto* resampledData = static_cast<DSDataType_t<ChannelDataType::Longword>*>(data);
                return T(&resampledData[pos], &resampledData[pos + step], dimensionSize[0], dimensionSize[1]);
            }
            case ChannelDataType::ComplexSingle:
            {
                if constexpr (std::is_same_v<T, DSMatrix<ComplexSingle>> || std::is_same_v<T, DSMatrix<ComplexDouble>>)
                {
                    const auto* resampledData = static_cast<DSDataType_t<ChannelDataType::ComplexSingle>*>(data);
                    return T(&resampledData[pos], &resampledData[pos + step], dimensionSize[0], dimensionSize[1]);
                }
            }
            break;
            case ChannelDataType::ComplexDouble:
            {
                if constexpr (std::is_same_v<T, DSMatrix<ComplexSingle>> || std::is_same_v<T, DSMatrix<ComplexDouble>>)
                {
                    const auto* resampledData = static_cast<DSDataType_t<ChannelDataType::ComplexDouble>*>(data);
                    return T(&resampledData[pos], &resampledData[pos + step], dimensionSize[0], dimensionSize[1]);
                }
            }
            break;
        }
    }

    template <typename U>
    U extractValue(const U& value) const
    {
        if constexpr (!std::is_same_v<U, Dewesoft::Processing::Api::Advanced::CANFDMessage> &&
                      !std::is_same_v<U, Dewesoft::Processing::Api::Advanced::CANMessage>)
        {
            if (isDIChannel)
                return (value >> bitOffsetInValue) & 1;
        }
        return value;
    }

    void setChannelDataType(ChannelDataType type)
    {
        channelDataType = type;
    }

    void setChannelDimension(std::array<size_t, 2> dimensions)
    {
        dimensionSize = dimensions;
    }

    void setStep(const size_t step)
    {
        this->step = step;
    }

    void setScaleData(bool scale)
    {
        scaleData = scale;
    }

    void setScaleAndOffset(double scale, double offset)
    {
        this->scale = scale;
        this->offset = offset;
    }

    void setIsDIChannel(bool isDIChannel, int bitOffsetInValue = -1)
    {
        this->isDIChannel = isDIChannel;
        this->bitOffsetInValue = bitOffsetInValue;
    }

    void* data;
    size_t* sizeValues;
    Dewesoft::Processing::Api::Advanced::Byte** binaryData;

private:

    T getValue(int pos) const
    {
        using namespace Dewesoft::Processing::Api::Advanced;
        switch (channelDataType)
        {
            case ChannelDataType::Byte:
            {
                return static_cast<T>(static_cast<DSDataType_t<ChannelDataType::Byte>*>(data)[pos]);
            }
            case ChannelDataType::ShortInt:
            {
                return static_cast<T>(extractValue(static_cast<DSDataType_t<ChannelDataType::ShortInt>*>(data)[pos]));
            }
            case ChannelDataType::SmallInt:
            {
                return static_cast<T>(extractValue(static_cast<DSDataType_t<ChannelDataType::SmallInt>*>(data)[pos]));
            }
            case ChannelDataType::Word:
            {
                return static_cast<T>(static_cast<DSDataType_t<ChannelDataType::Word>*>(data)[pos]);
            }
            case ChannelDataType::Integer:
            {
                return static_cast<T>(extractValue(static_cast<DSDataType_t<ChannelDataType::Integer>*>(data)[pos]));
            }
            case ChannelDataType::Single:
            {
                return static_cast<T>(static_cast<DSDataType_t<ChannelDataType::Single>*>(data)[pos]);
            }
            case ChannelDataType::Int64:
            {
                return static_cast<T>(static_cast<DSDataType_t<ChannelDataType::Int64>*>(data)[pos]);
            }
            case ChannelDataType::Double:
            {
                return static_cast<T>(static_cast<DSDataType_t<ChannelDataType::Double>*>(data)[pos]);
            }
            case ChannelDataType::Longword:
            {
                return static_cast<T>(static_cast<DSDataType_t<ChannelDataType::Longword>*>(data)[pos]);
            }
            case ChannelDataType::ComplexSingle:
            {
                if constexpr (std::is_same_v<T, ComplexSingle> || std::is_same_v<T, ComplexDouble>)
                {
                    return static_cast<T>(static_cast<DSDataType_t<ChannelDataType::ComplexSingle>*>(data)[pos]);
                }
                else
                {
                    return 0;
                }
            }
            case ChannelDataType::ComplexDouble:
            {
                if constexpr (std::is_same_v<T, ComplexSingle> || std::is_same_v<T, ComplexDouble>)
                {
                    return static_cast<T>(static_cast<DSDataType_t<ChannelDataType::ComplexDouble>*>(data)[pos]);
                }
                else
                {
                    return 0;
                }
            }
            case ChannelDataType::Binary:
            {
                if constexpr (std::is_same_v<T, Bytes>)
                {
                    const size_t len = sizeValues[pos];
                    if (len == 0)
                        return Bytes();
                    return Bytes(binaryData[pos], binaryData[pos] + len);
                }
                else
                {
                    return 0;
                }
            }
            case ChannelDataType::CANMessage:
            {
                if constexpr (std::is_same_v<T, CANMessage>)
                {
                    return static_cast<DSDataType_t<ChannelDataType::CANMessage>*>(data)[pos];
                }
                else
                {
                    return 0;
                }
            }
            case ChannelDataType::CANFDMessage:
            {
                if constexpr (std::is_same_v<T, CANFDMessage>)
                {
                    return static_cast<DSDataType_t<ChannelDataType::CANFDMessage>*>(data)[pos];
                }
                else
                {
                    return 0;
                }
            }

            default:
                return 0;
        }
    }
    ChannelDataType channelDataType = ChannelDataType::Double;
    bool scaleData = false;
    double scale = 1, offset = 0;
    bool isDIChannel = false;
    int bitOffsetInValue = -1;
    std::array<size_t, 2> dimensionSize;
    size_t step = 1;
};

template <typename T>
class InputChannel : public InputChannelBase
{
public:
    InputChannel()
    {
        constexpr size_t axisCount = [&]() {
            if constexpr (is_instance<T, Dewesoft::Processing::Api::Advanced::DSMatrix>::value)
                return 2;
            else if constexpr (is_instance<T, Dewesoft::Processing::Api::Advanced::DSVector>::value)
                return 1;
            else
                return 0;
        }();

        for (int i = 0; i < axisCount; ++i)
            axes.push_back(Axis(nullptr, i));

        determineDataType<T>();
        converter.data = &cache.directData;
    }

    InputChannel(InputChannelBase channel)
        : InputChannelBase(channel)
    {
        constexpr size_t axisCount = [&]() {
            if constexpr (is_instance<T, Dewesoft::Processing::Api::Advanced::DSMatrix>::value)
                return 2;
            else if constexpr (is_instance<T, Dewesoft::Processing::Api::Advanced::DSVector>::value)
                return 1;
            else
                return 0;
        }();

        for (int i = 0; i < axisCount; ++i)
            axes.push_back(Axis(nullptr, i));

        determineDataType<T>();
        converter.data = &cache.directData;
    }

    template <typename U = T>
    std::enable_if_t<std::is_void_v<U>> setChannelDimensions(ChannelDimension dimension)
    {
        channelDimension = dimension;
        const size_t axisCount = [&]() {
            if (dimension == ChannelDimension::Matrix)
                return 2;
            else if (dimension == ChannelDimension::Vector)
                return 1;
            else
                return 0;
        }();

        for (int i = 0; i < axisCount; ++i)
            axes.push_back(Axis(nullptr, i));
    }

    void cacheResampledBufferLocations(const DewesoftVersion& currentVersion) override
    {
        using namespace Dewesoft::Processing::Api::Advanced;

        this->InputChannelBase::cacheResampledBufferLocations(currentVersion);
        converter.data = cache.directData; // TODO: should this be a reference, like the ones above?
        if constexpr (std::is_same_v<innermost_t<T>, Bytes>)
        {
            converter.sizeValues = cache.sizeValues;
            converter.binaryData = cache.byteValues;
        }
    }

    void cacheChannelStructure() override
    {
        this->InputChannelBase::cacheChannelStructure();
        converter.setChannelDimension(cache.dimensionSize);
        converter.setStep(cache.arraySize);
        if (useOldResamplerMode())
        {
            if constexpr (is_instance<Dewesoft::Processing::Api::Advanced::contained_t<T>, std::complex>::value)
                converter.setChannelDataType(ChannelDataType::ComplexDouble);
            else
                converter.setChannelDataType(ChannelDataType::Double);
        }
    }

    InputChannelDataType getSlotDataType()
    {
        return slotDataType;
    }

    void setSlotDataType(InputChannelDataType type)
    {
        slotDataType = type;
    }

    ChannelDataType getDataType() const
    {
        if (isChannelScaled(dewesoftChannel))
        {
            if (dewesoftChannel->IsDoublePrecision)
                return ChannelDataType::Double;
            else
                return ChannelDataType::Single;
        }
        else if (slotDataType == InputChannelDataType::ComplexNumber || slotDataType == InputChannelDataType::Complex)
        {
            return ChannelDataType::ComplexDouble;
        }
        else if (slotDataType == InputChannelDataType::Number || slotDataType == InputChannelDataType::Float)
        {
            return ChannelDataType::Double;
        }
        else if (slotDataType == InputChannelDataType::Ordinal)
            return ChannelDataType::Int64;
        else if (slotDataType == InputChannelDataType::Any)
            return getChannelDataType();
        else
            return static_cast<ChannelDataType>(static_cast<long>(slotDataType));
    }

    Dewesoft::Processing::Api::Advanced::Time getTime(int pos = 0) const
    {
        pos += resampler->pastSamplesRequiredForCalculation;
        if (pos < 0 || pos >= resampler->size())
            throw std::runtime_error("Invalid pos in getTime call");

        if (resampler->samplingRate == ResamplerSamplingRate::SingleValue)
            return callInfo ? callInfo->startBlockTime : 0;

        return cache.times[timesIndexForIthObject(pos)];
    }

    void setDewesoftChannel(IChannelPtr newChannel) override
    {
        this->Channel::setDewesoftChannel(newChannel);
    }

    void configureChannelDataConverter() override
    {
        if (!dewesoftChannel)
            return;

        const bool isDoublePrecision = dewesoftChannel->IsDoublePrecision;
            
        const ChannelDataType inputChannelDataType = [&] {
            if (isChannelScaled(dewesoftChannel) && !(resampler->samplingRate == ResamplerSamplingRate::SingleValue))
            {
                if (isDoublePrecision)
                    return ChannelDataType::Double;
                else
                    return ChannelDataType::Single;
            }
            else
                return static_cast<ChannelDataType>(dewesoftChannel->DataType);
        }();

        converter.setChannelDataType(inputChannelDataType);

		std::vector<int> channelIndex = Dewesoft::Utils::Dcom::Utils::toStdVec<int>(dewesoftChannel->IndexEx);
		const bool isDIChannel = channelIndex.size() == 4 && channelIndex[1] == 100;
        converter.setIsDIChannel(false);
		if (isDIChannel)
			converter.setIsDIChannel(true, channelIndex[3]);

        converter.setScaleData(resampler->samplingRate == ResamplerSamplingRate::SingleValue);
        double offset, scale;
        if (isDoublePrecision)
        {
            offset = dewesoftChannel->ScaleValueDouble(0);
            scale = dewesoftChannel->ScaleValueDouble(1) - offset;
        }
        else
        {
            offset = dewesoftChannel->ScaleValue(0);
            scale = dewesoftChannel->ScaleValue(1) - offset;
        }
        converter.setScaleAndOffset(scale, offset);
    }

    template <typename U = T>
    std::enable_if_t<!std::is_void_v<U>, Dewesoft::Processing::Api::Advanced::common_type_t<T>> getScalar(int pos = 0) const
    {
        if (resampler->samplingRate == ResamplerSamplingRate::SingleValue)
            pos = 0;
        else
            pos += resampler->pastSamplesRequiredForCalculation;
        if (pos < 0 || pos >= resampler->size())
            throw std::runtime_error("Invalid pos in getSample call");

        return converter.getScalar(static_cast<int>(valuesIndexForIthObject(pos)));
    }

    template <typename U = T>
    std::enable_if_t<is_instance<U, Dewesoft::Processing::Api::Advanced::DSVector>::value,
                     Dewesoft::Processing::Api::Advanced::common_type_t<U>>
    getVector(int pos = 0) const
    {
        pos += resampler->pastSamplesRequiredForCalculation;
        if (pos < 0 || pos >= resampler->size())
            throw std::runtime_error("Invalid pos in getSample call");

        return converter.getVector(static_cast<int>(valuesIndexForIthObject(pos)));
    }

    template <typename U = T>
    std::enable_if_t<is_instance<U, Dewesoft::Processing::Api::Advanced::DSMatrix>::value,
                     Dewesoft::Processing::Api::Advanced::common_type_t<U>>
    getMatrix(int pos = 0) const
    {
        pos += resampler->pastSamplesRequiredForCalculation;
        if (pos < 0 || pos >= resampler->size())
            throw std::runtime_error("Invalid pos in getSample call");

        return converter.getMatrix(static_cast<int>(valuesIndexForIthObject(pos)));
    }

    template <typename U>
    U getSample(int pos = 0) const
    {
        using namespace Dewesoft::Processing::Api::Advanced;
        if constexpr (is_instance<U, DSVector>::value)
        {
            if constexpr (std::is_same_v<U, DSVector<ComplexSingle>> || std::is_same_v<U, DSVector<ComplexDouble>>)
            {
                const auto* resampledData = static_cast<contained_t<U>*>(cache.directData);
                return U(&resampledData[valuesIndexForIthObject(pos)], &resampledData[valuesIndexForIthObject(pos + 1)]);
            }
            else
            {
                const auto* resampledData = static_cast<innermost_t<U>*>(cache.directData);
                return U(&resampledData[valuesIndexForIthObject(pos)], &resampledData[valuesIndexForIthObject(pos + 1)]);
            }
        }
        else if constexpr (is_instance<U, DSMatrix>::value)
        {
            if constexpr (std::is_same_v<U, DSMatrix<ComplexSingle>>)
            {
                const auto* resampledData = static_cast<contained_t<U>*>(cache.directData);
                return U(&resampledData[valuesIndexForIthObject(pos)],
                         &resampledData[valuesIndexForIthObject(pos + 1)],
                         cache.dimensionSize[0],
                         cache.dimensionSize[1]);
            }
            else
            {
                const auto* resampledData = static_cast<innermost_t<U>*>(cache.directData);
                return U(&resampledData[valuesIndexForIthObject(pos)],
                         &resampledData[valuesIndexForIthObject(pos + 1)],
                         cache.dimensionSize[0],
                         cache.dimensionSize[1]);
            }
        }
        else
        {
            if constexpr (std::is_same_v<U, Bytes>)
            {
                const size_t len = cache.sizeValues[valuesIndexForIthObject(pos)];
                if (len == 0)
                    return Bytes();
				// byteValues is an unsigned char** for legacy reasons, but actually holds
				// indices into binaryBuffer (meaning its values are [0, cache.binaryBufferSize) ).
				uintptr_t start = (std::ptrdiff_t)(cache.byteValues[valuesIndexForIthObject(pos)] - cache.binaryBuffer);
				uintptr_t end = (start + len) % cache.binaryBufferSize;
				if (start <= end)
				{
					return Bytes(&cache.binaryBuffer[start], &cache.binaryBuffer[end]);
				} else // if (end < start)
				{
					Bytes result;
					for ( ; start != end; start = (start + 1) % cache.binaryBufferSize)
						result.push_back(cache.binaryBuffer[start]);
					return result;
				}
            }
            else
            {
                return converter.extractValue(static_cast<U*>(cache.directData)[valuesIndexForIthObject(pos)]);
            }
        }
    }

    template <typename U = T>
    std::enable_if_t<std::is_same_v<U, Dewesoft::Processing::Api::Advanced::DSComplex>, Dewesoft::Processing::Api::Advanced::ComplexDouble>
    getComplexScalar(int pos = 0)
    {
        return getScalar(pos);
    }

    template <typename U = T>
    std::enable_if_t<std::is_same_v<U, Dewesoft::Processing::Api::Advanced::DSVector<Dewesoft::Processing::Api::Advanced::DSComplex>>,
                     Dewesoft::Processing::Api::Advanced::DSVector<Dewesoft::Processing::Api::Advanced::ComplexDouble>>
    getComplexVector(int pos = 0)
    {
        return getVector(pos);
    }

    template <typename U = T>
    std::enable_if_t<std::is_same_v<U, Dewesoft::Processing::Api::Advanced::DSMatrix<Dewesoft::Processing::Api::Advanced::DSComplex>>,
                     Dewesoft::Processing::Api::Advanced::DSMatrix<Dewesoft::Processing::Api::Advanced::ComplexDouble>>
    getComplexMatrix(int pos = 0)
    {
        return getMatrix(pos);
    }

private:
    Converter<Dewesoft::Processing::Api::Advanced::common_type_t<T>> converter;

    friend class ModuleBridge;
    friend class ModuleContextBridge;
};

using ScalarInputChannel = InputChannel<Dewesoft::Processing::Api::Advanced::DSNumber>;
using VectorInputChannel = InputChannel<Dewesoft::Processing::Api::Advanced::DSVector<Dewesoft::Processing::Api::Advanced::DSNumber>>;
using MatrixInputChannel = InputChannel<Dewesoft::Processing::Api::Advanced::DSMatrix<Dewesoft::Processing::Api::Advanced::DSNumber>>;

using ComplexScalarInputChannel = InputChannel<Dewesoft::Processing::Api::Advanced::DSComplex>;
using ComplexVectorInputChannel =
    InputChannel<Dewesoft::Processing::Api::Advanced::DSVector<Dewesoft::Processing::Api::Advanced::DSComplex>>;
using ComplexMatrixInputChannel =
    InputChannel<Dewesoft::Processing::Api::Advanced::DSMatrix<Dewesoft::Processing::Api::Advanced::DSComplex>>;

using BinaryInputChannel = InputChannel<Dewesoft::Processing::Api::Advanced::Bytes>;
