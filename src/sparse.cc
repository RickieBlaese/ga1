#include "sparse.h"

SparseMatrix::SparseMatrix(std::int32_t x, std::int32_t y) : x(x), y(y) {
    if (x <= 0 || y <= 0) {
        throw std::range_error("SparseMatrix::SparseMatrix called with a zero dimension");
    }

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

SparseMatrix::~SparseMatrix() {
    RightHeaderNode *rlast = nullptr;
    for (RightHeaderNode *r = rhead; r != nullptr; r = r->right) {
        InternalNode *last = nullptr;
        for (InternalNode *next = r->down; next != nullptr; next = next->down) {
            delete last;
            last = next;
        }
        delete last;
        delete rlast;
        rlast = r;
    }
    delete rlast;
    
    DownHeaderNode *dlast = nullptr;
    for (DownHeaderNode *d = dhead; d != nullptr; d = d->down) {
        delete dlast;
        dlast = d;
    }
    delete dlast;
}

void SparseMatrix::set_value(std::int32_t x, std::int32_t y, std::int32_t value) {
    if (x >= this->x || x < 0 || y >= this->y || y < 0) { return; }

    RightHeaderNode *rloc = nullptr;
    DownHeaderNode *dloc = nullptr;

    for (RightHeaderNode *r = rhead; r != nullptr; r = r->right) {
        if (r->value == x) {
            rloc = r;
            break;
        }
    }
    
    for (DownHeaderNode *d = dhead; d != nullptr; d = d->down) {
        if (d->value == y) {
            dloc = d;
            /* prefer checking with down nodes */
            for (InternalNode *n = d->right; n != nullptr; n = n->right) {
                if (n->x == x) {
                    n->value = value;
                    return;
                }
            }
        }
    }

    /* now we know we have to create element */
    auto *newn = new InternalNode;
    newn->x = x;
    newn->y = y;
    newn->value = value;
    
    if (rloc->down == nullptr) {
        rloc->down = newn;
        /* do not set up */
    } else {
        InternalNode *n = nullptr;
        for (n = rloc->down; n->down != nullptr && n->y < y; n = n->down) {;}
        if (n->down != nullptr) {
            newn->down = n->down;
            n->up = newn;
        }
        n->down = newn;
        newn->up = n;
    }

    if (dloc->right == nullptr) {
        dloc->right = newn;
    } else {
        InternalNode *n = nullptr;
        for (n = dloc->right; n->right != nullptr && n->x < x; n = n->right) {;}
        if (n->down != nullptr) {
            newn->right = n;
            n->left = newn;
        }
        n = newn;
        newn->left = n;
    }
}


std::optional<std::int32_t> SparseMatrix::get_value(std::int32_t x, std::int32_t y) const noexcept {
    if (x >= this->x || x < 0 || y >= this->y || y < 0) { return std::nullopt; }

    for (RightHeaderNode *r = rhead; r != nullptr; r = r->right) {
        if (r->value == x) {
            for (InternalNode *n = r->down; n != nullptr; n = n->down) {
                if (n->y == y) {
                    return n->value;
                }
            }
        }
    }

    return 0; /* did not find */
}


SparseMatrix SparseMatrix::operator+(const SparseMatrix& other) const {
    if (other.x != x || other.y != y) {
        throw std::range_error("SparseMatrix::operator+ called on matrices with different dimensions");
    }

    SparseMatrix ret(x, y);
    
    for (std::int32_t i = 0; i < x; i++) {
        for (std::int32_t j = 0; j < y; j++) {
            std::optional<std::int32_t> asum = get_value(i, j), bsum = other.get_value(i, j);
            if (!asum.has_value() || !bsum.has_value()) {
                throw std::runtime_error("SparseMatrix::get_value returned bad optional");
            }
            ret.set_value(i, j, asum.value() + bsum.value());
        }
    }

    return ret;
}


SparseMatrix SparseMatrix::operator*(const SparseMatrix& other) const {
    if (other.x != y || other.y != x) {
        throw std::range_error("SparseMatrix::operator* called on matrices with non-inverted dimensions");
    }

    const std::int32_t dim = std::max(x, y);
    SparseMatrix ret(dim, dim);

    for (std::int32_t j = 0; j < dim; j++) {
        for (std::int32_t i = 0; i < dim; i++) {
            std::int32_t dotsum = 0;
            for (std::int32_t l = 0; l < x; l++) {
                std::optional<std::int32_t> asum = get_value(l, j), bsum = other.get_value(i, l);
                if (!asum.has_value() || !bsum.has_value()) {
                    throw std::runtime_error("SparseMatrix::get_value returned bad optional");
                }
                dotsum += asum.value() * bsum.value();
            }
            ret.set_value(i, j, dotsum);
        }
    }

    return ret;
}

SparseMatrix SparseMatrix::transpose() const {
    SparseMatrix ret(y, x);
    
    for (std::int32_t i = 0; i < x; i++) {
        for (std::int32_t j = 0; j < y; j++) {
            std::optional<std::int32_t> value = get_value(j, i);
            if (!value.has_value()) {
                throw std::runtime_error("SparseMatrix::get_value returned bad optional");
            }
            ret.set_value(i, j, value.value());
        }
    }
    return ret;
}
