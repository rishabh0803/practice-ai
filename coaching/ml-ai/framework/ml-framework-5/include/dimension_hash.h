#pragma once
#include <vector>
#include <iostream>
#include <cstdint>

using namespace std;

class DimensionHash
{
public:
    std::size_t operator()(const std::vector<uint32_t> &v) const;
};
