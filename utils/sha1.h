#ifndef SHA1_H
#define	SHA1_H

class sha1
{
public:
    sha1();
	void start();
	void write(const void* buf, int len);
	void end();

private:
    void SHA1ProcessMessageBlock();
    void SHA1PadMessage();

private:
    unsigned Message_Digest[5]; /* Message Digest (output)          */

    unsigned Length_Low; /* Message length in bits           */
    unsigned Length_High; /* Message length in bits           */

    unsigned char Message_Block[64]; /* 512-bit message blocks      */
    int Message_Block_Index; /* Index into message block array   */

    int Computed; /* Is the digest computed?          */
    int Corrupted; /* Is the message digest corruped?  */
};

#endif	/* SHA1_H */

