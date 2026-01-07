#pragma once

#include <cstdlib>
#include <cstddef>
#include <iostream>
#include <new>

// ======================================================
//  DEBUG HELPER - instrumentacao local por .cpp
// ======================================================
//
//  Uso:
//      #include "debug_helper.hpp"
//
//  No final do main():
//      debug_helper::report();
//
//  Tudo vale SOMENTE para este arquivo.
// ======================================================

namespace debug_helper {

inline std::size_t alloc_count      = 0;
inline std::size_t alloc_array_count= 0;
inline std::size_t free_count       = 0;
inline std::size_t free_array_count = 0;
inline std::size_t bytes_allocated  = 0;

inline void report() {
    std::cout << "\n=== DEBUG HELPER (LOCAL) ===\n";
    std::cout << "new:        " << alloc_count << '\n';
    std::cout << "new[]:      " << alloc_array_count << '\n';
    std::cout << "delete:     " << free_count << '\n';
    std::cout << "delete[]:   " << free_array_count << '\n';
    std::cout << "Bytes alloc:" << bytes_allocated << '\n';
}

} // namespace debug_helper

// ======================================================
//  operator new / delete (somente neste .cpp)
// ======================================================

inline void* operator new(std::size_t size) {
    debug_helper::alloc_count++;
    debug_helper::bytes_allocated += size;

    if (void* p = std::malloc(size))
        return p;

    throw std::bad_alloc{};
}

inline void operator delete(void* ptr) noexcept {
    debug_helper::free_count++;
    std::free(ptr);
}

inline void* operator new[](std::size_t size) {
    debug_helper::alloc_array_count++;
    debug_helper::bytes_allocated += size;

    if (void* p = std::malloc(size))
        return p;

    throw std::bad_alloc{};
}

inline void operator delete[](void* ptr) noexcept {
    debug_helper::free_array_count++;
    std::free(ptr);
}
