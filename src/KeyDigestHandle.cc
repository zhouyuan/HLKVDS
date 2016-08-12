#include <stdio.h>
#include <stdlib.h>

#include "KeyDigestHandle.h"

namespace kvdb{

    Kvdb_Key::~Kvdb_Key()
    {
        if (value)
        {
            delete[] value;
        }
        len = 0;
    }

    Kvdb_Key::Kvdb_Key(const char* key, uint32_t key_len)
    {
        len = key_len;
        value = new char[len];
        memcpy(value, key, len);
    }

    Kvdb_Key::Kvdb_Key(Kvdb_Key& toBeCopied)
    {
        len = toBeCopied.len;
        value = new char[len];
        memcpy(value, toBeCopied.value, len);
    }

    Kvdb_Digest::Kvdb_Digest()
    {
        memset(value, 0, sizeof(Kvdb_Digest));
    }

    Kvdb_Digest::Kvdb_Digest(const Kvdb_Digest& toBeCopied)
    {
        memcpy(value, toBeCopied.value, sizeof(Kvdb_Digest));
    }

    Kvdb_Digest::~Kvdb_Digest()
    {
        return;
    }

    Kvdb_Digest& Kvdb_Digest::operator=(const Kvdb_Digest& toBeCopied)
    {
        memcpy(value, toBeCopied.value, sizeof(Kvdb_Digest));
        return *this;
    }

    bool Kvdb_Digest::operator==(const Kvdb_Digest& toBeCompare)
    {
        if (!memcmp(value, toBeCompare.value, sizeof(Kvdb_Digest)))
        {
            return true;
        }
        return false;
    }

    void KeyDigestHandle::ComputeDigest(Kvdb_Key *key, Kvdb_Digest &digest)
    /*
     * returns RMD(message)
     * message should be a string terminated by '\0'
     */
    {
        dword         MDbuf[RMDsize/32];   /* contains (A, B, C, D(, E))   */
        //static byte   hashcode[RMDsize/8]; /* for final hash-value         */
        byte          hashcode[RMDsize/8]; /* for final hash-value         */
        dword         X[16];               /* current 16-word chunk        */
        unsigned int  i;                   /* counter                      */
        dword         length;              /* length in bytes of message   */
        dword         nbytes;              /* # of bytes not yet processed */
     
        /* initialize */
        MDinit(MDbuf);
        //length = (dword)strlen((char *)message);
        //length = key->len;

        length = key->GetLen();
        char* value = key->GetValue();

        /* process message in 16-word chunks */
        for (nbytes = length; nbytes > 63; nbytes -= 64) 
        {
            for (i = 0; i < 16; i++)
            {
                //X[i] = BYTES_TO_DWORD(key->value);
                X[i] = BYTES_TO_DWORD(value);
                //key->value += 4;
                value += 4;
            }
            compress(MDbuf, X);
        }                                    /* length mod 64 bytes left */
     
        /* finish: */
        //MDfinish(MDbuf, (byte *)key->value, length, 0);
        MDfinish(MDbuf, (byte *)value, length, 0);
     
        for (i = 0; i < RMDsize / 8; i += 4)
        {
            hashcode[i]   =  MDbuf[i>>2];         /* implicit cast to byte  */
            hashcode[i+1] = (MDbuf[i>>2] >>  8);  /*  extracts the 8 least  */
            hashcode[i+2] = (MDbuf[i>>2] >> 16);  /*  significant bits.     */
            hashcode[i+3] = (MDbuf[i>>2] >> 24);
        }
     
        //return (unsigned char*)hashcode;
        memcpy(digest.value, hashcode, sizeof(Kvdb_Digest));
    }
    
    uint32_t KeyDigestHandle::Hash(Kvdb_Key *key)
    {
        //Kvdb_Digest *result = new Kvdb_Digest;
        Kvdb_Digest result;
        //ComputeDigest(key, *result);
        ComputeDigest(key, result);

        //uint32_t hash_value = Hash(result);
        uint32_t hash_value = Hash(&result);

        //delete result;
        return hash_value;
    }

    uint32_t KeyDigestHandle::Hash(Kvdb_Digest *digest)
    {
        uint32_t hash_value;
        uint32_t *pi = &digest->value[4];
        unsigned char key_char[4];
        key_char[0] = *((char *)pi+0);
        key_char[1] = *((char *)pi+1);
        key_char[2] = *((char *)pi+2);
        key_char[3] = *((char *)pi+3);
        hash_value = (key_char[0]) + (key_char[1] << 8) + (key_char[2] << 16) + (key_char[3] << 24);
        return hash_value;
    }
    
    string KeyDigestHandle::Tostring(Kvdb_Digest *digest)
    {
        int digest_size = sizeof(Kvdb_Digest);
        unsigned char *temp = new unsigned char[digest_size];
        memcpy(temp, digest, digest_size);

        int str_len = 2 * digest_size + 1;
        char *res = new char[str_len];
        for (int i = 0; i < digest_size; i++)
        {
            sprintf(&res[2 * i],  "%02x",  temp[i]);
        }
        //res[str_len-1]= '\0';
        string result = string((const char*)res);
        delete[] res;
        delete[] temp;
        return result;
    }
}
