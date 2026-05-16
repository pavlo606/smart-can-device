#pragma once

#include "GpsData.hpp"

class GpsService {
public:
    void updateFromCGNSINF(const char* line);

    const GpsData& getData() const;

private:
    GpsData data{};
};