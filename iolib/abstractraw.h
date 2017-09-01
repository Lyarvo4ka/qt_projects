#ifndef ABSTRACT_RAW_H
#define ABSTRACT_RAW_H



#include "iodevice.h"
#include <memory>

#include "utility.h"

namespace IO
{
	class DataArray
	{
		ByteArray data_;
		uint32_t size_;
	public:
		using Ptr = std::unique_ptr<DataArray>;
		DataArray(const uint32_t size)
			:data_(nullptr)
			,size_(size)
		{
			if (size > 0)
			{
				data_ = new uint8_t[size];
			}
		}
		DataArray(ByteArray data, uint32_t size)
			: data_(data)
			, size_(size)
		{

		}
		DataArray(const uint8_t const_data[], uint32_t size)
			: data_(nullptr)
			, size_(size)
		{
			if (size_ > 0)
			{
				data_ = new uint8_t[size_];
				memcpy(data_, const_data, size_);

			}
		}

		~DataArray()
		{
			if (data_)
			{
				delete[] data_;
				data_ = nullptr;
//				printf("delete data\r\n");
			}
		}
		uint32_t size() const
		{
			return size_;
		}
		ByteArray data()
		{
			return data_;
		}
		ByteArray data() const
		{
			return data_;
		}
		operator ByteArray()
		{
			return data_;
		}
		friend bool operator == (const DataArray::Ptr & left, const DataArray::Ptr & right) 
		{
			if (left->size() == right->size())
				return (memcmp(left->data(), right->data(), left->size()) == 0);
			return false;
		}
		bool compareData( const ByteArray data , uint32_t size , uint32_t offset = 0) 
		{
			if (size >= this->size())
			{
				
				if (std::memcmp(data_ , data + offset, this->size()) == 0)
					return true;
			}
			return false;
		}
		bool compareData( const DataArray & dataArray , uint32_t offset = 0)
		{

			return compareData(dataArray.data(), dataArray.size(), offset);
		}
	};
	inline DataArray::Ptr makeDataArray(ByteArray data, uint32_t size)
	{
		return std::make_unique<DataArray>(data, size);
	}
	inline DataArray::Ptr makeDataArray(const uint8_t const_data[], uint32_t size)
	{
		return std::make_unique<DataArray>(const_data, size);
	}

	inline DataArray::Ptr makeDataArray(uint32_t size)
	{
		return std::make_unique<DataArray>(size);
	}

	using SignatureArray = std::vector<DataArray::Ptr>;

	class SignatureOffset
	{
		uint32_t signature_offset_ = 0;
		SignatureArray signatureArray_;

	public:
		using Ptr = std::unique_ptr<SignatureOffset>;
		SignatureOffset()
		{

		}
		SignatureOffset(DataArray::Ptr dataArray, uint32_t signature_offset = 0)
		{
			signature_offset_ = signature_offset;
			addSignature(std::move(dataArray));
		}
		SignatureOffset(ByteArray data, uint32_t size, uint32_t signature_offset = 0)
		{
			signature_offset_ = signature_offset;
			addSignature(data , size);
		}
		void setOffset(const uint32_t signature_offset)
		{
			signature_offset_ = signature_offset;
		}
		uint32_t getOffset() const
		{
			return signature_offset_;
		}
		void addSignature(DataArray::Ptr dataArray)
		{
			signatureArray_.emplace_back(std::move( dataArray));
		}
		void addSignature(DataArray * data_array)
		{
			signatureArray_.emplace_back(data_array);
		}
		void addSignature(ByteArray data, uint32_t size)
		{
			signatureArray_.emplace_back(std::make_unique<DataArray>(data, size));
		}
		void addSignature(const uint8_t const_data[], uint32_t size)
		{
			signatureArray_.emplace_back(std::make_unique<DataArray>(const_data, size));
		}
		bool FindSignature(const ByteArray data, uint32_t size)
		{
			for (auto & theSignature : signatureArray_)
			{
				if (theSignature->compareData(data, size, signature_offset_))
					return true;
			}
			return false;
		}
		bool find(const DataArray::Ptr & data_array)
		{
			auto iter = std::find(signatureArray_.begin(), signatureArray_.end(), data_array);
			return (iter != signatureArray_.end()) ? true : false;
		}

	};

	inline SignatureOffset::Ptr makeSignatureOffset()
	{
		return std::make_unique<SignatureOffset>();
	}
	inline SignatureOffset::Ptr makeSignatureOffset(ByteArray data, uint32_t size, uint32_t signature_offset = 0)
	{
		return std::make_unique<SignatureOffset>(data , size , signature_offset);
	}
	inline SignatureOffset::Ptr makeSignatureOffset(DataArray::Ptr data_array, uint32_t signature_offset = 0)
	{
		return std::make_unique<SignatureOffset>(std::move(data_array), signature_offset);
	}

	using HeaderArray = std::vector<SignatureOffset::Ptr>;

	class FileStruct
	{
		HeaderArray headers_;
		std::string formatName_;
		path_string extension_;
		DataArray::Ptr footer_;
		uint32_t footerTailEndSize_ = 0;
		uint64_t maxFileSize_ = 0;
		uint64_t minFileSize_ = 0;
	public:
		using Ptr = std::shared_ptr<FileStruct>;
		FileStruct(const std::string & formatName)
			: formatName_(formatName)
		{

		}
		std::string getName() const
		{
			return formatName_;
		}
		std::size_t headersCount() const
		{
			return headers_.size();
		}
		void addSignature(ByteArray data, uint32_t size, uint32_t header_offset)
		{
			addSignature(makeDataArray(data, size), header_offset);
		}

        void addSignature(DataArray::Ptr data_array, uint32_t offset)
		{
			auto iter = findByOffset(offset);

			if (iter != headers_.end())
			{
				if (!(*iter)->find(data_array))
					(*iter)->addSignature(std::move(data_array));
				else
				{
					printf("This signature is already present\r\n");
				}
			}
			else
				headers_.emplace_back(makeSignatureOffset(std::move(data_array), offset));
		}
		void addSignature(SignatureOffset::Ptr signAndOffset)
		{
			headers_.emplace_back(std::move(signAndOffset));
		}

		void addFooter(DataArray::Ptr & footer)
		{
			footer_ = std::move(footer);
		}
		void addFooter(ByteArray data, uint32_t size)
		{
			footer_ = std::move(makeDataArray(data, size));
		}
		void addFooter(const uint8_t const_data[], uint32_t size)
		{
			footer_ = std::move(makeDataArray(const_data, size));
		}
		DataArray * getFooter() const 
		{
			return footer_.get();
		}
		void setFooterTailEndSize(uint32_t footerTailEndSize)
		{
			footerTailEndSize_ = footerTailEndSize;
		}
		uint32_t getFooterTailEndSize() const
		{
			return footerTailEndSize_;
		}
		void setMaxFileSize(uint64_t maxFileSize)
		{
			maxFileSize_ = maxFileSize;
		}
		uint64_t getMaxFileSize() const
		{
			return maxFileSize_;
		}
		void setMinFileSize(const uint64_t minFileSize)
		{
			minFileSize_ = minFileSize;
		}
		uint64_t getMinFileSize() const 
		{
            return minFileSize_;
		}

		bool compareWithAllHeaders(ByteArray data, uint32_t size)
		{
			for (auto & theHeader : headers_)
			{
				if (!theHeader->FindSignature(data, size))
					return false;
			}
			return true;
		}
		void setExtension(const path_string & extension)
		{
			extension_ = extension;
		}
		path_string getExtension() const
		{
			return extension_;
		}
	private:
		HeaderArray::iterator findByOffset(uint32_t header_offset)
		{
			return std::find_if(headers_.begin(), headers_.end(),
				[header_offset](const SignatureOffset::Ptr & ptr)
			{
				return ptr->getOffset() == header_offset;
			}
			);

		}

	};
	
	inline FileStruct::Ptr makeFileStruct(const std::string & formatName)
	{
		return std::make_shared<FileStruct>(formatName);
	}


	class HeaderBase
	{
	public:
		using Ptr = std::shared_ptr<HeaderBase>;
	private:
		std::list<FileStruct::Ptr> listHeaders_;
		std::list<FileStruct::Ptr>::const_iterator iter_;
	public:
		void addFileFormat(FileStruct::Ptr new_file_format)
		{
			listHeaders_.emplace_back(new_file_format);
		}
		FileStruct * find(const ByteArray data , uint32_t size)
		{
			for (auto & theFileStruct : listHeaders_)
			{
				if (theFileStruct->compareWithAllHeaders(data, size))
					return theFileStruct.get();
			}
			return nullptr;
		}
	};

	class RawAlgorithm;

	class SignatureFinder
	{
		IODevicePtr device_;
		uint32_t block_size_ = default_block_size;
		uint32_t sector_size_ = default_sector_size;
		HeaderBase::Ptr header_base_;
	public:
		SignatureFinder(IODevicePtr device , HeaderBase::Ptr header_base)
			: device_(device)
			, header_base_(header_base)
		{

		}
		~SignatureFinder()
		{

		}
		void setBlockSize(const uint32_t block_size)
		{
			this->block_size_ = block_size;
		}
		FileStruct * findHeader(const uint64_t start_offset, uint64_t & header_pos)
		{
			header_pos = 0;
			if ( !device_->isOpen())
			if (!device_->Open(OpenMode::OpenRead))
			{
				wprintf_s(L"Error open device\n");
				return nullptr;
			}

			uint64_t file_size = device_->Size();
			uint64_t offset = start_offset;
			auto buffer = makeDataArray(block_size_);
			uint32_t bytes_read = 0;
			uint32_t result_offset = 0;

			while (offset < file_size)
			{
				device_->setPosition(offset);
				bytes_read = device_->ReadData(buffer->data(), block_size_);
				if (bytes_read == 0)
				{
                    //FormatErrorMessage(device_->GetErrorCode());
					printf("Error read drive\r\n");
					break;
				}
				if (auto header_ptr = cmpHeader(buffer, bytes_read, result_offset))
				{
					header_pos = offset;
					header_pos += result_offset;
					return header_ptr;
				}

				offset += bytes_read;
			}
			return nullptr;
		}

		FileStruct * cmpHeader(const DataArray::Ptr & buffer, const uint32_t size, uint32_t & header_pos)
		{
			for (header_pos = 0; header_pos < size; header_pos += sector_size_)
			{
				if (auto header = header_base_->find(buffer->data() + header_pos, buffer->size()))
					return header;
			}
			return nullptr;
		}

	};

	class RawAlgorithm
	{
	public:
		virtual uint64_t SaveRawFile(File & target_file, const uint64_t start_offset) = 0;
		virtual bool Specify(const uint64_t start_offset) = 0;
		virtual bool Verify(const IO::path_string & file_path) 
		{
            file_path.empty();

			return true;
		}
	};

	class DefaultRaw : public RawAlgorithm
	{
	private:
		IODevicePtr device_;
		uint32_t block_size_ = default_block_size;
		uint32_t sector_size_ = default_sector_size;
	public:
		DefaultRaw(IODevicePtr device)
			: device_(device)
		{

		}
		void setBlockSize(const uint32_t block_size)
		{
			this->block_size_ = block_size;
		}
		uint32_t getBlockSize() const
		{
			return block_size_;
		}
		void setSectorSize(const uint32_t sector_size)
		{
			this->sector_size_ = sector_size;
		}
		uint32_t getSectorSize() const
		{
			return sector_size_;
		}
		uint32_t ReadData(ByteArray data, uint32_t size)
		{
			return device_->ReadData(data, size);
		}
		void setPosition(uint64_t offset)
		{
			device_->setPosition(offset);
		}
		uint64_t getSize() const
		{
			return device_->Size();
		}
		uint64_t appendToFile(File & write_file, const uint64_t source_offset, const uint64_t write_size)
		{
			if (source_offset >= getSize())
			{
				printf("Error append to file. Source offset is more than size.\r\n");
				return 0;
			}

			uint64_t to_write = write_size;
			if (source_offset + write_size > getSize())	// ?????
				to_write = getSize() - write_size;

			if (to_write == 0)
			{
				printf("Error append to file. Write size is 0.\r\n");
			}

			auto target_offset = write_file.Size();
			uint32_t bytes_read = 0;
			uint32_t bytes_written = 0;
			uint64_t cur_pos = 0;
			uint64_t read_pos = source_offset;
			uint32_t bytes_to_write = 0;

			auto buffer = makeDataArray(getBlockSize());
			while (cur_pos < to_write)
			{
				bytes_to_write = calcBlockSize(cur_pos, write_size, getBlockSize());

				setPosition(read_pos);
				bytes_read = ReadData(buffer->data(), bytes_to_write);
				if (bytes_read == 0)
				{
					printf("Error read drive\r\n");
					return cur_pos;
				}
				read_pos += bytes_read;

				write_file.setPosition(target_offset);
				bytes_written = write_file.WriteData(buffer->data(), bytes_read);
				if (bytes_written == 0)
				{
					printf("Error write to file\r\n");
					return cur_pos;
				}

				target_offset += bytes_written;
				cur_pos += bytes_written;
			}

			return cur_pos;
		}


	};


/*
	class ZBKRaw
		: public RawAlgorithm
	{
	private:
		IODevicePtr device_;
		uint32_t block_size_ = default_block_size;
		uint32_t sector_size_ = default_sector_size;
	public:
		ZBKRaw(IODevicePtr device)
			:device_(device)
		{

		}
		void setBlockSize(const uint32_t block_size)
		{
			this->block_size_ = block_size;
		}
		uint32_t getBlockSize() const
		{
			return block_size_;
		}
		void setSectorSize(const uint32_t sector_size)
		{
			this->sector_size_ = sector_size;
		}
		uint32_t getSectorSize() const
		{
			return sector_size_;
		}
		HeaderPtr findHeaderAndOffset(const uint64_t start_offset, uint64_t & header_offset)
		{
			uint32_t bytes_read = 0;
			Buffer buffer(block_size_);
			
			uint32_t header_size = zbk_header_size;
			uint32_t signature_offset = 2;

			uint32_t footer_size = zbk_footer_size;
			uint32_t add_footer_size = 22;
			auto header_ptr = HeaderFactory((const ByteArray)&zbk_header[0], header_size);
			header_ptr->setHeaderOffset(signature_offset);
			header_ptr->setFooter((const ByteArray)&zbk_footer, footer_size);
			header_ptr->setAddFooterSize(add_footer_size);
			header_ptr->setMaxFileSize(0x12C00000);


			uint64_t offset = start_offset;
			

			while (true)
			{
				device_->setPosition(offset);
				bytes_read = device_->ReadData(buffer.data, block_size_);
				if (bytes_read == 0)
				{
					printf("Error read drive\r\n");
					break;
				}

				for (uint32_t iSector = 0; iSector < bytes_read; iSector += sector_size_)
				{
					if (header_ptr->isHeader(buffer.data+ iSector))
					{
						header_offset = offset;
						header_offset += iSector;
						return header_ptr;
					}
				}
				offset += bytes_read;
			}
			return nullptr;
		}
		uint64_t SaveRawFile(HeaderPtr &header_ptr, const uint64_t header_offset , const path_string & target_name) override
		{
			File target_file(target_name);
			if (!target_file.Open(OpenMode::Create))
			{
				wprintf(L"Error create file\n");
			}

			Buffer buffer(block_size_);
			uint32_t bytes_read = 0;
			uint64_t offset = header_offset;
			uint32_t footer_pos = 0;
			uint64_t target_size = 0;

			while (true)
			{
				device_->setPosition(offset);
				bytes_read = device_->ReadData(buffer.data, block_size_);
				if (bytes_read == 0)
				{
					printf("Error read drive\r\n");
					break;
				}

				if (header_ptr->isFooter(buffer.data, bytes_read, footer_pos))
				{
					uint32_t write_size = footer_pos + header_ptr->getAddFooterSize();
					if (target_file.WriteData(buffer.data, write_size) == 0)
					{
						printf("Error write to file\r\n");
						break;
					}
					target_size += write_size;
					return target_size;
				}
				else
				{
					if (target_file.WriteData(buffer.data, bytes_read) == 0)
					{
						printf("Error write to file\r\n");
						break;
					}
				}

				target_size += bytes_read;
				if (target_size > header_ptr->getMaxFileSize())
					return target_size;

				offset += bytes_read;
			}


			return 0;
		}

		void execute(const path_string &target_folder) 
		{
			if (!device_->Open(OpenMode::OpenRead))
			{
				wprintf(L"Error to open.\n");	// ????????
				return;
			}

			uint64_t offset = (uint64_t)0x0;
			uint64_t header_offset = 0;
			uint32_t counter = 0;
			while (true)
			{

				if (auto header_ptr = findHeaderAndOffset(offset, header_offset))
				{

					auto target_file = toFullPath(target_folder, counter++, L".zbk");
					uint64_t file_size = SaveRawFile(header_ptr, header_offset, target_file);
					//if (file_size != 0)
					//{
					//	header_offset += file_size;
					//	header_offset /= sector_size_;
					//	header_offset *= sector_size_;
					//	offset = header_offset;
					//}
					offset = header_offset;


				}
				else
				{
					wprintf(L"Not Found Header\n");
					break;
				}

				offset += sector_size_;

			}

		}

	};

*/
};



//class AbstractRaw
//{
//public:
//	AbstractRaw( const std::string & file_name  )
//		: hSource_( INVALID_HANDLE_VALUE )
//		, bReady_( false )
//	{
//		bReady_ = IO::open_read( hSource_ , file_name );
//	}
//	AbstractRaw( const DWORD drive_number  )
//		: hSource_( INVALID_HANDLE_VALUE )
//		, bReady_( false )
//	{
//		std::string drive_path( drivePathFromNumber( drive_number ) );
//		bReady_ = IO::open_read( hSource_ , drive_path );
//	}
//	virtual ~AbstractRaw()
//	{
//		close();
//	}
//	bool isReady() const
//	{
//		return bReady_;
//	}
//	void close()
//	{
//		if ( hSource_ != INVALID_HANDLE_VALUE )
//		{
//			CloseHandle( hSource_ );
//			hSource_ = INVALID_HANDLE_VALUE;
//			bReady_ = false;
//		}
//	}
//	HANDLE * getHandle()
//	{
//		return &hSource_;
//	}
//	virtual void execute() = 0;
//private:
//	HANDLE hSource_;
//	bool bReady_;
//};

#endif
