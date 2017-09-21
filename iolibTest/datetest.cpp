#include <gtest/gtest.h>
#include "date.h"


TEST(datetest , isDigitStringTest)
{
    ASSERT_TRUE(IO::isDigitString("2020",4));
    ASSERT_TRUE(IO::isDigitString("20",2));
    ASSERT_FALSE(IO::isDigitString("aa",2));
}

TEST(datetest , toSystemTimeTest)
{
    std::string good_date = "2017:11:22 11:31:45";
    auto sys_time = IO::toSystemTime(good_date);

    ASSERT_EQ(sys_time.wYear, 2017);
    ASSERT_EQ(sys_time.wMonth, 11);
    ASSERT_EQ(sys_time.wDay, 22);
    ASSERT_EQ(sys_time.wHour, 11);
    ASSERT_EQ(sys_time.wMinute , 31);
    ASSERT_EQ(sys_time.wSecond , 45);

    std::string wrong_date = "wrongwrongwrongwrong";
    sys_time = IO::toSystemTime(wrong_date);
    ASSERT_FALSE(IO::validateSystemTime(sys_time));
}

TEST(datetest , validateSystemTimeTest)
{
    std::string good_date ="2017:08:08 11:10:02";
    auto sys_time = IO::toSystemTime(good_date);
    ASSERT_TRUE(IO::validateSystemTime(sys_time));

    std::string wromg_month_28 ="2017:28:18 16:17:32";
    sys_time = IO::toSystemTime(wromg_month_28);
    ASSERT_FALSE(IO::validateSystemTime(sys_time));

    std::string wromg_hour_42 ="2017:08:08 42:10:02";
    sys_time = IO::toSystemTime(wromg_hour_42);
    ASSERT_FALSE(IO::validateSystemTime(sys_time));
}

TEST(datetest , isDigitsInDateStructTest)
{
    std::string digits_string = "2017:08:08";
    IO::DateStruct * ds = (IO::DateStruct *)digits_string.data();

    ASSERT_TRUE(IO::isDigitsInDateStruct(*ds));

    std::string wrong_string = "2017:AA:08";
    ds = (IO::DateStruct *)wrong_string.data();
    ASSERT_FALSE(IO::isDigitsInDateStruct(*ds));

    std::string trash_string = "asdasdasdasdasddasdasddasd";
    ds =(IO::DateStruct *)trash_string.data();
    ASSERT_FALSE(IO::isDigitsInDateStruct(*ds));
}

TEST(datetest , isDigitsInTimeStructTest)
{
    std::string digits_string = "11:10:02";
    IO::TimeStruct * ds = (IO::TimeStruct *)digits_string.data();

    ASSERT_TRUE(IO::isDigitsInTimeStruct(*ds));

    std::string wrong_string = "10:12:0A";
    ds = (IO::TimeStruct *)wrong_string.data();
    ASSERT_FALSE(IO::isDigitsInTimeStruct(*ds));

    std::string trash_string = "asdasdasdasdasddasdasddasd";
    ds =(IO::TimeStruct *)trash_string.data();
    ASSERT_FALSE(IO::isDigitsInTimeStruct(*ds));
}

TEST(datetest , isDateStringTest)
{
    std::string good_date = "2017:08:08 11:10:02";
    ASSERT_TRUE(IO::isDateString(good_date));

    std::string audio_date = "2017:08:0811:10:02";
    ASSERT_TRUE(IO::isDateString(audio_date,0));

    std::string bad_date ="asdasdasdasdasdaasdasdad";
    ASSERT_FALSE(IO::isDateString(bad_date));

    std::string wrong_date ="2017:08:08 76:10:02";
    ASSERT_FALSE(IO::isDateString(wrong_date));
}
