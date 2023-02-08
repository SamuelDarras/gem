#pragma once

#include <cstdarg>
#include <functional>
#include <numeric>
#include <iostream>
#include <math.h>
#include <assert.h>
#include <type_traits>

namespace gem {
    template<unsigned int DIM=3, typename ContainedType=float>
    struct vec {
        static const unsigned int n_dim = DIM;

        vec() {
            for (int i = DIM; i--; inner_data[i] = static_cast<ContainedType>(0.0));
        };

        template<class... Ts>
        vec(Ts ...args) : inner_data{args...} {
            static_assert(sizeof...(args) == DIM, "Invalid number of arguments");
        };

        ContainedType norm() {
            return sqrt(
                std::accumulate(inner_data.begin(), inner_data.end(), static_cast<ContainedType>(0.0), [](ContainedType acc, ContainedType& value) {
                    return acc + value*value;
                })
            );
        }

        vec normalize() {
            // std::cout << norm() << "\n";
            return (*this)/norm();
        }

        unsigned int arg_max() {
            ContainedType m = static_cast<ContainedType>(0.0);
            unsigned int m_idx = 0;
            for (unsigned int i = 0; i < DIM; i++) {
                if ((*this)(i) > m) {
                    m = (*this)(i);
                    m_idx = i;
                }
            }

            return m_idx;
        }

        ContainedType& operator[](unsigned int idx) {
            assert(idx < DIM);
            return inner_data[idx];
        }

        vec operator+(vec<>& rhs) {
            vec r;
            for (unsigned int i = DIM; i--; r[i] = inner_data[i] + rhs[i]);
            return r;
        }

        vec operator-(vec& rhs) {
            vec r;
            for (unsigned int i = DIM; i--; r[i] = inner_data[i] - rhs[i]);
            return r;
        }

        vec operator^(vec& rhs) {
            vec r;
            for (unsigned int i = DIM; i--; r[i] = inner_data[(i+1)%DIM] * rhs[(i+2)%DIM] - rhs[(i+1)%DIM] * inner_data[(i+2)%DIM]);
            return r;
        }

        ContainedType operator*(vec& rhs) {
            ContainedType sum = static_cast<ContainedType>(0);
            for (unsigned int i = DIM; i--; sum += inner_data[i] * rhs[i]);
            return sum;
        }

        ContainedType& operator()(unsigned int idx) {
            assert(idx < DIM);
            return inner_data[idx];
        }
        
        vec operator+(ContainedType rhs) {
            vec r;
            for (unsigned int i = DIM; i--; r(i) = (*this)(i) + rhs);
            return r;
        }

        vec operator-(ContainedType rhs) {
            vec r;
            for (unsigned int i = DIM; i--; r(i) = (*this)(i) - rhs);
            return r;
        }

        vec operator*(ContainedType rhs) {
            vec r;
            for (unsigned int i = DIM; i--; r(i) = (*this)(i) * rhs);
            return r;
        }

        vec operator/(ContainedType rhs) {
            assert(rhs != static_cast<ContainedType>(0.0));
            vec r;
            for (unsigned int i = DIM; i--; r(i) = (*this)(i) / rhs);
            return r;
        }

        template<unsigned int D>
        vec<D, ContainedType> proj(ContainedType padding) {
            vec<D, ContainedType> r;
            for (unsigned int i = 0; i < D; i++) r[i] = (i < DIM ? inner_data[i] : padding);
            return r;
        }
        
        template<unsigned int D>
        vec<D, ContainedType> proj() {
            return proj<D>(static_cast<ContainedType>(0));
        }

    private:
        std::array<ContainedType, DIM> inner_data;
    };

    template<unsigned int ROW, unsigned int COL, typename ContainedType=float>
    struct mat {
        static const unsigned int n_row  = ROW;
        static const unsigned int n_cols = COL;

        mat() {
            for (unsigned int i = 0; i < ROW; i++) {
                inner_data[i] = vec<COL, ContainedType>();
            }
        };

        template<unsigned int ROW2, unsigned int COL2>
        mat(mat<ROW2, COL2>& m) {
            static_assert(ROW2 <= ROW && COL2 <= COL, "Old mat must be same size or smaller");
            for (unsigned int i = 0; i < ROW; i++) {
                for (unsigned int j = 0; j < COL; j++) {
                    inner_data[i][j] = i < ROW2 && j < COL2 ? ContainedType{m(i, j)} : static_cast<ContainedType>(0.0);
                }
            }
        };

        const static mat identity() {
            return mat().map([](ContainedType, unsigned int i, unsigned int j) {
                return i == j ? static_cast<ContainedType>(1.0) : static_cast<ContainedType>(0.0);
            });
        }

        const static mat ones() {
            return mat().map([](ContainedType, unsigned int, unsigned int) {
                return static_cast<ContainedType>(1.0);
            });
        }

        vec<COL, ContainedType>& operator[](unsigned int idx) {
            return inner_data[idx];
        }

        ContainedType& operator()(unsigned int r, unsigned int c) {
            return inner_data[r][c];
        }
        vec<COL, ContainedType>& operator()(unsigned int r) {
            return inner_data[r];
        }

        inline vec<ROW> col(unsigned int idx) {
            vec<ROW> r;
            for (unsigned int i = ROW; i--; r(i) = (*this)(i, idx));
            return r;
        }
        inline vec<COL> row(unsigned int idx) {
            vec<COL> r;
            for (unsigned int i = COL; i--; r(i) = (*this)(idx, i));
            return r;
        }

        inline void setCol(unsigned int idx, vec<ROW, ContainedType> r) {
            for (unsigned int i = ROW; i--; (*this)(i, idx) = r(i));
        }
        inline void setRow(unsigned int idx, vec<ROW, ContainedType> r) {
            for (unsigned int i = COL; i--; (*this)(idx, i) = r(i));
        }

        mat<COL, ROW> T() {
            mat<COL, ROW> r;
            return r.map([&that = this](ContainedType, unsigned int i, unsigned int j) {
                return (*that)(j, i);
            });
        }

        mat<ROW, COL+1> appendCol(vec<ROW, ContainedType> v, int idxAfter) {
            mat<ROW, COL+1, ContainedType> r(*this);
            r.setCol((idxAfter+COL+1)%(COL+1), v);
            return r;
        }
        
        // TODO: https://www.cuemath.com/algebra/inverse-of-a-matrix/ 
        mat invert() {
            assert(false && "Not implemented yet!");
        }

        vec<ROW, ContainedType> solve(vec<ROW, ContainedType> b) {
            auto A = (*this).appendCol(b, -1);
            unsigned int r = 0;
            #pragma omp parallel for
            for (unsigned int j = 0; j < COL; j++) {
                unsigned int k = A.col(j).arg_max();

                if (A(k, j) == static_cast<ContainedType>(0.0)) continue;
                A(k) = A(k)/A(k, j);

                if (k != r) {
                    std::swap(A(k), A(r));
                }

                for (unsigned int i = 0; i < ROW; i++) {
                    if (i == r) continue;
                    auto t = A(r)*A(i, j);
                    A(i) = A(i)-t;
                }
                r++;
            }
            return A.col(COL);
        }

        mat operator+(mat rhs) {
            return map([&rhs](ContainedType v, unsigned int i, unsigned int j) {
                return v + rhs(i, j);
            });
        }

        mat<ROW, COL, ContainedType> operator-(mat rhs) {
            return map([&rhs](ContainedType v, unsigned int i, unsigned int j) {
                return v - rhs(i, j);
            });
        }

        template<unsigned int COL2>
        mat<ROW, COL2> operator*(mat<COL, COL2> rhs) {
            mat<ROW, COL2, ContainedType> r;
            return r.map([&](ContainedType, unsigned int r, unsigned int c) {
                auto left  = *this;
                auto right = rhs;
                ContainedType sum = static_cast<ContainedType>(0.0);
                for (unsigned int i = 0; i < COL; i++) {
                    sum += left(r, i)*right(i, c);
                }
                return sum;
            });
        }

        vec<ROW, ContainedType> operator*(vec<COL> rhs) {
            vec<ROW, ContainedType> r;
            for (unsigned int i = 0; i < ROW; i++) {
                ContainedType sum = static_cast<ContainedType>(0.0);
                for (unsigned int j = 0; j < COL; j++)
                {
                    sum += (*this)(i, j) * rhs(j);
                }
                r(i) = sum;
            }
            return r;
        }

        mat operator+(ContainedType factor) {
            return map([&factor](ContainedType value, unsigned int, unsigned int) {
                return value + factor;
            });
        }

        mat operator-(ContainedType factor) {
            return map([&factor](ContainedType value, unsigned int, unsigned int) {
                return value - factor;
            });
        }

        mat operator*(ContainedType factor) {
            return map([&factor](ContainedType value, unsigned int, unsigned int) {
                return value * factor;
            });
        }

        mat operator/(ContainedType factor) {
            static_assert(factor != static_cast<ContainedType>(0.0), "Division by 0");
            return map([&factor](ContainedType value, unsigned int, unsigned int) {
                return value / factor;
            });
        }

        inline mat map(std::function<ContainedType(ContainedType, unsigned int, unsigned int)> f) {
            mat r;
            for (unsigned int i = 0; i < ROW; i++) {
                for (unsigned int j = 0; j < COL; j++) {
                    r(i, j) = f((*this)(i, j), i, j);
                }
            }
            return r;
        }

    private:
        std::array<vec<COL, ContainedType>, ROW> inner_data;
    };
}


template<unsigned int DIM, typename ContainedType>
std::ostream& operator<<(std::ostream &outs, gem::vec<DIM, ContainedType> v) {
    std::string s = "";
    outs << "Dim : " << std::to_string(DIM) << " = [\n";
    for (unsigned int i = 0; i < DIM; i++) {
        outs << "\t" << std::to_string(v(i)) << ",\n";
    }
    outs << "]";
    return outs;
}

template<unsigned int ROW, unsigned int COL, typename ContainedType>
std::ostream& operator<<(std::ostream &outs, gem::mat<ROW, COL, ContainedType> m) {
    std::string s = "";
    outs << "Dim : " << std::to_string(ROW) << "x" << std::to_string(COL) << " = [\n\t";
    
    for (unsigned int i = 0; i < ROW; i++) {
        for (unsigned int j = 0; j < COL; j++) {
            outs << std::to_string(m(i, j)) << ", ";
        }
        outs << "\n\t";
    }
    outs << "]";

    return outs;
}
