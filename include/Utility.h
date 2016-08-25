#ifndef UTILITY_H_
#define UTILITY_H_

#define DEBUG_OUTPUT 1
#include "Types.h"

#include <chrono>
#include <random>
#include <string>

// TODO: write meto or combinin astables.

static inline void print_time(std::string str, TIME time) {

	if (DEBUG_OUTPUT)
		std::cout << str << " " << time << std::endl;
}

static inline std::string random_string(size_t length)
{
    static const std::string alphanums =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    static std::mt19937 rg{std::random_device{}()};
    static std::uniform_int_distribution<> pick(0, alphanums.size() - 1);

    std::string s;

    s.reserve(length);

    while(length--)
        s += alphanums[pick(rg)];

    return s;
}

#endif /* UTILITY_H_ */