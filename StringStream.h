
/*

 Copyright (c) <2025> <Vili Kervinen>
 
 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 
 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

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