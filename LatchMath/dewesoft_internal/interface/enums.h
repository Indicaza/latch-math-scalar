#pragma once
#include <vector>
#include <string>

enum class ChannelDataType : long
{
    Byte = 0,
    ShortInt = 1,
    SmallInt = 2,
    Word = 3,
    Integer = 4,
    Single = 5,
    Int64 = 6,
    Double = 7,
    Longword = 8,
    ComplexSingle = 9,
    ComplexDouble = 10,
    Text = 11,
    Binary = 12,
    CANMessage = 13,
    CANFDMessage = 14,
    Bytes8 = 15,
    Bytes16 = 16,
    Bytes32 = 17,
    Bytes64 = 18
};

enum class InputChannelDataType : long
{
    Byte = 0,
    ShortInt = 1,
    SmallInt = 2,
    Word = 3,
    Integer = 4,
    Single = 5,
    Int64 = 6,
    Double = 7,
    Longword = 8,
    ComplexSingle = 9,
    ComplexDouble = 10,
    Text = 11,
    Binary = 12,
    CANMessage = 13,
    CANFDMessage = 14,
    Bytes8 = 15,
    Bytes16 = 16,
    Bytes32 = 17,
    Bytes64 = 18,
    Any,
    ComplexNumber,
    Complex,
    Number,
    Float,
    Ordinal
};

enum class ChannelDimension : long
{
    Scalar,
    Vector,
    Matrix
};

enum class GroupType : long
{
    Control,
    AI,
    Pad,
    Math,
    CAN,
    GPS,
    CNT,
    DI,
    Plugins,
    Power,
    COM,
    AO,
    Remote,
    MathEx,
    VAR,
    Video,
    Import
};

enum class MarkerValueType : long
{
    Time = 0,
    Value = 1,
    Axis0 = 2,
    Axis1 = 3,
    //Axis2 = 4,
    //AxisPair = 5,
    Result = 6
};

enum class MarkerChannelType : long
{
    Scalar = 0,
    Vector = 1,
    Matrix = 2,
    None = 3
};

enum class MarkerStyle : long
{
    Point = 0,
    Result = 1,
    XYLine = 2,
    Region = 3,
    LineX = 4,
    LineY = 5,
};

enum class MultipassType : long
{
    None = 0,
    Forward = 1,
    Backward = 2
};

enum class CalibrationStatusStatus : long
{
    None = 0,
    Settling = 1,
    InProgress = 2, 
    Done = 3, 
    Failed = 4
};


class ModuleBridge;

struct CalibrationStatus
{
private:
    CalibrationStatusStatus status;
    int percentDone;
    std::string additionalInfo;

    CalibrationStatus(CalibrationStatusStatus aStatus = CalibrationStatusStatus::None, int aPercentDone = 0, const std::string& aAdditionalInfo = "")
        : status(aStatus)
        , percentDone(aPercentDone)
        , additionalInfo(aAdditionalInfo) {};

public:
    static CalibrationStatus none() { return {CalibrationStatusStatus::None}; }
    static CalibrationStatus settling() { return {CalibrationStatusStatus::Settling}; }
    static CalibrationStatus inProgress(int percentDone = -1) { return {CalibrationStatusStatus::InProgress, percentDone}; }
    static CalibrationStatus done() { return {CalibrationStatusStatus::Done}; }
    static CalibrationStatus failed(const std::string& info, int percentDone = 100) { return {CalibrationStatusStatus::Failed, percentDone, info}; }

    
    friend class ModuleBridge;
};

struct CalibrationDescription
{
public:
    std::wstring description;
    std::wstring additionalCalibrationInfo;
    double lastCalibrated;
    std::wstring displayName;
    bool mandatoryCalibration;

    CalibrationDescription(const std::wstring& aDisplayName = L"",
                           const std::wstring& aDescription = L"",
                           bool aMandatoryCalibration = false,
                           const std::wstring& aAdditionalCalibrationInfo = L"",
                           double aLastCalibrated = 0)
        : displayName(aDisplayName)
        , description(aDescription)
        , mandatoryCalibration(aMandatoryCalibration)
        , lastCalibrated(aLastCalibrated)
        , additionalCalibrationInfo(aAdditionalCalibrationInfo){};
};