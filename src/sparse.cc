#include "sparse.h"

SparseMatrix::SparseMatrix(std::int32_t x, std::int32_t y) : x(x), y(y) {
    if (x <= 0 || y <= 0) { return; }

    rhead = new RightHeaderNode;
    RightHeaderNode *rlast = rhead;
    for (std::int32_t i = 0; i < x; i++) {
        rlast->right = new RightHeaderNode;
        rlast->value = i;
        rlast = rlast->right;
    }

    dhead = new DownHeaderNode;
    DownHeaderNode *dlast = dhead;
    for (std::int32_t i = 0; i < y; i++) {
        dlast->down = new DownHeaderNode;
        dlast->value = i;
        dlast = dlast->down;
    }

}

void SparseMatrix::set_value(std::int32_t x, std::int32_t y, std::int32_t value) const noexcept {
    if (x > this->x || y > this->y) { return; }

    if (x <= y) { /* prefer traversing right nodes */
        for (RightHeaderNode *r = rhead; r->right != nullptr; r = r->right) {
            if (r->value == x) {
                for (InternalNode *n = r->down; n->down != nullptr; n = n->down) {
                    if (n->y == y) {
                        n->value = value;
                        return;
                    }
                }
            }
        }
    } else {
        for (DownHeaderNode *d = dhead; d->down != nullptr; d = d->down) {
            if (d->value == y) {
                for (InternalNode *n = d->right; n->right != nullptr; n = n->right) {
                    if (n->x == x) {
                        n->value = value;
                        return;
                    }
                }
                /* need to create element */
                return;
            }
        }
    }
}
