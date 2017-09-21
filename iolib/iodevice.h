#pragma once
#include "physicaldrive.h"
#include <string>

namespace IO
{


    inline uint32_t calcBlockSize(uint64_t current, uint64_t size, uint32_t block_size)
    {
        uint32_t bytes = 0;
        if (current + (uint64_t)block_size <= size)
            bytes = block_size;
        else
            bytes = (uint32_t)(size - current);
        return bytes;
    }
    inline bool isMultiple(uint64_t number, uint32_t multiple_by)
    {
        if (multiple_by == 0)
            return false;
        return (number % multiple_by == 0);
    }


    inline std::wstring FormatErrorMessage(uint32_t error_code)
    {
        static const int BUFFERLENGTH = 1024;
        std::vector<wchar_t> buf(BUFFERLENGTH);
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, error_code, 0, buf.data(),
            BUFFERLENGTH - 1, 0);
        return std::wstring(buf.data());
    }

    class Logger
    {
    public:
        virtual ~Logger() {};

        virtual void showMessage(const std::wstring & messageText) = 0;
    };

    class ConsoleLogger
            : public Logger
    {
    public:
        void showMessage(const std::wstring & messageText) override
        {
            wprintf(messageText.c_str());
            wprintf(L"\n");
        }
    };

    class IODevice
    {
    public:
        virtual ~IODevice() {};
        virtual bool Open(OpenMode) = 0;
        virtual void Close() = 0;
        virtual bool isOpen() = 0;
        virtual void setPosition(uint64_t offset) = 0;
        virtual uint32_t ReadData(ByteArray data, uint32_t read_size) = 0;
        virtual uint32_t WriteData(ByteArray data, uint32_t read_size) = 0;
        virtual uint64_t Size() const = 0;
    };

    using IODevicePtr = std::shared_ptr<IODevice>;


    inline std::wstring getOpenModeText(OpenMode openMode)
    {
        std::wstring mode_txt;
        switch (openMode)
        {
        case OpenMode::OpenRead:
            mode_txt = L"OpenRead";
            break;
        case OpenMode::OpenWrite:
            mode_txt = L"OpenWrite";
           break;
        case OpenMode::Create:
            mode_txt =  L"Create";
            break;
        default:
            mode_txt = L"Unknown";
        }
        return mode_txt;
    }

    class File
        : public IODevice
    {
    private:
        HANDLE hFile_ = INVALID_HANDLE_VALUE;
        uint64_t position_ = 0;
        uint64_t size_ = 0;
        path_string file_name_;
        bool bOpen_ = false;
        std::unique_ptr<Logger> logger_ = std::make_unique<ConsoleLogger>();
    public:
        File(const path_string & file_name)
            :file_name_(file_name)
        {

        }
        ~File()
        {
            Close();
        }

        bool Open( OpenMode openMode) override
        {
            uint32_t win_open_mode = 0;


            switch (openMode)
            {
            case OpenMode::OpenRead:
            case OpenMode::OpenWrite:
                win_open_mode = OPEN_EXISTING;
                break;
            case OpenMode::Create:
                win_open_mode = CREATE_ALWAYS;
                break;
            }

            hFile_ = ::CreateFile(file_name_.c_str(),
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                NULL,
                win_open_mode,
                0,
                NULL);

            if (hFile_ != INVALID_HANDLE_VALUE)
            {
                bOpen_ = true;
                LARGE_INTEGER liSize = LARGE_INTEGER();
                ::GetFileSizeEx(hFile_, &liSize);
                size_ = liSize.QuadPart;
            }
            else
            {
                bOpen_ = false;
                /* Error OpenRead.
                 * Error OpenWrite.
                 * Error Create.
                 *
                 * The system cannot find the file specified.
                 */
                std::wstring error_message = L"Error " + getOpenModeText(openMode)
                                            + L".\"" + file_name_ + L"\". "
                                            + FormatErrorMessage(::GetLastError());
                logger_->showMessage(error_message);
            }
            return bOpen_;
        }

        void Close() override
        {
            if (hFile_ != INVALID_HANDLE_VALUE)
            {
                ::CloseHandle(hFile_);
                hFile_ = INVALID_HANDLE_VALUE;
            }
            bOpen_ = false;

        }
        bool isOpen()
        {
            return bOpen_;
        }

        void setPosition(uint64_t offset) override
        {
            position_ = offset;
            LARGE_INTEGER liPos = LARGE_INTEGER();
            liPos.QuadPart = position_;
            ::SetFilePointerEx(hFile_, liPos, NULL, FILE_BEGIN);
        }

        uint32_t ReadData(ByteArray data, uint32_t read_size) override
        {
            if (data == nullptr)
                return 0;
            if (read_size == 0)
                return 0;

            DWORD bytes_read = 0;


            if (!::ReadFile(hFile_, data, read_size, &bytes_read, NULL))
                return 0;
            if ( bytes_read != read_size)
            {
                auto last_error = ::GetLastError();
                std::wstring error_message = L"Error read ";
                if (last_error > 0)
                    error_message += FormatErrorMessage(last_error);
                logger_->showMessage(error_message);
            }
            return bytes_read;
        };

        uint32_t WriteData(ByteArray data, uint32_t write_size) override
        {
            if (data == nullptr)
                return 0;
            if (write_size == 0)
                return 0;

            DWORD bytes_written = 0;


            if (!::WriteFile(hFile_, data, write_size, &bytes_written, NULL))
                return 0;
            size_ += bytes_written;
            return bytes_written;
        };

        uint64_t Size() const override
        {
            return size_;
        }
        void setSize(uint64_t new_size)
        {
            if (size_ != new_size)
            {
                size_ = new_size;
                LARGE_INTEGER li = LARGE_INTEGER();
                li.QuadPart = size_;
                ::SetFilePointerEx(hFile_, li, NULL, FILE_BEGIN);
                ::SetEndOfFile(hFile_);
            }
        }
        void setFileName(const path_string new_filename)
        {
            Close();
            file_name_ = new_filename;
        }
        path_string getFileName() const
        {
            return file_name_;
        }
    };

    using FilePtr = std::shared_ptr<File>;
    inline FilePtr makeFilePtr(const path_string & file_name)
    {
        return std::make_shared<File>(file_name);
    }

    class BlockDevice
        : public IODevice
    {
    public:
        virtual uint32_t ReadBlock(ByteArray data, uint32_t read_size) = 0;
        virtual uint32_t WriteBlock(ByteArray data, uint32_t read_size) = 0;

    };


    class DiskDevice
        : public BlockDevice
    {
    private:
        HANDLE hDrive_;
        bool bOpen_;
        uint64_t position_;
        PhysicalDrivePtr physical_drive_;
    public:
        DiskDevice(PhysicalDrivePtr physical_drive)
            :hDrive_(INVALID_HANDLE_VALUE)
            , bOpen_(false)
            , position_(0)
            , physical_drive_(physical_drive)
        {
            system_oi_ = std::make_unique<SystemIO>();
        }
        DiskDevice(PhysicalDrivePtr physical_drive , std::unique_ptr<SystemIO> system_io)
            :hDrive_(INVALID_HANDLE_VALUE)
            , bOpen_(false)
            , position_(0)
            , physical_drive_(physical_drive)
            , system_oi_(std::move(system_io))
        {
        }
        bool Open(OpenMode open_mode) override
        {
            if ( open_mode == OpenMode::Create)
            {
                printf("Error cannot create physical drive");
            }
            if (physical_drive_)
            {
                hDrive_ = system_oi_->OpenPhysicalDrive(physical_drive_->getPath());
                if (hDrive_ != INVALID_HANDLE_VALUE)
                {
                    bOpen_ = true;
                    return true;
                }

            }
            bOpen_ = false;
            return bOpen_;
        }
        void Close() override
        {
            if (hDrive_ != INVALID_HANDLE_VALUE)
                CloseHandle(hDrive_);
            bOpen_ = false;

        }
        bool isOpen() override
        {
            return bOpen_;
        }
        void setPosition(uint64_t offset) override
        {
            position_ = offset;
            LARGE_INTEGER li = LARGE_INTEGER();
            li.QuadPart = position_;
            ::SetFilePointerEx(hDrive_, li, nullptr, FILE_BEGIN);

        }
        uint64_t getPosition() const
        {
            return position_;
        }

        uint32_t ReadDataNotAligned(ByteArray data, uint32_t read_size)
        {
            DWORD numByteRead = 0;
            auto sector_size = physical_drive_->getBytesPerSector();
            uint32_t data_start = position_ % sector_size;
            int sector_to_read = (data_start + read_size) / sector_size + 1;
            int bytes_to_read = sector_to_read * sector_size;

            if (bytes_to_read > 0)
            {
                ByteArray temp_buffer = new uint8_t[bytes_to_read];

                uint64_t aling_offset = position_ / sector_size;
                aling_offset *= sector_size;
                setPosition(aling_offset);
                if (system_oi_->ReadFile(hDrive_, temp_buffer, bytes_to_read, &numByteRead, NULL))
                    if (numByteRead > 0)
                    {
                        memcpy(data, temp_buffer + data_start, read_size);
                        numByteRead = read_size;
                    }
                delete[] temp_buffer;
            }

            position_ += (numByteRead + data_start);
            return numByteRead;

        }
        uint32_t ReadData(ByteArray data, uint32_t read_size) override
        {
            if (!isOpen())
                return 0;
            if (data == nullptr)
                return 0;
            if (read_size == 0)
                return 0;

            auto sector_size = physical_drive_->getBytesPerSector();


            if (isMultiple(position_, sector_size) && isMultiple(read_size, sector_size))
            {
                return ReadBlock(data, read_size);
            }
            else
            {
                return ReadDataNotAligned(data, read_size);
            }
        }
        uint32_t WriteData(ByteArray data, uint32_t write_size) override
        {
            if (!isMultiple(position_, physical_drive_->getBytesPerSector()))
                return 0;
            if (!isMultiple(write_size , physical_drive_->getBytesPerSector()))
                return 0;

            return WriteBlock(data, write_size);
        }

        uint64_t Size() const override
        {
            return physical_drive_->getSize();	// return byte, not sectors
        }

        uint32_t ReadBlock(ByteArray data, uint32_t read_size) override
        {
            if (!isOpen())
                return 0;
            if (data == nullptr)
                return 0;
            if (read_size == 0)
                return 0;
            if (!isMultiple(position_, physical_drive_->getBytesPerSector()))
                return 0;
            if (!isMultiple(read_size , physical_drive_->getBytesPerSector()))
                return 0;

            DWORD bytes_read = 0;
            uint32_t data_pos = 0;
            uint32_t bytes_to_read = 0;
            auto transfer_size = this->physical_drive_->getTransferLength();

            while (data_pos < read_size)
            {
                bytes_to_read = calcBlockSize(data_pos, read_size, transfer_size);
                setPosition(position_);// ??? not work
                if (!system_oi_->ReadFile(hDrive_, data + data_pos, bytes_to_read, &bytes_read, NULL))
                    return 0;
                if (bytes_read == 0)
                    return 0;

                data_pos += bytes_read;
                position_ += bytes_read;
            }
            bytes_read = data_pos;
            return bytes_read;


        }
        uint32_t WriteBlock(ByteArray data, uint32_t write_size) override
        {
            if ( !isOpen())
            return 0;
            DWORD bytesWritten = 0;

            BOOL bResult = ::WriteFile(hDrive_, data, write_size, &bytesWritten, NULL);
            if (!bResult)
                return 0;
            return bytesWritten;
        }


        private:
            std::unique_ptr<SystemIO> system_oi_;



    };

}
