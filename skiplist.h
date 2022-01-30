#ifndef _SKIPLIST_H_
#define _SKIPLIST_H_

#include <functional>
#include <cstdlib>
#include <utility>
#include <cstring>
#include <ctime>

#include "mempool.h"

namespace fcontainer {
    
typedef unsigned lev_t, siz_t;

template <typename _Key, typename _Cmp = std::less<_Key>>
class skiplist {
    // typedef _Key key_t;
    // typedef key_t* key_p;
    // typedef key_t& key_r;

protected:
    struct node {
        _Key key;
        lev_t node_lev;
        off_t pre[skiplist::MAX_LEV];
        off_t nex[skiplist::MAX_LEV];
    };
    MemoryPool<node> mp;
    _Cmp comp;
    lev_t list_lev;
    off_t head;
    off_t tail;
    siz_t cnt;

public:
    const static float RAND_PROB;
    const static lev_t MAX_LEV;
    class iterator {
        friend class skiplist;
        MemoryPool<node>* mp;
        off_t nod;
    public:
        iterator() = default;
        iterator(MemoryPool<node> *_mp, off_t _nod = 0L);
        operator bool() const;
        _Key operator * () const;
        bool operator == (const iterator &o) const;
        bool operator != (const iterator &o) const;
        iterator& operator ++ ();
        iterator& operator -- ();
        iterator operator ++ (int);
        iterator operator -- (int);
        ~iterator() = default;
    };

protected:
    lev_t getRandLev();
    _Key  getNodeKey(off_t nod);
    void  getNodeKey(off_t nod, _Key &nkey);
    void  putNodeKey(off_t nod, const _Key &nkey);
    lev_t getNodeLev(off_t nod);
    void  putNodeLev(off_t nod, lev_t lev);
    off_t getNodePre(off_t nod, siz_t pos = 0);
    off_t getNodeNex(off_t nod, siz_t pos = 0);
    void  putNodePre(off_t nod, siz_t pos, off_t val);
    void  putNodeNex(off_t nod, siz_t pos, off_t val);

public:
    skiplist(const std::string &_name);
    skiplist(const skiplist &o) = delete;
    
    siz_t size();
    void clear();
    iterator begin();
    iterator end();
    iterator rbegin();
    iterator rend();

    iterator lower_bound(const _Key &nkey);
    iterator find(const _Key &nkey);
    iterator insert(const _Key &nkey);
    void erase(const _Key &nkey);
    void erase(const iterator it);
    
    ~skiplist() = default;
};

template <typename _Key, typename _Cmp>
const lev_t skiplist<_Key, _Cmp>::MAX_LEV = 20u;
template <typename _Key, typename _Cmp>
const float skiplist<_Key, _Cmp>::RAND_PROB = 0.5f;

template <typename _Key, typename _Cmp>
skiplist<_Key, _Cmp>::iterator::iterator(MemoryPool<node> *_mp, off_t _nod): mp(_mp), nod(_nod) {}

template <typename _Key, typename _Cmp>
skiplist<_Key, _Cmp>::iterator::operator bool() const {return nod; }

template <typename _Key, typename _Cmp>
_Key skiplist<_Key, _Cmp>::iterator::operator * () const {
    _Key tmp; 
    mp->read(tmp, nod + offsetof(node, key)); 
    return tmp;
}

template <typename _Key, typename _Cmp>
bool skiplist<_Key, _Cmp>::iterator::operator == (const iterator &o) const {
    return mp == o.mp && nod == o.nod;
}

template <typename _Key, typename _Cmp>
bool skiplist<_Key, _Cmp>::iterator::operator != (const iterator &o) const {
    return mp != o.mp || nod != o.nod;
}

template <typename _Key, typename _Cmp>
typename skiplist<_Key, _Cmp>::iterator& 
skiplist<_Key, _Cmp>::iterator::operator ++ () {
    off_t nex; 
    mp->read(nex, nod + offsetof(node, nex)); nod = nex; 
    return *this;
}

template <typename _Key, typename _Cmp>
typename skiplist<_Key, _Cmp>::iterator 
skiplist<_Key, _Cmp>::iterator::operator ++ (int) {
    iterator tmp(*this);
    off_t nex; 
    mp->read(nex, nod + offsetof(node, nex)); nod = nex; 
    return tmp;
}

template <typename _Key, typename _Cmp>
typename skiplist<_Key, _Cmp>::iterator& 
skiplist<_Key, _Cmp>::iterator::operator -- () {
    off_t nex; 
    mp->read(nex, nod + offsetof(node, pre)); nod = nex; 
    return *this;
}

template <typename _Key, typename _Cmp>
typename skiplist<_Key, _Cmp>::iterator 
skiplist<_Key, _Cmp>::iterator::operator -- (int) {
    iterator tmp(*this);
    off_t nex; 
    mp->read(nex, nod + offsetof(node, pre)); nod = nex; 
    return tmp;
}

template <typename _Key, typename _Cmp>
skiplist<_Key, _Cmp>::skiplist(const std::string &_name): mp(_name + ".bin") {
    mp.getHead(head);
    mp.getTail(tail);
    if(!head) {
        head = tail = mp.malloc();
        mp.write(node(), head);
        mp.putHead(head);
        mp.putTail(tail);
    }
    list_lev = 1;
    srand(time(0));
}

template <typename _Key, typename _Cmp>
lev_t skiplist<_Key, _Cmp>::getRandLev() {
    lev_t lev = 1;
    while(lev < MAX_LEV && rand() < RAND_PROB * RAND_MAX) lev++;
    return lev;
}

template <typename _Key, typename _Cmp>
_Key skiplist<_Key, _Cmp>::getNodeKey(off_t nod) {
    _Key tmp;
    mp.read(tmp, nod + offsetof(node, key));
    return tmp;
}

template <typename _Key, typename _Cmp>
void skiplist<_Key, _Cmp>::getNodeKey(off_t nod, _Key &nkey) {
    mp.read(nkey, nod + offsetof(node, key));
}

template <typename _Key, typename _Cmp>
void skiplist<_Key, _Cmp>::putNodeKey(off_t nod, const _Key &nkey) {
    mp.write(nkey, nod + offsetof(node, key));
}

template <typename _Key, typename _Cmp>
lev_t skiplist<_Key, _Cmp>::getNodeLev(off_t nod) {
    lev_t tmp;
    mp.read(tmp, nod + offsetof(node, node_lev));
    return tmp;
}

template <typename _Key, typename _Cmp>
void skiplist<_Key, _Cmp>::putNodeLev(off_t nod, lev_t lev) {
    mp.write(lev, nod + offsetof(node, node_lev));
}

template <typename _Key, typename _Cmp>
off_t skiplist<_Key, _Cmp>::getNodePre(off_t nod, siz_t pos) {
    off_t tmp;
    mp.read(tmp, nod + offsetof(node, pre) + pos * sizeof(off_t));
    return tmp;
}

template <typename _Key, typename _Cmp>
void skiplist<_Key, _Cmp>::putNodePre(off_t nod, siz_t pos, off_t val) {
    mp.write(val, nod + offsetof(node, pre) + pos * sizeof(off_t));
}

template <typename _Key, typename _Cmp>
off_t skiplist<_Key, _Cmp>::getNodeNex(off_t nod, siz_t pos) {
    off_t tmp;
    mp.read(tmp, nod + offsetof(node, nex) + pos * sizeof(off_t));
    return tmp;
}

template <typename _Key, typename _Cmp>
void skiplist<_Key, _Cmp>::putNodeNex(off_t nod, siz_t pos, off_t val) {
    mp.write(val, nod + offsetof(node, nex) + pos * sizeof(off_t));
}

template <typename _Key, typename _Cmp>
siz_t skiplist<_Key, _Cmp>::size() {return cnt; }

template <typename _Key, typename _Cmp>
typename skiplist<_Key, _Cmp>::iterator 
skiplist<_Key, _Cmp>::begin() {return iterator(&mp, getNodeNex(head)); }

template <typename _Key, typename _Cmp>
typename skiplist<_Key, _Cmp>::iterator 
skiplist<_Key, _Cmp>::end() {return iterator(&mp); }

template <typename _Key, typename _Cmp>
typename skiplist<_Key, _Cmp>::iterator 
skiplist<_Key, _Cmp>::rbegin() {return iterator(&mp, tail); }

template <typename _Key, typename _Cmp>
typename skiplist<_Key, _Cmp>::iterator 
skiplist<_Key, _Cmp>::rend() {return iterator(&mp); }

template <typename _Key, typename _Cmp>
void skiplist<_Key, _Cmp>::clear() {
    mp.clear();
    head = tail = mp.malloc();
    mp.write(node(), head);
    mp.putHead(head);
    mp.putTail(tail);
    list_lev = 1;
}

template <typename _Key, typename _Cmp>
typename skiplist<_Key, _Cmp>::iterator
skiplist<_Key, _Cmp>::lower_bound(const _Key &nkey) {
    off_t cur = head;
    for(int i = list_lev - 1; i >= 0; --i) {
        off_t nex;
        while((nex = getNodeNex(cur, i)) && 
            comp(getNodeKey(nex), nkey)) cur = nex;
    }
    return iterator(&mp, getNodeNex(cur));
} 

template <typename _Key, typename _Cmp>
typename skiplist<_Key, _Cmp>::iterator
skiplist<_Key, _Cmp>::find(const _Key &nkey) {
    off_t cur = head;
    for(int i = list_lev - 1; i >= 0; --i) {
        off_t nex;
        while((nex = getNodeNex(cur, i)) && 
            comp(getNodeKey(nex), nkey)) cur = nex;
    }
    cur = getNodeNex(cur);
    if(comp(nkey, getNodeKey(cur))) return iterator(&mp); 
    return iterator(&mp, cur);
}

template <typename _Key, typename _Cmp>
typename skiplist<_Key, _Cmp>::iterator
skiplist<_Key, _Cmp>::insert(const _Key &nkey) {
    off_t cur = head;
    off_t upd[MAX_LEV];
    memset(upd, 0, sizeof(upd));
    
    for(int i = list_lev - 1; i >= 0; --i) {
        off_t nex;
        while((nex = getNodeNex(cur, i)) &&
            comp(getNodeKey(nex), nkey)) cur = nex;
        upd[i] = cur;
    }
    cur = getNodeNex(cur);
    if(cur && !comp(nkey, getNodeKey(cur))) return iterator(&mp);

    lev_t new_lev = getRandLev();
    off_t new_node = mp.malloc();
    mp.write(node(), new_node);
    putNodeKey(new_node, nkey);
    putNodeLev(new_node, new_lev);

    if(new_lev > list_lev) {
        for(int i = list_lev; i < new_lev; ++i) upd[i] = head;
        list_lev = new_lev;
    }
    for(int i = 0; i < new_lev; ++i) {
        off_t nex = getNodeNex(upd[i], i);
        putNodePre(new_node, i, upd[i]);
        putNodeNex(new_node, i, nex);
        if(nex) putNodePre(nex, i, new_node);
        putNodeNex(upd[i], i, new_node);
    }
    if(tail == upd[0]) {
        tail = new_node;
        mp.putTail(tail);
    }
    cnt++;
    return iterator(&mp, new_node);
}

template <typename _Key, typename _Cmp>
void skiplist<_Key, _Cmp>::erase(const _Key &nkey) {
    off_t cur = head;
    off_t upd[MAX_LEV];
    memset(upd, 0, sizeof(upd));
    
    for(int i = list_lev - 1; i >= 0; --i) {
        off_t nex;
        while((nex = getNodeNex(cur, i)) &&
            comp(getNodeKey(nex), nkey)) cur = nex;
        upd[i] = cur;
    }
    cur = getNodeNex(cur);
    if(!cur || comp(nkey, getNodeKey(cur))) return ;

    lev_t cur_lev = getNodeLev(cur);
    for(int i = 0; i < cur_lev; ++i) {
        off_t nex = getNodeNex(cur, i);
        if(nex) putNodePre(nex, i, upd[i]);
        putNodeNex(upd[i], i, nex);
    }
    if(cur == tail) {
        tail = upd[0];
        mp.putTail(tail);
    }
    mp.free(cur);
    cnt--;
}

template <typename _Key, typename _Cmp>
void skiplist<_Key, _Cmp>::erase(const iterator it) {
    off_t cur = it.nod;
    if(!cur || cur == head) return ;
    lev_t cur_lev = getNodeLev(cur);
    for(int i = 0; i < cur_lev; ++i) {
        off_t pre = getNodePre(cur, i);
        off_t nex = getNodeNex(cur, i);
        if(nex) putNodePre(nex, i, pre);
        putNodeNex(pre, i, nex);
    }
    if(cur == tail) {
        tail = getNodePre(tail);
        mp.putTail(tail);
    }
    mp.free(cur);
    cnt--;
}

}

#endif // _SKIPLIST_H_