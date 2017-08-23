#pragma once

#include "AbstractRaw.h"
#include "Factories.h"

namespace IO
{
	#pragma pack (push) 
	#pragma pack (1)
	struct riff_header_struct
	{
		uint8_t riff_name[4];
		uint32_t size;

	};
	#pragma pack (pop)

	const uint32_t riff_header_struct_size = sizeof(riff_header_struct);

	const uint32_t ONE_MB = 1 * 1024 * 1024;
	class RawRIFF
		: public DefaultRaw
	{
	private:
		uint32_t minFileSize_ = ONE_MB;
	public:
		RawRIFF(IODevicePtr device)
			: DefaultRaw(device)
		{
		}
		~RawRIFF()
		{}
		uint64_t SaveRawFile(File & target_file, const uint64_t start_offset) override
		{
			if (!target_file.isOpen())
			{
				wprintf(L"Target file wasn't opened.\n");
				return 0;
			}

			riff_header_struct riff_struct = { 0 };
			readRiffStruct(riff_struct, start_offset);
			wprintf(L"Riff write size: %d\n", riff_struct.size);
			if (riff_struct.size > 0)
			{
				riff_struct.size += riff_header_struct_size;

				return appendToFile(target_file, start_offset, riff_struct.size);
			}
			else
				wprintf(L"Size RIFF is 0.\n");
			return 0;
		}

		bool Specify(const uint64_t start_offset) override
		{
			// Read header_sector if size is more than 1Mb return true
			riff_header_struct riff_struct = { 0 };
			readRiffStruct(riff_struct, start_offset);

			if (riff_struct.size >= 0xFFFFFFFF - riff_header_struct_size)
				return false;

			//if (riff_struct.size >= getMinFileSize())
				return true;


			return false;
		}
		void setMinFileSize(const uint32_t minFileSize)
		{
			minFileSize_ = minFileSize;
		}
		uint32_t getMinFileSize() const
		{
			return minFileSize_;
		}
		void readRiffStruct(riff_header_struct & riff_header, const uint64_t start_offset)
		{
			setPosition(start_offset);
			ReadData((ByteArray)&riff_header, riff_header_struct_size);
		}
	};

	class RawFIFFFactory
		: public RawFactory
	{
	public:
		RawAlgorithm * createRawAlgorithm(IODevicePtr device) override
		{
			return new RawRIFF(device);
		}
	};
}

