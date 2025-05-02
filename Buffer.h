#pragma once
#include "config.h"
#include <optional>

template<typename bufferType, size_t Tsize>
class FixedBuffer {
  bufferType event[Tsize];
  size_t mBegin = 0;
  size_t mEnd = 0;

public:
  bool empty() const {
    return size() == 0;
  }

  size_t size() const {
    return mEnd - mBegin;
  }

  bufferType* begin() {
    return event + mBegin;
  }

  void advanceEnd(size_t n) {
    if (n > available()) n = available();  // protect against overflow
    mEnd += n;
  }

  void consume(uint8_t n) {
    if (n > size()) n = size();  // safety
    mBegin += n;
  }

  bufferType* end() {
    return event + mEnd;
  }

  void increase(size_t s) {
    mEnd += s;
  }

  bufferType* peek() {
    if (size() < 1) return nullptr;
    return begin();
  }

  bufferType* read() {
    auto p = peek();
    if (p)
      mBegin += 1;
    return p;
  }

  bufferType pop_front() {
    bufferType temp = std::move(event[mBegin]);
    ++mBegin;
    return temp;
  }

  bool hasNext() const {
    return size() > 0;
  }

  bool pop(size_t index) {
    if (index >= size()) return false;

    for (uint8_t i = index; i < size() - 1; ++i) {
      event[mBegin + i] = std::move(event[mBegin + i + 1]);
    }

    mEnd -= 1;
    return true;
  }

  void finalize() {
    if (mBegin == 0) return;

    size_t s = size();
    for (int i = 0; i < s; i++) {
      event[i] = std::move(event[mBegin + i]);
    }
    mBegin = 0;
    mEnd = s;
  }

  size_t available() const {
    return Tsize - mEnd;
  }

  size_t used() const {
    return capacity() - available();
  }

  size_t write(bufferType data) {
    finalize();
    if (size() == Tsize) return -1;

    auto temp = end();
    *temp = std::move(data);
    increase(1);

    return 1;
  }

  bool push_back(bufferType data) {
    finalize();
    if (size() == Tsize) return false;

    auto temp = end();
    *temp = std::move(data);
    increase(1);

    return true;
  }

  size_t capacity() const {
    return Tsize;
  }

  bufferType& operator[](size_t index) {
    return event[mBegin + index];
  }

  const bufferType& operator[](size_t index) const {
    return event[mBegin + index];
  }
};

template<typename bufferType>
class RuntimeBuffer {
  bufferType* event;  // Pointer to dynamically allocated array
  uint8_t mBegin = 0;
  uint8_t mEnd = 0;
  uint mCapacity = 0;  // Dynamic size of the buffer

public:
  // Constructor: takes the size as a parameter
  RuntimeBuffer(uint8_t capacity) {
    mCapacity = capacity;
    event = new bufferType[mCapacity];  // Allocate memory dynamically
  }

  // Destructor: clean up dynamically allocated memory
  ~RuntimeBuffer() {
    delete[] event;  // Free memory
  }

  // Get the size of the data in the buffer
  uint8_t size() const {
    return mEnd - mBegin;
  }

  // Return the pointer to the beginning of the buffer
  bufferType* begin() {
    return event + mBegin;
  }

  // Return the pointer to the end of the buffer
  bufferType* end() {
    return event + mEnd;
  }

  // Increase the 'end' pointer by 's' positions
  void increase(uint8_t s) {
    mEnd += s;
  }

  // Peek at the first element without removing it
  bufferType* peek() {
    if (size() < 1) return nullptr;
    return begin();
  }

  // Read the first element and move the 'begin' pointer forward
  bufferType* read() {
    auto p = peek();
    if (p == nullptr) return nullptr;
    if (p) {
      mBegin += 1;
    }
    return p;
  }

  // Read the first element and move the 'begin' pointer forward
  bufferType* read(uint8_t count) {
    auto p = peek();
    if (p == nullptr) return nullptr;
    if (p) {
      mBegin += count;
    }
    return p;
  }

  bool pop(uint8_t index) {
    if (index >= size()) return false;

    for (uint8_t i = index; i < size() - 1; ++i) {
      event[mBegin + i] = event[mBegin + i + 1];
    }

    mEnd -= 1;
    return true;
  }


  // Finalize the buffer by shifting data to the start of the buffer
  void finalize() {
    if (mBegin == 0) return;

    uint8_t s = size();
    for (int i = 0; i < s; i++) {
      event[i] = event[mBegin + i];
    }
    mBegin = 0;
    mEnd = s;
  }

  // Return the number of available spaces in the buffer
  int available() const {
    return mCapacity - mEnd;
  }

  int used() const {
    return capacity() - available();
  }

  // Write data into the buffer
  int write(bufferType data) {
    finalize();
    if (size() == mCapacity) return -1;

    auto temp = end();
    *temp = data;
    increase(1);

    return 1;
  }

  int write(bufferType* data, uint8_t dataSize) {
    finalize();

    if (dataSize > available()) {
      return -1;
    }

    memcpy(end(), data, dataSize * sizeof(bufferType));

    increase(dataSize);

    return dataSize;  // Return the number of elements written
  }

  // Get the capacity of the buffer
  uint capacity() const {
    return mCapacity;
  }

  bufferType& operator[](uint8_t index) {
    return event[mBegin + index];
  }

  const bufferType& operator[](uint8_t index) const {
    return event[mBegin + index];
  }
};