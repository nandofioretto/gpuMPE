//
// Created by Ferdinando Fioretto on 11/2/15.
//

#ifndef MISC_UTILS_LMATRIX_HPP
#define MISC_UTILS_LMATRIX_HPP

#include <cmath>
#include <vector>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>

using namespace boost::numeric::ublas;

namespace misc_utils
{
    // Linear Algebra
    namespace lmatrix
    {

        /* Matrix inversion routine.
        Uses lu_factorize and lu_substitute in uBLAS to invert a matrix */
        template<class T>
        bool InvertMatrix(const matrix<T>& input, matrix<T>& inverse)
        {
            typedef permutation_matrix<std::size_t> pmatrix;

            // create a working copy of the input
            matrix<T> A(input);

            // create a permutation matrix for the LU-factorization
            pmatrix pm(A.size1());

            // perform LU-factorization
            int res = lu_factorize(A, pm);
            if (res != 0)
                return false;

            // create identity matrix of "inverse"
            inverse.assign(identity_matrix<T> (A.size1()));

            // backsubstitute to get the inverse
            lu_substitute(A, pm, inverse);

            return true;
        }

        template<class T>
        matrix<T> to_matrix(const std::vector<std::vector<T>>& other) {
            matrix<T> ret(other.size(), other[0].size());
            for (int i=0; i<other.size(); i++) {
                for (int j=0; j<other[i].size(); j++) {
                    ret.insert_element(i, j, other[i][j]);
                }
            }
            return ret;
        }

        template<class T>
        std::vector<std::vector<T>> to_matrix(matrix<T>& other) {
            std::vector<std::vector<T>> ret(other.size1());
            for (int i = 0; i < other.size1(); i++)
                ret[i].resize(other.size2());

            for (int i = 0; i < other.size1(); i++) {
                for (int j = 0; j < other.size2(); j++) {
                    ret[i][j] = other.at_element(i, j);
                }
            }
            return ret;
        }

        template <class T>
        std::vector<std::vector<T>> prod(const std::vector<std::vector<T>>& A, const std::vector<std::vector<T>>& B) {
            matrix<T> _A = to_matrix(A);
            matrix<T> _B = to_matrix(B);
            matrix<T> C = boost::numeric::ublas::prod(_A, _B);
            return to_matrix(C);
        }

        template <class T>
        std::vector<std::vector<T>> invert(const std::vector<std::vector<T>>& A) {
            matrix<T> _A = to_matrix(A);
            int order = A.size();
            matrix<T> _Z(order, order);
            InvertMatrix(_A, _Z);
           return to_matrix(_Z);
        }

        template <class T>
        std::vector<std::vector<T>> transpose(const std::vector<std::vector<T>>& A) {
            matrix<T> _A = to_matrix(A);
            matrix<T> _At = boost::numeric::ublas::trans(_A);
            return to_matrix(_At);
        }


        //template <class T>
        inline std::vector<std::vector<double>> alloc(int orderI, int orderJ) {
            std::vector<std::vector<double>> Z(orderI);
            for (int i = 0; i < orderI; i++)
                Z[i].resize(orderJ, 0.0);
            return Z;
        }

        template <class T>
        std::string to_string(std::vector<std::vector<T>> matrix) {
            std::string ret;
            for (int i=0; i<matrix.size(); i++) {
                for (int j=0; j<matrix[i].size(); j++) {
                    ret += std::to_string(matrix[i][j]) + " ";
                }
                ret += "\n";
            }
            return ret;
        }
    }
}

#endif
