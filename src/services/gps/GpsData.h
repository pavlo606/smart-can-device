#pragma once

#include <stdbool.h>

struct GpsData {
    bool valid;

    float latitude;
    float longitude;

    float speed;
    float altitude;

    char timestamp[32];
};