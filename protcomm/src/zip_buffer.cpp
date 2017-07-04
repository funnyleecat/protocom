#include "stdafx.h"
#include <string.h>
#include <zlib.h>
#include "zip_buffer.h"

#ifdef WIN32
#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "../../../../lib/cross/zlib1/x64/DynamicDebug/zlib.lib")
#else
#pragma comment(lib, "../../../../lib/cross/zlib1/x64/DynamicRelease/zlib.lib")
#endif  //_DEBUG
#else
#ifdef _DEBUG
#pragma comment(lib, "../../../../lib/cross/zlib1/x86/DynamicDebug/zlib.lib")
#else
#pragma comment(lib, "../../../../lib/cross/zlib1/x86/DynamicRelease/zlib.lib")
#endif  //_DEBUG
#endif  //_WIN64
#endif  //WIN32
 
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
			//ѹ��/��ѹ�ɹ� - ����ѭ�����������ݳ���
			break;
		}
		else if (Z_BUF_ERROR == temp)
		{
			//ѹ��/��ѹ���������� - ��������ڴ�
			buffer_length_ *= 2;
			data_length_ = buffer_length_;
			continue;
		}
		else
		{
			//ѹ��/��ѹ���ִ��� - ���ش���
			return -1;
		}
	} while (1);

	return data_length_;
}
