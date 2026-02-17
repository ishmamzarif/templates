#include<bits/stdc++.h>
using namespace std;


bool hashfunc1;
bool hashfunc2;
int c1, c2;

bool isPrime(int n){
    if(n <= 1){
        return  false;
    }

    if(n==2 || n==3){
        return true;
    }

    if(n%2 == 0 || n%3 == 0){
        return false;
    }

    int divisor = 6;
    while((divisor-1)*(divisor-1) <= n){
        if(n % (divisor-1) == 0){
            return false;
        }
        if(n % (divisor + 1) == 0){
            return false;
        }

        divisor = divisor + 6;
    }
    return true;
}

int nextPrime(int num){
    ++num;
    while(!isPrime(num)){
        ++num;
    }
    return num;
}

int prevPrime(int num){
    --num;
    while(!isPrime(num)){
        --num;
    }
    return num;
}

template<typename key, typename value>
class HashTableCustom{
private:
    enum EntryType{ACTIVE, EMPTY, DELETED}; // Lazy Deletion

    struct HashEntry{
        key element;
        value element_value;
        EntryType info;

        HashEntry(const key& e = key{}, const value& ev = value{}, EntryType i = EMPTY) : element(e), element_value(ev) , info(i){}
    };

    vector<HashEntry> array;

    int current_size;
    int table_size;
    int initial_size;
    double load_factor;
    double max_cut_off;
    double min_cut_off;

    int lastinsert, lastinsert_size;
    int lastdelete, lastdelete_size;

    // dynamically resizing prime number
    int rehash(){
        vector<HashEntry> oldArray = array;
        int count = 0;

        if(load_factor > max_cut_off && lastinsert >= lastinsert_size/2){
            table_size = nextPrime(2* table_size);
            array.resize(table_size);
            for(int j=0; j< array.size(); ++j){
                array[j].info = EMPTY;
            }

            current_size = 0;
            for(int i=0; i< oldArray.size(); ++i){
                if(oldArray[i].info == ACTIVE){
                    count = count + (insert(oldArray[i].element, oldArray[i].element_value)).second;
                }
            }

            lastinsert = 0;
            lastinsert_size = current_size;
        }

        else if(load_factor < min_cut_off && lastdelete >= lastdelete_size/2 && table_size > initial_size){
            table_size = prevPrime(table_size/2);
            array.resize(table_size);

            for(int j=0; j< array.size(); ++j){
                array[j].info = EMPTY;
            }
            current_size = 0;

            for(int i=0; i<oldArray.size(); ++i){
                if(oldArray[i].info == ACTIVE){
                    count = count + (insert(oldArray[i].element, oldArray[i].element_value)).second;
                }
            }

            lastdelete = 0;
            lastdelete_size = current_size;
        }
        return count;
    }

    int myhash(const key& k){
        int hashVal;

        stringstream ss;
        ss << k;

        string k_str = ss.str();

        if(hashfunc1){
            hashVal = hash1(k_str) % table_size;
        }else{
            hashVal = hash2(k_str) % table_size;
        }

        if(hashVal < 0){
            hashVal = hashVal + table_size;
        }
        return hashVal;
    }

    bool isActive(int currentPos){
        return array[currentPos].info == ACTIVE;
    }

    unsigned int hash1(string k) {
        unsigned int hash_val = 0;
    
        int prime = 37;
        for (char c : k) {
            hash_val = (hash_val *prime  + c);
        }
    
        hash_val = hash_val ^ (hash_val >> 16);
    
        return hash_val;
    }

    unsigned int hash2(string k){
        unsigned int hash_val = 2166136261U; // FNV offset basis
        for (char c : k) {
            hash_val ^= (unsigned char)c;
            hash_val *= 16777619U; // FNV prime
        }
        return hash_val;
    }
    
    unsigned int auxHash(const key& x){
        stringstream ss;
        ss << x;

        string k_str = ss.str();
        unsigned int hash = 5381;
        for (char c : k_str) {
            hash = ((hash << 5) + hash) + static_cast<unsigned char>(c);
        }

        int r = prevPrime(table_size);
        hash = r - (hash % r);

        return hash;
    }

    int customhash(const key& x, int i){
        int hashVal =  (myhash(x) + c1*i*auxHash(x) + c2*i*i) % table_size;

        if(hashVal < 0){
            hashVal = hashVal + table_size;
        }
        return hashVal;
    }

    pair<int,int> findPos(const key& x){
        int currentPos = customhash(x, 0);
        int count = 0;
        int i =1;

        bool firstDeleted = true;
        int firstDeletedPos = -1;

        if(array[currentPos].info == DELETED && firstDeleted){
            firstDeletedPos = currentPos;
            firstDeleted = false;
        }

        while(array[currentPos].info != EMPTY && array[currentPos].element !=x){
            currentPos = customhash(x, i);
            ++i;
            ++count;

            if(array[currentPos].info == DELETED && firstDeleted){
                firstDeletedPos = currentPos;
                firstDeleted = false;
            }
        }

        if(array[currentPos].element == x){
            return {currentPos, count};
        }else{
            int temp =  firstDeletedPos != -1 ? firstDeletedPos : currentPos;
            return {temp, count};
        }
    }

public:

    HashTableCustom(int t_size = 13, double max_c_f = 0.5, double min_c_f = 0.25){ // single-source variables
        table_size = isPrime(t_size) == true ? t_size : nextPrime(t_size);
        initial_size = table_size;
        max_cut_off = max_c_f;
        min_cut_off = min_c_f;
        load_factor = 0.0;
        array.resize(table_size);

        lastinsert = 0, lastinsert_size = 0;
        lastdelete = 0, lastdelete_size = 0;
    }

    // insertion
    pair<bool,int> insert(const key& x, const value& v){
        pair<int,int> temp = findPos(x);

        int currentPos = temp.first;
        int count = temp.second;

        if(isActive(currentPos)){
            return {false, count}; // No duplicates
        }

        array[currentPos] = HashEntry(x, v, ACTIVE);
        load_factor = (++current_size)/(table_size*1.0);
        lastinsert = lastinsert + 1;

        if(load_factor > max_cut_off && lastinsert >= lastinsert_size/2){
            count = count + rehash();
        }

        return {true,count};
    }

    // deletion
    pair<bool,int> remove(const key& x){
        pair<int,int> temp = findPos(x);

        int currentPos = temp.first;
        int count = temp.second;

        if(!isActive(currentPos)){
            return {false, count};
        }

        array[currentPos].info = DELETED;
        load_factor = (--current_size)/(table_size*1.0);

        lastdelete = lastdelete + 1;

        if(load_factor < min_cut_off && lastdelete >= lastdelete_size/2 && table_size> initial_size){
            count = count + rehash();
        }

        return {true,count};
    }

    // search
    pair<bool,pair<value, int>> search(const key& x){
        int currentPos = customhash(x, 0);
        int count = 0;
        int i = 1;

        while(array[currentPos].info != EMPTY && array[currentPos].element !=x){
            currentPos = customhash(x, i);
            ++count;
            ++i;
        }

        if(array[currentPos].info == EMPTY){
            return {false, {value{}, count+1}};
        }else{
            return {true, {array[currentPos].element_value, count+1}};
        }
    }
};

int main(){
    return 0;
}