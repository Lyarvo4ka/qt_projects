#include <QCoreApplication>


#include "quicktimeraw.h"
#include "finder.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

 //   if ( argc == 3)
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
            //finder.testSignatureMP4(theFile);
//        }
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
    else
       std::cout << "Wrong params" << std::endl;

    return a.exec();
}


//IO::File src_file(L"d:\\incoming\\42855\\xor.bin");
//if ( !src_file.Open(IO::OpenMode::OpenRead))
//    return -1;

//IO::File dst_file(L"d:\\incoming\\42855\\xor_ff.bin");
//if ( !dst_file.Open(IO::OpenMode::Create))
//    return -1;

//const uint32_t page_size = 18448;
//uint8_t buff[page_size];
//uint64_t offset = 0;
//uint32_t marker_start = 1024;
//uint32_t bytes_read = 0;
// uint32_t bytes_written = 0;

//while(offset<src_file.Size())
//{
//   src_file.setPosition(offset);
//   bytes_read = src_file.ReadData(buff , page_size);
//   if ( bytes_read == 0)
//       break;

//   memset(buff + marker_start , 0xFF , 8);

//   bytes_written = dst_file.WriteData(buff , page_size);
//   if ( bytes_written == 0)
//       break;
//   offset += page_size;
//}
