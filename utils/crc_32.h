#ifndef CRC32_H
#define	CRC32_H

class crc32
{
public:
    crc32();
	void start();
	void write(const void* buf, int len);
	void end();

private:
    unsigned int crc;
};

#endif	/* CRC32_H */

