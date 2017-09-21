#ifndef DATE_H
#define DATE_H
#include "utility.h"

namespace IO {

const std::string VIDEO_expr = "YYYY:MM:DD hh:mm:ss"; //2017:08:08 11:10:02
const std::string AUDIO_expr = "YYYY:MM:DDhh:mm:ss"; //2017:08:0811:10:02


#pragma pack( push, 1)
struct DateStruct
{
    //YYYY:MM:DD hh:mm:ss
    char year[4];
    char colon1;
    char month[2];
    char colon2;
    char day[2];
};
#pragma pack(pop)
const std::size_t DateStructSize = sizeof(DateStruct);


#pragma pack( push, 1)
struct TimeStruct
{
    //hh:mm:ss
    char hour[2];
    char colon3;
    char min[2];
    char colon4;
    char sec[2];
};
#pragma pack(pop)
const std::size_t TimeStructSize = sizeof(TimeStruct);

inline std::string getDateFromSystemtime(const SYSTEMTIME & system_time)
{
    char tmp_buffer[255];
    ::GetDateFormatA(LOCALE_USER_DEFAULT, 0, &system_time, "yyyy-MM-dd", tmp_buffer, 255);
    std::string tmp_str = tmp_buffer;

    return tmp_str;
}

inline std::string getTimeFromSystemtime(const SYSTEMTIME & system_time)
{
    char tmp_buffer[255];
    ::GetTimeFormatA(LOCALE_USER_DEFAULT, 0, &system_time, "HH-mm-ss", tmp_buffer, 255);
    std::string tmp_str = tmp_buffer;

    return tmp_str;
}

inline std::string getDateAndTimeFromSystemtime(const SYSTEMTIME & system_time)
{
    return getDateFromSystemtime(system_time) + '-' + getTimeFromSystemtime(system_time);
}

inline bool isDigitString(const char * str , const uint32_t size)
{
    for( uint32_t i = 0 ; i < size ; ++i)
        if ( isdigit(static_cast<char>(str[i])) == 0)
            return false;
    return true;
}

inline SYSTEMTIME toSystemTime(const std::string & original_date)
{
    // 2017-06-02T13:25:56+03:00
    SYSTEMTIME sys_time = SYSTEMTIME();
    std::string tmp_str;
    try
    {
        auto year_str = original_date.substr(0, 4);
        if ( !isDigitString(year_str.c_str(), 4))
            return SYSTEMTIME();
        auto int_val = std::stol(year_str);
        if ( int_val > 1600 && int_val < 30828 )
            sys_time.wYear = (WORD)int_val;
        sys_time.wMonth = std::stoi(original_date.substr(5, 2));
        sys_time.wDay = std::stoi(original_date.substr(8, 2));
        sys_time.wHour = std::stoi(original_date.substr(11, 2));
        sys_time.wMinute = std::stoi(original_date.substr(14, 2));
        sys_time.wSecond = std::stoi(original_date.substr(17, 2));
    }
    catch (std::exception &e)
    {
        printf("Caught: %s" , e.what());
    }

    return sys_time;
}

inline bool validateSystemTime(const SYSTEMTIME & sys_time )
{
    if ( sys_time.wYear > 1600 && sys_time.wYear < 30828 )
    if ( sys_time.wMonth >= 1 && sys_time.wMonth <= 12 )
    if ( sys_time.wDay >= 1 && sys_time.wDay <= 31)
    if ( sys_time.wHour <= 23)
    if ( sys_time.wMinute <= 59)
    if ( sys_time.wSecond <= 59)
        return true;

    return false;
}

inline std::string parse_string_date(const std::string & original_date)
{
    // 2017-06-02T13:25:56+03:00
    auto sys_time = toSystemTime(original_date);
    if (validateSystemTime(sys_time))
        return getDateAndTimeFromSystemtime(sys_time);
    else
        return "";
}




inline bool isDigitsInDateStruct(const DateStruct & date_struct)
{
    if (isDigitString(date_struct.year,4))
    if (isDigitString(date_struct.month,2))
    if (isDigitString(date_struct.month,2))
        return true;

    return false;
}

inline bool isDigitsInTimeStruct(const TimeStruct & time_struct)
{
    if (isDigitString(time_struct.hour,2))
    if (isDigitString(time_struct.min,2))
    if (isDigitString(time_struct.sec,2))
        return true;

    return false;
}

inline bool isDateString(const std::string & date_string , const uint32_t delimiter_length = 1 )
{
//    auto str_len = date_string.length();
    if ( date_string.length() < DateStructSize + TimeStructSize + delimiter_length )
        return false;

    DateStruct * dateStruct = (DateStruct *)date_string.data();
    TimeStruct * timeStruct = (TimeStruct *)(date_string.data() + DateStructSize + delimiter_length);
    if (isDigitsInDateStruct(*dateStruct))
    if (isDigitsInTimeStruct(*timeStruct))
        return validateSystemTime(toSystemTime(date_string));

    return false;
}

inline bool isVideoDateString(const std::string & date_string )
{
    return isDateString(date_string);
}

inline bool isAudioDateString(const std::string & date_string )
{
    return isDateString(date_string , 0);
}

class DateParser
{
//private:
//    uint16_t year;
//    uint16_t month;
//    uint16_t day;
//    uint16_t hour;
//    uint16_t min;
//    uint16_t sec;
public:
    static std::string findDateinQTAtom( const char * data , const uint32_t size, const uint32_t delimiter_length = 1)
    {
        if ( size < VIDEO_expr.length())
            return "";

        DateStruct * pDateStruct = nullptr;
        TimeStruct * pTimeStruct = nullptr;

        for ( auto iByte = 0; iByte < size - VIDEO_expr.length(); ++iByte)
        {
            pDateStruct = (DateStruct *)&data[iByte];
            pTimeStruct = (TimeStruct *)&data[iByte + delimiter_length + DateStructSize ];
            if (isDigitsInDateStruct(*pDateStruct))
            if (isDigitsInTimeStruct(*pTimeStruct))
            {
                std::string date_string(data+iByte);
                return parse_string_date(date_string);
            }

        }
        return "";
    }

    //static


};
}
#endif // DATE_H
