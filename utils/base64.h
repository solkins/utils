#ifndef BASE64_H
#define	BASE64_H

#include <functional>

class base64encoder
{
public:
	base64encoder();

    void start();
    void write(const void* buf, int len);
	void end();
    void bind_output(std::function<void(const void*, int)> f);

private:
    void encode(unsigned char src[3], unsigned char dest[4]);

private:
    unsigned char remain[3];
	int remainlen;
    std::function<void(const void*, int)> _out;
};

class base64decoder
{
public:
	base64decoder();
    
    void start();
    void write(const void* buf, int len);
	void end();
    void bind_output(std::function<void(const void*, int)> f);

private:
    bool is_base64(unsigned char c);
    unsigned char getindex(unsigned char c);
    void decode(unsigned char src[4], unsigned char dest[3]);

private:
    unsigned char remain[4];
	int remainlen;
    std::function<void(const void*, int)> _out;
};

#endif	/* BASE64_H */

