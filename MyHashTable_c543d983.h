#ifndef __MYHASHTABLE_H__
#define __MYHASHTABLE_H__

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <string>

#include "MyVector_c543d983.h"
#include "MyLinkedList_c543d983.h"

static const long long uh_param_a = 53;       // universal hash function parameter a
static const long long uh_param_b = 97;       // universal hash function parameter b
static const long long prime_digits = 19;     // parameter used for finding a Mersenne prime
static const long long mersenne_prime = (1 << prime_digits) - 1;  // the Mersenne prime for universal hashing

// fast calculation of (n modulo mersenne_prime)
long long fastMersenneModulo(const long long n)
{
    // code begins

    long long mp = (1<<prime_digits) - 1;   // gets the mersenne prime

    long long mod = n;  

    while (mod > mp) { 
        mod = (mod >> prime_digits) + (mod & mp); 
    }

    if (mod >= mp) mod-=mp;
    
    return mod;

    // code ends
}

// definition of the template hash function class
template <typename KeyType>
class HashFunc
{
  public:
    long long univHash(const KeyType key, const long long table_size) const;
};

// the hash function class that supports the hashing of the "long long" data type
template <>
class HashFunc<long long>
{
  public:
    long long univHash(const long long key, const long long table_size) const
    {
        long long hv = fastMersenneModulo(static_cast<long long>(uh_param_a * key + uh_param_b));
        hv = hv % table_size;
        return hv;
    }
};

// the has function class that supports the hashing of the "std::string" data type
template <>
class HashFunc<std::string>
{
  private:
    const int param_base = 37;      // the base used for inflating each character
  public:    
    long long univHash(const std::string& key, const long long table_size) const
    {
        long long hv = 0;
        for(size_t i = 0; i < key.length(); ++ i)
        {
            hv = param_base * hv + static_cast<long long>(key[i]);
        }
        hv = fastMersenneModulo(static_cast<long long>(uh_param_a * hv + uh_param_b));
        hv = hv % table_size;
        return hv;
    }
};

// definition of the template hashed object class
template <typename KeyType, typename ValueType>
class HashedObj
{
  public:

    KeyType key;
    ValueType value;

    HashedObj()
    {
        return;
    }

    HashedObj(const KeyType& k, const ValueType& v) :
        key(k),
        value(v)
    {
        return;
    }
  
    HashedObj(KeyType && k, ValueType && v) :
        key(std::move(k)),
        value(std::move(v))
    {
        return;
    }

    bool operator==(const HashedObj<KeyType, ValueType>& rhs)
    {
        return (key == rhs.key);
    }    

    bool operator!=(const HashedObj<KeyType, ValueType>& rhs)
    {
        return !(*this == rhs);
    }   

};

template <typename KeyType, typename ValueType>
class MyHashTable
{ 
  private:
    size_t theSize; // the number of data elements stored in the hash table
    MyVector<MyLinkedList<HashedObj<KeyType, ValueType> >* > hash_table;    // the hash table implementing the separate chaining approach
    MyVector<size_t> primes;    // a set of precomputed and sorted prime numbers

    // pre-calculate a set of primes using the sieve of Eratosthenes algorithm
    // will be called if table doubling requires a larger prime number for table size
    // expected to update the private member "primes"
    void preCalPrimes(const size_t n)
    {
        // code begins

        std::vector<bool> isPrime(n+1, true); 
        isPrime[0] = isPrime[1] = false; 
        for (size_t i = 2; i * i <= n; ++i)
        {
            if(isPrime[i])  //if there is a value
            {
                for (size_t j = i * i; j <= n; j += i) 
                    isPrime[j] = false;
            }
        }
        primes.resize(0);      
        for (size_t i = 2; i <= n; ++i)
        {
            if(isPrime[i])   
                primes.push_back(i);     
        }

        // code ends
    }

    // finding the smallest prime that is larger than or equal to n
    // should perform binary search against the private member "primes"
    size_t nextPrime(const size_t n)
    {
        // code begins

        if (primes.size() == 0 || n > primes[primes.size()-1])
        {
            preCalPrimes(n * 10);   //calculate them for use
        }

        int left = 0;   
        int right = primes.size()-1;   
        size_t num = primes[right];     
        while (left <= right){     
            int mid = (right-left / 2) + left;  
            if(primes[mid] >= n){   
                num = primes[mid];  
                if (left == right){    
                    break;
                }
                right = mid - 1;    
            }
            else{
                left = mid + 1;    
            }
        }

        return num; 

        // code ends
    }

    // finds the MyLinkedList itertor that corresponds to the hashed object that has the specified key
    // returns the end() iterator if not found
    typename MyLinkedList<HashedObj<KeyType, ValueType> >::iterator find(const KeyType& key)
    {
        // code begins
        HashFunc<KeyType> hash_func;    
        size_t bucket = hash_func.univHash(key, hash_table.size()); 
        typename MyLinkedList<HashedObj<KeyType, ValueType> >::iterator start = hash_table[bucket] -> begin();  
        typename MyLinkedList<HashedObj<KeyType, ValueType> >::iterator end = hash_table[bucket] -> end(); 

        for (typename MyLinkedList<HashedObj<KeyType, ValueType> >::iterator it = start; it != end; ++it){ 
            if ((*it).key == key){  
                return it;  
            }
        }
        return end; //otherwise return end iterator


        // code ends
    }

    // rehashes all data elements in the hash table into a new hash table with new_size
    // note that the new_size can be either smaller or larger than the existing size
    void rehash(const size_t new_size)
    {
        // code begins

            size_t oldSize = theSize;  

            MyVector<MyLinkedList<HashedObj<KeyType, ValueType> >* > tmp_hash_table(new_size); 
            
            for(size_t i = 0; i < tmp_hash_table.size(); ++ i){
                tmp_hash_table[i] = new MyLinkedList<HashedObj<KeyType, ValueType> >;   
            }

            std::swap(hash_table, tmp_hash_table); 

            for(size_t i = 0; i < tmp_hash_table.size(); ++ i){
                for(auto itr = tmp_hash_table[i]->begin(); itr != tmp_hash_table[i]->end(); ++ itr){ 
                    insert(std::move(*itr));
                }
            }

            theSize = oldSize;  
            return;

        // code ends
    }

    // doubles the size of the table and perform rehashing
    // the new table size should be the smallest prime that is larger than the expected new table size (double of the old size)
    void doubleTable()
    {
        size_t new_size = nextPrime(2 * hash_table.size()); 
        this->rehash(new_size); 
        return;
    }

    // halves the size of the table and perform rehahsing
    // the new table size should be the smallest prime that is larger than the expected new table size (half of the old size)
    void halveTable()
    {
        size_t new_size = nextPrime(ceil(hash_table.size() / 2)); 
        this->rehash(new_size); 
        return;
    }

  public:

    // the default constructor; allocate memory if necessary
    explicit MyHashTable(const size_t init_size = 3)
    {
        // code begins

        hash_table.resize(init_size);  
        for (size_t i = 0; i< init_size; ++i){
            hash_table[i] = new MyLinkedList<HashedObj<KeyType, ValueType> >;  
        }
        theSize = 0;   
        return;

        // code ends
    }

    // the default destructor; collect memory if necessary
    ~MyHashTable()
    {
        // code begins

        for (size_t i = 0; i < hash_table.size(); ++ i){
            delete hash_table[i];        
        }
        return;

        // code ends
    }

    // checks if the hash tabel contains the given key
    bool contains(const KeyType& key)
    {
        // code begins

        HashFunc<KeyType> hash_func;  

        long long bucket = hash_func.univHash(key, static_cast<long long>(hash_table.size()));  

        for(auto itr = hash_table[bucket]->begin(); itr != hash_table[bucket]->end(); ++ itr){  
            if ((*itr).key == key) return true;     
        }
        return false;  



        // code ends
    }

    // retrieves the data element that has the specified key
    // returns true if the key is contained in the hash table
    // return false otherwise
    bool retrieve(const KeyType& key, HashedObj<KeyType, ValueType>& data)
    {
        // code begins

        HashFunc<KeyType> hash_func;

        long long bucket = hash_func.univHash(key, static_cast<long long>(hash_table.size()));

        for(auto itr = hash_table[bucket]->begin(); itr != hash_table[bucket]->end(); ++ itr){ 
            if ((*itr).key == key){
                data = *itr;
                return true;   
            }
        }
        return false;  

        // code ends
    }

    // inserts the given data element into the hash table (copy)
    // returns true if the key is not contained in the hash table
    // return false otherwise
    bool insert(const HashedObj<KeyType, ValueType>& x)
    {
        // code begins

        if(contains(x.key)) return false;  

        HashFunc<KeyType> hash_func;
        
        long long bucket = hash_func.univHash(x.key, static_cast<long long>(hash_table.size()));
        
        hash_table[bucket]->push_back(x);  
        
        ++theSize;
        
        if(2 * theSize >= hash_table.size()) doubleTable(); return true;
        
        // code ends
    }

    // inserts the given data element into the hash table (move)
    // returns true if the key is not contained in the hash table
    // return false otherwise
    bool insert(HashedObj<KeyType, ValueType> && x)
    {
        // code begins

        if(contains(x.key)) return false;   

        HashFunc<KeyType> hash_func;
        
        long long bucket = hash_func.univHash(x.key, static_cast<long long>(hash_table.size()));
        
        hash_table[bucket]->push_back(std::move(x));    
        
        ++theSize;  
        
        if(2 * theSize >= hash_table.size()) doubleTable(); return true;   

        // code ends
    }

    // removes the data element that has the key from the hash table
    // returns true if the key is contained in the hash table
    // returns false otherwise
    bool remove(const KeyType& key)
    {
        // code begins

        HashFunc<KeyType> hash_func;

        long long bucket = hash_func.univHash(key, static_cast<long long>(hash_table.size()));

        for(auto itr = hash_table[bucket]->begin(); itr != hash_table[bucket]->end(); ++ itr){  
            if((*itr).key == key){
                hash_table[bucket]->erase(itr); 
                -- theSize;    
                if(8 * theSize < hash_table.size()) halveTable(); 
                    return true;
                }
            }
        return false;   

        // code ends
    }

    // returns the number of data elements stored in the hash table
    size_t size()
    {
        // code begins

        return theSize;

        // code ends
    }

    // returns the capacity of the hash table
    size_t capacity()
    {
        // code begins

        return hash_table.size();  

        // code ends
    }

};


#endif // __MYHASHTABLE_H__
