#pragma once
#include <vector>
#include <functional>
#include <algorithm>
#include <complex>
#include "version.h"
#include "interface/basic_types.h"
#include "dcomlib/dcom_utils/plugin_types.h"
#include "enums.h"

class ModuleBridge;

enum class ChannelTimebase
{
    Unknown = 0,
    Synchronous = 1 << 0,
    Asynchronous = 1 << 1,
    SingleValue = 1 << 2,
    Any = 7
};
inline ChannelTimebase operator|(ChannelTimebase a, ChannelTimebase b)
{
    return (ChannelTimebase)((int) a | (int) b);
}
inline ChannelTimebase operator&(ChannelTimebase a, ChannelTimebase b)
{
    return (ChannelTimebase)((int) a & (int) b);
}

enum class ResamplerSamplingRate
{
    Synchronous = 0,
    AsynchronousSingleMaster = 1,
    SingleValue = 2
};

enum class ResamplerBase
{
    PerModule = 0,
    SharedModule = 1
};

enum class ExpectedAsyncRateType
{
    SemiFixed = 0,
    Variable = 1
};

enum class ResamplerBlockSizeType
{
    Constant = 0,
    Variable = 1
};

enum class ResamplerSupportsSRDivChannels
{
	No = 0,
	OnlyAllChannelsSyncAndSameSRDiv = 1
};

class InputChannelBase;

class Resampler
{
public:
    ResamplerSamplingRate samplingRate;
    int blockSizeInSamples;
    int pastSamplesRequiredForCalculation;
    int futureSamplesRequiredForCalculation;
    ResamplerBase resamplerBase;
    ResamplerBlockSizeType blockSizeType;
	ResamplerSupportsSRDivChannels supportsSRDivChannels;

    int size() const;
    void setMasterChannel(InputChannelBase* master);

    double acqSampleRate() const;
    double currentSampleRate() const;

private:
    void setCppResamplerProperties();
    void setResamplerOldMode(bool isOld);
    void getCppResamplerProperties();
    InputChannelBase* masterChannel = nullptr;
    ICppResamplerEnginePtr cppResampler;
    bool useOldResamplerMode = true;
    DewesoftVersion currentVersion;

    friend class ModuleBridge;
    friend class ContextBridge;
    friend class InputChannelBase;
};

struct ExpectedAsyncRate
{
    float rate;
    ExpectedAsyncRateType type;
};

class Axis
{
private:
    IChannelPtr dewesoftChannel;
    int axisIndex;

public:
    Axis(IChannelPtr chn, int axisIndex);
    ~Axis();
    void buildAxisValues(const Dewesoft::Processing::Api::Advanced::Vector& values);
    void buildAxisValues(double start, int count, double step = 1);
    void buildAxisValues(int count);
    void buildAxisValues(const std::vector<std::string>& values);
    void buildAxisValues(const std::vector<std::wstring>& values);
    void copyAxis(const Axis& axis);

    void setUnit(const std::string& unit);
    void setName(const std::string& name);
    void setUnit(const std::wstring& unit);
    void setName(const std::wstring& name);
    void setPrecision(int precision);

    std::wstring unit() const;
    std::wstring name() const;
    int precision() const;

    size_t size() const;
    double value(size_t index) const;
    double minValue() const;
    double maxValue() const;

    friend class OutputChannels;
    friend class Channel;
};

class Channel
{
protected:
    IChannelPtr dewesoftChannel = nullptr;
    virtual void setDewesoftChannel(IChannelPtr newChannel);
    void clearDewesoftChannel();

public:
    Channel(IChannelPtr dewesoftChannel = nullptr);

    operator bool() const;
    operator IChannelPtr() const;
    IChannelPtr DCOMChannel() const;

    bool isScalarChannel() const;
    bool isBinaryChannel() const;
    bool isVectorChannel() const;
    bool isMatrixChannel() const;
    bool isComplexChannel() const;

    ChannelDataType getChannelDataType() const;

    std::wstring name() const;
    std::wstring unit() const;
    ExpectedAsyncRate expectedAsyncRate() const;
    bool isEnabled() const;

    ChannelTimebase timebase() const;

    std::vector<Axis> axes;
};