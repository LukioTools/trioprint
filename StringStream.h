#pragma once
#include <string>
#include <utility>

class StringStream : public String{
private:
public:

    String& str(){
        return *this;
    }

    StringStream(const StringStream& copy):String(copy){}
    void operator=(const StringStream& copy){String::operator=(copy);}

    StringStream(StringStream&& move):String(std::move(move)){}
    void operator=(StringStream&& move){String::operator=(std::move(move));}

    friend StringStream& operator<<(StringStream& ss, const String& s){
        ss.str()+=s;
        return ss;
    }
    friend StringStream& operator<<(StringStream& ss, String&& s){
        ss.str()+=std::move(s);
        return ss;
    }
    friend StringStream& operator<<(StringStream& ss, const char* s){
        ss.str()+=s;
        return ss;
    }
    friend StringStream& operator<<(StringStream& ss, char s){
        ss.str()+=s;
        return ss;
    }
    using String::String;
    using String::operator+=;
    using String::operator[];
    ~StringStream(){}

};