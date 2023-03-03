#include <fstream>
#include <iostream>
#include <sstream>
#include <random>
#include <string>

#include "sparse.h"


void output_matrix(const SparseMatrix& matrix, std::ostream& outstream = std::cout) {
    for (std::int32_t j = 0; j < matrix.y; j++) {
        for (std::int32_t i = 0; i < matrix.x; i++) {
            std::optional<std::int32_t> value = matrix.get_value(i, j);
            if (!value.has_value()) {
                throw std::runtime_error("SparseMatrix::get_value returned bad optional");
            }
            outstream << value.value();
            if (matrix.x > 1 && i < matrix.x - 1) {
                outstream << ", ";
            }
        }
        if (matrix.y > 1 && j < matrix.y - 1) {
            outstream << '\n';
        }
    }
}

void random_fill_matrix(SparseMatrix& matrix, std::int32_t min, std::int32_t max) {
    static std::random_device device{};
    static std::default_random_engine engine(device());

    std::uniform_int_distribution<std::int32_t> dist(min, max);
    for (std::int32_t i = 0; i < matrix.x; i++) {
        for (std::int32_t j = 0; j < matrix.y; j++) {
            matrix.set_value(i, j, dist(engine));
        }
    }
}


int main() {
    std::ifstream infile("input1.txt");
    std::ofstream outfile("output1.txt");
    std::stringstream outss;

    try {
        char op = '\0';
        infile >> op;

        if (op == 'T') { /* only has one matrix */
            std::int32_t dimx = 0, dimy = 0;

            infile >> dimy;
            infile >> dimx;
            SparseMatrix sm(dimx, dimy);

            while (infile) {
                std::int32_t x = 0, y = 0, value = 0;
                infile >> y;
                infile >> x;
                infile >> value;
                if (infile.peek() == -1) {
                    break;
                }
                sm.set_value(x, y, value);
                infile.get();
            }
            output_matrix(sm);

            output_matrix(sm.transpose(), outss);

        } else {
            std::int32_t dimx = 0, dimy = 0;

            infile >> dimy;
            infile >> dimx;
            SparseMatrix sm(dimx, dimy);

            unsigned char last = '\0';
            while (infile.peek() != '\n' || last != '\n') {
                std::int32_t x = 0, y = 0, value = 0;
                infile >> y;
                infile >> x;
                infile >> value;
                sm.set_value(x, y, value);
                last = infile.get();
            }

            infile >> dimy;
            infile >> dimx;
            SparseMatrix am(dimx, dimy);

            while (infile) {
                std::int32_t x = 0, y = 0, value = 0;
                infile >> y;
                infile >> x;
                infile >> value;
                if (infile.peek() == -1) {
                    break;
                }
                am.set_value(x, y, value);
                infile.get();
            }
            output_matrix(sm);
            std::cout << "\n------\n";
            output_matrix(am);

            if (op == '+') {
                output_matrix(sm + am, outss);
            } else {
                output_matrix(sm * am, outss);
            }
        }

    } catch (std::exception& e) {
        outfile << "Invalid Matrix Operation";
        outfile.close();
        return 1;
    }

    outfile << outss.view();

    outfile.close();
    infile.close();

    return 0;
}
