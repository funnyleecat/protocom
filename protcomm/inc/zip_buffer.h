#pragma once

typedef unsigned char	zip_char_t;
typedef unsigned long	zip_size_t;

class zip_buffer
{
public:
	typedef int zip_func(zip_char_t*, zip_size_t*, const zip_char_t*, zip_size_t);

public:
	zip_buffer();
	~zip_buffer();

	int zip(const zip_char_t* input, zip_size_t input_length);
	int unzip(const zip_char_t* input, zip_size_t input_length);
	zip_char_t* buffer();
	zip_size_t length();

private:
	zip_buffer(zip_buffer& chs);
	zip_buffer& operator =(zip_buffer& chs);
	int zip_internal(const zip_char_t* input, zip_size_t input_length, zip_func f);
	zip_char_t*		buffer_;
	zip_size_t		buffer_length_;
	zip_size_t		data_length_;
};


