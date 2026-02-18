#include "2305035.cpp"

// problems with just their solutions
// swap map/set for your own hash implementation

// for context
// bool insert(const K& key, const V& value)
// bool remove(const K& key)
// pair<bool, V> search(const K& key, int& hits)

/*
Two Sum
Given an array nums and a target, return indices of two numbers
such that they add up to target.
*/

vector<int> twoSum(vector<int>& nums, int target) {
    ChainingHashTable<int,int> ht;

    for(int i = 0; i < nums.size(); i++) {
        int need = target - nums[i];
        int hits = 0;

        auto res = ht.search(need, hits);
        if(res.first)
            return {res.second, i};

        ht.insert(nums[i], i);
    }

    return {};
}


/*
Contains Duplicate
Return true if any value appears at least twice.
*/

bool containsDuplicate(vector<int>& nums) {
    ChainingHashTable<int,bool> ht;

    for(int x : nums) {
        int hits = 0;
        if(ht.search(x, hits).first)
            return true;

        ht.insert(x, true);
    }

    return false;
}


/*
Valid Anagram
Return true if t is an anagram of s.
*/

bool isAnagram(string s, string t) {
    if(s.size() != t.size()) return false;

    ChainingHashTable<char,int> ht;

    for(char c : s) {
        int hits = 0;
        auto res = ht.search(c, hits);

        if(res.first)
            ht.insert(c, res.second + 1);
        else
            ht.insert(c, 1);
    }

    for(char c : t) {
        int hits = 0;
        auto res = ht.search(c, hits);

        if(!res.first || res.second == 0)
            return false;

        ht.insert(c, res.second - 1);
    }

    return true;
}



/*
Intersection of Two Arrays
Return unique intersection elements.
*/

vector<int> intersection(vector<int>& a, vector<int>& b) {
    ChainingHashTable<int,bool> ht;
    vector<int> ans;

    for(int x : a)
        ht.insert(x, true);

    for(int x : b) {
        int hits = 0;
        if(ht.search(x, hits).first) {
            ans.push_back(x);
            ht.remove(x); // ensure uniqueness
        }
    }

    return ans;
}


/*
Subarray Sum Equals K
Return count of subarrays with sum equal to k.
*/

int subarraySum(vector<int>& nums, int k) {
    ChainingHashTable<int,int> freq;
    freq.insert(0, 1);

    int sum = 0, ans = 0;

    for(int x : nums) {
        sum += x;

        int hits = 0;
        auto res = freq.search(sum - k, hits);
        if(res.first)
            ans += res.second;

        auto cur = freq.search(sum, hits);
        if(cur.first) {
            freq.remove(sum);
            freq.insert(sum, cur.second + 1);
        } else {
            freq.insert(sum, 1);
        }
    }
    return ans;
}


// iterating through chaining hash table
// void iterateAllSimple() {
//     for(int i = 0; i < this->tableSize; i++) {

//         for(auto &kv : table[i]) {
//             cout << kv.first << " -> " << kv.second << "\n";
//         }

//     }
// }


// for the open addressing tables
// void iterateAllSimple() {
//     for(int i = 0; i < this->tableSize; i++) {

//         if(table[i].occupied && !table[i].deleted) {
//             cout << table[i].key
//                  << " -> "
//                  << table[i].value
//                  << "\n";
//         }

//     }
// }



/*
Majority Element
Return the element that appears more than floor(n/2) times.
*/

int majorityElement(vector<int>& nums) {
    ChainingHashTable<int,int> freq;

    for(int x : nums) {
        int hits = 0;
        auto res = freq.search(x, hits);

        if(res.first) {
            freq.remove(x);
            freq.insert(x, res.second + 1);
        } else {
            freq.insert(x, 1);
        }

        if(freq.search(x, hits).second > nums.size()/2)
            return x;
    }
    return -1;
}



/*
Single Number
Every element appears twice except one. Find that one.
*/

int singleNumber(vector<int>& nums) {
    ChainingHashTable<int,int> freq;

    for(int x : nums) {
        int hits = 0;
        auto res = freq.search(x, hits);

        if(res.first) {
            freq.remove(x);
            freq.insert(x, res.second + 1);
        } else {
            freq.insert(x, 1);
        }
    }

    // iterate
    for(int x : nums) {
        int hits = 0;
        if(freq.search(x, hits).second == 1)
            return x;
    }

    return -1;
}


/*
Find All Duplicates in an Array
Return all elements that appear twice.
*/

vector<int> findDuplicates(vector<int>& nums) {
    ChainingHashTable<int,int> freq;
    vector<int> ans;

    for(int x : nums) {
        int hits = 0;
        auto res = freq.search(x, hits);

        if(res.first) {
            freq.remove(x);
            freq.insert(x, res.second + 1);
        } else {
            freq.insert(x, 1);
        }
    }

    for(int x : nums) {
        int hits = 0;
        if(freq.search(x, hits).second == 2)
            ans.push_back(x);
    }

    return ans;
}



/*
Isomorphic Strings
Two strings are isomorphic if characters map one-to-one.
*/

bool isIsomorphic(string s, string t) {
    if(s.size() != t.size()) return false;

    ChainingHashTable<char,char> mp1;
    ChainingHashTable<char,char> mp2;

    for(int i = 0; i < s.size(); i++) {
        char a = s[i], b = t[i];
        int hits = 0;

        auto r1 = mp1.search(a, hits);
        auto r2 = mp2.search(b, hits);

        if(r1.first && r1.second != b) return false;
        if(r2.first && r2.second != a) return false;

        if(!r1.first) mp1.insert(a,b);
        if(!r2.first) mp2.insert(b,a);
    }

    return true;
}



/*
Ransom Note
Return true if ransomNote can be built from magazine.
*/

bool canConstruct(string ransomNote, string magazine) {
    ChainingHashTable<char,int> freq;

    for(char c : magazine) {
        int hits = 0;
        auto res = freq.search(c, hits);

        if(res.first) {
            freq.remove(c);
            freq.insert(c, res.second + 1);
        } else {
            freq.insert(c, 1);
        }
    }

    for(char c : ransomNote) {
        int hits = 0;
        auto res = freq.search(c, hits);

        if(!res.first || res.second == 0)
            return false;

        freq.remove(c);
        freq.insert(c, res.second - 1);
    }

    return true;
}


/*
Happy Number
Detect cycle using hashing.
*/

int digitSquareSum(int n) {
    int sum = 0;
    while(n) {
        int d = n % 10;
        sum += d*d;
        n /= 10;
    }
    return sum;
}

bool isHappy(int n) {
    ChainingHashTable<int,bool> seen;

    while(n != 1) {
        int hits = 0;
        if(seen.search(n, hits).first)
            return false;

        seen.insert(n, true);
        n = digitSquareSum(n);
    }
    return true;
}



/*
Longest Substring Without Repeating Characters
*/

int lengthOfLongestSubstring(string s) {
    ChainingHashTable<char,int> lastPos;

    int l = 0, best = 0;

    for(int r = 0; r < s.size(); r++) {
        int hits = 0;
        auto res = lastPos.search(s[r], hits);

        if(res.first && res.second >= l)
            l = res.second + 1;

        if(res.first) lastPos.remove(s[r]);
        lastPos.insert(s[r], r);

        best = max(best, r - l + 1);
    }

    return best;
}


/*
Count Elements
Count x such that x+1 exists.
*/

int countElements(vector<int>& arr) {
    ChainingHashTable<int,bool> ht;

    for(int x : arr)
        ht.insert(x, true);

    int cnt = 0, hits = 0;

    for(int x : arr)
        if(ht.search(x+1, hits).first)
            cnt++;

    return cnt;
}


/*
Group Anagrams
Group words with same sorted form.
*/

vector<vector<string>> groupAnagrams(vector<string>& strs) {
    ChainingHashTable<string, vector<string>> mp;

    for(string s : strs) {
        string key = s;
        sort(key.begin(), key.end());

        int hits = 0;
        auto res = mp.search(key, hits);

        if(res.first) {
            auto vec = res.second;
            vec.push_back(s);
            mp.remove(key);
            mp.insert(key, vec);
        } else {
            mp.insert(key, {s});
        }
    }

    // collect results (needs iteration helper)
    vector<vector<string>> ans;

    // write this function inside the class and call it
    // like this mp.print_values()
    // for(int i = 0; i < mp.tableSize; i++) {
    //     for(auto &kv : mp.table[i]) {
    //         ans.push_back(kv.second);
    //     }
    // }

    return ans;
}



/*
Word Pattern
Check if pattern bijectively maps to words.
*/

// valid example
// a -> dog
// b -> cat
// a -> dog
// pattern = "abba"
// s       = "dog cat cat dog"

bool wordPattern(string pattern, string s) {
    stringstream ss(s);
    vector<string> words;
    string w;

    while(ss >> w) words.push_back(w);
    if(words.size() != pattern.size()) return false;

    ChainingHashTable<char,string> mp1;
    ChainingHashTable<string,char> mp2;

    for(int i = 0; i < pattern.size(); i++) {
        char p = pattern[i];
        string word = words[i];
        int hits = 0;

        auto r1 = mp1.search(p, hits);
        auto r2 = mp2.search(word, hits);

        if(r1.first && r1.second != word) return false;
        if(r2.first && r2.second != p) return false;

        if(!r1.first) mp1.insert(p, word);
        if(!r2.first) mp2.insert(word, p);
    }

    return true;
}


/*
Find elements present in one array but not the other.
Return {uniqueA, uniqueB}.
*/

vector<vector<int>> findDifference(vector<int>& a, vector<int>& b) {
    ChainingHashTable<int,bool> A, B;

    for(int x : a) A.insert(x, true);
    for(int x : b) B.insert(x, true);

    vector<int> onlyA, onlyB;

    for(int x : a) {
        int hits = 0;
        if(!B.search(x, hits).first)
            onlyA.push_back(x);
    }

    for(int x : b) {
        int hits = 0;
        if(!A.search(x, hits).first)
            onlyB.push_back(x);
    }

    return {onlyA, onlyB};
}


/*
Number of Good Pairs
Count pairs (i,j) where nums[i] == nums[j], i < j.
*/

// alternate solution -> map everyones freq
// for every num such that freq (k) > 1, ans += k * (k + 1) / 2

int numIdenticalPairs(vector<int>& nums) {
    ChainingHashTable<int,int> freq;
    int ans = 0;

    for(int x : nums) {
        int hits = 0;
        auto r = freq.search(x, hits);

        if(r.first) {
            ans += r.second;
            freq.insert(x, r.second + 1);
        }
        else {
            freq.insert(x, 1);
        }
    }

    return ans;
}



/*
Longest Consecutive Sequence
Return length of longest consecutive integer sequence.
O(n) hashing solution.
*/

int longestConsecutive(vector<int>& nums) {
    ChainingHashTable<int,bool> mp;

    for(int x : nums)
        mp.insert(x, true);

    int best = 0;

    for(int x : nums) {
        int hits = 0;

        // start only if x-1 not present
        if(!mp.search(x-1, hits).first) {

            int len = 1;
            int cur = x + 1;

            while(mp.search(cur, hits).first) {
                len++;
                cur++;
            }

            best = max(best, len);
        }
    }

    return best;
}



/*
Return length of longest subarray with sum = k.
*/

int longestSubarraySumK(vector<int>& nums, int k) {
    ChainingHashTable<int,int> firstIndex;

    int prefix = 0, best = 0;

    firstIndex.insert(0, -1);

    for(int i = 0; i < nums.size(); i++) {
        prefix += nums[i];
        int hits = 0;

        auto r = firstIndex.search(prefix - k, hits);
        
        if(r.first)
            best = max(best, i - r.second);

        if(!firstIndex.search(prefix, hits).first)
            firstIndex.insert(prefix, i);
    }

    return best;
}

/*
Longest subarray with equal number of 0s and 1s.
*/

int findMaxLength(vector<int>& nums) {
    ChainingHashTable<int,int> first;

    int sum = 0, best = 0;
    first.insert(0, -1);

    for(int i = 0; i < nums.size(); i++) {
        sum += (nums[i] == 0 ? -1 : 1);

        int hits = 0;
        auto r = first.search(sum, hits);

        if(r.first)
            best = max(best, i - r.second);
        else
            first.insert(sum, i);
    }

    return best;
}


/*
Return k most frequent elements.
*/

vector<int> topKFrequent(vector<int>& nums, int k) {
    ChainingHashTable<int,int> freq;

    for(int x : nums) {
        int hits = 0;
        auto r = freq.search(x, hits);

        if(r.first) freq.insert(x, r.second + 1);
        else freq.insert(x, 1);
    }

    vector<pair<int,int>> arr;

    // implement this using a separate function
    // for(int i = 0; i < freq.tableSize; i++) {
    //     for(auto &kv : freq.table[i])
    //         arr.push_back({kv.second, kv.first});
    // }

    sort(arr.rbegin(), arr.rend());

    vector<int> ans;
    for(int i = 0; i < k; i++)
        ans.push_back(arr[i].second);

    return ans;
}


/*
Return length of smallest window containing all chars of t.
(Hash frequency core only)
*/

int minWindowLength(string s, string t) {
    ChainingHashTable<char,int> need;

    for(char c : t) {
        int hits = 0;
        auto r = need.search(c, hits);
        if(r.first) need.insert(c, r.second + 1);
        else need.insert(c, 1);
    }

    int required = t.size();
    int l = 0, best = INT_MAX;

    for(int r = 0; r < s.size(); r++) {
        int hits = 0;
        auto rr = need.search(s[r], hits);

        if(rr.first) {
            if(rr.second > 0) required--;
            need.insert(s[r], rr.second - 1);
        }

        while(required == 0) {
            best = min(best, r - l + 1);

            auto rl = need.search(s[l], hits);
            if(rl.first) {
                need.insert(s[l], rl.second + 1);
                if(rl.second + 1 > 0) required++;
            }
            l++;
        }
    }

    return best == INT_MAX ? 0 : best;
}


/*
Count triplets (i,j,k) where sum = 0.
Hashing inside two-sum.
*/

int countTriplets(vector<int>& nums) {
    int n = nums.size(), ans = 0;

    for(int i = 0; i < n; i++) {
        ChainingHashTable<int,bool> seen;

        for(int j = i + 1; j < n; j++) {
            int need = -(nums[i] + nums[j]);
            int hits = 0;

            if(seen.search(need, hits).first)
                ans++;

            seen.insert(nums[j], true);
        }
    }

    return ans;
}


/*
Length of smallest subarray containing all distinct elements.
*/

int smallestDistinctWindow(vector<int>& nums) {
    ChainingHashTable<int,bool> all;

    for(int x : nums)
        all.insert(x, true);

    int total = 0;
    for(int i = 0; i < all.tableSize; i++)
        for(auto &kv : all.table[i])
            total++;

    ChainingHashTable<int,int> freq;

    int l = 0, formed = 0, best = INT_MAX;

    for(int r = 0; r < nums.size(); r++) {
        int hits = 0;
        auto rr = freq.search(nums[r], hits);

        if(!rr.first || rr.second == 0) formed++;

        freq.insert(nums[r], rr.first ? rr.second + 1 : 1);

        while(formed == total) {
            best = min(best, r - l + 1);

            auto rl = freq.search(nums[l], hits);
            freq.insert(nums[l], rl.second - 1);

            if(rl.second - 1 == 0) formed--;
            l++;
        }
    }

    return best;
}

/*
Group Shifted Strings
Strings belong together if shifting letters makes them equal.
Return number of groups.
*/

int countShiftGroups(vector<string>& strs) {
    ChainingHashTable<string,bool> groups;

    for(string s : strs) {
        string key = "";

        for(int i = 1; i < s.size(); i++) {
            int diff = (s[i] - s[i-1] + 26) % 26;
            key += char('a' + diff);
        }

        int hits = 0;
        if(!groups.search(key, hits).first)
            groups.insert(key, true);
    }

    return groups.getNumElements();
}



/*
Count unique pairs with absolute difference = k.
*/

int countKDiffPairs(vector<int>& nums, int k) {
    ChainingHashTable<int,bool> seen;
    ChainingHashTable<int,bool> used;

    int ans = 0;

    for(int x : nums) {
        int hits = 0;

        if(seen.search(x - k, hits).first &&
           !used.search(x - k, hits).first) {
            ans++;
            used.insert(x - k, true);
        }

        if(seen.search(x + k, hits).first &&
           !used.search(x, hits).first) {
            ans++;
            used.insert(x, true);
        }

        seen.insert(x, true);
    }

    return ans;
}



/*
Check if array can be paired so sum of each pair divisible by k.
*/

bool canArrange(vector<int>& nums, int k) {
    ChainingHashTable<int,int> freq;

    for(int x : nums) {
        int rem = ((x % k) + k) % k;

        int hits = 0;
        auto r = freq.search(rem, hits);

        if(r.first) freq.insert(rem, r.second + 1);
        else freq.insert(rem, 1);
    }

    for(int i = 0; i < freq.tableSize; i++) {
        for(auto &kv : freq.table[i]) {
            int rem = kv.first;
            int cnt = kv.second;

            if(rem == 0) {
                if(cnt % 2) return false;
            }
            else {
                int hits = 0;
                auto r = freq.search(k - rem, hits);

                if(!r.first || r.second != cnt)
                    return false;
            }
        }
    }

    return true;
}



bool isValidSudoku(vector<vector<char>>& board) {
    ChainingHashTable<string,bool> seen;

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            char c = board[i][j];
            if (c == '.') continue;

            string rowKey = "row" + to_string(i) + c;
            string colKey = "col" + to_string(j) + c;
            string boxKey = "box" + to_string(i/3) + to_string(j/3) + c;

            int hits = 0;
            if (seen.search(rowKey, hits).first ||
                seen.search(colKey, hits).first ||
                seen.search(boxKey, hits).first)
                return false;

            seen.insert(rowKey, true);
            seen.insert(colKey, true);
            seen.insert(boxKey, true);
        }
    }
    return true;
}
