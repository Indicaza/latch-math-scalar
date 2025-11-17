#pragma once
#include <type_traits>
#include "basic_types.h"
#include "enums.h"
#include "dcomlib/dcom_utils/variant_helper.h"
#include "dcomlib/dcom_utils/plugin_types.h"
#include "channels.h"

using Dimensions = std::vector<int>;
struct OutputChannelCache
{
    void* dataBuffer = nullptr;
    double* timeBuffer = nullptr;
    long dbPos = 0;
    long dbBufSize = 0;
    std::array<size_t, 2> dimensionSize;
    bool used = true;
};

class OutputChannelBase : public Channel
{
protected:
    CallInfo** callInfo;
    ChannelTimebase outputTimebase;
    ChannelDataType dataType = ChannelDataType::Single;
    ChannelDimension channelDimension = ChannelDimension::Scalar;
    OutputChannelCache cache;
    long samplesThisCalculate = 0;

    void IncDBPos(long numberOfSamples)
    {
        samplesThisCalculate += numberOfSamples;

        cache.dbPos = (cache.dbPos + numberOfSamples) % cache.dbBufSize;
    }

    void updateDBPos()
    {
        dewesoftChannel->IncDBSamples(samplesThisCalculate);
        samplesThisCalculate = 0;
    }

	void writeBytes(const Dewesoft::Processing::Api::Advanced::Bytes& bytes, Dewesoft::Processing::Api::Advanced::Time time)
	{
		// This is a helper function, because for whatever reason if I try to call AddAsyncBinarySample
		// directly from .addScalar, linker says it cannot find the definition and errors out.
		// But if the call is in this base class, linker is completely happy. /shrug
		dewesoftChannel->AddAsyncBinarySample(time, Dewesoft::Utils::Dcom::Utils::fromStdVec(bytes));
	}

    void writeText(const Dewesoft::Processing::Api::Advanced::Text& text, Dewesoft::Processing::Api::Advanced::Time time)
    {
        dewesoftChannel->AddAsyncString(text.c_str(), time);
    }

    void cacheBufferLocation()
    {
        if (dewesoftChannel && dewesoftChannel->Used)
        {
            cache.dataBuffer = reinterpret_cast<void*>(dewesoftChannel->GetDBAddress64());
            cache.timeBuffer = reinterpret_cast<double*>(dewesoftChannel->GetTSAddress64());
            cache.dbPos = dewesoftChannel->DBPos;
            cache.dbBufSize = dewesoftChannel->DBBufSize;
            cache.dimensionSize[0] = dewesoftChannel->ArrayInfo && dewesoftChannel->ArrayInfo->DimCount > 0
                                         ? static_cast<size_t>(dewesoftChannel->ArrayInfo->DimSizes[0])
                                         : 1;
            cache.dimensionSize[1] = dewesoftChannel->ArrayInfo && dewesoftChannel->ArrayInfo->DimCount > 1
                                         ? static_cast<size_t>(dewesoftChannel->ArrayInfo->DimSizes[1])
                                         : 1;
        }
    }

    template <typename T>
    void determineDataType()
    {
        using namespace Dewesoft::Processing::Api::Advanced;
        if constexpr (is_instance<T, DSVector>{})
            channelDimension = ChannelDimension::Vector;
        else if constexpr (is_instance<T, DSMatrix>{})
            channelDimension = ChannelDimension::Matrix;
        else
            channelDimension = ChannelDimension::Scalar;

        if constexpr (std::is_same_v<T, Byte> || std::is_same_v<T, DSVector<Byte>> || std::is_same_v<T, DSMatrix<Byte>>)
            dataType = ChannelDataType::Byte;
        else if constexpr (std::is_same_v<T, ShortInt> || std::is_same_v<T, DSVector<ShortInt>> || std::is_same_v<T, DSMatrix<ShortInt>>)
            dataType = ChannelDataType::ShortInt;
        else if constexpr (std::is_same_v<T, SmallInt> || std::is_same_v<T, DSVector<SmallInt>> || std::is_same_v<T, DSMatrix<SmallInt>>)
            dataType = ChannelDataType::SmallInt;
        else if constexpr (std::is_same_v<T, Word> || std::is_same_v<T, DSVector<Word>> || std::is_same_v<T, DSMatrix<Word>>)
            dataType = ChannelDataType::Word;
        else if constexpr (std::is_same_v<T, Integer> || std::is_same_v<T, DSVector<Integer>> || std::is_same_v<T, DSMatrix<Integer>>)
            dataType = ChannelDataType::Integer;
        else if constexpr (std::is_same_v<T, Single> || std::is_same_v<T, DSVector<Single>> || std::is_same_v<T, DSMatrix<Single>>)
            dataType = ChannelDataType::Single;
        else if constexpr (std::is_same_v<T, Int64> || std::is_same_v<T, DSVector<Int64>> || std::is_same_v<T, DSMatrix<Int64>>)
            dataType = ChannelDataType::Int64;
        else if constexpr (std::is_same_v<T, Double> || std::is_same_v<T, DSVector<Double>> || std::is_same_v<T, DSMatrix<Double>>)
            dataType = ChannelDataType::Double;
        else if constexpr (std::is_same_v<T, LongWord> || std::is_same_v<T, DSVector<LongWord>> || std::is_same_v<T, DSMatrix<LongWord>>)
            dataType = ChannelDataType::Longword;
        else if constexpr (std::is_same_v<T, Bytes> || std::is_same_v<T, DSVector<Bytes>> || std::is_same_v<T, DSMatrix<Bytes>>)
            dataType = ChannelDataType::Binary;
        else if constexpr (std::is_same_v<T, ComplexSingle> || std::is_same_v<T, DSVector<ComplexSingle>> ||
                           std::is_same_v<T, DSMatrix<ComplexSingle>>)
            dataType = ChannelDataType::ComplexSingle;
        else if constexpr (std::is_same_v<T, ComplexDouble> || std::is_same_v<T, DSVector<ComplexDouble>> ||
                           std::is_same_v<T, DSMatrix<ComplexDouble>>)
            dataType = ChannelDataType::ComplexDouble;
        else if constexpr (std::is_same_v<T, Text>)
            dataType = ChannelDataType::Text;
        else if constexpr (std::is_same_v < T, CANMessage>)
            dataType = ChannelDataType::CANMessage;
        else if constexpr (std::is_same_v < T, CANFDMessage>)
            dataType = ChannelDataType::CANFDMessage;
        else if constexpr (std::is_void_v<T>)
            ;
        else
            static_assert(false, "Data type not supported");  // Static assert should fail if the type does not match
    }

public:
    OutputChannelBase(IChannelPtr dewesoftChannel = nullptr, CallInfo** calculateCallInfo = nullptr)
        : Channel(dewesoftChannel)
        , callInfo(calculateCallInfo)
    {
        outputTimebase = timebase();
    }
    void setExpectedAsyncRate(float rate, ExpectedAsyncRateType type = ExpectedAsyncRateType::SemiFixed) const
    {
        dewesoftChannel->ExpectedAsyncRate = rate;
        dewesoftChannel->ExpectedAsyncRateType = static_cast<int>(type);
    }
    void setExpectedAsyncRate(const ExpectedAsyncRate& expectedAsyncRate) const
    {
        setExpectedAsyncRate(expectedAsyncRate.rate, expectedAsyncRate.type);
    }
    void setName(const std::string& name) const
    {
        dewesoftChannel->Name = name.c_str();
    }
    void setName(const std::wstring& name) const
    {
        dewesoftChannel->Name = name.c_str();
    }
    void setUnit(const std::string& unit) const
    {
        dewesoftChannel->Unit_ = unit.c_str();
    }
    void setUnit(const std::wstring& unit) const
    {
        dewesoftChannel->Unit_ = unit.c_str();
    }
    void setDescription(const std::string& description) const
    {
        dewesoftChannel->Description = description.c_str();
    }
    void setDescription(const std::wstring& description) const
    {
        dewesoftChannel->Description = description.c_str();
    }
    void setEnabled(const bool enabled) const
    {
        dewesoftChannel->Used = enabled;
    }

    ChannelDataType getDataType() const
    {
        return dataType;
    }

    ChannelDimension getDimension() const
    {
        return channelDimension;
    }

    void cacheChannelProperties()
    {
        cache.used = dewesoftChannel->Used;

        const bool isAsync = dewesoftChannel->Async;
        const bool isSingleValue = dewesoftChannel->IsSingleValue;

        if (isAsync)
            outputTimebase = ChannelTimebase::Asynchronous;
        else if (isSingleValue)
            outputTimebase = ChannelTimebase::SingleValue;
        else
            outputTimebase = ChannelTimebase::Synchronous;
    }

    friend class OutputChannels;
    friend class ModuleBridge;
    friend class ContextBridge;
};

template <typename T>
class OutputChannel : public OutputChannelBase
{
public:
    OutputChannel(IChannelPtr dewesoftChannel = nullptr, CallInfo** callInfo = nullptr)
        : OutputChannelBase(dewesoftChannel, callInfo)
    {
        determineDataType<T>();

        if constexpr (is_instance<T, Dewesoft::Processing::Api::Advanced::DSMatrix>{})
        {
            axes.push_back(Axis(dewesoftChannel, 0));
            axes.push_back(Axis(dewesoftChannel, 1));
        }
        else if constexpr (is_instance<T, Dewesoft::Processing::Api::Advanced::DSVector>{})
        {
            axes.push_back(Axis(dewesoftChannel, 0));
        }
    }

    template <typename U, typename R = T>
    std::enable_if_t<!is_instance<R, Dewesoft::Processing::Api::Advanced::DSVector>::value &&
                         !is_instance<R, Dewesoft::Processing::Api::Advanced::DSMatrix>::value ||
                     std::is_void<R>::value>
    addScalar(const U value, const Dewesoft::Processing::Api::Advanced::Time time = -1)
    {
        if (!cache.used)
            return;
        using namespace Dewesoft::Processing::Api::Advanced;

        const auto correctedTime = time == -1 && callInfo ? (*callInfo)->endBlockTime : time;
        if constexpr (std::is_void_v<T>)
        {
            const auto valueToWrite = value;
            switch (outputTimebase)
            {
                case ChannelTimebase::Asynchronous:
                {
                    cache.timeBuffer[cache.dbPos] = correctedTime;
                }
                case ChannelTimebase::Synchronous:
                {
                    const size_t dataSize = sizeof(U);
                    const size_t byteOffset = cache.dbPos * dataSize;
                    memcpy(static_cast<Byte*>(cache.dataBuffer) + byteOffset, &valueToWrite, dataSize);
                    IncDBPos(1);
                }
                break;
                case ChannelTimebase::SingleValue:
                {
                    const size_t dataSize = sizeof(U);
                    memcpy(static_cast<Byte*>(cache.dataBuffer), &valueToWrite, dataSize);
                    if (cache.dbPos == 0)
                    {
                        samplesThisCalculate = 1;
                        cache.dbPos = 1;
                    }
                }
                break;
            }
        } else if constexpr(std::is_same<T, Dewesoft::Processing::Api::Advanced::Bytes>::value)
        {
            writeBytes(static_cast<T>(value), correctedTime);
        }
        else if constexpr (std::is_same<T, Dewesoft::Processing::Api::Advanced::Text>::value)
        {
            writeText(static_cast<T>(value), correctedTime);
        }
        else
        {
            const auto valueToWrite = static_cast<T>(value);
            switch (outputTimebase)
            {
                case ChannelTimebase::Asynchronous:
                {
                    cache.timeBuffer[cache.dbPos] = correctedTime;
                }
                case ChannelTimebase::Synchronous:
                {
                    const size_t dataSize = sizeof(T);
                    const size_t byteOffset = cache.dbPos * dataSize;
                    memcpy(static_cast<Byte*>(cache.dataBuffer) + byteOffset, &valueToWrite, dataSize);
                    IncDBPos(1);
                }
                break;
                case ChannelTimebase::SingleValue:
                {
                    const size_t dataSize = sizeof(T);
                    memcpy(static_cast<Byte*>(cache.dataBuffer), &valueToWrite, dataSize);
                    if (cache.dbPos == 0)
                    {
                        samplesThisCalculate = 1;
                        cache.dbPos = 1;
                    }
                }
                break;
            }
        }
    }

    template <typename U, typename R = T>
    std::enable_if_t<is_instance<R, Dewesoft::Processing::Api::Advanced::DSVector>::value || std::is_void<R>::value> addVector(
        const Dewesoft::Processing::Api::Advanced::DSVector<U>& value, const Dewesoft::Processing::Api::Advanced::Time time = -1)
    {
        using namespace Dewesoft::Processing::Api::Advanced;
        if (!cache.used)
            return;

        const Time correctedTime = time == -1 && callInfo ? (*callInfo)->endBlockTime : time;

        if (cache.dimensionSize[0] != value.size())
            throw std::runtime_error("Incompatible dimensions");

        if constexpr (std::is_void_v<T>)
        {
            const std::vector<U> res(value.begin(), value.end());

            switch (outputTimebase)
            {
                case ChannelTimebase::Asynchronous:
                {
                    cache.timeBuffer[cache.dbPos] = correctedTime;
                    const size_t dataSize = cache.dimensionSize[0] * sizeof(U);
                    const size_t byteOffset = cache.dbPos * dataSize;
                    memcpy(static_cast<Byte*>(cache.dataBuffer) + byteOffset, res.data(), dataSize);

                    IncDBPos(1);
                }
                break;
                case ChannelTimebase::SingleValue:
                {
                    const size_t dataSize = cache.dimensionSize[0] * sizeof(U);
                    memcpy(static_cast<Byte*>(cache.dataBuffer), res.data(), dataSize);

                    if (cache.dbPos == 0)
                    {
                        samplesThisCalculate = 1;
                        cache.dbPos = 1;
                    }
                }
                break;
            }
        }
        else
        {
            const std::vector<contained_t<T>> res(value.begin(), value.end());

            switch (outputTimebase)
            {
                case ChannelTimebase::Asynchronous:
                {
                    cache.timeBuffer[cache.dbPos] = correctedTime;
                    const size_t dataSize = cache.dimensionSize[0] * sizeof(contained_t<T>);
                    const size_t byteOffset = cache.dbPos * dataSize;
                    memcpy(static_cast<Byte*>(cache.dataBuffer) + byteOffset, res.data(), dataSize);

                    IncDBPos(1);
                }
                break;
                case ChannelTimebase::SingleValue:
                {
                    const size_t dataSize = cache.dimensionSize[0] * sizeof(contained_t<T>);
                    memcpy(static_cast<Byte*>(cache.dataBuffer), res.data(), dataSize);

                    if (cache.dbPos == 0)
                    {
                        samplesThisCalculate = 1;
                        cache.dbPos = 1;
                    }
                }
                break;
            }
        }
    }

    template <typename U, typename R = T>
    std::enable_if_t<is_instance<R, Dewesoft::Processing::Api::Advanced::DSMatrix>::value || std::is_void<R>::value> addMatrix(
        const Dewesoft::Processing::Api::Advanced::DSMatrix<U>& value, const Dewesoft::Processing::Api::Advanced::Time time = -1)
    {
        using namespace Dewesoft::Processing::Api::Advanced;
        if (!cache.used)
            return;
        const Time correctedTime = time == -1 && callInfo ? (*callInfo)->endBlockTime : time;

        if (cache.dimensionSize[0] != value.m() || cache.dimensionSize[1] != value.n())
            throw std::runtime_error("Incompatible dimensions");

        if constexpr (std::is_void_v<T>)
        {
            const std::vector<U> res(value.begin(), value.end());

            switch (outputTimebase)
            {
                case ChannelTimebase::Asynchronous:
                {
                    cache.timeBuffer[cache.dbPos] = correctedTime;
                    const size_t dataSize = cache.dimensionSize[0] * cache.dimensionSize[1] * sizeof(U);
                    const size_t byteOffset = cache.dbPos * dataSize;
                    memcpy(static_cast<Byte*>(cache.dataBuffer) + byteOffset, res.data(), dataSize);

                    IncDBPos(1);
                }
                break;
                case ChannelTimebase::SingleValue:
                {
                    const size_t dataSize = cache.dimensionSize[0] * cache.dimensionSize[1] * sizeof(U);
                    memcpy(static_cast<Byte*>(cache.dataBuffer), res.data(), dataSize);
                    if (cache.dbPos == 0)
                    {
                        samplesThisCalculate = 1;
                        cache.dbPos = 1;
                    }
                }
                break;
            }
        }
        else
        {
            const std::vector<contained_t<T>> res(value.begin(), value.end());

            switch (outputTimebase)
            {
                case ChannelTimebase::Asynchronous:
                {
                    cache.timeBuffer[cache.dbPos] = correctedTime;
                    const size_t dataSize = cache.dimensionSize[0] * cache.dimensionSize[1] * sizeof(contained_t<T>);
                    const size_t byteOffset = cache.dbPos * dataSize;
                    memcpy(static_cast<Byte*>(cache.dataBuffer) + byteOffset, res.data(), dataSize);

                    IncDBPos(1);
                }
                break;
                case ChannelTimebase::SingleValue:
                {
                    const size_t dataSize = cache.dimensionSize[0] * cache.dimensionSize[1] * sizeof(contained_t<T>);
                    memcpy(static_cast<Byte*>(cache.dataBuffer), res.data(), dataSize);
                    if (cache.dbPos == 0)
                    {
                        samplesThisCalculate = 1;
                        cache.dbPos = 1;
                    }
                }
                break;
            }
        }
    }

    // Just for backwads compatibility
    template <typename U, typename R = T>
    std::enable_if_t<true == is_instance<U, std::complex>::value && !is_instance<U, Dewesoft::Processing::Api::Advanced::DSVector>::value &&
                     !is_instance<U, Dewesoft::Processing::Api::Advanced::DSMatrix>::value && !std::is_void<R>::value>
    addComplexScalar(const U value, const Dewesoft::Processing::Api::Advanced::Time time = -1)
    {
        addScalar(value, time);
    }

    template <typename U, typename R = T>
    std::enable_if_t<is_instance<U, std::complex>::value && is_instance<R, Dewesoft::Processing::Api::Advanced::DSVector>::value &&
                     !is_instance<R, Dewesoft::Processing::Api::Advanced::DSMatrix>::value && !std::is_void<R>::value>
    addComplexVector(const Dewesoft::Processing::Api::Advanced::DSVector<U>& value,
                     const Dewesoft::Processing::Api::Advanced::Time time = -1)
    {
        addVector(value, time);
    }

    template <typename U, typename R = T>
    std::enable_if_t<is_instance<U, std::complex>::value && !is_instance<R, Dewesoft::Processing::Api::Advanced::DSVector>::value &&
                     is_instance<R, Dewesoft::Processing::Api::Advanced::DSMatrix>::value && !std::is_void<U>::value>
    addComplexMatrix(const Dewesoft::Processing::Api::Advanced::DSMatrix<U>& value,
                     const Dewesoft::Processing::Api::Advanced::Time time = -1)
    {
        addMatrix(value, time);
    }

    template <typename U>
    void addSample(const U& value, const Dewesoft::Processing::Api::Advanced::Time time = -1)
    {
        if constexpr (is_instance<U, Dewesoft::Processing::Api::Advanced::DSVector>::value)
            addVector(value, time);
        else if constexpr (is_instance<U, Dewesoft::Processing::Api::Advanced::DSMatrix>::value)
            addMatrix(value, time);
        else
            addScalar(value, time);
    }

    template <typename U = T>
    std::enable_if_t<std::is_void<U>::value> setChannelDataType(const ChannelDataType dataType)
    {
        dewesoftChannel->SetDataType(static_cast<long>(dataType));
        this->dataType = dataType;
    }

    template <typename U = T>
    std::enable_if_t<std::is_void<U>::value> setChannelDimension(const ChannelDimension type)
    {
        channelDimension = type;
        const Dimensions dimensions = [&]() -> Dimensions {
            switch (channelDimension)
            {
                case ChannelDimension::Matrix:
                    return {1, 1};
                case ChannelDimension::Vector:
                    return {1};
                case ChannelDimension::Scalar:
                    return {};
            }
        }();

        if (!dimensions.empty() && dewesoftChannel)
        {
            dewesoftChannel->ArrayChannel = true;
            dewesoftChannel->ArrayInfo->DimCount = (long) dimensions.size();
            for (long d = 0; d < (long) dimensions.size(); ++d)
                dewesoftChannel->ArrayInfo->DimSizes[d] = dimensions[d];

            dewesoftChannel->ArrayInfo->Init();
            axes.clear();
            for (long d = 0; d < (long) dimensions.size(); ++d)
            {
                dewesoftChannel->ArrayInfo->AxisDef[d]->AxisType = atFloatLinearFunc;
                dewesoftChannel->ArrayInfo->AxisDef[d]->StartValue = 0;
                dewesoftChannel->ArrayInfo->AxisDef[d]->StepValue = 1;
                axes.push_back(Axis(dewesoftChannel, d));
            }
        }
    }

    void setChannelTimebase(const ChannelTimebase timebase)
    {
        const auto isAsync = timebase == ChannelTimebase::Asynchronous;
        const auto isSingleValue = timebase == ChannelTimebase::SingleValue;

        outputTimebase = timebase;

        if (dewesoftChannel)
        {
            dewesoftChannel->SetAsync(isAsync);
            if (isAsync)
                dewesoftChannel->ExpectedAsyncRate = NAN;
            dewesoftChannel->SetIsSingleValue(isSingleValue);
        }
    }
};

using ScalarOutputChannel = OutputChannel<Dewesoft::Processing::Api::Advanced::Single>;
using VectorOutputChannel = OutputChannel<Dewesoft::Processing::Api::Advanced::DSVector<Dewesoft::Processing::Api::Advanced::Single>>;
using MatrixOutputChannel = OutputChannel<Dewesoft::Processing::Api::Advanced::DSMatrix<Dewesoft::Processing::Api::Advanced::Single>>;
using ComplexScalarOutputChannel = OutputChannel<Dewesoft::Processing::Api::Advanced::ComplexSingle>;
using ComplexVectorOutputChannel =
    OutputChannel<Dewesoft::Processing::Api::Advanced::DSVector<Dewesoft::Processing::Api::Advanced::ComplexSingle>>;
using ComplexMatrixOutputChannel =
    OutputChannel<Dewesoft::Processing::Api::Advanced::DSMatrix<Dewesoft::Processing::Api::Advanced::ComplexSingle>>;
