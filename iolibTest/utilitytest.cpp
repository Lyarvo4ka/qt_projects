#include <gtest/gtest.h>
#include "utility.h"


TEST(Utilitytest , addBackSlashTest)
{
    ASSERT_STREQ(IO::addBackSlash(L"text").c_str() , L"text\\");
    ASSERT_STREQ(IO::addBackSlash(L"text\\").c_str() , L"text\\");
}

TEST(Utilitytest , isDirectoryAttributeTest)
{
    WIN32_FIND_DATA wFindData = WIN32_FIND_DATA();
    ASSERT_FALSE(IO::isDirectoryAttribute(wFindData));
    wFindData.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
    ASSERT_TRUE(IO::isDirectoryAttribute(wFindData));
}

TEST(Utilitytest , isOneDotOrTwoDotsTest)
{
    ASSERT_FALSE(IO::isOneDotOrTwoDots(L"..."));
    ASSERT_TRUE(IO::isOneDotOrTwoDots(L"."));
    ASSERT_TRUE(IO::isOneDotOrTwoDots(L".."));
}

TEST(Utilitytest , isPresentInListTest)
{
    IO::path_list theList = {L"aaa" , L"bbb" , L"ccc"};
    IO::path_string str = L"aaa";
    ASSERT_TRUE(IO::isPresentInList(str , theList));
    str = L"Bbb";
    ASSERT_TRUE(IO::isPresentInList(str , theList));
    str = L"CCC";
    ASSERT_TRUE(IO::isPresentInList(str , theList));
    str = L"ddd";
    ASSERT_FALSE(IO::isPresentInList(str , theList));
}
TEST(Utilitytest , toNumberStringTest)
{
    ASSERT_STREQ(IO::toNumberString(1000).c_str() ,L"1000" );
    ASSERT_STREQ(IO::toNumberString(2000398933504).c_str(),L"2000398933504");
}

TEST(Utilitytest , toHexStringTest)
{
    auto res = IO::toHexString(0xaaaa);
    ASSERT_TRUE(res.compare(L"aaaa") == 0);

    res = IO::toHexString(2000398933504);
    ASSERT_TRUE(res.compare(L"1d1c1115e00") == 0);

}

TEST(Utilitytest , insertNullsInFrontTest)
{
    auto res = IO::insertNullsInFront(IO::toHexString(0xaaaa));
    ASSERT_TRUE(res.compare(L"000000000000aaaa") == 0);

    res = IO::insertNullsInFront(IO::toHexString(2000398933504));
    ASSERT_TRUE(res.compare(L"000001d1c1115e00") == 0);

    res = IO::insertNullsInFront(IO::toHexString(0xaaaa) , 3);
    ASSERT_TRUE(res.compare(L"aaaa") == 0);

}

TEST(Utilitytest, toNumberExtensionTest)
{
    auto res = IO::toNumberExtension(1000,L".ext");
    ASSERT_TRUE(res.compare(L"0000000000001000.ext") == 0);
}

TEST(Utilitytest, offsetToPathTest)
{
    auto res = IO::offsetToPath(L"folder" ,0x80000 ,L".ext");
    ASSERT_TRUE(res.compare(L"folder\\0000000000000400.ext") == 0);
}

TEST(Utilitytest , isDigitStringTest)
{
    ASSERT_TRUE(IO::isDigitString("2020",4));
    ASSERT_TRUE(IO::isDigitString("20",2));
    ASSERT_FALSE(IO::isDigitString("aa",2));
}

TEST(Utilitytest , toSystemTimeTest)
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

TEST(Utilitytest , validateSystemTimeTest)
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

TEST(Utilitytest , isDigitsInDateStructTest)
{
    std::string digits_string = "2017:08:08 11:10:02";
    IO::DateStruct * ds = (IO::DateStruct *)digits_string.data();

    ASSERT_TRUE(IO::isDigitsInDateStruct(*ds));

    std::string wrong_string = "2017:AA:08 11:10:02";
    ds = (IO::DateStruct *)wrong_string.data();
    ASSERT_FALSE(IO::isDigitsInDateStruct(*ds));

    std::string trash_string = "asdasdasdasdasddasdasddasd";
    ds =(IO::DateStruct *)trash_string.data();
    ASSERT_FALSE(IO::isDigitsInDateStruct(*ds));
}

TEST(Utilitytest , isDateStringTest)
{
    std::string good_date = "2017:08:08 11:10:02";
    ASSERT_TRUE(IO::isDateString(good_date));

    std::string bad_date ="asdasdasdasdasdaasdasdad";
    ASSERT_FALSE(IO::isDateString(bad_date));

    std::string wrong_date ="2017:08:08 76:10:02";
    ASSERT_FALSE(IO::isDateString(wrong_date));
}








