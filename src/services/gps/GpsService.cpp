#include "GpsService.h"

#include <cstring>
#include <cstdlib>

void GpsService::updateFromCGNSINF(const char* line)
{
    char buffer[256];

    strncpy(buffer, line, sizeof(buffer));

    char* token = strtok(buffer, ",");

    int index = 0;

    while (token != nullptr)
    {
        switch (index)
        {
            case 1:
                data.valid = atoi(token) == 1;
                break;

            case 3:
                strncpy(data.timestamp, token, sizeof(data.timestamp));
                break;

            case 4:
                data.latitude = atof(token);
                break;

            case 5:
                data.longitude = atof(token);
                break;

            case 6:
                data.altitude = atof(token);
                break;

            case 7:
                data.speed = atof(token);
                break;
        }

        token = strtok(nullptr, ",");

        index++;
    }
}

const GpsData& GpsService::getData() const {
  return data;
}