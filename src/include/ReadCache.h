#ifndef READ_CACHE_H
#define REACH_CACHE_H

#include <cstdint> 
#include <iostream> 
#include <functional>
#include <vector>
#include <mutex> 
#include "ReadCacheSegment.h" 
using namespace std;
#define HASH_SIZE_DEFAULT 1031 // A prime number as hash size gives a better distribution of values in buckets
namespace dslab //Concurrent Thread Safe Library
{
    //The class represting the hash map.
    //It is expected for user defined types, the hash function will be provided.
    //By default, the std::hash function will be used
    //If the hash size is not provided, then a defult size of 1031 will be used
    //The hash table itself consists of an array of hash buckets.
    //Each hash bucket is implemented as singly linked list with the head as a dummy node created 
    //during the creation of the bucket. All the hash buckets are created during the construction of the map.
    //Locks are taken per bucket, hence multiple threads can write simultaneously in different buckets in the hash map
    class ReadCache
    {
        public:
            ReadCache(CachePolicy policy = LRU, size_t segment_size = 1024, int percent = 50, bool is_dedup = 0, size_t hashSize_ = HASH_SIZE_DEFAULT):hashSize(hashSize_)
            {
		for(int i = 0; i < hashSize; i++){
			hashTable.push_back(new ReadCacheSegment(policy, segment_size, is_dedup) );//create the hash table as an array of hash buckets
		}
            }

            ~ReadCache()
            {
		for(int i = 0; i < hashSize; i++){
			delete hashTable[i] ;
		}
            }
            //Copy and Move of the HashMap are not supported at this moment
            ReadCache(const ReadCache&) = delete;
            ReadCache(ReadCache&&) = delete;
            ReadCache& operator=(const ReadCache&) = delete;  
            ReadCache& operator=(ReadCache&&) = delete;
        
            //Function to find an entry in the hash map matching the key.
            //If key is found, the corresponding value is copied into the parameter "value" and function returns true.
            //If key is not found, function returns false.
            bool Get(const string &key, string &value) const 
            {
                size_t hashValue = hashFn(key) % hashSize ;
                return hashTable[hashValue]->Get(key, value);
            }

            //Function to insert into the hash map.
            //If key already exists, update the value, else insert a new node in the bucket with the <key, value> pair.
            void Put(const string &key, const string &value) 
            {
                size_t hashValue = hashFn(key) % hashSize ;
                hashTable[hashValue]->Put(key, value);
            }

            //Function to remove an entry from the bucket, if found
            void Delete(const string &key) 
            {
                size_t hashValue = hashFn(key) % hashSize ;
                hashTable[hashValue]->Delete(key);
            }   

        private:
            vector<ReadCacheSegment*> hashTable;
            hash<string> hashFn;
            size_t hashSize;
    };
}
#endif

