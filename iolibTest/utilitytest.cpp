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








