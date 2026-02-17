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
class HashTableChaining{
private:

    vector<list<pair<key, value>>> theLists;

    int current_size;
    int table_size;
    int initial_size;
    double load_factor;
    double max_cut_off, min_cut_off;

    int lastinsert, lastdelete;
    int lastinsert_size, lastdelete_size;

    // dynamically resizing prime number
    int rehash(){
        vector<list<pair<key, value>>> oldLists = theLists;
        int count = 0;

        if(load_factor > max_cut_off && lastinsert >= lastinsert_size/2){

            table_size = nextPrime(2* table_size);
            theLists.resize(table_size);
            
            for(int j=0; j< theLists.size(); ++j){
                theLists[j].clear();
            }

            current_size = 0;
            load_factor = 0.0;
            for(int i=0; i< oldLists.size(); ++i){
                typename list<pair<key,value>>::iterator it = oldLists[i].begin();

                while(it != oldLists[i].end()){
                    count = count + (insert((*it).first, (*it).second)).second;
                    it++;
                }
            }

            lastinsert = 0;
            lastinsert_size = current_size;
        }

        else if(load_factor < min_cut_off && lastdelete >= lastdelete_size/2 && table_size > initial_size){
            table_size = prevPrime(table_size / 2);

            theLists.resize(table_size);

            for(int j=0; j< theLists.size(); ++j){
                theLists[j].clear();
            }

            current_size = 0;
            load_factor = 0.0;
            for(int i=0; i< oldLists.size(); ++i){
                typename list<pair<key,value>>::iterator it = oldLists[i].begin();

                while(it != oldLists[i].end()){
                    count = count + (insert((*it).first, (*it).second)).second;
                    ++it;
                }
            }

            lastdelete = 0;
            lastdelete_size = current_size;
        }
        return count;
    }

    int myHash(const key& k){
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

    unsigned int hash1(string k) {
        unsigned int hash_val = 0;
    
        int prime = 37;
        for (char c : k) {
            hash_val = (hash_val * prime + c);
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

public:

    HashTableChaining(int t_size = 13, double max_c_f = 0.5, double min_c_f = 0.25){ // single-source variables
        table_size = isPrime(t_size) == true ? t_size : nextPrime(t_size);
        initial_size = table_size;
        max_cut_off = max_c_f;
        min_cut_off = min_c_f;
        load_factor = 0.0;

        lastinsert = 0, lastinsert_size = 0;
        lastdelete = 0, lastdelete_size = 0;

        theLists.resize(table_size);
    }

    // insertion
    pair<bool,int> insert(const key& x, const value& v){
        int indx = myHash(x);

        list<pair<key, value>>& whichList = theLists[indx];
        int count = 0;

        pair<bool,pair<value, int>> temp = search(x);
        if(temp.first == true){
            return {false, count};
        }

        if(!whichList.empty()){
            count = 1;
        }

        pair<key, value> p = {x,v};
        whichList.push_back(p);

        load_factor = (++current_size)/(table_size*1.0);
        lastinsert = lastinsert + 1;

        if(load_factor > max_cut_off && lastinsert >= lastinsert_size/2){
            count = count + rehash();
        }

        return {true,count};
    }

    // deletion
    pair<bool,int> remove(const key& x){
        int indx = myHash(x);
        list<pair<key, value>>& whichList = theLists[indx];
        
        pair<bool,pair<value, int>> temp = search(x);
        int count = temp.second.second;

        if(temp.first == true){ // only remove if found
            whichList.remove({x, temp.second.first});

            load_factor = (--current_size)/(table_size*1.0);

            lastdelete = lastdelete + 1;

            if(load_factor < min_cut_off && lastdelete >= lastdelete_size/2 && table_size > initial_size){
                count = count + rehash();
            }

            return {true, count};
        }

        return {false, count};
    }

    // search
    pair<bool,pair<value, int>> search(const key& x){
        int indx = myHash(x);
        const list<pair<key, value>>& whichList = theLists[indx];

        int count =0;
        for(pair<key, value> temp : whichList){
            if(temp.first == x){
                pair<value, int> p = {temp.second, count+1};
                pair<bool, pair<value,int>> rt = {true, p};
                return rt;
            }
            ++count;
        }
        return {false, {value{}, count + 1}}; // missing
    }
};


int main(){

    // the class has three functions
        // bool hashfunc1 hashfunc2;--- must be set to make sure which hashfunction to use

    // pair<bool,int> insert(const key& x, const value& v)
        // every insertion takes input key-value pair
        // using key -- uses the hash function-- gets the list
        // searches for the key-- whether the key is present before-- doesn't count this collision if not found
        // if found then only counts it
        // push_back the pair
        // updates the load_factor
        // returns bool--- for insertion success or not
        // returns int--- for the amount of collision


    // pair<bool,int> remove(const key& x)
        // it takes the key as input-- uses the hash function-- gets the list
        // searches for the key-- whether the key is present before
        // if found, then it removes the key-value pair--- updates the load_factor(only with certain conditions met)
        // if not found, returns false and the collision number
        // returns bool--- for insertion success or not
        // returns int--- for the amount of collision


    // pair<bool,pair<value, int>> search(const key& x)
        // it takes the key as input-- uses the hash function-- gets the list
        // bool --- whether the code was able to find it-- key present or not
        // value --- what value is there for the key -- if not present then dummy
        // int --- how many collisions did it take to find the key

    
    // helper functions
        // isPrime, nextPrime, prevPrime--- all takes integer input

}