#ifndef BASE64_H
#define	BASE64_H

class base64encoder
{
public:
	base64encoder();

    void start();
    void write(const void* buf, int len);
	void end();

private:
    void encode(unsigned char src[3], unsigned char dest[4]);

private:
    unsigned char remain[3];
	int remainlen;
};

class base64decoder
{
public:
	base64decoder();
    
    void start();
    void write(const void* buf, int len);
	void end();

private:
    bool is_base64(unsigned char c);
    unsigned char getindex(unsigned char c);
    void decode(unsigned char src[4], unsigned char dest[3]);

private:
    unsigned char remain[4];
	int remainlen;
};

#endif	/* BASE64_H */

