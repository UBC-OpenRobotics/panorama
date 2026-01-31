// #ifndef PANORAMA_UTILS
// #define PANORAMA_UTILS

// #include <iostream>

// template <typename... Args>
// void pinfo(const Args&... args) {
//     (std::cout << "[INFO][Client]\t" << ... << args);
// }

// #endif

#ifndef PANORAMA_UTILS_HPP
#define PANORAMA_UTILS_HPP

#include <iostream>

template <typename... Args>
void pinfo(const Args&... args) {
    std::cout << "[INFO][Client]\t";
    (std::cout << ... << args);   // fold expression over operator<<
    std::cout << std::endl;
}

#endif // PANORAMA_UTILS_HPP
