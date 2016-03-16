/*
 * Copyright (c) 2013, German Aerospace Center (DLR)
 *
 * This file is part of libCOBC 0.5.
 *
 * It is distributed under the terms of the GNU General Public License with a
 * linking exception. See the file "LICENSE" for the full license governing
 * this code.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */
// ----------------------------------------------------------------------------

#ifndef COBC_TIME_DATE_H
#define COBC_TIME_DATE_H

#include "time_epoch.h"

namespace cobc
{
namespace time
{

// forward declaration
struct Date;

/**
 * Helper class to simplify the calculation of dates.
 *
 * Based on the following algorithm:
 * https://alcor.concordia.ca/~gpkatch/gdate-method.html
 */
struct DateUtils
{
    static int64_t
    getDay(Date date);

    static Date
    getDate(int64_t day);
};

/**
 * Represents a specific instant in time.
 *
 * The date is given in UTC and the conversion functions accommodates for leap
 * seconds in the time between January 1980 and ~2016 (last leap second
 * which is incorporated is from June 2015).
 *
 * \author Fabian Greif
 */
struct Date
{
    static Date
    fromUnixTime(UnixTime time);

    static UnixTime
    toUnixTime(const Date& date);

    /**
     * Check that given value represent a valid date.
     */
    bool
    isValid() const;

    bool
    operator==(const Date& other) const;

    bool
    operator!=(const Date& other) const;


    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

/**
 * GPS time representation as week and seconds within that week.
 */
struct GpsDate
{
    static GpsDate
    fromGpsTime(GpsTime time);

    static GpsTime
    toGpsTime(GpsDate date);


    // Week number (WN)
    uint16_t weekNumber;

    /**
     * Time Of Week (TOW)
     *
     * Number of seconds within the week. Range 0..604800.
     */
    uint32_t timeOfWeek;
};

}
}

#endif // COBC_TIME_DATE_H
