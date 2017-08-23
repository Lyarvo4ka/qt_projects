#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "iodevice.h"


TEST(oidevicetest , testErrorOpenFile)
{
    IO::File file(L"z:Wrong_file");

    file.Open(IO::OpenMode::OpenRead);
    file.Open(IO::OpenMode::OpenWrite);
    file.Open(IO::OpenMode::Create);
    int k =1;
    k = 1;

}
TEST(oidevicetest , testErrorReadFile)
{

}
