// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// This file contains the specification, but not the implementations,
// of the types/operations/etc. that should be defined by a platform
// specific port_<platform>.h file.  Use this file as a reference for
// how to port this package to a new platform.

#ifndef STORAGE_LEVELDB_PORT_PORT_EXAMPLE_H_
#define STORAGE_LEVELDB_PORT_PORT_EXAMPLE_H_

namespace leveldb {
namespace port {

// TODO(jorlow): Many of these belong more in the environment class rather than
//               here. We should try moving them and see if it affects perf.

// The following boolean constant must be true on a little-endian machine
// and false otherwise.
static const bool kLittleEndian = true /* or some other expression */;

// ------------------ Threading -------------------

// A Mutex represents an exclusive lock.
class Mutex {
 public:
  Mutex();
  ~Mutex();

  // Lock the mutex.  Waits until other lockers have exited.
  // Will deadlock if the mutex is already locked by this thread.
  void Lock();

  // Unlock the mutex.
  // REQUIRES: This mutex was locked by this thread.
  void Unlock();

  // Optionally crash if this thread does not hold this mutex.
  // The implementation must be fast, especially if NDEBUG is
  // defined.  The implementation is allowed to skip all checks.
  void AssertHeld();
};

class CondVar {
 public:
  explicit CondVar(Mutex* mu);
  ~CondVar();

  // Atomically release *mu and block on this condition variable until
  // either a call to SignalAll(), or a call to Signal() that picks
  // this thread to wakeup.
  // REQUIRES: this thread holds *mu
  void Wait();

  // If there are some threads waiting, wake up at least one of them.
  void Signal();

  // Wake up all waiting threads.
  void SignallAll();
};

// A type that holds a pointer that can be read or written atomically
// (i.e., without word-tearing.)
class AtomicPointer {
 private:
  intptr_t rep_;
 public:
  // Initialize to arbitrary value
  AtomicPointer();

  // Initialize to hold v
  explicit AtomicPointer(void* v) : rep_(v) { }

  // Read and return the stored pointer with the guarantee that no
  // later memory access (read or write) by this thread can be
  // reordered ahead of this read.
  void* Acquire_Load() const;

  // Set v as the stored pointer with the guarantee that no earlier
  // memory access (read or write) by this thread can be reordered
  // after this store.
  void Release_Store(void* v);

  // Read the stored pointer with no ordering guarantees.
  void* NoBarrier_Load() const;

  // Set va as the stored pointer with no ordering guarantees.
  void NoBarrier_Store(void* v);
};

// ------------------ Checksumming -------------------

// Store a 160-bit hash of "data[0..len-1]" in "hash_array[0]..hash_array[19]"
extern void SHA1_Hash(const char* data, size_t len, char* hash_array);

// ------------------ Compression -------------------

// Store the snappy compression of "input[0,input_length-1]" in *output.
// Returns false if snappy is not supported by this port.
extern bool Snappy_Compress(const char* input, size_t input_length,
                            std::string* output);

// Attempt to snappy uncompress input[0,input_length-1] into *output.
// Returns true if successful, false if the input is invalid lightweight
// compressed data.
extern bool Snappy_Uncompress(const char* input_data, size_t input_length,
                              std::string* output);

// ------------------ Miscellaneous -------------------

// If heap profiling is not supported, returns false.
// Else repeatedly calls (*func)(arg, data, n) and then returns true.
// The concatenation of all "data[0,n-1]" fragments is the heap profile.
extern bool GetHeapProfile(void (*func)(void*, const char*, int), void* arg);

}
}

#endif  // STORAGE_LEVELDB_PORT_PORT_EXAMPLE_H_
