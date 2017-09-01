#pragma once

#include "constants.h"
#include "iodevice.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <windows.h>

namespace IO
{
    const uint32_t MaxNullsCount = 16;

	inline path_string addBackSlash(const path_string & path_str)
	{
		path_string new_string(path_str);
		if (*path_str.rbegin() != back_slash)
			new_string.push_back(back_slash);

		return new_string;
	}

	inline bool isDirectoryAttribute(const WIN32_FIND_DATA & attributes)
	{
		return (attributes.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
	}

	inline bool isOneDotOrTwoDots(const path_string & name_string)
	{
		if (name_string.compare(OneDot) == 0)
			return true;
		if (name_string.compare(TwoDot) == 0)
			return true;
		return false;
	}


	inline bool isPresentInList(const path_string & text_value, const path_list & listToFind)
	{
		auto str = text_value;
		boost::algorithm::to_lower(str);
        auto findIter = std::find(listToFind.begin(), listToFind.end(), str);
        return (findIter != listToFind.end()) ? true : false;
	}

	inline path_string getExtension(const path_string & file_name)
	{
		boost::filesystem::path file_path(file_name);	// can crash ????
		return file_path.extension().generic_wstring();
	}

	inline bool isOffice2003(const path_string & file_extension)
	{
		return isPresentInList(file_extension, listExtensions2003);
	}

	inline bool isOffice2007(const path_string & file_extension)
	{
		return isPresentInList(file_extension, listExtensions2007);
	}

	inline void JoinWithService(const path_string & data_file, 
								const path_string & service_file,
								const path_string & target_file,
								const uint32_t data_size,
								const uint32_t service_size)
	{
		const uint32_t target_page = data_size + service_size;
		
		File f_data(data_file);
		if (!f_data.Open(OpenMode::OpenRead))
		{
			wprintf_s(L"Error open file. %s\n", f_data.getFileName().c_str());
			return;
		}

		File f_sevice(service_file);
		if (!f_sevice.Open(OpenMode::OpenRead))
		{
			wprintf_s(L"Error open file. %s\n", f_sevice.getFileName().c_str());
			return;
		}

		File f_target(target_file);
		if (!f_target.Open(OpenMode::Create))
		{
			wprintf_s(L"Error open file. %s\n", f_target.getFileName().c_str());
			return;
		}

		
		Buffer data_buffer(data_size);
		Buffer service_buffer(service_size);
		Buffer target_buffer(target_page);

        //bool bResult = false;

		LONGLONG max_size = f_data.Size();

		LONGLONG cur = 0;
		while (cur < max_size)
		{
			if ( f_data.ReadData(data_buffer.data, data_buffer.data_size) == 0 )
			{
				wprintf_s(L"Error read data file.\n");
				break;
			}

			
			if (f_sevice.ReadData(service_buffer.data, service_buffer.data_size) == 0)
			{
				wprintf_s(L"Error read service file.\n");
				break;
			}

			memcpy(target_buffer.data, data_buffer.data, data_buffer.data_size);
			memcpy(target_buffer.data + data_buffer.data_size, service_buffer.data, service_buffer.data_size);

			if (f_target.WriteData(target_buffer.data, target_buffer.data_size) == 0)
			{
				wprintf_s(L"Error write  file.\n");
				break;
			}


			cur += data_size;

		}


	}

    inline path_string toNumberString(const uint64_t number)
	{
        const int numValues = MaxNullsCount;
		wchar_t buff[numValues] = {0};
		ZeroMemory(buff, sizeof(wchar_t) * numValues);

        swprintf_s(buff, numValues, L"%I64u", number);
		path_string number_str(buff);
		return number_str;
	}

	inline path_string toHexString(const uint64_t big_value)
	{
        const int numValues = MaxNullsCount;
		wchar_t buff[numValues] = { 0 };
        ZeroMemory(buff, sizeof(wchar_t) * numValues);

        swprintf_s(buff, numValues, L"%I64x", big_value);
		path_string number_str(buff);
		return number_str;
	}

    inline path_string insertNullsInFront(const path_string & theString , const uint32_t maxNullsCount = MaxNullsCount)
    {
        if ( maxNullsCount < theString.length())
            return theString;

        int nulls_count = maxNullsCount - theString.length();
        path_string resString(nulls_count,'0');
        resString += theString;
        return resString;
    }

	inline path_string toNumberExtension(const uint32_t number, const path_string & extension)
	{
        return (insertNullsInFront(toNumberString(number))) + extension;
	}
	inline path_string toFullPath(const path_string & folder, const uint32_t number, const path_string & extension)
	{
        return addBackSlash(folder) + insertNullsInFront(toNumberString(number)) + extension;
	}
	inline path_string offsetToPath(const path_string & folder, const uint64_t byte_offset, const path_string & extension, uint32_t sector_size = 512)
	{
        return addBackSlash(folder) + insertNullsInFront(toHexString(byte_offset / sector_size)) + extension;
	}

	inline bool createDirectory(const path_string & folder, const path_string & new_folder, path_string & result_folder)
	{
		result_folder = addBackSlash(folder) + new_folder;
		if (!boost::filesystem::exists(result_folder))
			return boost::filesystem::create_directory(result_folder);
	}


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

    #pragma pack( push, 1)
    struct CanonDate
    {
        //2017:08:08 11:10:02
        char year[4];
        char colon1;
        char month[2];
        char colon2;
        char day[2];
        char white_space;
        char hour[2];
        char colon3;
        char min[2];
        char colon4;
        char sec[2];
        char null;
    };
    #pragma pack(pop)
    const std::size_t CanonDateSize = sizeof(CanonDate);
    using DateStruct = CanonDate;

     inline bool isDigitsInDateStruct(const DateStruct & date_struct)
     {
         if (isDigitString(date_struct.year,4))
             if (isDigitString(date_struct.month,2))
                 if (isDigitString(date_struct.month,2))
                     if (isDigitString(date_struct.hour,2))
                         if (isDigitString(date_struct.min,2))
                            if (isDigitString(date_struct.sec,2))
                                 return true;
        return false;
     }

    inline bool isDateString(const std::string & date_string)
    {
        if (date_string.length() < sizeof(DateStruct) - 1)
            return false;

        DateStruct * canon_date =  (DateStruct *)date_string.data();
        if (isDigitsInDateStruct(*canon_date))
            return validateSystemTime(toSystemTime(date_string));

        return false;
    }




};
