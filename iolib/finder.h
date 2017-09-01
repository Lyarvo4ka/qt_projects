#ifndef FINDER_H
#define FINDER_H

#include "iofs.h"
#include <iostream>
#include <boost/filesystem.hpp>
#include <locale>
#include <codecvt>
#include <boost/algorithm/string.hpp>
#include <algorithm>

#include "IODevice.h"
#include "dbf.h"
#include "quicktimeraw.h"


namespace IO
{
	inline int NotNullPosFromEnd(const Buffer & buffer)
	{
		int pos = buffer.data_size - 1;
		while (pos >= 0)
		{
			if (buffer.data[pos] != 0)
				return (buffer.data_size - pos);
			--pos;
		}
		return 0;
	}

	inline void removeNullsFromEndFile(const path_string & file_path , uint32_t sizeToTest = 4096)
	{
		File file(file_path);
		if (!file.Open(OpenMode::OpenWrite))
		{
			wprintf_s(L"Error open file.\n");
			return;
		}

		auto file_size = file.Size();
		Buffer buffer(sizeToTest);
		if (file_size >= sizeToTest)
		{
//			uint64_t lastBlock = file_size /*- sizeToTest*/;
			file.setPosition(0);
			auto bytesRead = file.ReadData(buffer.data, buffer.data_size);
			if (bytesRead == sizeToTest)
			{
				int not_null = NotNullPosFromEnd(buffer);
				if (not_null > 0)
				{
					uint64_t new_size = file_size - not_null + 1;
					file.setSize(new_size);
					wprintf_s(L"FIle size has been changed %s.\n", file_path.c_str());
				}
			}
		}

	}

//	inline void addDateToFile(const path_string & file_path)
//	{
		//File file(file_path);
		//if (!file.Open(OpenMode::OpenWrite))
		//{
		//	wprintf_s(L"Error open file.\n");
		//	return;
		//}

		//const char cmp_to[] = { 0xC2 , 0xF0 , 0xE5 , 0xEC , 0xFF , 0x3A };

		//auto file_size = file.Size();
		//Buffer buffer(file_size);
		//auto bytesRead = file.ReadData(buffer.data, buffer.data_size);
		//file.Close();

		//bool bFound = false;
		//uint32_t offset_pos = 0;
		//for (offset_pos = 0; offset_pos < file_size; ++offset_pos)
		//	if (memcmp(buffer.data + offset_pos, cmp_to, SIZEOF_ARRAY(cmp_to)) == 0 )
		//	{
		//		bFound = true;
		//		break;
		//	}


		//if (!bFound)
		//{
		//	wprintf_s(L"Not found string!!!!.\n");
		//	return;
		//}
		//const uint32_t date_size = 19 + 1;
		////const uint32_t date_offset = 0x37;
		//char raw_date[date_size];
		//ZeroMemory(raw_date, date_size);

		//memcpy(raw_date, buffer.data + offset_pos + SIZEOF_ARRAY(cmp_to) + 1, date_size - 1);

		//std::string date_string(raw_date);
		//std::replace(date_string.begin(), date_string.end(), ' ', '-');
		//std::replace(date_string.begin(), date_string.end(), '.', '-');
		//std::replace(date_string.begin(), date_string.end(), ':', '-');

		//IO::path_string new_date_str(date_string.begin(), date_string.end());

		//boost::filesystem::path src_path(file_path);
		//auto folder_path = src_path.parent_path().generic_wstring();
		//auto only_name_path = src_path.stem().generic_wstring();
		//auto ext = src_path.extension().generic_wstring();
		//auto new_name = folder_path + L"//" + only_name_path + L"-" + new_date_str + ext;

		//try
		//{
		//	boost::filesystem::rename(file_path, new_name);
		//}
		//catch (const boost::filesystem::filesystem_error& e)
		//{
		//	std::cout << "Error: " << e.what() << std::endl;
		//}

		//
		//int k = 1;
		//k = 1;
//	}



	struct pck_t
	{
		char header[3];
		char reserved[29];
		char digits[15];
		char text[33];
	}; 

	class Finder
	{
	private:
		DirectoryNode::Ptr rootFolder_;
		path_list list_ext_;
		path_list files_;

	public:
		void add_extension(path_string ext)
		{
			list_ext_.push_back(ext);
		}
		Finder()
		{

		}
		void FindFiles(const path_string & folder)
		{
			this->rootFolder_ = DirectoryNode::CreateDirectoryNode(folder);
			Find(rootFolder_);
		}
		void FindFiles(const path_string & folder, const path_list & list_extensions)
		{
			this->list_ext_ = list_extensions;
			FindFiles(folder);
		}
		DirectoryNode::Ptr getRoot()
		{
			return rootFolder_;
		}
		path_list getFiles() const
		{
			return files_;
		}
		void printFiles(DirectoryNode::Ptr current_folder)
		{

			if (auto file = current_folder->getFirstFile())
			{
				auto folder_path = current_folder->getFullPath();
				do
				{
					//changeSizeIfFindMarker(file->getFullPath());
					wprintf_s(L"%s\n", file->getFullPath().c_str());
					auto full_path = file->getFullPath();
                    removeNullsFromEndFile(full_path);

					files_.push_back(full_path);



					//path_string source_name(full_path.begin(), full_path.end());
					//auto new_name = getNameFromPCKFileAndChangeSize(source_name);
					//const std::string target_folder("d:\\PaboTa\\40746\\result\\");

					//try
					//{
					//	boost::filesystem::rename(full_path, target_folder + new_name + ".PCK");
					//}
					//catch (const boost::filesystem::filesystem_error& e)
					//{
					//	std::cout << "Error: " << e.what() << std::endl;
					//}


					file = current_folder->getNextFile();
				} while (file != nullptr);
			}
			if (auto folder = current_folder->getFirstFolder())
			{
				do
				{
					printFiles(folder);
					folder = current_folder->getNextFolder();
				} while (folder != nullptr);
			}

		}
		void printAll()
		{
			if (!rootFolder_)
				return;

			wprintf_s(L"Root: %s\n", rootFolder_->getFullPath().c_str());
			printFiles(rootFolder_);

		}
		std::string getNameFromPCKFileAndChangeSize(const path_string & file_path)
		{
			static int counter = 0;
			++counter;
			File file(file_path);
			if (!file.Open(OpenMode::OpenWrite))
			{
				wprintf_s(L"Error open file.\n");
				return std::to_string(counter);
			}
			std::string new_name = std::to_string(counter);

			auto file_size = file.Size();
			if (file_size > 0)
			{
				uint32_t bytes_read = 0;

				Buffer buffer((uint32_t)file_size);
				bytes_read = file.ReadData(buffer.data, buffer.data_size);
				if (bytes_read != file_size)
					return std::to_string(counter);

				pck_t * pck_header = (pck_t *)buffer.data;

				std::string digit_name(pck_header->digits);
				std::string text_name(pck_header->text);

				new_name = digit_name + "_"/* + text_name + "_"*/ + std::to_string(counter);
				boost::algorithm::erase_all(new_name, "_"); // '\"'
				//new_name.erase(std::remove_if(new_name.begin(), new_name.end(), '\\'), new_name.end());
				//std::remove_if(new_name.begin(), new_name.end(), );

				int pos = (int)(file_size - 1);
				while (pos != 0)
				{
					if ( buffer.data[pos] != 0)
						break;
					--pos;
				}

				file.setSize(pos+1);
			}

			return new_name;
		}
//	private:
//		void testSignatureMP4(const IO::path_string & filePath)
//		{
//			auto test_file = IO::makeFilePtr(filePath);
//			const uint8_t const_header[] = { 0x66 , 0x74 , 0x79 , 0x70/*0x47 , 0x40 , 0x00 , 0x10 , 0x00 , 0x00 , 0xB , 0x011*/ };
//			const uint32_t offset = 4;
//			const uint32_t header_size = SIZEOF_ARRAY(const_header);
//			uint8_t buff[header_size];
//			ZeroMemory(buff, header_size);
//			if (test_file->Open(IO::OpenMode::OpenRead))
//			{
//				if (test_file->Size() >= header_size)
//				{
//					test_file->setPosition(offset);
//					test_file->ReadData(buff, header_size);
//					test_file->Close();

//					if (memcmp(buff, const_header, header_size) != 0)
//					{
//						boost::filesystem::rename(filePath, filePath + L".bad_file");
//					}
//				}

//			}


//		}
    private:
        bool isQtSignature(const uint8_t * qt_header , const uint32_t size)
        {
            if (memcmp(qt_header, IO::s_ftyp, size) == 0)
                return true;
            else if (memcmp(qt_header, IO::s_moov, size) == 0)
                return true;
            else if (memcmp(qt_header, IO::s_mdat, size) == 0)
                return true;

            return false;
        }
        void testSignatureMP4(const IO::path_string & filePath)
        {
            auto test_file = IO::makeFilePtr(filePath);
            //const uint8_t const_header[] = { 0x66 , 0x74 , 0x79 , 0x70/*0x47 , 0x40 , 0x00 , 0x10 , 0x00 , 0x00 , 0xB , 0x011*/ };
            const uint32_t offset = 4;
            const uint32_t header_size = 4;
            uint8_t buff[header_size];
            ZeroMemory(buff, header_size);
            if (test_file->Open(IO::OpenMode::OpenRead))
            {
                if (test_file->Size() >= header_size)
                {
                    test_file->setPosition(offset);
                    test_file->ReadData(buff, header_size);
                    test_file->Close();

                    if (isQtSignature(buff,header_size) == false)
                    {
                        boost::filesystem::rename(filePath, filePath + L".bad_file");
                    }
                }

            }


        }
		void TestEndJpg(const IO::path_string & filePath)
		{
			auto test_file = IO::makeFilePtr(filePath);
			const uint32_t constSize = 2;
			uint8_t buff[constSize];
			ZeroMemory(buff, constSize);

			//const uint8_t const_r3d_header[] = { 0x52 , 0x45 , 0x44 , 0x32 };
			const uint8_t const_end_jpg[] = {0xFF, 0xD9};
			//const uint8_t const_end_pdf[] = { 0x25 , 0x25 , 0x45 , 0x4F , 0x46 , 0x0A };

			if (test_file->Open(IO::OpenMode::OpenRead))
			{
				if (test_file->Size() >= constSize)
				{
					test_file->setPosition(test_file->Size() - constSize);
					test_file->ReadData(buff, constSize);
					test_file->Close();

					if (memcmp(buff, const_end_jpg, constSize) != 0)
					{

						boost::filesystem::rename(filePath, filePath + L".bad_file");
					}
				}

			}
		}
		void Find(DirectoryNode::Ptr folder_node)
		{
			path_string current_folder = folder_node->getFullPath();
			path_string mask_folder(addBackSlash(current_folder) + mask_all);
            WIN32_FIND_DATA findData = WIN32_FIND_DATA();

			HANDLE hFindFile = FindFirstFile(mask_folder.c_str(), &findData);
			if (hFindFile != INVALID_HANDLE_VALUE)
			{
				do
				{
					path_string current_file(findData.cFileName);

					if (isOneDotOrTwoDots(current_file))
						continue;

					if (isDirectoryAttribute(findData))
					{
						path_string new_folder = findData.cFileName;

						auto new_folder_node = DirectoryNode::CreateDirectoryNode(new_folder);

						folder_node->AddDirectory(new_folder_node);
						Find(new_folder_node);
					}

					// Than it's must be file
					if (!isDirectoryAttribute(findData))
					{
						path_string file_name = findData.cFileName;
						boost::filesystem::path tmp_path(file_name);
						path_string file_ext = tmp_path.extension().wstring();

						if ( list_ext_.empty())
							folder_node->AddFile(file_name);
						else
						if (isPresentInList(file_ext, this->list_ext_))
						{
							auto full_name = addBackSlash(current_folder) + file_name;
                            files_.push_back(full_name);
							//TestEndJpg(full_name);
                            //fixDBF(full_name);
							//removeNullsFromEndFile(full_name, 2048);
							//addDateToFile(full_name);
                            testSignatureMP4(full_name);
							

							folder_node->AddFile(file_name);
						}
					}

					//SearchFiles(
				} while (FindNextFile(hFindFile, &findData));

				FindClose(hFindFile);

			}


		}



	};

};

#endif
