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
