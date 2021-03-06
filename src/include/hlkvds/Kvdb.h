#ifndef _HLKVDS_KVDB_H_
#define _HLKVDS_KVDB_H_

#include <iostream>
#include <string>
#include <mutex>

#include "hlkvds/Options.h"
#include "hlkvds/Status.h"
#include "hlkvds/Write_batch.h"
#include "hlkvds/Iterator.h"

namespace hlkvds {

class KVDS;

class DB {
public:
    static bool CreateDB(std::string filename, Options opts = Options());
    static bool OpenDB(std::string filename, DB** db, Options opts = Options());

    virtual ~DB();

    Status Insert(const char* key, uint32_t key_len, const char* data,
                uint16_t length, bool immediately = false);
    Status Delete(const char* key, uint32_t key_len);
    Status Get(const char* key, uint32_t key_len, std::string &data);

    Status InsertBatch(WriteBatch *batch);
    Iterator* NewIterator();

    void Do_GC();
    void printDbStates();

private:
    DB();
    DB(const DB &);
    DB& operator=(const DB &);

    //singleton reaper
    class DBReaper {
    public:
        DBReaper();
        ~DBReaper();
    };
    friend class DBReaper;
    static DBReaper reaper_;


    KVDS *kvds_;
    std::mutex mtx_;
    static DB *db_;
};

} // namespace hlkvds

#endif //_HLKVDS_KVDB_H_
