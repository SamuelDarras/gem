#pragma once

#include <cstdarg>
#include <functional>
#include <numeric>
#include <math.h>
#include <assert.h>

template<unsigned int DIM=3, typename ContainedType=float>
struct vec {
    vec() {
        for (int i = DIM; i--; inner_data[i] = static_cast<ContainedType>(0.0));
    };

    template<class... Ts>
    vec(Ts ...args) : inner_data{args...} {
        static_assert(sizeof...(args) == DIM);
    };

    ContainedType norm() {
        return sqrt(
            std::accumulate(inner_data.begin(), inner_data.end(), static_cast<ContainedType>(0.0), [](ContainedType acc, ContainedType& value) {
                return acc + value*value;
            })
        );
    }

    vec normalize() {
        return (*this)/norm();
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
    mat() {
        for (unsigned int i = 0; i < ROW; i++) {
            inner_data[i] = vec<COL, ContainedType>();
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

    mat<COL, ROW> T() {
        mat<COL, ROW> r;
        return r.map([&that = this](ContainedType, unsigned int i, unsigned int j) {
            return (*that)(j, i);
        });
    }
    
    // TODO: https://www.cuemath.com/algebra/inverse-of-a-matrix/ 
    mat invert() {
        mat m;

        return m;
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
        static_assert(factor != static_cast<ContainedType>(0.0));
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



template<unsigned int DIM, typename ContainedType>
std::ostream& operator<<(std::ostream &outs, vec<DIM, ContainedType> v) {
    std::string s = "";
    outs << "Dim : " << std::to_string(DIM) << " = ";
    
    for (unsigned int i = 0; i < DIM; i++) outs << std::to_string(v[i]) << ", ";

    return outs;
}

template<unsigned int ROW, unsigned int COL, typename ContainedType>
std::ostream& operator<<(std::ostream &outs, mat<ROW, COL, ContainedType> m) {
    std::string s = "";
    outs << "Dim : " << std::to_string(ROW) << "x" << std::to_string(COL) << " = [\n\t";
    
    for (unsigned int i = 0; i < ROW; i++) {
        for (unsigned int j = 0; j < COL; j++) {
            outs << std::to_string(m[i][j]) << ", ";
        }
        outs << "\n\t";
    }
    outs << "]";

    return outs;
}
