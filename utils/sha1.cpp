
#include "sha1.h"

#define SHA1CircularShift(bits,word) ((((word) << (bits)) & 0xFFFFFFFF) | ((word) >> (32-(bits))))

void sha1::SHA1ProcessMessageBlock()
{
    const unsigned K[] = /* Constants defined in SHA-1   */ {
        0x5A827999,
        0x6ED9EBA1,
        0x8F1BBCDC,
        0xCA62C1D6
    };
    int t; /* Loop counter                 */
    unsigned temp; /* Temporary word value         */
    unsigned W[80]; /* Word sequence                */
    unsigned A, B, C, D, E; /* Word buffers                 */

    /*
     *  Initialize the first 16 words in the array W
     */
    for (t = 0; t < 16; t++)
    {
        W[t] = ((unsigned) Message_Block[t * 4]) << 24;
        W[t] |= ((unsigned) Message_Block[t * 4 + 1]) << 16;
        W[t] |= ((unsigned) Message_Block[t * 4 + 2]) << 8;
        W[t] |= ((unsigned) Message_Block[t * 4 + 3]);
    }

    for (t = 16; t < 80; t++)
        W[t] = SHA1CircularShift(1, W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16]);

    A = Message_Digest[0];
    B = Message_Digest[1];
    C = Message_Digest[2];
    D = Message_Digest[3];
    E = Message_Digest[4];

    for (t = 0; t < 20; t++)
    {
        temp = SHA1CircularShift(5, A) + ((B & C) | ((~B) & D)) + E + W[t] + K[0];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    for (t = 20; t < 40; t++)
    {
        temp = SHA1CircularShift(5, A) + (B ^ C ^ D) + E + W[t] + K[1];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    for (t = 40; t < 60; t++)
    {
        temp = SHA1CircularShift(5, A) + ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    for (t = 60; t < 80; t++)
    {
        temp = SHA1CircularShift(5, A) + (B ^ C ^ D) + E + W[t] + K[3];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    Message_Digest[0] = (Message_Digest[0] + A) & 0xFFFFFFFF;
    Message_Digest[1] = (Message_Digest[1] + B) & 0xFFFFFFFF;
    Message_Digest[2] = (Message_Digest[2] + C) & 0xFFFFFFFF;
    Message_Digest[3] = (Message_Digest[3] + D) & 0xFFFFFFFF;
    Message_Digest[4] = (Message_Digest[4] + E) & 0xFFFFFFFF;

    Message_Block_Index = 0;
}

void sha1::SHA1PadMessage()
{
    if (Message_Block_Index > 55)
    {
        Message_Block[Message_Block_Index++] = 0x80;
        while (Message_Block_Index < 64)
            Message_Block[Message_Block_Index++] = 0;

        SHA1ProcessMessageBlock();

        while (Message_Block_Index < 56)
            Message_Block[Message_Block_Index++] = 0;
    }
    else
    {
        Message_Block[Message_Block_Index++] = 0x80;
        while (Message_Block_Index < 56)
            Message_Block[Message_Block_Index++] = 0;
    }

    Message_Block[56] = (Length_High >> 24) & 0xFF;
    Message_Block[57] = (Length_High >> 16) & 0xFF;
    Message_Block[58] = (Length_High >> 8) & 0xFF;
    Message_Block[59] = (Length_High) & 0xFF;
    Message_Block[60] = (Length_Low >> 24) & 0xFF;
    Message_Block[61] = (Length_Low >> 16) & 0xFF;
    Message_Block[62] = (Length_Low >> 8) & 0xFF;
    Message_Block[63] = (Length_Low) & 0xFF;

    SHA1ProcessMessageBlock();
}

sha1::sha1()
{
}

void sha1::start()
{
    Message_Digest[0] = 0x67452301;
    Message_Digest[1] = 0xEFCDAB89;
    Message_Digest[2] = 0x98BADCFE;
    Message_Digest[3] = 0x10325476;
    Message_Digest[4] = 0xC3D2E1F0;
    Length_Low = 0;
    Length_High = 0;
    Message_Block_Index = 0;
    Computed = 0;
    Corrupted = 0;
}

void sha1::write(const void* buf, int len)
{
    if (!len)
        return;

    if (Computed || Corrupted)
    {
        Corrupted = 1;
        return;
    }

    const unsigned char * message_array = (const unsigned char *) buf;
    while (len-- && !Corrupted)
    {
        Message_Block[Message_Block_Index++] = (*message_array & 0xFF);

        Length_Low += 8;
        /* Force it to 32 bits */
        Length_Low &= 0xFFFFFFFF;
        if (Length_Low == 0)
        {
            Length_High++;
            /* Force it to 32 bits */
            Length_High &= 0xFFFFFFFF;
            if (Length_High == 0) /* Message is too long */
                Corrupted = 1;
        }

        if (Message_Block_Index == 64)
            SHA1ProcessMessageBlock();

        message_array++;
    }
}

void sha1::end()
{
    if (Corrupted)
        return;

    if (!Computed)
    {
        SHA1PadMessage();
        Computed = 1;
    }

    unsigned char* head = (unsigned char*) Message_Digest;
    for (int i = 3; i < 20; i += 4)
    {
//        filter::write(head + i, 1);
//        filter::write(head + i - 1, 1);
//        filter::write(head + i - 2, 1);
//        filter::write(head + i - 3, 1);
    }
}
