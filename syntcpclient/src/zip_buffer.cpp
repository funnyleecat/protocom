#include "zip_buffer.h"
#include "zlib.h"

namespace SynTcpCli
{
	zip_buffer::zip_buffer()
		: buffer_(0), buffer_length_(0), data_length_(0)
	{
	}

	zip_buffer::~zip_buffer()
	{
		if (buffer_)
			delete[] buffer_;

		buffer_ = 0;
		buffer_length_ = 0;
		data_length_ = 0;
	}

	int zip_buffer::zip(const zip_char_t* input, zip_size_t input_length)
	{
		return zip_internal(input, input_length, compress);
	}

	int zip_buffer::unzip(const zip_char_t* input, zip_size_t input_length)
	{
		return zip_internal(input, input_length, uncompress);
	}

	zip_char_t* zip_buffer::buffer()
	{
		return buffer_;
	}

	zip_size_t zip_buffer::length()
	{
		return data_length_;
	}

	int zip_buffer::zip_internal(const zip_char_t* input, zip_size_t input_length, zip_func f)
	{
		if (!input || !input_length || input == buffer_)
			return -1;

		buffer_length_ = input_length;
		data_length_ = buffer_length_;

		do
		{
			if (buffer_)
				delete[] buffer_;

			buffer_ = new zip_char_t[buffer_length_];

			if (!buffer_ || !buffer_length_)
				return -1;

			int temp = f(buffer_, &data_length_, input, input_length);

			if (Z_OK == temp)
			{
				//压缩/解压成功 - 跳出循环，返回数据长度
				break;
			}
			else if (Z_BUF_ERROR == temp)
			{
				//压缩/解压缓冲区不够 - 申请更多内存
				buffer_length_ *= 2;
				data_length_ = buffer_length_;
				continue;
			}
			else
			{
				//压缩/解压出现错误 - 返回错误
				return -1;
			}
		} while (1);

		return data_length_;
	}
}
