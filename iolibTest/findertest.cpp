#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "finder.h"

TEST(findertest , testSignatureQT)
{

    IO::path_string folder_path(L"e:\\Root\\");
    IO::path_list ext_list;

    ext_list.push_back(L".mov");
    auto finder = IO::Finder();
    finder.FindFiles(folder_path, ext_list);
}
