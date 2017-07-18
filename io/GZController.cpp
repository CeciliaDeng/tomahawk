#include "../third_party/zlib/zlib.h"
#include "IOConstants.h"
#include "GZController.h"
#include <fstream>
#include "TGZFHeader.h"

namespace Tomahawk {
namespace IO {

GZController::GZController(){}

GZController::GZController(const char* data, const U32 length){}

GZController::GZController(const U32 largest_block_size) : buffer_(largest_block_size){}

GZController::~GZController(){}

void GZController::Clear(){ this->buffer_.reset(); }

U32 GZController::InflateSize(buffer_type& input) const{
	const TGZFHeader* header = reinterpret_cast<const TGZFHeader*>(&input.data[0]);
	if(!header->Validate()){
		 std::cerr << Helpers::timestamp("ERROR","TGZF") << "Invalid TGZF header" << std::endl;
		 std::cerr << Helpers::timestamp("DEBUG","TGZF") << "Output length: " << header->BSIZE << std::endl;
		 std::cerr << Helpers::timestamp("DEBUG","TGZF") << std::endl;
		 std::cerr << *header << std::endl;
		 exit(1);
	}

	return header->BSIZE;
}

bool GZController::Inflate(buffer_type& input, buffer_type& output) const{
	const TGZFHeader* header = reinterpret_cast<const TGZFHeader*>(&input[0]);
	if(!header->Validate()){
		 std::cerr << Helpers::timestamp("ERROR","TGZF") << "Invalid TGZF header" << std::endl;
		 std::cerr << Helpers::timestamp("DEBUG","TGZF") << "Output length: " << header->BSIZE << std::endl;
		 std::cerr << Helpers::timestamp("DEBUG","TGZF") << std::endl;
		 std::cerr << *header << std::endl;
		 exit(1);
	}

	U32* uncompressedLength = reinterpret_cast<U32*>(&input.data[input.size() - sizeof(U32)]);
	if(output.size() + *uncompressedLength >= output.capacity())
		output.resize((output.size() + *uncompressedLength) * 1.2);

	U32* crc = reinterpret_cast<U32*>(&input.data[input.size() - 2*sizeof(U32)]);

	// Bug fix for ZLIB when overflowing an U32
	U64 avail_out = output.capacity() - output.size();
	if(avail_out > ~(U32)0)
		avail_out = ~(U32)0;


	z_stream zs;
	zs.zalloc    = NULL;
	zs.zfree     = NULL;
	zs.next_in   = (Bytef*)&input.data[Constants::TGZF_BLOCK_HEADER_LENGTH];
	zs.avail_in  = (header->BSIZE + 1) - 16;
	zs.next_out  = (Bytef*)&output.data[output.pointer];
	zs.avail_out = (U32)avail_out;

	int status = inflateInit2(&zs, Constants::GZIP_WINDOW_BITS);

	if(status != Z_OK){
		std::cerr << Helpers::timestamp("ERROR","TGZF") << "Zlib inflateInit failed: " << (int)status << std::endl;
		 exit(1);
	}

	// decompress
	status = inflate(&zs, Z_FINISH);
	if(status != Z_STREAM_END){
		inflateEnd(&zs);
		std::cerr << Helpers::timestamp("ERROR","TGZF") << "Zlib inflateEnd failed: " << (int)status << std::endl;
		exit(1);
	}

	// finalize
	status = inflateEnd(&zs);
	if(status != Z_OK){
		inflateEnd(&zs);
		std::cerr << Helpers::timestamp("ERROR","TGZF") << "Zlib inflateFinalize failed: " << (int)status << std::endl;
		exit(1);
	}

	if(zs.total_out == 0)
		std::cerr << Helpers::timestamp("LOG", "TGZF") << "Detected empty TGZF block" << std::endl;

	output.pointer += zs.total_out;

	return(true);
}

bool GZController::Deflate(buffer_type& meta, buffer_type& rle){
	// initialize the gzip header
	//char* buffer = new char[input_length + 10];

	meta += rle;
	this->buffer_.resize(meta);

	memset(this->buffer_.data, 0, Constants::TGZF_BLOCK_HEADER_LENGTH);

	this->buffer_.data[0]  = Constants::GZIP_ID1;
	this->buffer_.data[1]  = Constants::GZIP_ID2;
	this->buffer_.data[2]  = Constants::CM_DEFLATE;
	this->buffer_.data[3]  = Constants::FLG_FEXTRA;
	this->buffer_.data[9]  = Constants::OS_UNKNOWN;
	this->buffer_.data[10] = Constants::TGZF_XLEN;
	this->buffer_.data[12] = Constants::TGZF_ID1;
	this->buffer_.data[13] = Constants::TGZF_ID2;
	this->buffer_.data[14] = Constants::TGZF_LEN;
	//buffer 16->20 is set below

	// set compression level
	const int compressionLevel = Z_DEFAULT_COMPRESSION;
	//const int compressionLevel = 9;

	// initialize zstream values
    z_stream zs;
    zs.zalloc    = NULL;
    zs.zfree     = NULL;
    zs.next_in   = (Bytef*)meta.data;
    zs.avail_in  = meta.pointer;
    zs.next_out  = (Bytef*)&this->buffer_.data[Constants::TGZF_BLOCK_HEADER_LENGTH];
    zs.avail_out = this->buffer_.width -
                   Constants::TGZF_BLOCK_HEADER_LENGTH -
                   Constants::TGZF_BLOCK_FOOTER_LENGTH;

	// Initialise the zlib compression algorithm
	int status = deflateInit2(&zs,
							  compressionLevel,
							  Z_DEFLATED,
							  Constants::GZIP_WINDOW_BITS,
							  Constants::Z_DEFAULT_MEM_LEVEL,
							  Z_DEFAULT_STRATEGY);

	if ( status != Z_OK ){
		std::cerr << Helpers::timestamp("ERROR", "ZLIB") << "DeflateBlock: zlib deflateInit2 failed" << std::endl;
		return false;
	}

	// compress the data
	status = deflate(&zs, Z_FINISH);

	// if not at stream end
	if ( status != Z_STREAM_END ) {
		deflateEnd(&zs);

		// there was not enough space available in buffer
		std::cerr << Helpers::timestamp("ERROR", "ZLIB") << "DeflateBlock: zlib deflate failed (insufficient space)" << std::endl;
		return false;
	}

	// finalize the compression routine
	status = deflateEnd(&zs);
	if ( status != Z_OK ){
		std::cerr << Helpers::timestamp("ERROR", "ZLIB") << "DeflateBlock: zlib deflateEnd failed (not ok)" << std::endl;
		return false;
	}

	// update compressedLength
	U32 compressedLength = zs.total_out +
					       Constants::TGZF_BLOCK_HEADER_LENGTH +
					       Constants::TGZF_BLOCK_FOOTER_LENGTH;

	// store the compressed length
	U32* test = reinterpret_cast<U32*>(&this->buffer_.data[16]);
	*test = compressedLength - 1;
	//std::cerr << Helpers::timestamp("DEBUG") << data.pointer << "->" << compressedLength-1 << " stored: " << *test << std::endl;

	std::time_t result = std::time(nullptr);
	std::asctime(std::localtime(&result));
	U32* time = reinterpret_cast<U32*>(&this->buffer_.data[4]);
	*time = result;
	//std::cerr << Helpers::timestamp("DEBUG") << "Time: " << *time << std::endl;

	// store the CRC32 checksum
	U32 crc = crc32(0, NULL, 0);
	crc = crc32(crc, (Bytef*)meta.data, meta.pointer);
	U32* c = reinterpret_cast<U32*>(&this->buffer_.data[compressedLength - 8]);
	*c = crc;
	U32* uncompressed = reinterpret_cast<U32*>(&this->buffer_.data[compressedLength - 4]);
	*uncompressed = meta.pointer; // Store uncompressed length

	//std::cerr << *uncompressed << std::endl;
	//this->buffer_.data[compressedLength - 8] = crc;
	//this->buffer_.data[compressedLength - 4] = meta.pointer; // uncompressed size
	this->buffer_.pointer = compressedLength;

	return true;
}

}
}