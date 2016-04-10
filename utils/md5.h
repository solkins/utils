#ifndef MD5_H
#define MD5_H

class md5
{
public:
    md5();
	void start();
	void write(const void* buf, int len);
	void end();

private:
    unsigned int state[4];		/* state (ABCD) */
    unsigned int count[2];		/* number of bits, modulo 2^64 (lsb first) */
    unsigned char buffer[64];       /* input buffer */
    unsigned char PADDING[64];		/* What? */
    unsigned char digest[16];
};

#endif	/* MD5_H */
