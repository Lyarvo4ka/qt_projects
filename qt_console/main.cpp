#include <QCoreApplication>


#include "quicktimeraw.h"
#include "finder.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if ( argc == 3)
    {
        std::string tmp_src = argv[1];
        IO::path_string src_dir(tmp_src.begin() , tmp_src.end());
        std::string tmp_dst = argv[2];
        IO::path_string dst_dir(tmp_dst.begin() , tmp_dst.end());


        IO::Finder finder;
        finder.add_extension(L".mov");

        finder.FindFiles(src_dir);

        auto fileList = finder.getFiles();

        for ( auto theFile : fileList )
        {
            auto str_date = IO::readQuickTimeDate(theFile);
            if ( str_date.empty())
                continue;

            IO::path_string new_date_str(str_date.begin(), str_date.end());

            boost::filesystem::path src_path(theFile);
            auto folder_path = src_path.parent_path().generic_wstring();
            auto only_name_path = src_path.stem().generic_wstring();
            auto ext = src_path.extension().generic_wstring();
            auto new_name = dst_dir + L"//" + new_date_str + L"-" + only_name_path + ext;

            std::wcout << L"File " << theFile;

            try
            {
                boost::filesystem::rename(theFile, new_name);
                std::cout << " successfully renamed to " << str_date << std::endl;
            }
            catch (const boost::filesystem::filesystem_error& e)
            {
                std::cout << "Error: " << e.what() << std::endl;
            }

        }

    }
    return a.exec();
}
