#include <bits/stdc++.h>
#include <iomanip>
using namespace std;

using ll = long long;
const ll INF = 1e18;
using pll = pair<ll, ll>;
using tll = tuple<ll, ll, ll>;

// single-source variables for configuration
const int INITIAL_TABLE_SIZE = 13;
const double LOAD_FACTOR_UPPER = 0.5;
const double LOAD_FACTOR_LOWER = 0.25;

// custom probing constants
const int C1 = 1;
const int C2 = 3;

// which hash function to choose
const int HASH1 = 1;
const int HASH2 = 2;

// sieve
const int MAX = 1e7 + 1;
vector<bool> is_prime(MAX, true);
vector<int> primes;

void sieve() {
    is_prime[0] = is_prime[1] = false;

    for (int i = 2; i * i < MAX; i++) {
        if (is_prime[i]) {
            for (int j = i * i; j < MAX; j += i) {
                is_prime[j] = false;
            }
        }
    }

    for (int i = 2; i < MAX; i++) {
        if (is_prime[i]) {
            primes.push_back(i);
        }
    }
}

// next closest prime greater than n
int nextPrime(int n) {
    if (n <= 1) return 2;
    
    int left = 0;
    int right = primes.size() - 1;
    int result = primes[primes.size() - 1];
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (primes[mid] > n) {
            result = primes[mid];
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    
    return result;
}

// prev closest prime less than n
int prevPrime(int n) {
    if (n <= 2) return 2;
    
    int left = 0;
    int right = primes.size() - 1;
    int result = 2;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (primes[mid] < n) {
            result = primes[mid];
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    
    return result;
}

ll hash1(const string& key, int tableSize) {
    ll hash = 5381;
    for (int i = 0; i < (int)key.length(); i++) {
        char c = key[i];
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash % tableSize;
}

ll hash2(const string& key, int tableSize) {
    ll hash = 0;
    ll p = 31;
    ll p_pow = 1;

    for (int i = 0; i < (int)key.length(); i++) {
        int charVal = (key[i] - 'a' + 1);
        hash = (hash + charVal * p_pow) % tableSize;
        p_pow = (p_pow * p) % tableSize;
    }

    return hash;
}

// auxiliary hash function for double hashing and custom probing
ll auxHash(const string& key, int tableSize) {
    ll hash = 0;
    for (int i = 0; i < (int)key.length(); i++) {
        char c = key[i];
        hash = hash * 31 + c;
    }

    ll result = hash % (tableSize);
    ll R = prevPrime(tableSize);
    return R - (result % R);
}

template <typename T, typename V>
class HashTableBase {
protected:
    int tableSize;
    int numElements;
    int N;
    int collisionCount;
    int insertionsSinceExpansion;
    int deletionsSinceCompaction;
    int hashType;
    
public:
    HashTableBase(int size = INITIAL_TABLE_SIZE, int hType = 1) : 
        tableSize(size), 
        numElements(0), 
        N(0),
        collisionCount(0),
        insertionsSinceExpansion(0), 
        deletionsSinceCompaction(0),
        hashType(hType) 
    {
        if (tableSize != INITIAL_TABLE_SIZE) {
            // making sure it's the closest prime number
            tableSize = nextPrime(tableSize);
        }
    }
    
    virtual ~HashTableBase() {}

    virtual bool insert(const T& key, const V& value) = 0;
    virtual pair<bool, V> search(const T& key, int& hits) = 0;
    virtual bool remove(const T& key) = 0;
    
    double getLoadFactor() const {
        return 1.0 * numElements / tableSize;
    }
    
    int getCollisionCount() const {
        return collisionCount;
    }
    
    int getNumElements() const {
        return numElements;
    }

    ll myhash(const T& key) {
        stringstream ss;
        ss << key;
        string s = ss.str();

        if (hashType == 1) {
            return hash1(s, tableSize);
        }
        else {
            return hash2(s, tableSize);
        }
    }
};

template <typename T, typename V>
class ChainingHashTable : public HashTableBase <T, V> {
private:
    vector<list<pair<T, V>>> table;
    
    void rehash(int newSize) {
        vector<list<pair<T, V>>> oldTable = table;
        int oldSize = this->tableSize;

        this->tableSize = newSize;
        this->numElements = 0; 

        this->table.clear();
        this->table.resize(newSize);


        for (list<pair<T, V>> &theList : oldTable) {
            for (auto &kv : theList) {
                int idx = this->myhash(kv.first);

                if (!table[idx].empty())
                    this->collisionCount++;

                table[idx].push_back(kv);
                this->numElements++; 
            }
        }
        
        this->N = this->numElements;
    }
    
public:
    ChainingHashTable(int size = INITIAL_TABLE_SIZE, int hType = 1) : HashTableBase<T, V>(size, hType) {
        table.resize(this->tableSize);
    }

    ~ChainingHashTable() {}
    
    bool insert(const T& key, const V& value) {
        // cout << "Inserting " << key << " into ChainingHashTable (size " << this->tableSize << ")" << endl;
        int index = this->myhash(key);
        
        list<pair<T, V>>& theList = table[index];
        
        for (auto &kv : theList) {
            if (kv.first == key) {
                // cout << "found repeat" << endl;
                return false;
            }
        }
        
        // count collision
        if (theList.size() != 0) {
            this->collisionCount++;
        }
        
        // insert
        theList.push_back({key, value});
        this->numElements++;
        this->insertionsSinceExpansion++;

        // check for expansion
        if (this->getLoadFactor() > LOAD_FACTOR_UPPER && this->insertionsSinceExpansion >= this->N / 2) {
            int newSize = nextPrime(2 * this->tableSize);
            this->rehash(newSize);
            this->insertionsSinceExpansion = 0;
        }

        return true;
    }
    
    pair<bool, V> search(const T& key, int& hits) {
        hits = 0;
        int index = this->myhash(key);

        list<pair<T, V>>& theList = table[index];

        for (auto &kv : theList) {
            hits++;

            if (kv.first == key) {
                return {true, kv.second};
            }
        }

        // returns default argument for V
        // cant be nullptr or -1 for compile safety
        return {false, V()};
    }
    
    bool remove(const T& key) {
        int index = this->myhash(key);
        list<pair<T, V>>& theList = table[index];
        
        auto itr = theList.begin();

        for (; itr != theList.end(); itr++) {
            if (itr->first == key) {
                break;
            }
        }

        if (itr == theList.end()) {
            return false;
        }

        theList.erase(itr);
        this->numElements--;
        this->deletionsSinceCompaction++;

        if (this->tableSize > INITIAL_TABLE_SIZE && this->getLoadFactor() < LOAD_FACTOR_LOWER && this->deletionsSinceCompaction >= this->N / 2) {
            int newSize = prevPrime(this->tableSize / 2);
            rehash(newSize);
            this->deletionsSinceCompaction = 0;
        }

        return true;
    }
};

template <typename T, typename V>
class DoubleHashingTable : public HashTableBase <T, V> {
protected:
    struct Slot {
        T key;
        V value;
        bool occupied;
        bool deleted;
        
        Slot() : occupied(false), deleted(false) {}
    };

    vector<Slot> table;
    
    void rehash(int newSize) {
        vector<Slot> oldTable = table;
        int oldSize = this->tableSize;

        this->tableSize = newSize;
        this->N = this->numElements;
        table.clear();
        table.resize(this->tableSize, Slot());

        this->numElements = 0;

        for (Slot &oldSlot : oldTable) {
            if (oldSlot.occupied && !oldSlot.deleted) {
                T key = oldSlot.key;
                V value = oldSlot.value;

                for (int i = 0; i < this->tableSize; i++) {
                    int index = probe(key, i);

                    if (table[index].occupied) {
                        this->collisionCount++;
                    }

                    if (!table[index].occupied) {
                        table[index].key = key;
                        table[index].value = value;

                        table[index].occupied = true;
                        table[index].deleted = false;

                        this->numElements++;
                        break;
                    }
                }

            }
        }
    }
    
public:
    DoubleHashingTable(int hType, int size = INITIAL_TABLE_SIZE) : HashTableBase<T, V>(size, hType) {
        table.resize(this->tableSize);
    }

    // double hash function 
    ll probe(const T& key, int i) {
        ll h = this->myhash(key);
        stringstream ss;
        ss << key;
        ll aux = auxHash(ss.str(), this->tableSize);
        return (h + (ll)i * aux) % this->tableSize;
    }

    void printProbeSequence(const T& key) {
        for (int i = 0; i < this->tableSize; i++) {
            int index = probe(key, i);

            cout << index << " ";

            if (table[index].occupied && !table[index].deleted && table[index].key == key) {
                return;
            }
            else if (!table[index].occupied && !table[index].deleted) {
                return;
            }
            else {
                cout << "-> ";
            }
        }
    }
    
    bool insert(const T& key, const V& value) {
        // check duplicate
        int h = 0;
        if (search(key, h).first == true) {
            return false;
        }

        for (int i = 0; i < this->tableSize; i++) {
            int index = probe(key, i);

            if (table[index].occupied) {
                this->collisionCount++;
            }

            if (!table[index].occupied) {
                table[index].key = key;
                table[index].value = value;

                table[index].occupied = true;
                table[index].deleted = false;

                this->numElements++;
                this->insertionsSinceExpansion++;

                if (this->getLoadFactor() > LOAD_FACTOR_UPPER && this->insertionsSinceExpansion >= this->N / 2) {
                    int newSize = nextPrime(2 * this->tableSize);
                    rehash(newSize);
                    this->insertionsSinceExpansion = 0;
                }
                return true;
            }
        }
        return false;
    }
    
    pair<bool, V> search(const T& key, int& hits) {
        hits = 0;
        for (int i = 0; i < this->tableSize; i++) {
            hits++;
            int index = probe(key, i);

            if (table[index].occupied && !table[index].deleted && table[index].key == key) {
                return {true, table[index].value};
            }
            else if (!table[index].occupied && !table[index].deleted) {
                return {false, V()};
            }
        }
        return {false, V()};
    }
    
    bool remove(const T& key) {
        for (int i = 0; i < this->tableSize; i++) {
            int index = probe(key, i);

            if (!table[index].occupied && !table[index].deleted) {
                return false;
            }            

            if (table[index].occupied && !table[index].deleted && table[index].key == key) {
                table[index].deleted = true;
                table[index].occupied = false;

                this->numElements--;
                this->deletionsSinceCompaction++;
                
                if (this->tableSize > INITIAL_TABLE_SIZE && 
                    this->getLoadFactor() < LOAD_FACTOR_LOWER && 
                    this->deletionsSinceCompaction >= this->N / 2) {
                    int newSize = prevPrime(this->tableSize / 2);
                    rehash(newSize);
                    this->deletionsSinceCompaction = 0;
                }
                return true;
            }
        }
        return false;
    }
};


template <typename T, typename V>
class CustomHashingTable : public HashTableBase <T, V> {
protected:
    struct Slot {
        T key;
        V value;
        bool occupied;
        bool deleted;
        
        Slot() : occupied(false), deleted(false) {}
    };

    vector<Slot> table;
    
    void rehash(int newSize) {
        vector<Slot> oldTable = table;
        int oldSize = this->tableSize;

        this->tableSize = newSize;
        this->N = this->numElements;
        table.clear();
        table.resize(this->tableSize, Slot());

        this->numElements = 0;

        for (Slot &oldSlot : oldTable) {
            if (oldSlot.occupied && !oldSlot.deleted) {
                T key = oldSlot.key;
                V value = oldSlot.value;
                for (int i = 0; i < this->tableSize; i++) {
                    int index = probe(key, i);

                    if (table[index].occupied) {
                        this->collisionCount++;
                    }

                    if (!table[index].occupied) {
                        table[index].key = key;
                        table[index].value = value;

                        table[index].occupied = true;
                        table[index].deleted = false;

                        this->numElements++;

                        break;
                    }
                }

            }
        }
    }
    
public:
    CustomHashingTable(int hType, int size = INITIAL_TABLE_SIZE) : HashTableBase<T, V>(size, hType) {
        table.resize(this->tableSize);
    }

    // double hash function 
    ll probe(const T& key, int i) {
        ll h = this->myhash(key);
        stringstream ss; ss << key;
        ll aux = auxHash(ss.str(), this->tableSize);
        // (h(k) + C1 * i * aux(k) + C2 * i^2) % m
        return (h + (ll)C1 * i * aux + (ll)C2 * i * i) % this->tableSize;
    }

    void printProbeSequence(const T& key) {
        for (int i = 0; i < this->tableSize; i++) {
            int index = probe(key, i);

            cout << index << " ";

            if (table[index].occupied && !table[index].deleted && table[index].key == key) {
                return;
            }
            else if (!table[index].occupied && !table[index].deleted) {
                return;
            }
            else {
                cout << "-> ";
            }
        }
    }
    
    bool insert(const T& key, const V& value) {
        // check duplicate
        int h = 0;
        if (search(key, h).first == true) {
            // this->collisionCount += h;
            return false;
        }

        for (int i = 0; i < this->tableSize; i++) {
            int index = probe(key, i);

            if (table[index].occupied) {
                this->collisionCount++;
            }

            if (!table[index].occupied) {
                table[index].key = key;
                table[index].value = value;

                table[index].occupied = true;
                table[index].deleted = false;

                this->numElements++;
                this->insertionsSinceExpansion++;

                if (this->getLoadFactor() > LOAD_FACTOR_UPPER && this->insertionsSinceExpansion >= this->N / 2) {
                    int newSize = nextPrime(2 * this->tableSize);
                    rehash(newSize);
                    this->insertionsSinceExpansion = 0;
                }
                return true;
            }
        }
        return false;
    }
    
    pair<bool, V> search(const T& key, int& hits) {
        hits = 0;
        for (int i = 0; i < this->tableSize; i++) {
            hits++;
            int index = probe(key, i);

            if (table[index].occupied && !table[index].deleted && table[index].key == key) {
                return {true, table[index].value};
            }
            else if (!table[index].occupied && !table[index].deleted) {
                return {false, V()};
            }
        }
        return {false, V()};
    }
    
    bool remove(const T& key) {
        for (int i = 0; i < this->tableSize; i++){
            int index = probe(key, i);


            if (!table[index].occupied && !table[index].deleted) {
                return false;
            }            

            if (table[index].occupied && !table[index].deleted && table[index].key == key) {
                table[index].deleted = true;
                table[index].occupied = false;

                this->numElements--;
                this->deletionsSinceCompaction++;
                
                if (this->tableSize > INITIAL_TABLE_SIZE && 
                    this->getLoadFactor() < LOAD_FACTOR_LOWER && 
                    this->deletionsSinceCompaction >= this->N / 2) 
                {
                    int newSize = prevPrime(this->tableSize / 2);
                    rehash(newSize);
                    this->deletionsSinceCompaction = 0;
                }
                return true;
            }
        }
        return true;
    }
};



string generateWord(int length, mt19937 &rng) {
    string word;
    uniform_int_distribution<int> dist(0, 25); // a-z

    for (int i = 0; i < length; i++) {
        word += 'a' + dist(rng);
    }

    return word;
}

vector<pair<string, int>> generateUniqueWords(int count, int length) {
    vector<pair<string, int>> result;
    unordered_set<string> uniqueWords;
    int sequenceNumber = 1;

    random_device rd;
    mt19937 rng(rd());

    while ((int)result.size() < count) {
        string word = generateWord(length, rng);

        if (uniqueWords.find(word) == uniqueWords.end()) {
            uniqueWords.insert(word);
            result.push_back({word, sequenceNumber});
            sequenceNumber++;
        }
    }

    return result;
}

// Helper function to calculate average hits
double calculateAverageHits(HashTableBase<string, int>* ht, vector<string>& searchWords) {
    int totalHits = 0;
    for (int i = 0; i < (int)searchWords.size(); i++) {
        const string& word = searchWords[i];
        int hits = 0;
        ht->search(word, hits);
        totalHits += hits;
    }
    return (double)totalHits / searchWords.size();
}

// helper function to count unique hashes for each hash function
int countUniqueHashes(vector<pair<string, int>>& words, int hashType, int tableSize) {
    unordered_set<ll> uniqueHashes;
    for (int i = 0; i < (int)words.size(); i++) {
        string word = words[i].first;

        if (hashType == 1) {
            uniqueHashes.insert(hash1(word, tableSize));
        }
        else {
            uniqueHashes.insert(hash2(word, tableSize));
        }
    }
    return uniqueHashes.size();
}


void evaluatePerformance(int n) {
    // cout << "Generating words..." << endl;
    vector<pair<string, int>> words = generateUniqueWords(10000, n);
    // cout << "Generated " << words.size() << " words." << endl;
    
    ChainingHashTable<string, int> chainHash1(INITIAL_TABLE_SIZE, 1);
    ChainingHashTable<string, int> chainHash2(INITIAL_TABLE_SIZE, 2);
    DoubleHashingTable<string, int> doubleHash1(INITIAL_TABLE_SIZE, 1);
    DoubleHashingTable<string, int> doubleHash2(INITIAL_TABLE_SIZE, 2);
    CustomHashingTable<string, int> customHash1(INITIAL_TABLE_SIZE, 1);
    CustomHashingTable<string, int> customHash2(INITIAL_TABLE_SIZE, 2);
    
    // Insert all words
    // cout << "Inserting words..." << endl;
    for (int i = 0; i < (int)words.size(); i++) {
        // if (i % 1000 == 0) cout << "Inserted " << i << " items..." << endl;
        const string& word = words[i].first;
        int value = words[i].second;
        
        chainHash1.insert(word, value);
        chainHash2.insert(word, value);
        doubleHash1.insert(word, value);
        doubleHash2.insert(word, value);
        customHash1.insert(word, value);
        customHash2.insert(word, value);
    }
    // cout << "Insertion complete." << endl;
    
    // 1000 random words for searching
    vector<string> searchWords;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, words.size() - 1);
    
    for (int i = 0; i < 1000; i++) {
        searchWords.push_back(words[dis(gen)].first);
    }
    
    // Calculate average hits for each method
    double chainAvgHits1 = calculateAverageHits(&chainHash1, searchWords);
    double chainAvgHits2 = calculateAverageHits(&chainHash2, searchWords);
    double doubleAvgHits1 = calculateAverageHits(&doubleHash1, searchWords);
    double doubleAvgHits2 = calculateAverageHits(&doubleHash2, searchWords);
    double customAvgHits1 = calculateAverageHits(&customHash1, searchWords);
    double customAvgHits2 = calculateAverageHits(&customHash2, searchWords);
    
    // Print results in tabular format
    cout << "================================================================================\n";
    cout << setw(34) << left << "" 
    << setw(30) << "Hash1" 
    << setw(25) << "Hash2" << "\n";
    cout << "--------------------------------------------------------------------------------\n";
    cout << setw(25) << left << "" 
    << setw(15) << "Collisions" 
    << setw(15) <<  "Avg Hits"
    << setw(15) << "Collisions" 
    << setw(15) << "Avg Hits" << "\n";
    cout << "--------------------------------------------------------------------------------\n";
    
    cout << setw(27) << left << "Chaining Method"
    << setw(15) << chainHash1.getCollisionCount()
    << setw(15) << fixed << setprecision(2) << chainAvgHits1
    << setw(15) << chainHash2.getCollisionCount()
    << setw(15) << fixed << setprecision(2) << chainAvgHits2 << "\n";
    
    cout << setw(27) << left << "Double Hashing"
    << setw(15) << doubleHash1.getCollisionCount()
    << setw(15) << fixed << setprecision(2) << doubleAvgHits1
    << setw(15) << doubleHash2.getCollisionCount()
    << setw(15) << fixed << setprecision(2) << doubleAvgHits2 << "\n";
    
    cout << setw(27) << left << "Custom Probing"
    << setw(15) << customHash1.getCollisionCount()
    << setw(15) << fixed << setprecision(2) << customAvgHits1
    << setw(15) << customHash2.getCollisionCount()
    << setw(15) << fixed << setprecision(2) << customAvgHits2 << "\n";
    
    cout << "================================================================================\n";
    
    // calculate unique hash values
    int uniqueHash1 = countUniqueHashes(words, HASH1, 20011); 
    int uniqueHash2 = countUniqueHashes(words, HASH2, 20011);
    
    cout << "\nHash Function Quality:\n";
    cout << "Hash1: " << uniqueHash1 << " unique values out of 10000 ("
         << fixed << setprecision(1) << (uniqueHash1 * 100.0 / 10000) << "%)\n";
    cout << "Hash2: " << uniqueHash2 << " unique values out of 10000 ("
         << fixed << setprecision(1) << (uniqueHash2 * 100.0 / 10000) << "%)\n";


    int m;
    cout << "m = ";
    cin >> m;
    for (int i = 0; i < m; i++) {
        string s;
        cin >> s;

        doubleHash1.printProbeSequence(s);
        cout << endl;
        customHash1.printProbeSequence(s);
    }
}

int main() {
    // cout << "Starting sieve..." << endl;
    sieve();
    // cout << "Sieve done. Primes count: " << primes.size() << endl;
    
    int n;
    cout << "n = ";
    cin >> n;
    
    evaluatePerformance(n);
    
    
    return 0;
}
