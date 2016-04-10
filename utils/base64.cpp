#include "base64.h"
#include <string.h>

#define min(a,b) (((a) < (b)) ? (a) : (b))

const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

void base64encoder::encode(unsigned char src[3], unsigned char dest[4])
{
    dest[0] = base64_chars[(src[0] & 0xfc) >> 2];
    dest[1] = base64_chars[((src[0] & 0x03) << 4) + ((src[1] & 0xf0) >> 4)];
    dest[2] = base64_chars[((src[1] & 0x0f) << 2) + ((src[2] & 0xc0) >> 6)];
    dest[3] = base64_chars[src[2] & 0x3f];
}

bool base64decoder::is_base64(unsigned char c)
{
    if (c >= 'A' && c <= 'Z')
        return true;
    else if (c >= 'a' && c <= 'z')
        return true;
    else if (c >= '0' && c <= '9')
        return true;
    else if (c == '+')
        return true;
    else if (c == '/')
        return true;
    return false;
}

unsigned char base64decoder::getindex(unsigned char c)
{
    if (c >= 'A' && c <= 'Z')
        return c - 'A';
    else if (c >= 'a' && c <= 'z')
        return c - 'a' + 26;
    else if (c >= '0' && c <= '9')
        return c - '0' + 52;
    else if (c == '+')
        return 62;
    else if (c == '/')
        return 63;
    return 0;
}

void base64decoder::decode(unsigned char src[4], unsigned char dest[3])
{
    src[0] = getindex(src[0]);
    src[1] = getindex(src[1]);
    src[2] = getindex(src[2]);
    src[3] = getindex(src[3]);

    dest[0] = (src[0] << 2) + ((src[1] & 0x30) >> 4);
    dest[1] = ((src[1] & 0xf) << 4) + ((src[2] & 0x3c) >> 2);
    dest[2] = ((src[2] & 0x3) << 6) + src[3];
}

base64encoder::base64encoder()
{
}

void base64encoder::start()
{
    remainlen = 0;
}

void base64encoder::write(const void* buf, int len)
{
    int offset = 0;
    int picklen;
    unsigned char dest[4];
    while (offset < len)
    {
        picklen = min(3 - remainlen, len - offset);
        memcpy(remain + remainlen, ((unsigned char*) buf) + offset, picklen);
        offset += picklen;
        remainlen += picklen;
        if (remainlen == 3)
        {
            encode(remain, dest);
            remainlen = 0;
//            filter::write(dest, 4);
        }
    }
}

void base64encoder::end()
{
    if (remainlen)
    {
        unsigned char dest[4];
        memset(remain + remainlen, 0, 3 - remainlen);
        encode(remain, dest);
        memset(dest + remainlen + 1, '=', 3 - remainlen);
        remainlen = 0;
//        filter::write(dest, 4);
    }
}

base64decoder::base64decoder()
{
}

void base64decoder::start()
{
    remainlen = 0;
}

void base64decoder::write(const void* buf, int len)
{
    int offset = 0;
    unsigned char dest[3];
    while (offset < len)
    {
        memcpy(remain + remainlen, ((unsigned char*) buf) + offset++, 1);
        if (is_base64(remain[remainlen]))
            ++remainlen;
        else
            continue;

        if (remainlen == 4)
        {
            decode(remain, dest);
            remainlen = 0;
//            filter::write(dest, 3);
        }
    }
}

void base64decoder::end()
{
    if (remainlen)
    {
        unsigned char dest[3];
        memset(remain + remainlen, '0', 4 - remainlen);
        decode(remain, dest);
//        filter::write(dest, remainlen - 1);
        remainlen = 0;
    }
}
