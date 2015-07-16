// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
// https://developers.google.com/protocol-buffers/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Author: kenton@google.com (Kenton Varda) and others
//
// Contains basic types and utilities used by the rest of the library.

#ifndef GOOGLE_PROTOBUF_COMMON_H__
#define GOOGLE_PROTOBUF_COMMON_H__

#include <assert.h>
#include <stdlib.h>
#include <cstddef>
#include <string>
#include <string.h>
#if defined(__osf__)
// Tru64 lacks stdint.h, but has inttypes.h which defines a superset of
// what stdint.h would define.
#include <inttypes.h>
#elif !defined(_MSC_VER)
#include <stdint.h>
#endif

#undef PROTOBUF_LITTLE_ENDIAN
#ifdef _MSC_VER
  // Assuming windows is always little-endian.
  #if !defined(PROTOBUF_DISABLE_LITTLE_ENDIAN_OPT_FOR_TEST)
    #define PROTOBUF_LITTLE_ENDIAN 1
  #endif
  #if _MSC_VER >= 1300
    // If MSVC has "/RTCc" set, it will complain about truncating casts at
    // runtime.  This file contains some intentional truncating casts.
    #pragma runtime_checks("c", off)
  #endif
#else
  #include <sys/param.h>   // __BYTE_ORDER
  #if ((defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__)) || \
         (defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN)) && \
      !defined(PROTOBUF_DISABLE_LITTLE_ENDIAN_OPT_FOR_TEST)
    #define PROTOBUF_LITTLE_ENDIAN 1
  #endif
#endif

#ifndef PROTOBUF_USE_EXCEPTIONS
#if defined(_MSC_VER) && defined(_CPPUNWIND)
  #define PROTOBUF_USE_EXCEPTIONS 1
#elif defined(__EXCEPTIONS)
  #define PROTOBUF_USE_EXCEPTIONS 1
#else
  #define PROTOBUF_USE_EXCEPTIONS 0
#endif
#endif

#if PROTOBUF_USE_EXCEPTIONS
#include <exception>
#endif

#include <google/protobuf/stubs/platform_macros.h>

#if defined(__APPLE__)
#include <TargetConditionals.h>  // for TARGET_OS_IPHONE
#endif

#if defined(__ANDROID__) || defined(GOOGLE_PROTOBUF_OS_ANDROID) || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE) || defined(GOOGLE_PROTOBUF_OS_IPHONE)
#include <pthread.h>
#endif

#if defined(_WIN32) && defined(GetMessage)
// Allow GetMessage to be used as a valid method name in protobuf classes.
// windows.h defines GetMessage() as a macro.  Let's re-define it as an inline
// function.  The inline function should be equivalent for C++ users.
inline BOOL GetMessage_Win32(
    LPMSG lpMsg, HWND hWnd,
    UINT wMsgFilterMin, UINT wMsgFilterMax) {
  return GetMessage(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
}
#undef GetMessage
inline BOOL GetMessage(
    LPMSG lpMsg, HWND hWnd,
    UINT wMsgFilterMin, UINT wMsgFilterMax) {
  return GetMessage_Win32(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
}
#endif


namespace std {}

namespace google {
namespace protobuf {

#undef GOOGLE_DISALLOW_EVIL_CONSTRUCTORS
#define GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(TypeName)    \
  TypeName(const TypeName&);                           \
  void operator=(const TypeName&)

#undef GOOGLE_DISALLOW_IMPLICIT_CONSTRUCTORS
#define GOOGLE_DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
  TypeName();                                           \
  TypeName(const TypeName&);                            \
  void operator=(const TypeName&)

#if defined(_MSC_VER) && defined(PROTOBUF_USE_DLLS)
  #ifdef LIBPROTOBUF_EXPORTS
    #define LIBPROTOBUF_EXPORT __declspec(dllexport)
  #else
    #define LIBPROTOBUF_EXPORT __declspec(dllimport)
  #endif
  #ifdef LIBPROTOC_EXPORTS
    #define LIBPROTOC_EXPORT   __declspec(dllexport)
  #else
    #define LIBPROTOC_EXPORT   __declspec(dllimport)
  #endif
#else
  #define LIBPROTOBUF_EXPORT
  #define LIBPROTOC_EXPORT
#endif

namespace internal {

// Some of these constants are macros rather than const ints so that they can
// be used in #if directives.

// The current version, represented as a single integer to make comparison
// easier:  major * 10^6 + minor * 10^3 + micro
#define GOOGLE_PROTOBUF_VERSION 3000000

// The minimum library version which works with the current version of the
// headers.
#define GOOGLE_PROTOBUF_MIN_LIBRARY_VERSION 3000000

// The minimum header version which works with the current version of
// the library.  This constant should only be used by protoc's C++ code
// generator.
static const int kMinHeaderVersionForLibrary = 3000000;

// The minimum protoc version which works with the current version of the
// headers.
#define GOOGLE_PROTOBUF_MIN_PROTOC_VERSION 3000000

// The minimum header version which works with the current version of
// protoc.  This constant should only be used in VerifyVersion().
static const int kMinHeaderVersionForProtoc = 3000000;

// Verifies that the headers and libraries are compatible.  Use the macro
// below to call this.
void LIBPROTOBUF_EXPORT VerifyVersion(int headerVersion, int minLibraryVersion,
                                      const char* filename);

// Converts a numeric version number to a string.
std::string LIBPROTOBUF_EXPORT VersionString(int version);

}  // namespace internal

// Place this macro in your main() function (or somewhere before you attempt
// to use the protobuf library) to verify that the version you link against
// matches the headers you compiled against.  If a version mismatch is
// detected, the process will abort.
#define GOOGLE_PROTOBUF_VERIFY_VERSION                                    \
  ::google::protobuf::internal::VerifyVersion(                            \
    GOOGLE_PROTOBUF_VERSION, GOOGLE_PROTOBUF_MIN_LIBRARY_VERSION,         \
    __FILE__)

// ===================================================================
// from google3/base/port.h

typedef unsigned int uint;

#ifdef _MSC_VER
typedef signed __int8  int8;
typedef __int16 int16;
typedef __int32 int32;
typedef __int64 int64;

typedef unsigned __int8  uint8;
typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;
#else
typedef signed char  int8;
typedef short int16;
typedef int int32;
// NOTE: This should be "long long" for consistency with upstream, but
// something is stacked against this particular type for 64bit hashing.
// Switching it causes an obvious missing hash function (with an unobvious
// cause) when building the tests.
typedef int64_t int64;

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
// NOTE: This should be "unsigned long long" for consistency with upstream, but
// something is stacked against this particular type for 64bit hashing.
// Switching it causes an obvious missing hash function (with an unobvious
// cause) when building the tests.
typedef uint64_t uint64;
#endif

// long long macros to be used because gcc and vc++ use different suffixes,
// and different size specifiers in format strings
#undef GOOGLE_LONGLONG
#undef GOOGLE_ULONGLONG
#undef GOOGLE_LL_FORMAT

#ifdef _MSC_VER
#define GOOGLE_LONGLONG(x) x##I64
#define GOOGLE_ULONGLONG(x) x##UI64
#define GOOGLE_LL_FORMAT "I64"  // As in printf("%I64d", ...)
#else
#define GOOGLE_LONGLONG(x) x##LL
#define GOOGLE_ULONGLONG(x) x##ULL
#define GOOGLE_LL_FORMAT "ll"  // As in "%lld". Note that "q" is poor form also.
#endif

static const int32 kint32max = 0x7FFFFFFF;
static const int32 kint32min = -kint32max - 1;
static const int64 kint64max = GOOGLE_LONGLONG(0x7FFFFFFFFFFFFFFF);
static const int64 kint64min = -kint64max - 1;
static const uint32 kuint32max = 0xFFFFFFFFu;
static const uint64 kuint64max = GOOGLE_ULONGLONG(0xFFFFFFFFFFFFFFFF);

// -------------------------------------------------------------------
// Annotations:  Some parts of the code have been annotated in ways that might
//   be useful to some compilers or tools, but are not supported universally.
//   You can #define these annotations yourself if the default implementation
//   is not right for you.

#ifndef GOOGLE_ATTRIBUTE_ALWAYS_INLINE
#if defined(__GNUC__) && (__GNUC__ > 3 ||(__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
// For functions we want to force inline.
// Introduced in gcc 3.1.
#define GOOGLE_ATTRIBUTE_ALWAYS_INLINE __attribute__ ((always_inline))
#else
// Other compilers will have to figure it out for themselves.
#define GOOGLE_ATTRIBUTE_ALWAYS_INLINE
#endif
#endif

#ifndef GOOGLE_ATTRIBUTE_NOINLINE
#if defined(__GNUC__) && (__GNUC__ > 3 ||(__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
// For functions we want to force not inline.
// Introduced in gcc 3.1.
#define GOOGLE_ATTRIBUTE_NOINLINE __attribute__ ((noinline))
#else
// Other compilers will have to figure it out for themselves.
#define GOOGLE_ATTRIBUTE_NOINLINE
#endif
#endif

#ifndef GOOGLE_ATTRIBUTE_DEPRECATED
#ifdef __GNUC__
// If the method/variable/type is used anywhere, produce a warning.
#define GOOGLE_ATTRIBUTE_DEPRECATED __attribute__((deprecated))
#else
#define GOOGLE_ATTRIBUTE_DEPRECATED
#endif
#endif

#ifndef GOOGLE_PREDICT_TRUE
#ifdef __GNUC__
// Provided at least since GCC 3.0.
#define GOOGLE_PREDICT_TRUE(x) (__builtin_expect(!!(x), 1))
#else
#define GOOGLE_PREDICT_TRUE
#endif
#endif

#ifndef GOOGLE_PREDICT_FALSE
#ifdef __GNUC__
// Provided at least since GCC 3.0.
#define GOOGLE_PREDICT_FALSE(x) (__builtin_expect(x, 0))
#else
#define GOOGLE_PREDICT_FALSE
#endif
#endif

// Delimits a block of code which may write to memory which is simultaneously
// written by other threads, but which has been determined to be thread-safe
// (e.g. because it is an idempotent write).
#ifndef GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN
#define GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN()
#endif
#ifndef GOOGLE_SAFE_CONCURRENT_WRITES_END
#define GOOGLE_SAFE_CONCURRENT_WRITES_END()
#endif

#define GOOGLE_GUARDED_BY(x)
#define GOOGLE_FALLTHROUGH_INTENDED
#define GOOGLE_ATTRIBUTE_COLD

// x86 and x86-64 can perform unaligned loads/stores directly.
#if defined(_M_X64) || defined(__x86_64__) || \
    defined(_M_IX86) || defined(__i386__)

#define GOOGLE_UNALIGNED_LOAD16(_p) (*reinterpret_cast<const uint16 *>(_p))
#define GOOGLE_UNALIGNED_LOAD32(_p) (*reinterpret_cast<const uint32 *>(_p))
#define GOOGLE_UNALIGNED_LOAD64(_p) (*reinterpret_cast<const uint64 *>(_p))

#define GOOGLE_UNALIGNED_STORE16(_p, _val) (*reinterpret_cast<uint16 *>(_p) = (_val))
#define GOOGLE_UNALIGNED_STORE32(_p, _val) (*reinterpret_cast<uint32 *>(_p) = (_val))
#define GOOGLE_UNALIGNED_STORE64(_p, _val) (*reinterpret_cast<uint64 *>(_p) = (_val))

#else
inline uint16 GOOGLE_UNALIGNED_LOAD16(const void *p) {
  uint16 t;
  memcpy(&t, p, sizeof t);
  return t;
}

inline uint32 GOOGLE_UNALIGNED_LOAD32(const void *p) {
  uint32 t;
  memcpy(&t, p, sizeof t);
  return t;
}

inline uint64 GOOGLE_UNALIGNED_LOAD64(const void *p) {
  uint64 t;
  memcpy(&t, p, sizeof t);
  return t;
}

inline void GOOGLE_UNALIGNED_STORE16(void *p, uint16 v) {
  memcpy(p, &v, sizeof v);
}

inline void GOOGLE_UNALIGNED_STORE32(void *p, uint32 v) {
  memcpy(p, &v, sizeof v);
}

inline void GOOGLE_UNALIGNED_STORE64(void *p, uint64 v) {
  memcpy(p, &v, sizeof v);
}
#endif

#if defined(_MSC_VER)
#define GOOGLE_THREAD_LOCAL __declspec(thread)
#else
#define GOOGLE_THREAD_LOCAL __thread
#endif

// ===================================================================
// from google3/base/basictypes.h

// The GOOGLE_ARRAYSIZE(arr) macro returns the # of elements in an array arr.
// The expression is a compile-time constant, and therefore can be
// used in defining new arrays, for example.
//
// GOOGLE_ARRAYSIZE catches a few type errors.  If you see a compiler error
//
//   "warning: division by zero in ..."
//
// when using GOOGLE_ARRAYSIZE, you are (wrongfully) giving it a pointer.
// You should only use GOOGLE_ARRAYSIZE on statically allocated arrays.
//
// The following comments are on the implementation details, and can
// be ignored by the users.
//
// ARRAYSIZE(arr) works by inspecting sizeof(arr) (the # of bytes in
// the array) and sizeof(*(arr)) (the # of bytes in one array
// element).  If the former is divisible by the latter, perhaps arr is
// indeed an array, in which case the division result is the # of
// elements in the array.  Otherwise, arr cannot possibly be an array,
// and we generate a compiler error to prevent the code from
// compiling.
//
// Since the size of bool is implementation-defined, we need to cast
// !(sizeof(a) & sizeof(*(a))) to size_t in order to ensure the final
// result has type size_t.
//
// This macro is not perfect as it wrongfully accepts certain
// pointers, namely where the pointer size is divisible by the pointee
// size.  Since all our code has to go through a 32-bit compiler,
// where a pointer is 4 bytes, this means all pointers to a type whose
// size is 3 or greater than 4 will be (righteously) rejected.
//
// Kudos to Jorg Brown for this simple and elegant implementation.

#undef GOOGLE_ARRAYSIZE
#define GOOGLE_ARRAYSIZE(a) \
  ((sizeof(a) / sizeof(*(a))) / \
   static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))

// The COMPILE_ASSERT macro can be used to verify that a compile time
// expression is true. For example, you could use it to verify the
// size of a static array:
//
//   COMPILE_ASSERT(ARRAYSIZE(content_type_names) == CONTENT_NUM_TYPES,
//                  content_type_names_incorrect_size);
//
// or to make sure a struct is smaller than a certain size:
//
//   COMPILE_ASSERT(sizeof(foo) < 128, foo_too_large);
//
// The second argument to the macro is the name of the variable. If
// the expression is false, most compilers will issue a warning/error
// containing the name of the variable.

namespace internal {

template <bool>
struct CompileAssert {
};

}  // namespace internal

#undef GOOGLE_COMPILE_ASSERT
#define GOOGLE_COMPILE_ASSERT(expr, msg) \
  ::google::protobuf::internal::CompileAssert<(bool(expr))> \
          msg[bool(expr) ? 1 : -1]; \
  (void)msg


// Implementation details of COMPILE_ASSERT:
//
// - COMPILE_ASSERT works by defining an array type that has -1
//   elements (and thus is invalid) when the expression is false.
//
// - The simpler definition
//
//     #define COMPILE_ASSERT(expr, msg) typedef char msg[(expr) ? 1 : -1]
//
//   does not work, as gcc supports variable-length arrays whose sizes
//   are determined at run-time (this is gcc's extension and not part
//   of the C++ standard).  As a result, gcc fails to reject the
//   following code with the simple definition:
//
//     int foo;
//     COMPILE_ASSERT(foo, msg); // not supposed to compile as foo is
//                               // not a compile-time constant.
//
// - By using the type CompileAssert<(bool(expr))>, we ensures that
//   expr is a compile-time constant.  (Template arguments must be
//   determined at compile-time.)
//
// - The outter parentheses in CompileAssert<(bool(expr))> are necessary
//   to work around a bug in gcc 3.4.4 and 4.0.1.  If we had written
//
//     CompileAssert<bool(expr)>
//
//   instead, these compilers will refuse to compile
//
//     COMPILE_ASSERT(5 > 0, some_message);
//
//   (They seem to think the ">" in "5 > 0" marks the end of the
//   template argument list.)
//
// - The array size is (bool(expr) ? 1 : -1), instead of simply
//
//     ((expr) ? 1 : -1).
//
//   This is to avoid running into a bug in MS VC 7.1, which
//   causes ((0.0) ? 1 : -1) to incorrectly evaluate to 1.

// ===================================================================
// from google3/base/scoped_ptr.h

namespace internal {

//  This is an implementation designed to match the anticipated future TR2
//  implementation of the scoped_ptr class, and its closely-related brethren,
//  scoped_array, scoped_ptr_malloc, and make_scoped_ptr.

template <class C> class scoped_ptr;
template <class C> class scoped_array;

// A scoped_ptr<T> is like a T*, except that the destructor of scoped_ptr<T>
// automatically deletes the pointer it holds (if any).
// That is, scoped_ptr<T> owns the T object that it points to.
// Like a T*, a scoped_ptr<T> may hold either NULL or a pointer to a T object.
//
// The size of a scoped_ptr is small:
// sizeof(scoped_ptr<C>) == sizeof(C*)
template <class C>
class scoped_ptr {
 public:

  // The element type
  typedef C element_type;

  // Constructor.  Defaults to initializing with NULL.
  // There is no way to create an uninitialized scoped_ptr.
  // The input parameter must be allocated with new.
  explicit scoped_ptr(C* p = NULL) : ptr_(p) { }

  // Destructor.  If there is a C object, delete it.
  // We don't need to test ptr_ == NULL because C++ does that for us.
  ~scoped_ptr() {
    enum { type_must_be_complete = sizeof(C) };
    delete ptr_;
  }

  // Reset.  Deletes the current owned object, if any.
  // Then takes ownership of a new object, if given.
  // this->reset(this->get()) works.
  void reset(C* p = NULL) {
    if (p != ptr_) {
      enum { type_must_be_complete = sizeof(C) };
      delete ptr_;
      ptr_ = p;
    }
  }

  // Accessors to get the owned object.
  // operator* and operator-> will assert() if there is no current object.
  C& operator*() const {
    assert(ptr_ != NULL);
    return *ptr_;
  }
  C* operator->() const  {
    assert(ptr_ != NULL);
    return ptr_;
  }
  C* get() const { return ptr_; }

  // Comparison operators.
  // These return whether two scoped_ptr refer to the same object, not just to
  // two different but equal objects.
  bool operator==(C* p) const { return ptr_ == p; }
  bool operator!=(C* p) const { return ptr_ != p; }

  // Swap two scoped pointers.
  void swap(scoped_ptr& p2) {
    C* tmp = ptr_;
    ptr_ = p2.ptr_;
    p2.ptr_ = tmp;
  }

  // Release a pointer.
  // The return value is the current pointer held by this object.
  // If this object holds a NULL pointer, the return value is NULL.
  // After this operation, this object will hold a NULL pointer,
  // and will not own the object any more.
  C* release() {
    C* retVal = ptr_;
    ptr_ = NULL;
    return retVal;
  }

 private:
  C* ptr_;

  // Forbid comparison of scoped_ptr types.  If C2 != C, it totally doesn't
  // make sense, and if C2 == C, it still doesn't make sense because you should
  // never have the same object owned by two different scoped_ptrs.
  template <class C2> bool operator==(scoped_ptr<C2> const& p2) const;
  template <class C2> bool operator!=(scoped_ptr<C2> const& p2) const;

  // Disallow evil constructors
  scoped_ptr(const scoped_ptr&);
  void operator=(const scoped_ptr&);
};

// scoped_array<C> is like scoped_ptr<C>, except that the caller must allocate
// with new [] and the destructor deletes objects with delete [].
//
// As with scoped_ptr<C>, a scoped_array<C> either points to an object
// or is NULL.  A scoped_array<C> owns the object that it points to.
//
// Size: sizeof(scoped_array<C>) == sizeof(C*)
template <class C>
class scoped_array {
 public:

  // The element type
  typedef C element_type;

  // Constructor.  Defaults to initializing with NULL.
  // There is no way to create an uninitialized scoped_array.
  // The input parameter must be allocated with new [].
  explicit scoped_array(C* p = NULL) : array_(p) { }

  // Destructor.  If there is a C object, delete it.
  // We don't need to test ptr_ == NULL because C++ does that for us.
  ~scoped_array() {
    enum { type_must_be_complete = sizeof(C) };
    delete[] array_;
  }

  // Reset.  Deletes the current owned object, if any.
  // Then takes ownership of a new object, if given.
  // this->reset(this->get()) works.
  void reset(C* p = NULL) {
    if (p != array_) {
      enum { type_must_be_complete = sizeof(C) };
      delete[] array_;
      array_ = p;
    }
  }

  // Get one element of the current object.
  // Will assert() if there is no current object, or index i is negative.
  C& operator[](std::ptrdiff_t i) const {
    assert(i >= 0);
    assert(array_ != NULL);
    return array_[i];
  }

  // Get a pointer to the zeroth element of the current object.
  // If there is no current object, return NULL.
  C* get() const {
    return array_;
  }

  // Comparison operators.
  // These return whether two scoped_array refer to the same object, not just to
  // two different but equal objects.
  bool operator==(C* p) const { return array_ == p; }
  bool operator!=(C* p) const { return array_ != p; }

  // Swap two scoped arrays.
  void swap(scoped_array& p2) {
    C* tmp = array_;
    array_ = p2.array_;
    p2.array_ = tmp;
  }

  // Release an array.
  // The return value is the current pointer held by this object.
  // If this object holds a NULL pointer, the return value is NULL.
  // After this operation, this object will hold a NULL pointer,
  // and will not own the object any more.
  C* release() {
    C* retVal = array_;
    array_ = NULL;
    return retVal;
  }

 private:
  C* array_;

  // Forbid comparison of different scoped_array types.
  template <class C2> bool operator==(scoped_array<C2> const& p2) const;
  template <class C2> bool operator!=(scoped_array<C2> const& p2) const;

  // Disallow evil constructors
  scoped_array(const scoped_array&);
  void operator=(const scoped_array&);
};

}  // namespace internal

// We made these internal so that they would show up as such in the docs,
// but we don't want to stick "internal::" in front of them everywhere.
using internal::scoped_ptr;
using internal::scoped_array;

// ===================================================================
// emulates google3/base/logging.h

enum LogLevel {
  LOGLEVEL_INFO,     // Informational.  This is never actually used by
                     // libprotobuf.
  LOGLEVEL_WARNING,  // Warns about issues that, although not technically a
                     // problem now, could cause problems in the future.  For
                     // example, a // warning will be printed when parsing a
                     // message that is near the message size limit.
  LOGLEVEL_ERROR,    // An error occurred which should never happen during
                     // normal use.
  LOGLEVEL_FATAL,    // An error occurred from which the library cannot
                     // recover.  This usually indicates a programming error
                     // in the code which calls the library, especially when
                     // compiled in debug mode.

#ifdef NDEBUG
  LOGLEVEL_DFATAL = LOGLEVEL_ERROR
#else
  LOGLEVEL_DFATAL = LOGLEVEL_FATAL
#endif
};

class StringPiece;
namespace util {
class Status;
}
namespace internal {

class LogFinisher;

class LIBPROTOBUF_EXPORT LogMessage {
 public:
  LogMessage(LogLevel level, const char* filename, int line);
  ~LogMessage();

  LogMessage& operator<<(const std::string& value);
  LogMessage& operator<<(const char* value);
  LogMessage& operator<<(char value);
  LogMessage& operator<<(int value);
  LogMessage& operator<<(unsigned int value);
  LogMessage& operator<<(long value);
  LogMessage& operator<<(unsigned long value);
  LogMessage& operator<<(long long value);
  LogMessage& operator<<(unsigned long long value);
  LogMessage& operator<<(double value);
  LogMessage& operator<<(void* value);
  LogMessage& operator<<(const StringPiece& value);
  LogMessage& operator<<(const ::google::protobuf::util::Status& status);

 private:
  friend class LogFinisher;
  void Finish();

  LogLevel level_;
  const char* filename_;
  int line_;
  std::string message_;
};

// Used to make the entire "LOG(BLAH) << etc." expression have a void return
// type and print a newline after each message.
class LIBPROTOBUF_EXPORT LogFinisher {
 public:
  void operator=(LogMessage& other);
};

template<typename T>
bool IsOk(T status) { return status.ok(); }
template<>
inline bool IsOk(bool status) { return status; }

}  // namespace internal

// Undef everything in case we're being mixed with some other Google library
// which already defined them itself.  Presumably all Google libraries will
// support the same syntax for these so it should not be a big deal if they
// end up using our definitions instead.
#undef GOOGLE_LOG
#undef GOOGLE_LOG_IF

#undef GOOGLE_CHECK
#undef GOOGLE_CHECK_OK
#undef GOOGLE_CHECK_EQ
#undef GOOGLE_CHECK_NE
#undef GOOGLE_CHECK_LT
#undef GOOGLE_CHECK_LE
#undef GOOGLE_CHECK_GT
#undef GOOGLE_CHECK_GE
#undef GOOGLE_CHECK_NOTNULL

#undef GOOGLE_DLOG
#undef GOOGLE_DCHECK
#undef GOOGLE_DCHECK_OK
#undef GOOGLE_DCHECK_EQ
#undef GOOGLE_DCHECK_NE
#undef GOOGLE_DCHECK_LT
#undef GOOGLE_DCHECK_LE
#undef GOOGLE_DCHECK_GT
#undef GOOGLE_DCHECK_GE

#define GOOGLE_LOG(LEVEL)                                                 \
  ::google::protobuf::internal::LogFinisher() =                           \
    ::google::protobuf::internal::LogMessage(                             \
      ::google::protobuf::LOGLEVEL_##LEVEL, __FILE__, __LINE__)
#define GOOGLE_LOG_IF(LEVEL, CONDITION) \
  !(CONDITION) ? (void)0 : GOOGLE_LOG(LEVEL)

#define GOOGLE_CHECK(EXPRESSION) \
  GOOGLE_LOG_IF(FATAL, !(EXPRESSION)) << "CHECK failed: " #EXPRESSION ": "
#define GOOGLE_CHECK_OK(A) GOOGLE_CHECK(::google::protobuf::internal::IsOk(A))
#define GOOGLE_CHECK_EQ(A, B) GOOGLE_CHECK((A) == (B))
#define GOOGLE_CHECK_NE(A, B) GOOGLE_CHECK((A) != (B))
#define GOOGLE_CHECK_LT(A, B) GOOGLE_CHECK((A) <  (B))
#define GOOGLE_CHECK_LE(A, B) GOOGLE_CHECK((A) <= (B))
#define GOOGLE_CHECK_GT(A, B) GOOGLE_CHECK((A) >  (B))
#define GOOGLE_CHECK_GE(A, B) GOOGLE_CHECK((A) >= (B))

namespace internal {
template<typename T>
T* CheckNotNull(const char* /* file */, int /* line */,
                const char* name, T* val) {
  if (val == NULL) {
    GOOGLE_LOG(FATAL) << name;
  }
  return val;
}
}  // namespace internal
#define GOOGLE_CHECK_NOTNULL(A) \
  ::google::protobuf::internal::CheckNotNull(\
      __FILE__, __LINE__, "'" #A "' must not be NULL", (A))

#ifdef NDEBUG

#define GOOGLE_DLOG GOOGLE_LOG_IF(INFO, false)

#define GOOGLE_DCHECK(EXPRESSION) while(false) GOOGLE_CHECK(EXPRESSION)
#define GOOGLE_DCHECK_OK(E) GOOGLE_DCHECK(::google::protobuf::internal::IsOk(E))
#define GOOGLE_DCHECK_EQ(A, B) GOOGLE_DCHECK((A) == (B))
#define GOOGLE_DCHECK_NE(A, B) GOOGLE_DCHECK((A) != (B))
#define GOOGLE_DCHECK_LT(A, B) GOOGLE_DCHECK((A) <  (B))
#define GOOGLE_DCHECK_LE(A, B) GOOGLE_DCHECK((A) <= (B))
#define GOOGLE_DCHECK_GT(A, B) GOOGLE_DCHECK((A) >  (B))
#define GOOGLE_DCHECK_GE(A, B) GOOGLE_DCHECK((A) >= (B))

#else  // NDEBUG

#define GOOGLE_DLOG GOOGLE_LOG

#define GOOGLE_DCHECK    GOOGLE_CHECK
#define GOOGLE_DCHECK_OK GOOGLE_CHECK_OK
#define GOOGLE_DCHECK_EQ GOOGLE_CHECK_EQ
#define GOOGLE_DCHECK_NE GOOGLE_CHECK_NE
#define GOOGLE_DCHECK_LT GOOGLE_CHECK_LT
#define GOOGLE_DCHECK_LE GOOGLE_CHECK_LE
#define GOOGLE_DCHECK_GT GOOGLE_CHECK_GT
#define GOOGLE_DCHECK_GE GOOGLE_CHECK_GE

#endif  // !NDEBUG

typedef void LogHandler(LogLevel level, const char* filename, int line,
                        const std::string& message);

// The protobuf library sometimes writes warning and error messages to
// stderr.  These messages are primarily useful for developers, but may
// also help end users figure out a problem.  If you would prefer that
// these messages be sent somewhere other than stderr, call SetLogHandler()
// to set your own handler.  This returns the old handler.  Set the handler
// to NULL to ignore log messages (but see also LogSilencer, below).
//
// Obviously, SetLogHandler is not thread-safe.  You should only call it
// at initialization time, and probably not from library code.  If you
// simply want to suppress log messages temporarily (e.g. because you
// have some code that tends to trigger them frequently and you know
// the warnings are not important to you), use the LogSilencer class
// below.
LIBPROTOBUF_EXPORT LogHandler* SetLogHandler(LogHandler* new_func);

// Create a LogSilencer if you want to temporarily suppress all log
// messages.  As long as any LogSilencer objects exist, non-fatal
// log messages will be discarded (the current LogHandler will *not*
// be called).  Constructing a LogSilencer is thread-safe.  You may
// accidentally suppress log messages occurring in another thread, but
// since messages are generally for debugging purposes only, this isn't
// a big deal.  If you want to intercept log messages, use SetLogHandler().
class LIBPROTOBUF_EXPORT LogSilencer {
 public:
  LogSilencer();
  ~LogSilencer();
};

// ===================================================================
// emulates google3/base/callback.h

// Abstract interface for a callback.  When calling an RPC, you must provide
// a Closure to call when the procedure completes.  See the Service interface
// in service.h.
//
// To automatically construct a Closure which calls a particular function or
// method with a particular set of parameters, use the NewCallback() function.
// Example:
//   void FooDone(const FooResponse* response) {
//     ...
//   }
//
//   void CallFoo() {
//     ...
//     // When done, call FooDone() and pass it a pointer to the response.
//     Closure* callback = NewCallback(&FooDone, response);
//     // Make the call.
//     service->Foo(controller, request, response, callback);
//   }
//
// Example that calls a method:
//   class Handler {
//    public:
//     ...
//
//     void FooDone(const FooResponse* response) {
//       ...
//     }
//
//     void CallFoo() {
//       ...
//       // When done, call FooDone() and pass it a pointer to the response.
//       Closure* callback = NewCallback(this, &Handler::FooDone, response);
//       // Make the call.
//       service->Foo(controller, request, response, callback);
//     }
//   };
//
// Currently NewCallback() supports binding zero, one, or two arguments.
//
// Callbacks created with NewCallback() automatically delete themselves when
// executed.  They should be used when a callback is to be called exactly
// once (usually the case with RPC callbacks).  If a callback may be called
// a different number of times (including zero), create it with
// NewPermanentCallback() instead.  You are then responsible for deleting the
// callback (using the "delete" keyword as normal).
//
// Note that NewCallback() is a bit touchy regarding argument types.  Generally,
// the values you provide for the parameter bindings must exactly match the
// types accepted by the callback function.  For example:
//   void Foo(string s);
//   NewCallback(&Foo, "foo");          // WON'T WORK:  const char* != string
//   NewCallback(&Foo, string("foo"));  // WORKS
// Also note that the arguments cannot be references:
//   void Foo(const string& s);
//   string my_str;
//   NewCallback(&Foo, my_str);  // WON'T WORK:  Can't use referecnes.
// However, correctly-typed pointers will work just fine.
class LIBPROTOBUF_EXPORT Closure {
 public:
  Closure() {}
  virtual ~Closure();

  virtual void Run() = 0;

 private:
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(Closure);
};

template<typename R, typename A1>
class LIBPROTOBUF_EXPORT ResultCallback1 {
 public:
  ResultCallback1() {}
  virtual ~ResultCallback1() {}

  virtual R Run(A1) = 0;

 private:
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(ResultCallback1);
};

template<typename R, typename A1, typename A2>
class LIBPROTOBUF_EXPORT ResultCallback2 {
 public:
  ResultCallback2() {}
  virtual ~ResultCallback2() {}

  virtual R Run(A1,A2) = 0;

 private:
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(ResultCallback2);
};

namespace internal {

class LIBPROTOBUF_EXPORT FunctionClosure0 : public Closure {
 public:
  typedef void (*FunctionType)();

  FunctionClosure0(FunctionType function, bool self_deleting)
    : function_(function), self_deleting_(self_deleting) {}
  ~FunctionClosure0();

  void Run() {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    function_();
    if (needs_delete) delete this;
  }

 private:
  FunctionType function_;
  bool self_deleting_;
};

template <typename Class>
class MethodClosure0 : public Closure {
 public:
  typedef void (Class::*MethodType)();

  MethodClosure0(Class* object, MethodType method, bool self_deleting)
    : object_(object), method_(method), self_deleting_(self_deleting) {}
  ~MethodClosure0() {}

  void Run() {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    (object_->*method_)();
    if (needs_delete) delete this;
  }

 private:
  Class* object_;
  MethodType method_;
  bool self_deleting_;
};

template <typename Arg1>
class FunctionClosure1 : public Closure {
 public:
  typedef void (*FunctionType)(Arg1 arg1);

  FunctionClosure1(FunctionType function, bool self_deleting,
                   Arg1 arg1)
    : function_(function), self_deleting_(self_deleting),
      arg1_(arg1) {}
  ~FunctionClosure1() {}

  void Run() {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    function_(arg1_);
    if (needs_delete) delete this;
  }

 private:
  FunctionType function_;
  bool self_deleting_;
  Arg1 arg1_;
};

template <typename Class, typename Arg1>
class MethodClosure1 : public Closure {
 public:
  typedef void (Class::*MethodType)(Arg1 arg1);

  MethodClosure1(Class* object, MethodType method, bool self_deleting,
                 Arg1 arg1)
    : object_(object), method_(method), self_deleting_(self_deleting),
      arg1_(arg1) {}
  ~MethodClosure1() {}

  void Run() {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    (object_->*method_)(arg1_);
    if (needs_delete) delete this;
  }

 private:
  Class* object_;
  MethodType method_;
  bool self_deleting_;
  Arg1 arg1_;
};

template <typename Arg1, typename Arg2>
class FunctionClosure2 : public Closure {
 public:
  typedef void (*FunctionType)(Arg1 arg1, Arg2 arg2);

  FunctionClosure2(FunctionType function, bool self_deleting,
                   Arg1 arg1, Arg2 arg2)
    : function_(function), self_deleting_(self_deleting),
      arg1_(arg1), arg2_(arg2) {}
  ~FunctionClosure2() {}

  void Run() {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    function_(arg1_, arg2_);
    if (needs_delete) delete this;
  }

 private:
  FunctionType function_;
  bool self_deleting_;
  Arg1 arg1_;
  Arg2 arg2_;
};

template <typename Class, typename Arg1, typename Arg2>
class MethodClosure2 : public Closure {
 public:
  typedef void (Class::*MethodType)(Arg1 arg1, Arg2 arg2);

  MethodClosure2(Class* object, MethodType method, bool self_deleting,
                 Arg1 arg1, Arg2 arg2)
    : object_(object), method_(method), self_deleting_(self_deleting),
      arg1_(arg1), arg2_(arg2) {}
  ~MethodClosure2() {}

  void Run() {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    (object_->*method_)(arg1_, arg2_);
    if (needs_delete) delete this;
  }

 private:
  Class* object_;
  MethodType method_;
  bool self_deleting_;
  Arg1 arg1_;
  Arg2 arg2_;
};

template<typename R, typename Arg1>
class FunctionResultCallback_0_1 : public ResultCallback1<R, Arg1> {
 public:
  typedef R (*FunctionType)(Arg1 arg1);

  FunctionResultCallback_0_1(FunctionType function, bool self_deleting)
      : function_(function), self_deleting_(self_deleting) {}
  ~FunctionResultCallback_0_1() {}

  R Run(Arg1 a1) {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    R result = function_(a1);
    if (needs_delete) delete this;
    return result;
  }

 private:
  FunctionType function_;
  bool self_deleting_;
};

template<typename R, typename P1, typename A1>
class FunctionResultCallback_1_1 : public ResultCallback1<R, A1> {
 public:
  typedef R (*FunctionType)(P1, A1);

  FunctionResultCallback_1_1(FunctionType function, bool self_deleting,
                             P1 p1)
      : function_(function), self_deleting_(self_deleting), p1_(p1) {}
  ~FunctionResultCallback_1_1() {}

  R Run(A1 a1) {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    R result = function_(p1_, a1);
    if (needs_delete) delete this;
    return result;
  }

 private:
  FunctionType function_;
  bool self_deleting_;
  P1 p1_;
};

// Duplicate this again in the type_traits.h, due to dependency problems.
template <class T> struct internal_remove_reference;
template<typename T> struct internal_remove_reference { typedef T type; };
template<typename T> struct internal_remove_reference<T&> { typedef T type; };

template <typename T>
struct InternalConstRef {
  typedef typename internal_remove_reference<T>::type base_type;
  typedef const base_type& type;
};

template <typename R, typename T, typename P1, typename P2, typename P3,
          typename P4, typename P5, typename A1, typename A2>
class MethodResultCallback_5_2 : public ResultCallback2<R, A1, A2> {
 public:
  typedef R (T::*MethodType)(P1, P2, P3, P4, P5, A1, A2);
  MethodResultCallback_5_2(T* object, MethodType method, bool self_deleting,
                           P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
      : object_(object),
        method_(method),
        self_deleting_(self_deleting),
        p1_(p1),
        p2_(p2),
        p3_(p3),
        p4_(p4),
        p5_(p5) {}
  ~MethodResultCallback_5_2() {}

  R Run(A1 a1, A2 a2) {
    bool needs_delete = self_deleting_;
    R result = (object_->*method_)(p1_, p2_, p3_, p4_, p5_, a1, a2);
    if (needs_delete) delete this;
    return result;
  }

 private:
  T* object_;
  MethodType method_;
  bool self_deleting_;
  typename internal_remove_reference<P1>::type p1_;
  typename internal_remove_reference<P2>::type p2_;
  typename internal_remove_reference<P3>::type p3_;
  typename internal_remove_reference<P4>::type p4_;
  typename internal_remove_reference<P5>::type p5_;
};

}  // namespace internal

// See Closure.
inline Closure* NewCallback(void (*function)()) {
  return new internal::FunctionClosure0(function, true);
}

// See Closure.
inline Closure* NewPermanentCallback(void (*function)()) {
  return new internal::FunctionClosure0(function, false);
}

// See Closure.
template <typename Class>
inline Closure* NewCallback(Class* object, void (Class::*method)()) {
  return new internal::MethodClosure0<Class>(object, method, true);
}

// See Closure.
template <typename Class>
inline Closure* NewPermanentCallback(Class* object, void (Class::*method)()) {
  return new internal::MethodClosure0<Class>(object, method, false);
}

// See Closure.
template <typename Arg1>
inline Closure* NewCallback(void (*function)(Arg1),
                            Arg1 arg1) {
  return new internal::FunctionClosure1<Arg1>(function, true, arg1);
}

// See Closure.
template <typename Arg1>
inline Closure* NewPermanentCallback(void (*function)(Arg1),
                                     Arg1 arg1) {
  return new internal::FunctionClosure1<Arg1>(function, false, arg1);
}

// See Closure.
template <typename Class, typename Arg1>
inline Closure* NewCallback(Class* object, void (Class::*method)(Arg1),
                            Arg1 arg1) {
  return new internal::MethodClosure1<Class, Arg1>(object, method, true, arg1);
}

// See Closure.
template <typename Class, typename Arg1>
inline Closure* NewPermanentCallback(Class* object, void (Class::*method)(Arg1),
                                     Arg1 arg1) {
  return new internal::MethodClosure1<Class, Arg1>(object, method, false, arg1);
}

// See Closure.
template <typename Arg1, typename Arg2>
inline Closure* NewCallback(void (*function)(Arg1, Arg2),
                            Arg1 arg1, Arg2 arg2) {
  return new internal::FunctionClosure2<Arg1, Arg2>(
    function, true, arg1, arg2);
}

// See Closure.
template <typename Arg1, typename Arg2>
inline Closure* NewPermanentCallback(void (*function)(Arg1, Arg2),
                                     Arg1 arg1, Arg2 arg2) {
  return new internal::FunctionClosure2<Arg1, Arg2>(
    function, false, arg1, arg2);
}

// See Closure.
template <typename Class, typename Arg1, typename Arg2>
inline Closure* NewCallback(Class* object, void (Class::*method)(Arg1, Arg2),
                            Arg1 arg1, Arg2 arg2) {
  return new internal::MethodClosure2<Class, Arg1, Arg2>(
    object, method, true, arg1, arg2);
}

// See Closure.
template <typename Class, typename Arg1, typename Arg2>
inline Closure* NewPermanentCallback(
    Class* object, void (Class::*method)(Arg1, Arg2),
    Arg1 arg1, Arg2 arg2) {
  return new internal::MethodClosure2<Class, Arg1, Arg2>(
    object, method, false, arg1, arg2);
}

// See ResultCallback1
template<typename R, typename A1>
inline ResultCallback1<R, A1>* NewCallback(R (*function)(A1)) {
  return new internal::FunctionResultCallback_0_1<R, A1>(function, true);
}

// See ResultCallback1
template<typename R, typename A1>
inline ResultCallback1<R, A1>* NewPermanentCallback(R (*function)(A1)) {
  return new internal::FunctionResultCallback_0_1<R, A1>(function, false);
}

// See ResultCallback1
template<typename R, typename P1, typename A1>
inline ResultCallback1<R, A1>* NewCallback(R (*function)(P1, A1), P1 p1) {
  return new internal::FunctionResultCallback_1_1<R, P1, A1>(
      function, true, p1);
}

// See ResultCallback1
template<typename R, typename P1, typename A1>
inline ResultCallback1<R, A1>* NewPermanentCallback(
    R (*function)(P1, A1), P1 p1) {
  return new internal::FunctionResultCallback_1_1<R, P1, A1>(
      function, false, p1);
}

// See MethodResultCallback_5_2
template <typename R, typename T, typename P1, typename P2, typename P3,
          typename P4, typename P5, typename A1, typename A2>
inline ResultCallback2<R, A1, A2>* NewPermanentCallback(
    T* object, R (T::*function)(P1, P2, P3, P4, P5, A1, A2),
    typename internal::InternalConstRef<P1>::type p1,
    typename internal::InternalConstRef<P2>::type p2,
    typename internal::InternalConstRef<P3>::type p3,
    typename internal::InternalConstRef<P4>::type p4,
    typename internal::InternalConstRef<P5>::type p5) {
  return new internal::MethodResultCallback_5_2<R, T, P1, P2, P3, P4, P5, A1,
                                                A2>(object, function, false, p1,
                                                    p2, p3, p4, p5);
}

// A function which does nothing.  Useful for creating no-op callbacks, e.g.:
//   Closure* nothing = NewCallback(&DoNothing);
void LIBPROTOBUF_EXPORT DoNothing();

// ===================================================================
// emulates google3/base/mutex.h

namespace internal {

// A Mutex is a non-reentrant (aka non-recursive) mutex.  At most one thread T
// may hold a mutex at a given time.  If T attempts to Lock() the same Mutex
// while holding it, T will deadlock.
class LIBPROTOBUF_EXPORT Mutex {
 public:
  // Create a Mutex that is not held by anybody.
  Mutex();

  // Destructor
  ~Mutex();

  // Block if necessary until this Mutex is free, then acquire it exclusively.
  void Lock();

  // Release this Mutex.  Caller must hold it exclusively.
  void Unlock();

  // Crash if this Mutex is not held exclusively by this thread.
  // May fail to crash when it should; will never crash when it should not.
  void AssertHeld();

 private:
  struct Internal;
  Internal* mInternal;

  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(Mutex);
};

// Undefine the macros  to workaround the conflicts with Google internal
// MutexLock implementation.
// TODO(liujisi): Remove the undef once internal macros are removed.
#undef MutexLock
#undef ReaderMutexLock
#undef WriterMutexLock
#undef MutexLockMaybe

// MutexLock(mu) acquires mu when constructed and releases it when destroyed.
class LIBPROTOBUF_EXPORT MutexLock {
 public:
  explicit MutexLock(Mutex *mu) : mu_(mu) { this->mu_->Lock(); }
  ~MutexLock() { this->mu_->Unlock(); }
 private:
  Mutex *const mu_;
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(MutexLock);
};

// TODO(kenton):  Implement these?  Hard to implement portably.
typedef MutexLock ReaderMutexLock;
typedef MutexLock WriterMutexLock;

// MutexLockMaybe is like MutexLock, but is a no-op when mu is NULL.
class LIBPROTOBUF_EXPORT MutexLockMaybe {
 public:
  explicit MutexLockMaybe(Mutex *mu) :
    mu_(mu) { if (this->mu_ != NULL) { this->mu_->Lock(); } }
  ~MutexLockMaybe() { if (this->mu_ != NULL) { this->mu_->Unlock(); } }
 private:
  Mutex *const mu_;
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(MutexLockMaybe);
};

#if defined(GOOGLE_PROTOBUF_NO_THREADLOCAL)
template<typename T>
class ThreadLocalStorage {
 public:
  ThreadLocalStorage() {
    pthread_key_create(&key_, &ThreadLocalStorage::Delete);
  }
  ~ThreadLocalStorage() {
    pthread_key_delete(key_);
  }
  T* Get() {
    T* result = static_cast<T*>(pthread_getspecific(key_));
    if (result == NULL) {
      result = new T();
      pthread_setspecific(key_, result);
    }
    return result;
  }
 private:
  static void Delete(void* value) {
    delete static_cast<T*>(value);
  }
  pthread_key_t key_;

  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(ThreadLocalStorage);
};
#endif

}  // namespace internal

// We made these internal so that they would show up as such in the docs,
// but we don't want to stick "internal::" in front of them everywhere.
using internal::Mutex;
using internal::MutexLock;
using internal::ReaderMutexLock;
using internal::WriterMutexLock;
using internal::MutexLockMaybe;

// ===================================================================
// from google3/util/utf8/public/unilib.h

namespace internal {

// Checks if the buffer contains structurally-valid UTF-8.  Implemented in
// structurally_valid.cc.
LIBPROTOBUF_EXPORT bool IsStructurallyValidUTF8(const char* buf, int len);

}  // namespace internal

// ===================================================================
// from google3/base/port.h

// The following guarantees declaration of the byte swap functions, and
// defines __BYTE_ORDER for MSVC
#ifdef _MSC_VER
#include <stdlib.h>  // NOLINT(build/include)
#define __BYTE_ORDER __LITTLE_ENDIAN
#define bswap_16(x) _byteswap_ushort(x)
#define bswap_32(x) _byteswap_ulong(x)
#define bswap_64(x) _byteswap_uint64(x)

#elif defined(__APPLE__)
// Mac OS X / Darwin features
#include <libkern/OSByteOrder.h>
#define bswap_16(x) OSSwapInt16(x)
#define bswap_32(x) OSSwapInt32(x)
#define bswap_64(x) OSSwapInt64(x)

#elif defined(__GLIBC__) || defined(__CYGWIN__)
#include <byteswap.h>  // IWYU pragma: export

#else

static inline uint16 bswap_16(uint16 x) {
  return static_cast<uint16>(((x & 0xFF) << 8) | ((x & 0xFF00) >> 8));
}
#define bswap_16(x) bswap_16(x)
static inline uint32 bswap_32(uint32 x) {
  return (((x & 0xFF) << 24) |
          ((x & 0xFF00) << 8) |
          ((x & 0xFF0000) >> 8) |
          ((x & 0xFF000000) >> 24));
}
#define bswap_32(x) bswap_32(x)
static inline uint64 bswap_64(uint64 x) {
  return (((x & GOOGLE_ULONGLONG(0xFF)) << 56) |
          ((x & GOOGLE_ULONGLONG(0xFF00)) << 40) |
          ((x & GOOGLE_ULONGLONG(0xFF0000)) << 24) |
          ((x & GOOGLE_ULONGLONG(0xFF000000)) << 8) |
          ((x & GOOGLE_ULONGLONG(0xFF00000000)) >> 8) |
          ((x & GOOGLE_ULONGLONG(0xFF0000000000)) >> 24) |
          ((x & GOOGLE_ULONGLONG(0xFF000000000000)) >> 40) |
          ((x & GOOGLE_ULONGLONG(0xFF00000000000000)) >> 56));
}
#define bswap_64(x) bswap_64(x)

#endif

// ===================================================================
// from google3/util/endian/endian.h
LIBPROTOBUF_EXPORT uint32 ghtonl(uint32 x);

class BigEndian {
 public:
#ifdef PROTOBUF_LITTLE_ENDIAN

  static uint16 FromHost16(uint16 x) { return bswap_16(x); }
  static uint16 ToHost16(uint16 x) { return bswap_16(x); }

  static uint32 FromHost32(uint32 x) { return bswap_32(x); }
  static uint32 ToHost32(uint32 x) { return bswap_32(x); }

  static uint64 FromHost64(uint64 x) { return bswap_64(x); }
  static uint64 ToHost64(uint64 x) { return bswap_64(x); }

  static bool IsLittleEndian() { return true; }

#else

  static uint16 FromHost16(uint16 x) { return x; }
  static uint16 ToHost16(uint16 x) { return x; }

  static uint32 FromHost32(uint32 x) { return x; }
  static uint32 ToHost32(uint32 x) { return x; }

  static uint64 FromHost64(uint64 x) { return x; }
  static uint64 ToHost64(uint64 x) { return x; }

  static bool IsLittleEndian() { return false; }

#endif /* ENDIAN */

  // Functions to do unaligned loads and stores in big-endian order.
  static uint16 Load16(const void *p) {
    return ToHost16(GOOGLE_UNALIGNED_LOAD16(p));
  }

  static void Store16(void *p, uint16 v) {
    GOOGLE_UNALIGNED_STORE16(p, FromHost16(v));
  }

  static uint32 Load32(const void *p) {
    return ToHost32(GOOGLE_UNALIGNED_LOAD32(p));
  }

  static void Store32(void *p, uint32 v) {
    GOOGLE_UNALIGNED_STORE32(p, FromHost32(v));
  }

  static uint64 Load64(const void *p) {
    return ToHost64(GOOGLE_UNALIGNED_LOAD64(p));
  }

  static void Store64(void *p, uint64 v) {
    GOOGLE_UNALIGNED_STORE64(p, FromHost64(v));
  }
};

// ===================================================================
// Shutdown support.

// Shut down the entire protocol buffers library, deleting all static-duration
// objects allocated by the library or by generated .pb.cc files.
//
// There are two reasons you might want to call this:
// * You use a draconian definition of "memory leak" in which you expect
//   every single malloc() to have a corresponding free(), even for objects
//   which live until program exit.
// * You are writing a dynamically-loaded library which needs to clean up
//   after itself when the library is unloaded.
//
// It is safe to call this multiple times.  However, it is not safe to use
// any other part of the protocol buffers library after
// ShutdownProtobufLibrary() has been called.
LIBPROTOBUF_EXPORT void ShutdownProtobufLibrary();

namespace internal {

// Register a function to be called when ShutdownProtocolBuffers() is called.
LIBPROTOBUF_EXPORT void OnShutdown(void (*func)());

}  // namespace internal

#if PROTOBUF_USE_EXCEPTIONS
class FatalException : public std::exception {
 public:
  FatalException(const char* filename, int line, const std::string& message)
      : filename_(filename), line_(line), message_(message) {}
  virtual ~FatalException() throw();

  virtual const char* what() const throw();

  const char* filename() const { return filename_; }
  int line() const { return line_; }
  const std::string& message() const { return message_; }

 private:
  const char* filename_;
  const int line_;
  const std::string message_;
};
#endif

// This is at the end of the file instead of the beginning to work around a bug
// in some versions of MSVC.
using namespace std;  // Don't do this at home, kids.

}  // namespace protobuf
}  // namespace google

#endif  // GOOGLE_PROTOBUF_COMMON_H__
