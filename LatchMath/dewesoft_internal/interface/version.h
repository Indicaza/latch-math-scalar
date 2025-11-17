#pragma once
#include <functional>

struct DewesoftVersion
{
    long major = -1;
    long minor = -1;
    long dcomVersion = -1;

    bool operator<=(const DewesoftVersion& version) const
    {
        return dcomVersion <= version.dcomVersion;
    }
};

#define IF_COMPATIBLE_VERSION(featureVersion, scope) \
    if (featureVersion <= currentVersion)            \
    scope

constexpr DewesoftVersion binaryChannelsVersion = {0, 0, 375};
constexpr DewesoftVersion genericChannelsUpgradeVersion = {0, 0, 383};
constexpr DewesoftVersion basicSRDivSupport = {0, 0, 431};
constexpr DewesoftVersion calibrationSupportVersion = {0, 0, 432};
constexpr DewesoftVersion calibrationUpgradeVersion = {0, 0, 438};
