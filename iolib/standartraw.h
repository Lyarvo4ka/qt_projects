#pragma once


#include "abstractraw.h"
#include "factories.h"

namespace IO
{
	class StandartRaw
		: public DefaultRaw
	{
	private:
		DataArray::Ptr footer_;
		uint32_t tailSize_ = 0;
		uint64_t maxFileSize_ = 0;
		bool bFoundFooter_ = false;
	public:
		StandartRaw(IODevicePtr device)
			:DefaultRaw(device)
		{

		}

		void setFooter(DataArray * footer_data , const uint32_t tailSize)
		{
			if (footer_data)
			{
				if (footer_data->size() > 0)
				{
					footer_ = makeDataArray(footer_data->size());
					memcpy(footer_->data(), footer_data->data(), footer_data->size());
				}
			}
			setTailSize(tailSize);
		}
		DataArray * getFooter() const
		{
			return footer_.get();
		}

		void setTailSize(const uint32_t tailSize)
		{
			tailSize_ = tailSize;
		}

		void setMaxFileSize(const uint64_t maxFileSize)
		{
			maxFileSize_ = maxFileSize;
		}

		uint32_t calcToSectorSize(const uint32_t add_size)
		{
			uint32_t size = add_size / getSectorSize() + 1;
			size *= getSectorSize();
			return size;
		}

		uint64_t SaveRawFile(File & target_file, const uint64_t start_offset)   override
		{
			if (!target_file.isOpen())
			{
				wprintf(L"File wasn't opened.\n Try to create file.");
				if ( !target_file.Open(OpenMode::Create) )
				{
					wprintf(L"Error to create file.");
					return 0;
				}
			}

			if (maxFileSize_ == 0)
			{
				wprintf(L"Error maximum target file size is 0.\n");
				return 0;
			}

			
			auto footer_data = footer_.get();;
			if (!footer_data)
				return appendToFile(target_file, start_offset, maxFileSize_);;

			uint32_t bytes_read = 0;
			uint32_t bytes_written = 0;

			uint64_t offset = start_offset;

			uint32_t footer_pos = 0;
			uint32_t bytes_to_write = getBlockSize();
			uint64_t written_size = 0;

			uint32_t sizeToRead = getBlockSize() + calcToSectorSize(footer_data->size());
			auto buffer = makeDataArray(sizeToRead);

			while (offset < this->getSize())
			{
				sizeToRead = calcBlockSize(offset, this->getSize(), sizeToRead);
				  
				setPosition(offset);
				bytes_read = ReadData(buffer->data(), sizeToRead);
				if (bytes_read == 0 )
				{
					//	????????????
					wprintf(L"Error read block\n");
					break;
				}

				if (findFooter(*buffer.get(), bytes_read, *footer_data, footer_pos))
				{
					uint32_t sizeToWrite = footer_pos + footer_data->size() + tailSize_;
					target_file.setPosition(written_size);
					bytes_written = target_file.WriteData(buffer->data(), sizeToWrite);
					written_size += bytes_written;
					bFoundFooter_ = true;
					break;
				}

				if (bytes_read < getBlockSize() )
					bytes_to_write = bytes_read;

				uint64_t after_written = written_size + bytes_to_write;
				if (after_written >= maxFileSize_)
				{
					uint32_t last_size = (uint32_t)(after_written - maxFileSize_);
					if ( last_size > 0 )
					{
						target_file.setPosition(written_size);
						bytes_written = target_file.WriteData(buffer->data(), last_size);
						if (bytes_written == 0)
						{
							wprintf(L"Error write block\n");
							break;
						}
						written_size += last_size;
					}
					return written_size;
				}

				target_file.setPosition(written_size);
				bytes_written = target_file.WriteData(buffer->data(), bytes_to_write);
				if  (bytes_written == 0 )
				{
					wprintf(L"Error write block\n");
					break;
				}

				offset += bytes_written;
				written_size += bytes_written;
			}
			return written_size;

		}
		bool Specify(const uint64_t header_offset) override
		{
			return true;
		}
		bool Verify(const IO::path_string & file_path) override
		{
			if ( footer_)
				return bFoundFooter_;
			return true;
		}

		bool findFooter(const DataArray &data_array, uint32_t data_size, const DataArray & footer_data, uint32_t & footer_pos)
		{
			for (footer_pos = 0; footer_pos < data_size - footer_data.size(); ++footer_pos)
			{
				if (memcmp(data_array.data() + footer_pos, footer_data.data(), footer_data.size()) == 0)
				{
					printf("Found footer.\r\n");
					return true;
				}
			}
			return false;
		}
		bool compareBetween(const DataArray & data1, const DataArray & data2, const DataArray & footer_data, uint32_t & footer_pos)
		{
			ByteArray pData = nullptr;
			uint32_t pos = 0;
			for (uint32_t iByte = 1; iByte < footer_data.size(); ++iByte)
			{
				pos = data1.size() - footer_data.size() + iByte;
				pData = (ByteArray)(data1.data() + pos);
				if (memcmp(pData, footer_data.data(), footer_data.size() - iByte) == 0)
					if (memcmp(data2.data(), footer_data.data() + footer_data.size() - iByte, iByte) == 0)
					{
						footer_pos = pos;
						return true;
					}
			}
			return false;
		}
		uint32_t appendBetween(File & write_file, const DataArray & data1, const DataArray & data2, const uint32_t data2_size)
		{
			uint32_t bytes_written = 0;
			if ((bytes_written = write_file.WriteData(data1.data(), data1.size())))
			{
				auto target_size = bytes_written;

				// Save second chunk
				if ((bytes_written = write_file.WriteData(data2.data(), data2_size)))
				{
					target_size += data2_size;
					return target_size;
				}
			}
			return 0;
		}

		/*
		uint64_t SaveRawFile(FileStruct::Ptr file_struct, const uint64_t header_offset, const path_string & target_name) override
		{
		File target_file(target_name);
		if (!target_file.Open(OpenMode::Create))
		{
		wprintf(L"Error create file\n");
		return 0;
		}

		//auto buffer = makeDataArray(block_size_);
		uint32_t bytes_read = 0;
		uint32_t bytes_written = 0;

		uint64_t offset = header_offset;
		uint64_t target_size = 0;

		DataArray::Ptr currPtr = makeDataArray(block_size_);
		auto current = currPtr.get();
		DataArray::Ptr nextPtr = makeDataArray(block_size_);
		auto next = nextPtr.get();
		//DataArray * current = new DataArray(block_size_);
		//DataArray * next = new DataArray(block_size_);
		DataArray * temp = nullptr;

		uint32_t footer_pos = 0;
		auto footer_data = file_struct->getFooter();
		if (!footer_data)
		return appendToFile(target_file, header_offset, file_struct->getMaxFileSize());


		if ((bytes_read = ReadBlockData(*current, offset)))
		while (offset < this->getSize())
		{
		// Find footer in first block
		if (findFooter(*current, current->size(), *footer_data, footer_pos))
		{
		// found footer
		auto write_size = footer_pos + file_struct->getFooterTailEndSize();
		bytes_written = target_file.WriteData(current->data(), write_size);
		if (write_size != bytes_written)
		{
		printf("Error write to file\r\n");
		return 0;
		}

		return target_size + write_size;
		}

		// Read next block
		if ((bytes_read = ReadNextBlock(*next, offset)))
		{
		if (compareBetween(*current, *next, *footer_data, footer_pos))
		{
		auto write_chunk = footer_data->size() - (current->size() - footer_pos) + file_struct->getFooterTailEndSize();
		bytes_written = appendBetween(target_file, *current, *next, write_chunk);
		return target_size + bytes_written;
		}

		if ((bytes_written = target_file.WriteData(current->data(), bytes_read)))
		{
		target_size += bytes_written;
		if (target_size > file_struct->getMaxFileSize())
		return target_size;

		// change pointers
		temp = current;
		current = next;
		next = current;
		}
		else
		{
		printf("Error write to file\r\n");
		return 0;
		}
		}
		else
		return 0;
		}

		return 0;
		}
		*/
	};

	class StandartRawFactory
		: public RawFactory
	{
	public:
		RawAlgorithm * createRawAlgorithm(IODevicePtr device) override
		{
			return new StandartRaw(device);
		}
	};

	class OnlyHeadersRaw
		: public StandartRaw
	{
	public:
		OnlyHeadersRaw(IODevicePtr device)
			: StandartRaw(device)
		{

		}

		bool Specify(const uint64_t header_offset) override
		{
			return true;
		}
		uint64_t SaveRawFile(File & target_file, const uint64_t start_offset)   override
		{
			if (!target_file.isOpen())
			{
				wprintf(L"File wasn't opened.\n Try to create file.");
				if (!target_file.Open(OpenMode::Create))
				{
					wprintf(L"Error to create file.");
					return 0;
				}
			}

			auto footer_data = getFooter();
			if (!footer_data)
				return 0;

			uint32_t bytes_read = 0;
			uint32_t bytes_written = 0;

			uint64_t offset = start_offset;

			uint32_t footer_pos = 0;
			uint32_t bytes_to_write = getBlockSize();
			uint64_t written_size = 0;

			uint32_t start_cmp = 0;

			auto buffer = makeDataArray(getBlockSize());
			uint32_t sizeToRead = getBlockSize();

			while (offset < this->getSize())
			{
				sizeToRead = calcBlockSize(offset, this->getSize(), sizeToRead);

				setPosition(offset);
				bytes_read = ReadData(buffer->data(), sizeToRead);
				if (bytes_read == 0)
				{
					//	????????????
					wprintf(L"Error read block\n");
					break;
				}
				start_cmp = 0;
				if (offset == start_offset)
					start_cmp = getSectorSize();
				for (footer_pos = start_cmp; footer_pos < bytes_read; footer_pos += getSectorSize())
					if (memcmp(buffer->data() + footer_pos, footer_data->data(), footer_data->size()) == 0)
					{
						if (footer_pos > 0)
						{
							target_file.setPosition(written_size);
							bytes_written = target_file.WriteData(buffer->data(), footer_pos);
							written_size += bytes_written;
						}
						return written_size;
					}

				if (bytes_read < getBlockSize())
					bytes_to_write = bytes_read;

				target_file.setPosition(written_size);
				bytes_written = target_file.WriteData(buffer->data(), bytes_to_write);
				if (bytes_written == 0)
				{
					wprintf(L"Error write block\n");
					break;
				}

				offset += bytes_written;
				written_size += bytes_written;
			}
			return written_size;

		}

	};


};
