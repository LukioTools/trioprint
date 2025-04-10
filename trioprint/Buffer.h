#pragma once
#include "config.h"

template<typename bufferType, int Tsize>
class Buffer{
    bufferType event[Tsize];
    uint8_t mBegin = 0;
    uint8_t mEnd = 0;

    public:

    uint8_t size(){
        return mEnd - mBegin;
    }

    bufferType* begin(){
        return event+mBegin;
    }

    bufferType* end(){
        return event+mEnd;
    }

    void increase(uint8_t s) { mEnd+=s; }

    bufferType* peek(){
        if(size < 1) return nullptr;
        String* t = reinterpret_cast<String*>(begin());
        return t;
    }

    bufferType* read(){
        auto p = peek();
        if(p)
            mBegin+=1;
        return p;
    }


    void finalize(){
        if(mBegin == 0) return;

        uint8_t s = size();
        for (int i = 0; i < s; i++) {
            event[i] = event[mBegin+i];
        }
        mBegin = 0;
        mEnd = s;
    }

    int available(){
        return Tsize - mEnd;
    }

    
    int write(bufferType data){
        finalize();
        if(size() == Tsize) return -1;

        auto temp = end();
        *temp = data;
        increase(1);

        return 1;
    }

};