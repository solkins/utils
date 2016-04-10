#ifndef BITS_H
#define BITS_H

class bits
{
public:
    static bool isbigendian()
    {
        static const unsigned int one = 1;
        return *(unsigned char*)&one == 0;
    }

    static void reverse(void* data, int len)
    {
        unsigned char* head = (unsigned char*)data;
        unsigned char* tail = head + len - 1;
        int half = len >> 1;
        unsigned char c;
        for (int i=0; i<half; ++i)
        {
            c = *head;
            *head = *tail;
            *tail = c;
            ++head;
            --tail;
        }
    }

    static void reverse(char* s)
    {
        reverse(s, strlen(s));
    }

    template<typename T>
    static void reverse(T* t)
    {
        reverse(t, sizeof(T));
    }
};

#endif // BITS_H
