#ifndef _SPARSE_H
#define _SPARSE_H

#include <cinttypes>

struct InternalNode {
    InternalNode *up = nullptr, *right = nullptr, *left = nullptr, *down = nullptr;
    std::int32_t x = 0, y = 0, column = 0, value = 0;
};

struct RightHeaderNode {
    InternalNode *down = nullptr;
    RightHeaderNode *right = nullptr;
    std::int32_t value = 0;
};

struct DownHeaderNode {
    DownHeaderNode *down = nullptr;
    InternalNode *right = nullptr;
    std::int32_t value = 0;
};

class SparseMatrix {
public:
    RightHeaderNode *rhead = nullptr;
    DownHeaderNode *dhead = nullptr;
    std::int32_t x = 0, y = 0;

    SparseMatrix(std::int32_t x, std::int32_t y);

    /* sliently fails if x and y are outside bounds */
    void set_value(std::int32_t x, std::int32_t y, std::int32_t value) const noexcept;

};


#endif
