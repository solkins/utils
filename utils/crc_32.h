#ifndef CRC32_H
#define	CRC32_H

#include <string>

class crc32
{
public:
    crc32();
	void start();
	void write(const void* buf, int len);
	void end();
    std::string result();

private:
    unsigned int crc;
};

#endif	/* CRC32_H */

