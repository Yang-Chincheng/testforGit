#ifndef _FCONTAINER_H_
#define _FCONTAINER_H_

#include "skiplist.h"
#include <algorithm>

namespace fcontainer {

using std::pair;

template <typename _Key, typename _Cmp = std::less<_Key>>
class fset: public skiplist<_Key, _Cmp> {
public:
    fset(const string &_name): skiplist<_Key, _Cmp>(_name) {}
    ~fset() = default;
};

template <typename _Key, typename _Tp, typename _Cmp>
struct fmapCmp {
    _Cmp comp;
    bool operator () (const pair<const _Key, _Tp> &lhs, const pair<const _Key, _Tp> &rhs) const {
        return comp(lhs.first, rhs.first);
    } 
};

template <typename _Key, typename _Tp, typename _Cmp = std::less<_Key>>
class fmap: public skiplist<pair<const _Key, _Tp>, fmapCmp<_Key, _Tp, _Cmp>> {
public:
    fmap(const string &_name): skiplist<_Key, _Cmp>(_name) {}
    void set(const _Key &key, const _Tp &tp) {
        pair<const _Key, _Tp> tmp = make_pair(key, tp);
        erase(tmp), insert(tmp);
    }
    _Tp at(const  _Key &key) const {
        auto it = find(key);
        if(!it) return std::move(_Tp());
        return std::move((*it).second);
    }
    ~fmap() = default;
};

}

#endif // _FCONTAINER_H_