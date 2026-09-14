#include <dimension_hash.h>
#include <cstdint>

std::size_t DimensionHash::operator()(const std::vector<uint32_t> &v) const
{
    std::size_t seed = v.size();
    for (int i : v)
    {
        // Golden ratio shift and XOR combination to minimize collisions
        seed ^= std::hash<int>()(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
}