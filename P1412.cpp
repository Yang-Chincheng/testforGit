#include <bits/stdc++.h>
#include "fcontainer.h"
using namespace std;
using namespace fcontainer;

fset<pair<size_t, int>> fs("data");

int main() {
    int n;
    cin >> n;
    while(n--) {
        int val;
        string opt, str;
        cin >> opt >> str;
        size_t key = hash<string>{}(str);
        if(opt == "insert") {
            cin >> val;
            fs.insert(make_pair(key, val));
        }
        if(opt == "delete") {
            cin >> val;
            fs.erase(make_pair(key, val));
        }
        if(opt == "find") {
            auto beg = fs.lower_bound(make_pair(key, INT32_MIN));
            auto end = fs.lower_bound(make_pair(key, INT32_MAX));
            for(auto it = beg; it != end; ++it) cout << (*it).second << " ";
            if(beg == end) cout << "null";
            cout << endl;
        }
    }
    return 0;
}