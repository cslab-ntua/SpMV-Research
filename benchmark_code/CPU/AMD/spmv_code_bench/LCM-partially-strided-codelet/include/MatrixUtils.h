//
// Created by lwilkinson on 11/2/21.
//

#ifndef DDT_MATRIXUTILS_H
#define DDT_MATRIXUTILS_H

namespace DDT {
typedef std::vector<std::tuple<int,int,double>> RawMatrix;

class Matrix {
public:
    Matrix() = default;

    Matrix(int r, int c, int nz) : r(r), c(c), nz(nz), Lp(nullptr), Li(nullptr), Lx(nullptr) {}

    ~Matrix() {
      delete[] this->Lp;
      delete[] this->Li;
      delete[] this->Lx;
    };

    // Assignment operator
    Matrix &operator=(Matrix &&lhs) noexcept {
      this->nz = lhs.nz;
      this->r = lhs.r;
      this->c = lhs.c;

      this->Lp = lhs.Lp;
      this->Li = lhs.Li;
      this->Lx = lhs.Lx;

      lhs.Lp = nullptr;
      lhs.Li = nullptr;
      lhs.Lx = nullptr;

      return *this;
    }

    // Assignment operator
    Matrix &operator=(const Matrix &lhs) {
      if (this == &lhs) {
        return *this;
      }
      this->nz = lhs.nz;
      this->r = lhs.r;
      this->c = lhs.c;

      this->Lp = new int[lhs.r + 1]();
      this->Li = new int[lhs.nz]();
      this->Lx = new double[lhs.nz]();

      std::memcpy(this->Lp, lhs.Lp, sizeof(int) * lhs.r + 1);
      std::memcpy(this->Li, lhs.Li, sizeof(int) * lhs.nz);
      std::memcpy(this->Lx, lhs.Lx, sizeof(double) * lhs.nz);

      return *this;
    }

    // Copy Constructor
    Matrix(const Matrix &lhs) {
      this->r = lhs.r;
      this->c = lhs.c;
      this->nz = lhs.nz;

      this->Lp = new int[lhs.r + 1]();
      this->Li = new int[lhs.nz]();
      this->Lx = new double[lhs.nz]();

      std::memcpy(this->Lp, lhs.Lp, sizeof(int) * lhs.r + 1);
      std::memcpy(this->Li, lhs.Li, sizeof(int) * lhs.nz);
      std::memcpy(this->Lx, lhs.Lx, sizeof(double) * lhs.nz);
    }

    // Move Constructor
    Matrix(Matrix &&lhs) noexcept {
      this->r = lhs.r;
      this->c = lhs.c;
      this->nz = lhs.nz;

      this->Lp = lhs.Lp;
      this->Lx = lhs.Lx;
      this->Li = lhs.Li;

      lhs.Lp = nullptr;
      lhs.Li = nullptr;
      lhs.Lx = nullptr;
    }

    int r;
    int c;
    int nz;
    int stype;

    int *Lp;
    int *Li;
    double *Lx;

    void print() {
      for (int i = 0; i < this->r; ++i) {
        for (int j = this->Lp[i]; j < this->Lp[i + 1]; ++j) {
          std::cout << i << "," << this->Li[j] << std::endl;
        }
      }
    }
};

class CSR : public Matrix {
public:
    ~CSR() {
      delete[] this->Lp;
      delete[] this->Li;
      delete[] this->Lx;
    }

    CSR(int r, int c, int nz) : Matrix(r, c, nz) {
      this->Lp = new int[r + 1]();
      this->Li = new int[nz]();
      this->Lx = new double[nz]();
    }

    CSR(int r, int c, int nz, RawMatrix m) : Matrix(r, c, nz) {
      this->Lp = new int[r + 1]();
      this->Li = new int[nz]();
      this->Lx = new double[nz]();

      std::sort(m.begin(), m.end(), [](std::tuple<int, int, double> lhs, std::tuple<int, int, double> rhs) {
          bool c0 = std::get<0>(lhs) == std::get<0>(rhs);
          bool c1 = std::get<0>(lhs) < std::get<0>(rhs);
          bool c2 = std::get<1>(lhs) < std::get<1>(rhs);
          return c0 ? c2 : c1;
      });

      // Parse CSR Matrix
      for (int i = 0, LpCnt = 0; i < nz; i++) {
        auto &v = m[i];

        int ov = std::get<0>(v);
        double im = std::get<2>(v);
        int iv = std::get<1>(v);

        this->Li[i] = iv;
        this->Lx[i] = im;

        if (i == 0) {
          this->Lp[LpCnt] = i;
        }
        if (i != 0 && std::get<0>(m[i - 1]) != ov) {
          while (LpCnt != ov) {
            this->Lp[++LpCnt] = i;
          }
        }
        if (nz - 1 == i) {
          this->Lp[++LpCnt] = i + 1;
        }
      }
    }

    // Copy constructor
    CSR(const CSR &lhs) : Matrix(lhs.r, lhs.c, lhs.nz) {
      this->Lp = new int[lhs.r + 1]();
      this->Li = new int[lhs.nz]();
      this->Lx = new double[lhs.nz]();

      std::memcpy(this->Lp, lhs.Lp, sizeof(int) * lhs.r + 1);
      std::memcpy(this->Li, lhs.Li, sizeof(int) * lhs.nz);
      std::memcpy(this->Lx, lhs.Lx, sizeof(double) * lhs.nz);
    }

    // Assignment operator
    CSR &operator=(const CSR &lhs) {
      this->nz = lhs.nz;
      this->r = lhs.r;
      this->c = lhs.c;

      this->Lp = new int[lhs.r + 1]();
      this->Li = new int[lhs.nz]();
      this->Lx = new double[lhs.nz]();

      std::memcpy(this->Lp, lhs.Lp, sizeof(int) * lhs.r + 1);
      std::memcpy(this->Li, lhs.Li, sizeof(int) * lhs.nz);
      std::memcpy(this->Lx, lhs.Lx, sizeof(double) * lhs.nz);

      return *this;
    }

    // Move Constructor
    CSR(CSR &&lhs) noexcept: Matrix(lhs.r, lhs.c, lhs.nz) {
      this->Lp = lhs.Lp;
      this->Lx = lhs.Lx;
      this->Li = lhs.Li;

      lhs.Lp = nullptr;
      lhs.Li = nullptr;
      lhs.Lx = nullptr;
    }

};

class CSC : public Matrix {
public:
    ~CSC() {
      delete[] this->Lp;
      delete[] this->Li;
      delete[] this->Lx;
    }

    CSC(int r, int c, int nz) : Matrix(r, c, nz) {
      this->Lp = new int[c + 1]();
      this->Li = new int[nz]();
      this->Lx = new double[nz]();
    }

    CSC(int r, int c, int nz, RawMatrix m) : Matrix(r, c, nz) {
      this->Lp = new int[c + 1]();
      this->Li = new int[nz]();
      this->Lx = new double[nz]();

      std::sort(m.begin(), m.end(), [](std::tuple<int, int, double> lhs, std::tuple<int, int, double> rhs) {
          bool c0 = std::get<1>(lhs) == std::get<1>(rhs);
          bool c1 = std::get<0>(lhs) < std::get<0>(rhs);
          bool c2 = std::get<1>(lhs) < std::get<1>(rhs);
          return c0 ? c1 : c2;
      });

      // Parse CSR Matrix
      for (int i = 0, LpCnt = 0; i < nz; i++) {
        auto &v = m[i];

        int ov = std::get<0>(v);
        double im = std::get<2>(v);
        int iv = std::get<1>(v);

        this->Li[i] = ov;
        this->Lx[i] = im;

        if (i == 0) {
          this->Lp[LpCnt] = i;
        }
        if (i != 0 && std::get<1>(m[i - 1]) != iv) {
          while (LpCnt != iv) {
            this->Lp[++LpCnt] = i;
          }
        }
        if (nz - 1 == i) {
          this->Lp[++LpCnt] = i + 1;
        }
      }
    }

    void make_full() {
      auto lpc = new int[this->c + 1]();
      auto lxc = new double[this->nz * 2 - this->c]();
      auto lic = new int[this->nz * 2 - this->c]();

      auto ind = new int[this->c]();

      for (size_t i = 0; i < this->c; i++) {
        for (size_t p = this->Lp[i]; p < this->Lp[i + 1]; p++) {
          int row = this->Li[p];
          ind[i]++;
          if (row != i)
            ind[row]++;
        }
      }
      lpc[0] = 0;
      for (size_t i = 0; i < this->c; i++)
        lpc[i + 1] = lpc[i] + ind[i];

      for (size_t i = 0; i < this->c; i++)
        ind[i] = 0;
      for (size_t i = 0; i < this->c; i++) {
        for (size_t p = this->Lp[i]; p < this->Lp[i + 1]; p++) {
          int row = this->Li[p];
          int index = lpc[i] + ind[i];
          lic[index] = row;
          lxc[index] = this->Lx[p];
          ind[i]++;
          if (row != i) {
            index = lpc[row] + ind[row];
            lic[index] = i;
            lxc[index] = this->Lx[p];
            ind[row]++;
          }
        }
      }
      delete[]ind;
      delete Lp;
      delete Li;
      delete Lx;

      this->nz = this->nz * 2 - this->c;
      this->Lx = lxc;
      this->Li = lic;
      this->Lp = lpc;
    }

    // Assignment copy operator
    CSC &operator=(CSC &&lhs) noexcept {
      this->Lp = lhs.Lp;
      this->Lx = lhs.Lx;
      this->Li = lhs.Li;

      lhs.Lp = nullptr;
      lhs.Li = nullptr;
      lhs.Lx = nullptr;

      return *this;
    }

    // Assignment operator
    CSC &operator=(const CSC &lhs) {
      if (&lhs == this) {
        return *this;
      }
      this->r = lhs.r;
      this->c = lhs.c;
      this->nz = lhs.nz;

      this->Lp = new int[lhs.r + 1]();
      this->Li = new int[lhs.nz]();
      this->Lx = new double[lhs.nz]();

      std::copy(lhs.Lp, lhs.Lp + lhs.r + 1, this->Lp);
      std::copy(lhs.Lx, lhs.Lx + lhs.nz, this->Lx);
      std::copy(lhs.Li, lhs.Li + lhs.nz, this->Li);

      return *this;
    }

    // Copy Constructor
    CSC(CSC &lhs) : Matrix(lhs.r, lhs.c, lhs.nz) {
      this->Lp = new int[lhs.r + 1]();
      this->Li = new int[lhs.nz]();
      this->Lx = new double[lhs.nz]();

      std::copy(lhs.Lp, lhs.Lp + lhs.r + 1, this->Lp);
      std::copy(lhs.Lx, lhs.Lx + lhs.nz, this->Lx);
      std::copy(lhs.Li, lhs.Li + lhs.nz, this->Li);
    }

    // Move Constructor
    CSC(CSC &&lhs) noexcept: Matrix(lhs.r, lhs.c, lhs.nz) {
      this->Lp = lhs.Lp;
      this->Lx = lhs.Lx;
      this->Li = lhs.Li;

      lhs.Lp = nullptr;
      lhs.Li = nullptr;
      lhs.Lx = nullptr;
    }
};
} // namespace DDT

#endif //DDT_MATRIXUTILS_H
