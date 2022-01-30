#ifndef _FILE_ACCESSOR_H_
#define _FILE_ACCESSOR_H_

#include <iostream>
#include <fstream>
#include <string>
using std::fstream;
using std::string;
using std::ios;

template <typename T>
class MemoryPool {
    const static off_t sizeof_off = sizeof(off_t);
    const static off_t null_off = 0L;
    const static off_t head_off = sizeof_off * 1L;
    const static off_t tail_off = sizeof_off * 2L;
    const static off_t del_top_off = sizeof_off * 3L;
    const static int info_cnt = 4;
    fstream file;
    string file_name;

public:
    MemoryPool(const MemoryPool &) = delete;
    MemoryPool(const string &_name): file_name(_name) {
        file.open(file_name);
        if(!file) {
            file.close();
            file.open(file_name, ios::binary | ios::out);
            off_t tmp = null_off;
            for(int i = 0; i < info_cnt; ++i) {
                file.write(reinterpret_cast<const char *>(&tmp), sizeof_off);    
            }
        }
        file.close();
        file.open(file_name, ios::binary | ios::in | ios::out);
    }

    void getHead(off_t &head) {
        read(head, head_off);
    }

    void getTail(off_t &tail) {
        read(tail, tail_off);
    }

    void putHead(const off_t head) {
        write(head, head_off);
    }

    void putTail(const off_t tail) {
        write(tail, tail_off);
    }

    void clear() {
        file.close();
        file.open(file_name, ios::binary | ios::trunc | ios::out);
        off_t tmp = null_off;
        for(int i = 0; i < info_cnt; ++i) {
            file.write(reinterpret_cast<const char *>(&tmp), sizeof_off);
        }
        file.close();
        file.open(file_name, ios::binary | ios::in | ios::out);
    }

    template <typename U>
    void read(U& obj, const off_t off = 0) {
        file.seekg(off);
        file.read(reinterpret_cast<char *>(&obj), sizeof(U));
    }

    template <typename U>
    void write(const U& obj, const off_t off = 0) {
        file.seekp(off);
        file.write(reinterpret_cast<const char *>(&obj), sizeof(U));
    }

    off_t malloc() {
        off_t del_top;
        read(del_top, del_top_off);
        off_t ret;
        if(del_top) {
            off_t del_nex;
            read (del_nex, del_top);
            write(del_nex, del_top_off);
            ret = del_top + sizeof_off;
        }
        else {
            file.seekp(sizeof_off, ios::end);
            ret = file.tellp();
        }
        return ret;
    }

    void free(const off_t off) {
        off_t tmp;
        read (tmp, del_top_off);
        write(tmp, off - sizeof_off);
        tmp = off - sizeof_off;
        write(tmp, del_top_off);
    }

    ~MemoryPool() {
        file.close();
    }
};


#endif // _FILE_ACCESSOR_H_