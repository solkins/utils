#ifndef AES_H
#define	AES_H

class aes
{
public:
    aes(unsigned char *key, int nbits);

    void encrypt(unsigned char input[], unsigned char output[]);
    void decrypt(unsigned char input[], unsigned char output[]);

private:
    unsigned int KT0[256];
    unsigned int KT1[256];
    unsigned int KT2[256];
    unsigned int KT3[256];
    unsigned int erk[64]; /* encryption round keys */
    unsigned int drk[64]; /* decryption round keys */
    int nr; /* number of rounds */
};

#endif	/* AES_H */
