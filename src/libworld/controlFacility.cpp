// 
// This file is part of AT&C project which simulates virtual world of air traffic and ATC.
// Code licensing terms are available at https://github.com/felix-b/atc/blob/master/LICENSE
// 
#include <iomanip>
#include <sstream>
#include "libworld.h"

using namespace std;

namespace world
{
    shared_ptr<ControllerPosition> ControlFacility::tryFindPosition(ControllerPosition::Type type, const GeoPoint& location) const
    {
        return tryFindFirst<shared_ptr<ControllerPosition>>(m_positions, [this, type](const shared_ptr<ControllerPosition>& position) {
            return (position->type() == type);
        });
    }

    shared_ptr<ControllerPosition> ControlFacility::findPositionOrThrow(ControllerPosition::Type type, const GeoPoint& location) const 
    {
        auto positionOrNull = tryFindPosition(type, location);
        if (positionOrNull)
        {
            return positionOrNull;
        }

        stringstream errorMessage;
        errorMessage << setprecision(11)
                     << "Controller position type ["
                     << (int)type
                     << "] not found at facility ["
                     << m_callSign
                     << "] for location (" << location.latitude << "," << location.longitude << "). "
                     << m_positions.size() << " position(s) exist: ";
        for (const auto position : m_positions)
        {
            errorMessage << "[type=" << (int)position->type()
                         << "|" << position->frequency()->khz() << "|"
                         << position->callSign() << "] ";
        }

        throw runtime_error(errorMessage.str());
    }

    void ControlFacility::progressTo(chrono::microseconds timestamp)
    {
        for (const auto position : m_positions)
        {
            position->progressTo(timestamp);
        }
    }

    void ControlFacility::clearFlights()
    {
        for (const auto& position : m_positions)
        {
            position->clearFlights();
        }
    }

    // shared_ptr<ControllerPosition> ControlFacility::tryFindPosition(
    //     ControllerPosition::Type type, 
    //     const GeoPoint& location) const
    // {

    // }
}