//
// This file is part of AT&C project which simulates virtual world of air traffic and ATC.
// Code licensing terms are available at https://github.com/felix-b/atc/blob/master/LICENSE
//
#pragma once

#include <fstream>
#include <charconv>
#include "libworld.h"

using namespace std;
using namespace world;

// from https://stackoverflow.com/a/1120224
class CSVRow
{
private:
    // from https://stackoverflow.com/a/6089413
    static istream &safeGetline(istream &is, string &t)
    {
        t.clear();

        // The characters in the stream are read one-by-one using a std::streambuf.
        // That is faster than reading them one-by-one using the std::istream.
        // Code that uses streambuf this way must be guarded by a sentry object.
        // The sentry object performs various tasks,
        // such as thread synchronization and updating the stream state.

        istream::sentry se(is, true);
        streambuf *sb = is.rdbuf();

        for (;;)
        {
            int c = sb->sbumpc();
            switch (c)
            {
            case '\n':
                return is;
            case '\r':
                if (sb->sgetc() == '\n')
                    sb->sbumpc();
                return is;
            case streambuf::traits_type::eof():
                // Also handle the case when the last line has no line ending
                is.setstate(ios::eofbit);
                if (t.empty())
                {
                    // No data, row is not valid : stream extraction has to fail
                    is.setstate(ios::failbit);
                }
                return is;
            default:
                t += (char)c;
            }
        }
    }

public:
    string_view operator[](size_t index) const
    {
        if (index >= size())
            throw out_of_range{"CSVRow : index out of bounds"};
        return string_view(&m_line[m_data[index] + 1], m_data[index + 1] - (m_data[index] + 1));
    }
    size_t size() const
    {
        return m_data.size() - 1;
    }
    void readNextRow(istream &str)
    {
        safeGetline(str, m_line);

        m_data.clear();
        m_data.emplace_back(-1);
        string::size_type pos = 0;
        while ((pos = m_line.find(',', pos)) != string::npos)
        {
            m_data.emplace_back(pos);
            ++pos;
        }
        // This checks for a trailing comma with no data after it.
        pos = m_line.size();
        m_data.emplace_back(pos);
    }

private:
    string m_line;
    vector<int> m_data;
};

istream &operator>>(istream &str, CSVRow &data)
{
    data.readNextRow(str);
    return str;
}

class CsvParser
{
public:
    typedef function<void(CSVRow &)> LineRedCallback;

    static void parse(istream &input, LineRedCallback callback)
    {
        CSVRow row;

        try
        {
            while (input >> row)
            {
                callback(row);
            }
        }
        // catch the fail exception here if thz stream has the failbit exception set
        catch (ifstream::failure e)
        {
            if (!input.eof())
                // rethrow the exception if we are not at eof
                throw;
        }
    }
};

struct OpenFlightsAirline
{
private:
    string m_icao;
    string m_callsign;

public:
    const string &callsign() const { return m_callsign; }
    const string &icao() const { return m_icao; }

public:
    OpenFlightsAirline(string_view _icao, string_view _callsign) : m_icao(_icao),
                                                                             m_callsign(_callsign){};
};

class OpenFlightsRoutes : public world::WorldRoutes
{
private:
    friend class OpenFlightDataReader;

private:
    unordered_map<string, string> m_airportIata2Icao;
    unordered_map<string, string> m_airframeIata2Icao;
    unordered_map<int, shared_ptr<OpenFlightsAirline>> m_airlines;
    vector<shared_ptr<world::WorldRoutes::Route>> m_routes;
    unordered_map<string, vector<shared_ptr<world::WorldRoutes::Route>>> m_routesFrom;
    unordered_map<string, vector<shared_ptr<world::WorldRoutes::Route>>> m_routesTo;
    random_selector<default_random_engine> m_selector;

public:
    // seed the RNG with current time,
    OpenFlightsRoutes(default_random_engine randomGenerator = default_random_engine(chrono::system_clock::now().time_since_epoch().count()))
    {
        m_selector = random_selector<default_random_engine>(randomGenerator);
    }
    // Used to force Rng for testing purpose
    void setRng(default_random_engine randomGenerator)
    {
        m_selector = random_selector<default_random_engine>(randomGenerator);
        ;
    }
    const shared_ptr<Route> findRandomRouteFrom(const string &fromICAO, const string &airframe, const vector<string> &allowedAirlines)
    {
        return findRandomRoute( getValueOrThrow(m_routesFrom, fromICAO), airframe, allowedAirlines);
    }
    const shared_ptr<Route> findRandomRouteTo(const string &toICAO, const string &airframe, const vector<string> &allowedAirlines)
    {
        return findRandomRoute( getValueOrThrow(m_routesTo, toICAO), airframe, allowedAirlines);
    }

private:

    const shared_ptr<Route> findRandomRoute(vector<shared_ptr<world::WorldRoutes::Route>> routes, const string &airframe, const vector<string> &allowedAirlines)
    {
        int totalRoutes = routes.size();
        do
        {
            totalRoutes--;

            auto route = m_selector(routes);
            auto allowedAirFrames = route->usedAirframes();
            // Any airline is allowed if allowedAirlines is empty
            auto routeAirline = (allowedAirlines.size() > 0) ? route->airline() : "";
            // Airframe asked must be used on this route
            if (!hasStringInsensitive(allowedAirFrames, airframe))
                continue;

            // Route must be operated by one of the airlines passed as a parameter
            if (!hasStringInsensitive(allowedAirlines, routeAirline))
                continue;

            // return found route
            return route;
        } while (totalRoutes > 0);

        // No route found
        return nullptr;
    }

    bool hasStringInsensitive(const vector<string> v, const string &s)
    {
        // An empty string is always valid
        if (s.size())
            return true;

        // An empty vector cannot have our string
        if (v.size() == 0)
            return false;

        return hasAny<string>(v, [s](string test) {
            return (stricmp(s.c_str(), test.c_str()) == 0);
            });
    }
};

class OpenFlightDataReader
{
private:
    shared_ptr<HostServices> m_host;
    shared_ptr<OpenFlightsRoutes> m_datas;

public:
private:
public:
    explicit OpenFlightDataReader(shared_ptr<HostServices> _host) : m_host(_host)
    {
        m_datas = make_shared<OpenFlightsRoutes>();
    };

public:
    shared_ptr<WorldRoutes> getWorldRoutes() { return m_datas; }

    // Parsing OpenFlights airpors.dat file.
    // Generates a mapping from iata to icao
    //
    // Columns are :
    //
    // - Airport ID Unique OpenFlights identifier for this airport.
    // - Name       Name of airport. May or may not contain the City name.
    // - City       Main city served by airport. May be spelled differently from Name.
    // - Country    Country or territory where airport is located. See Countries to cross-reference to ISO 3166-1 codes.
    // - IATA       3-letter IATA code. Null if not assigned/unknown.
    // - ICAO       4-letter ICAO code. Null if not assigned.
    // - Latitude   Decimal degrees, usually to six significant digits. Negative is South, positive is North.
    // - Longitude  Decimal degrees, usually to six significant digits. Negative is West, positive is East.
    // - Altitude   In feet.
    // - Timezone   Hours offset from UTC. Fractional hours are expressed as decimals, eg. India is 5.5.
    // - DST        Daylight savings time. One of E (Europe), A (US/Canada), S (South America), O (Australia), Z (New Zealand), N (None) or U (Unknown). See also: Help: Time
    // - Tz database time zone Timezone in "tz" (Olson) format, eg. "America/Los_Angeles".
    // - Type       Type of the airport. Value "airport" for air terminals, "station" for train stations, "port" for ferry terminals and "unknown" if not known. In airports.csv, only type=airport is included.
    // - Source     Source of this data. "OurAirports" for data sourced from OurAirports, "Legacy" for old data not matched to OurAirports (mostly DAFIF), "User" for unverified user contributions. In airports.csv, only source=OurAirports is included.
    void readAirports(istream &input)
    {
        CsvParser::parse(input,
                         [this](CSVRow &row) {
                             if ((row.size() == 14) && isValidString(row[4]) && isValidString(row[5]))
                             {
                                 string iata(row[4].substr(1, row[4].size() - 2));
                                 string icao(row[5].substr(1, row[5].size() - 2));
                                 m_datas->m_airportIata2Icao[iata] = icao;
                             }
                         });
        m_host->writeLog("OPENFLIGHTS| Airports parsed : %d association IATA->ICAO found", m_datas->m_airportIata2Icao.size());
    }

    // Parsing OpenFlights planes.dat file.
    // Generates a mapping from iata to icao
    //
    // Columns are :
    //
    // Name         Full name of the aircraft.
    // IATA code    Unique three-letter IATA identifier for the aircraft.
    // ICAO code    Unique four-letter ICAO identifier for the aircraft.
    void readPlanes(istream &input)
    {
        CsvParser::parse(input,
                         [this](CSVRow &row) {
                             if ((row.size() == 3) && isValidString(row[1]) && isValidString(row[2]))
                             {
                                 string iata(row[1].substr(1, row[1].size() - 2));
                                 string icao(row[2].substr(1, row[2].size() - 2));
                                 m_datas->m_airframeIata2Icao[iata] = icao;
                             }
                         });
        m_host->writeLog("OPENFLIGHTS| Planes parsed : %d association IATA->ICAO found", m_datas->m_airframeIata2Icao.size());
    }

    // Parsing OpenFlights airlines.dat file.
    // Stores ICAO and callsign for an airline.
    //
    // Columns are :
    //
    // Airline  ID Unique OpenFlights identifier for this airline.
    // Name     Name of the airline.
    // Alias    Alias of the airline. For example, All Nippon Airways is commonly known as "ANA".
    // IATA     2-letter IATA code, if available.
    // ICAO     3-letter ICAO code, if available.
    // Callsign Airline callsign.
    // Country  Country or territory where airport is located. See Countries to cross-reference to ISO 3166-1 codes.
    // Active   "Y" if the airline is or has until recently been operational, "N" if it is defunct. This field is not reliable: in particular, major airlines that stopped flying long ago, but have not had their IATA code reassigned (eg. Ansett/AN), will incorrectly show as "Y".
    void readAirlines(istream &input)
    {
        int lineCount = 0;
        CsvParser::parse(input,
                         [&, this](CSVRow &row) {
                             lineCount++;
                             if ((row.size() == 8) && isValidString(row[4]) && isValidString(row[5]))
                             {
                                 try
                                 {
                                     int openflightAirlineId = svtoi(row[0]);
                                     shared_ptr<OpenFlightsAirline> airline = make_shared<OpenFlightsAirline>(
                                         row[4].substr(1, row[4].size() - 2), 
                                         row[5].substr(1, row[5].size() - 2));
                                     m_datas->m_airlines[openflightAirlineId] = airline;
                                 }
                                 catch (const exception &e)
                                 {
                                     m_host->writeLog("OPENFLIGHTS| Exception [%s] airline route at line %d", e.what(), lineCount);
                                 }
                             }
                         });
        // An airline can be invalid if the ICAO or callsign is invalid
        m_host->writeLog("OPENFLIGHTS| Airlines parsed : %d/%d valid airlines found", m_datas->m_airlines.size(), lineCount);
    }

    // Parsing OpenFlights routes.dat file.
    // Stores routes cf. world::WorldRoutes::Route.
    //
    // Columns are :
    //
    // Airline                  2-letter (IATA) or 3-letter (ICAO) code of the airline.
    // Airline ID               Unique OpenFlights identifier for airline (see Airline).
    // Source airport           3-letter (IATA) or 4-letter (ICAO) code of the source airport.
    // Source airport ID        Unique OpenFlights identifier for source airport (see Airport)
    // Destination airport      3-letter (IATA) or 4-letter (ICAO) code of the destination airport.
    // Destination airport ID   Unique OpenFlights identifier for destination airport (see Airport)
    // Codeshare                "Y" if this flight is a codeshare (that is, not operated by Airline, but another carrier), empty otherwise.
    // Stops                    Number of stops on this flight ("0" for direct)
    // Equipment                3-letter codes for plane type(s) generally used on this flight, separated by spaces
    void readRoutes(istream &input)
    {
        int lineCount = 0;
        CsvParser::parse(input,
                         [&, this](CSVRow &row) {
                             lineCount++;
                             if ((row.size() == 9) && isValidEntry(row[1]) && isValidEntry(row[2]) && isValidEntry(row[4]))
                             {
                                 try
                                 {
                                     int openflightAirlineId = svtoi(row[1]);
                                     string iataDeparture(row[2]);
                                     string iataArrival(row[4]);
                                     auto icaoDeparture = getValueOrThrow(m_datas->m_airportIata2Icao, iataDeparture);
                                     auto icaoArrival = getValueOrThrow(m_datas->m_airportIata2Icao, iataArrival);
                                     auto iataAirframes = split(string(row[8]), ' ');
                                     vector<string> icaoAirframes;
                                     for (string iataAirframe : iataAirframes)
                                     {
                                         try
                                         {
                                             icaoAirframes.push_back(getValueOrThrow(m_datas->m_airframeIata2Icao, iataAirframe));
                                         }
                                         catch(const exception& e)
                                         {
                                             // Nothing to do if the iata code for one of the planes operating the routes 
                                             // has no ICAO equivalent. but the route might still be valid
                                         }
                                     }
                                     auto airline = getValueOrThrow(m_datas->m_airlines, openflightAirlineId);
                                     auto route = make_shared<world::WorldRoutes::Route>(
                                         icaoDeparture,
                                         icaoArrival,
                                         airline->icao(),
                                         airline->callsign(),
                                         move(icaoAirframes));

                                     m_datas->m_routes.push_back(route);
                                     m_datas->m_routesFrom[icaoDeparture].push_back(route);
                                     m_datas->m_routesTo[icaoArrival].push_back(route);
                                 }
                                 catch (const exception &e)
                                 {
                                     //  m_host->writeLog("OPENFLIGHTS| Exception [%s] parsing route at line %d", e.what(), lineCount);
                                 }
                             }
                         });
        // A route can be invalid if we cannot find the icao code of one of the airports (stored as iata in openflights files)
        m_host->writeLog("OPENFLIGHTS| Routes parsed : %d/%d valid routes found", m_datas->m_routes.size(), lineCount);
    }

private:
    // A value of \N means an invalid/unknown field  in openflights files
    bool isValidEntry(string_view value)
    {
        return (value.compare("\\N") != 0);
    }
    bool isValidString(string_view value)
    {
        return (isValidEntry(value) && (value.front() == '"') && (value.back() == '"'));
    }

    // Convert a string view to an int with exception handling
    int svtoi(string_view v)
    {
        int intValue;
        auto result = from_chars(v.data(), v.data() + v.size(), intValue);
        // These two exceptions reflect the behavior of std::stoi.
        if (result.ec == errc::invalid_argument)
        {
            throw invalid_argument(string("svtoi invalid_argument") + string(v));
        }
        else if (result.ec == errc::result_out_of_range)
        {
            throw out_of_range{"out_of_range"};
        }
        return intValue;
    }

private:
};
