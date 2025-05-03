
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

template<typename K, typename V, size_t MAX_ITEMS>
class TinyMap {
  struct Entry {
    K first;   // key
    V second;  // value
    bool used = false;

    // For backwards compatibility (optional)
    K& key = first;
    V& value = second;
  };

  Entry entries[MAX_ITEMS];

public:
  TinyMap() = default;

  bool insert(const K& key, const V& value) {
    for (size_t i = 0; i < MAX_ITEMS; ++i) {
      if (entries[i].used && entries[i].key == key) {
        entries[i].value = value;
        return true;
      }
    }
    for (size_t i = 0; i < MAX_ITEMS; ++i) {
      if (!entries[i].used) {
        entries[i].key = key;
        entries[i].value = value;
        entries[i].used = true;
        return true;
      }
    }
    return false;
  }

  V* get(const K& key) {
    for (size_t i = 0; i < MAX_ITEMS; ++i) {
      if (entries[i].used && entries[i].key == key) {
        return &entries[i].value;
      }
    }
    return nullptr;
  }

  void remove(const K& key) {
    for (size_t i = 0; i < MAX_ITEMS; ++i) {
      if (entries[i].used && entries[i].key == key) {
        entries[i].used = false;
        break;
      }
    }
  }

  bool contains(const K& key) const {
    for (size_t i = 0; i < MAX_ITEMS; ++i) {
      if (entries[i].used && entries[i].key == key) {
        return true;
      }
    }
    return false;
  }

  void clear() {
    for (size_t i = 0; i < MAX_ITEMS; ++i) {
      entries[i].used = false;
    }
  }

  V& operator[](const K& key) {
    for (size_t i = 0; i < MAX_ITEMS; ++i) {
      if (entries[i].used && entries[i].key == key) {
        return entries[i].value;
      }
    }
    for (size_t i = 0; i < MAX_ITEMS; ++i) {
      if (!entries[i].used) {
        entries[i].key = key;
        entries[i].value = V();
        entries[i].used = true;
        return entries[i].value;
      }
    }
    static V dummy;
    return dummy;
  }

  // New: Entry find and end
  Entry* find(const K& key) {
    for (size_t i = 0; i < MAX_ITEMS; ++i) {
      if (entries[i].used && entries[i].key == key) {
        return &entries[i];
      }
    }
    return end();
  }

  const Entry* find(const K& key) const {
    for (size_t i = 0; i < MAX_ITEMS; ++i) {
      if (entries[i].used && entries[i].key == key) {
        return &entries[i];
      }
    }
    return end();
  }

  Entry* end() {
    return entries + MAX_ITEMS;
  }

  const Entry* end() const {
    return entries + MAX_ITEMS;
  }
};
