#ifndef MISC_UTILS_UTILS_HPP
#define MISC_UTILS_UTILS_HPP

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdarg>
#include <functional>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


namespace misc_utils {

  namespace utils {

    template <class T> struct norm {
      T operator() (const T& x, const T& y) const {return std::abs(x-y);}
      typedef T first_argument_type;
      typedef T second_argument_type;
      typedef T result_type;
    };
    
    /**
     * Given vectors A = <a1...an>, and B = <b1...bn>, produces a vector C = <c1 ... cn>,
     * where ci = ai + bi.
     */
    template <typename T>
    std::vector<T> operator+(const std::vector<T>& _A, const std::vector<T>& _B) {
      // assert(_A.size() == _B.size());
      std::vector<T> result;
      result.reserve(_A.size());
      std::transform(_A.begin(), _A.end(), _B.begin(), std::back_inserter(result), std::plus<T>());
      return result;
    }
    
    /**
     * Given vectors A = <a1...an>, and B = <b1...bn>, produces a vector C = <c1 ... cn>,
     * where ci = ai + bi.
     */
    template <typename T>
    std::vector<T> operator-(const std::vector<T>& _A, const std::vector<T>& _B) {
      // assert(_A.size() == _B.size());
      std::vector<T> result;
      result.reserve(_A.size());
      std::transform(_A.begin(), _A.end(), _B.begin(), std::back_inserter(result), std::minus<T>());
      return result;
    }
    
    /**
     * Given vectors A = <a1...an>, and B = <b1...bn>, produces a vector C = <c1 ... cn>,
     * where ci = ai * bi.
     */
    template <typename T>
    std::vector<T> operator*(const std::vector<T>& _A, const std::vector<T>& _B) {
      // assert(_A.size() == _B.size());
      std::vector<T> result;
      result.reserve(_A.size());
      std::transform(_A.begin(), _A.end(), _B.begin(), std::back_inserter(result), std::multiplies<T>());
      return result;
    }
    
    /**
     * Given vectors A = <a1...an>, and B = <b1...bm>, produces a vector C = <c1 ... cn>,
     * where ci = ai / b{i%|B|}.
     */
    template <typename T>
    std::vector<T> operator/(const std::vector<T>& _A, const std::vector<T>& _B) {
      std::vector<T> result(_A);
      auto out = result.begin(); auto itb = _B.begin();
      while (out != result.end()) {
	*out /= *itb;
	++out; ++itb;
	if (itb == _B.end())
	  itb = _B.begin();
      }
      return result;
    }
    
    template <class T>
    std::vector<T> abs(const std::vector<T>& _A) {
      std::vector<T> result(_A);
      for (T& a : result) {
	a = std::abs(a);
      }
      return result;
    }
    
    /**
     * Given vectors A = <a1...an>, and a scalar labmda, produces a vector C = <c1 ... cn>,
     * where ci = ai - lambda.
     */
    template <typename T>
    std::vector<T> operator-(const std::vector<T>& _A, T lambda) {
      std::vector<T> result;
      result.reserve(_A.size());
      std::transform(_A.begin(), _A.end(), std::back_inserter(result), std::bind2nd(std::minus<T>(), lambda));
      return result;
    }
    
    /**
     * Given vectors A = <a1...an>, and a scalar labmda, produces a vector C = <c1 ... cn>,
     * * where ci = ai * lambda.
     * */
    template <typename T>
    std::vector<T> operator*(const std::vector<T>& _A, T lambda) {
      std::vector<T> result;
      result.reserve(_A.size());
      std::transform(_A.begin(), _A.end(), std::back_inserter(result), std::bind2nd(std::multiplies<T>(), lambda));
      return result;
    }
    
    /**
     * Given vectors A = <a1...an>, and a scalar labmda, produces a vector C = <c1 ... cn>,
     * * where ci = ai / lambda.
     * */
    template <typename T>
    std::vector<T> operator/(const std::vector<T>& _A, T lambda) {
      std::vector<T> result;
      result.reserve(_A.size());
      std::transform(_A.begin(), _A.end(), std::back_inserter(result), std::bind2nd(std::divides<T>(), lambda));
      return result;
    }
    
    /**
     * Given vectors A = <a1...an>, and a scalar labmda, produces a vector C = <c1 ... cn>,
     * where ci = ai + lambda.
     */
    template <typename T>
    std::vector<T> operator+(const std::vector<T>& _A, T lambda) {
      std::vector<T> result;
      result.reserve(_A.size());
      std::transform(_A.begin(), _A.end(), std::back_inserter(result), std::bind2nd(std::plus<T>(), lambda));
      return result;
    }
    
    // Bit-to-Bit sum of two pairs
    template <typename T, typename U>
    std::pair<T,U> operator+(const std::pair<T, U>& _A, const std::pair<T, U>& _B) {
      return std::make_pair<T, U>(_A.first + _B.first, _A.second + _B.second);
    }
    
    // Bit-to-Bit sum of two pairs
    template <typename T, typename U>
    std::pair<T,U> operator-(const std::pair<T, U>& _A, const std::pair<T, U>& _B) {
      return std::make_pair<T, U>(_A.first - _B.first, _A.second - _B.second);
    }
    
    /**
     * Checks if, for each element of the vector A is less then or equal to the corresponding
     * element of the vector B [Ordered Comparison].
     */
    template <typename T>
    bool leq(const std::vector<T>& A, const std::vector<T>& B) {
      int min_size = A.size() < B.size() ? A.size() : B.size();
      for (int i=0; i<min_size; i++) {
	if(A[i] > B[i]) return false;
      }
      return true;
    }
    
    template <typename T>
    bool operator<=(const std::vector<T>& A, const std::vector<T>& B) {
      return leq(A, B);
    }
    
    template <typename InputIt1, typename InputIt2>
    bool leq(InputIt1 first1, InputIt1 last1, InputIt2 first2) {
      while (first1!=last1) {
	if (*first1 > *first2) return false;
	
	++first1; ++first2;
      }
      return true;
    }
    
    /**
     * Checks if, for each element of the sub-vectors in A (of same size as B)
     * is less then or equal to the corresponding element of the vector B [Ordered Comparison].
     * Assumes B is smaller than A;
     */
    template <typename InputIt1, typename InputIt2>
    bool rep_leq(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2) {
      
      InputIt2 first2cpy = first2;
      while (first1!=last1) {
	if (*first1 > *first2) return false;
	
	++first1; ++first2;
	if (first2 == last2)
	  first2 = first2cpy;
      }
      return true;
    }


    template <typename InputIterator, typename T, typename UnaryPred>
    T sum_if(InputIterator first, InputIterator last, UnaryPred condition) {
      T sum = 0;
      while (first!=last) {
	if (condition(*first)) sum += (*first);
	++first;
      }
      return sum;
    }
    
    template <typename T>
    T sum_if_geq(std::vector<T> _A, T m) {
      //return sum_if(_A.begin(), _A.end(), std::bind1st(std::greater_equal<T>(), m));
      T sum = 0;
      for (T a : _A) {
	if (a >= m) sum += a;
      }
      return sum;
    }
    
    template <typename InputIterator, typename T>
    T sum_if_geq(InputIterator first, InputIterator last, T m) {
      T sum = 0;
      while (first!=last) {
	if (*first >= m) sum += (*first);
	++first;
      }
      return sum;
    }
    
    
    template <typename InputIterator, typename T, typename UnaryPred>
    bool check_if(InputIterator first, InputIterator last, UnaryPred condition) {
      while (first!=last) {
	if (!condition(*first)) return false;
	++first;
      }
      return true;
    }
    
    template <typename InputIterator, typename T>
    bool check_if_gt(InputIterator first, InputIterator last, T m) {
      return utils::check_if(first, last, std::bind1st(std::greater<T>(), m));
    }
    
    template <class InputIt1, class InputIt2, class BinaryPred, class UnaryPred>
    bool check_cmp_if(InputIt1 first1, InputIt1 last1, InputIt2 first2,
		      BinaryPred binary_op, UnaryPred unary_op) {
      while (first1!=last1) {
	if (!unary_op(binary_op(*first1, *first2))) return false;
	++first1;
	++first2;
      }
      return true;
    }
    
    
    // Merges Two vectors: It returs the union of the sets given as a parameters.
    template<typename T>
    std::vector<T> merge(const std::vector<T> _A, const std::vector<T> _B) {
      std::vector<T> A = _A, B = _B; // copy
      
      std::vector<T> res(A.size() + B.size());
      std::sort(A.begin(), A.end());
      std::sort(B.begin(), B.end());
      typename std::vector<T>::iterator it;
      it = std::set_union(A.begin(), A.end(), B.begin(), B.end(), res.begin());
      res.resize(it - res.begin());
      return res;
    }
    
    // Merges Two vectors: It returs the union of the sets given as a parameters
    // in the first set given as parameter.
    template<typename T>
    void merge_emplace(std::vector<T> &A, const std::vector<T> B) {
      std::vector<T> res = merge(A, B);
      A.swap(res);
    }
    
    // Intersects two vectors: It returs the intersection of the sets given as
    // a parameters.
    template<typename T>
    std::vector<T> intersect(const std::vector<T> _A, const std::vector<T> _B) {
      std::vector<T> A = _A, B = _B; // copy
      
      std::vector<T> res(std::max(A.size(), B.size()));
      std::sort(A.begin(), A.end());
      std::sort(B.begin(), B.end());
      typename std::vector<T>::iterator it;
      it = std::set_intersection(A.begin(), A.end(), B.begin(), B.end(), res.begin());
      res.resize(it - res.begin());
      return res;
    }
    
    // Intersects two vectors: It returs the intersection of the sets given as
    // a parameters.
    // @return The result of the intersection is saved in the vector A.
    template<typename T>
    void intersect_emplace(std::vector<T> &A, const std::vector<T> B) {
      std::vector<T> res = intersect(A, B);
      A.swap(res);
    }
    
    // It excludes the vector B from A:
    // It Returns the set difference of the two sets given as a parameters.
    template<typename T>
    std::vector<T> exclude(std::vector<T> _B, std::vector<T> _A) {
      std::vector<T> A = _A, B = _B; // copy
      
      std::vector<T> res(A.size());
      std::sort(A.begin(), A.end());
      std::sort(B.begin(), B.end());
      typename std::vector<T>::iterator it;
      it = std::set_difference(A.begin(), A.end(), B.begin(), B.end(), res.begin());
      res.resize(it - res.begin());
      return res;
    }
    
    // It excludes the vector B from A:
    // It Returns the set difference of the two sets given as a parameters
    // in second set given as a parameter.
    template<typename T>
    void exclude_emplace(const std::vector<T> B, std::vector<T> &A) {
      std::vector<T> res = exclude(B, A);
      A.swap(res);
    }
    
    // It excludes the vector B from A:
    // It Returns the set difference of the two sets given as a parameters
    // in second set given as a parameter.
    template<typename T>
    void exclude_emplace(const T B, std::vector<T> &A) {
      auto it = std::find(A.begin(), A.end(), B);
      if (it != A.end())
	A.erase(it);
    }
    
    // It returns the concatenation of the vectors A and B.
    template<typename T>
    std::vector<T> concat(T A, T B) {
      std::vector<T> out(2);
      out[0] = A;
      out[1] = B;
      return out;
    }
    
    // It returns the concatenation of the vectors A and B.
    template<typename T>
    std::vector<T> concat(const std::vector<T> A, T B) {
      std::vector<T> out(A.size() + 1);
      int j = 0;
      for (int i = 0; i < A.size(); ++i) out[j++] = A[i];
      out[j] = B;
      return out;
    }
    
    // It returns the concatenation of the vectors A and B.
    template<typename T>
    std::vector<T> concat(T A, const std::vector<T> B) {
      std::vector<T> out(B.size() + 1);
      out[0] = A;
      for (int i = 0; i < B.size(); ++i) out[i + 1] = B[i];
      return out;
    }
    
    // It returns the concatenation of the vectors A and B.
    template<typename T>
    std::vector<T> concat(const std::vector<T> A, const std::vector<T> B) {
      std::vector<T> out(A.size() + B.size());
      int j = 0;
      for (int i = 0; i < A.size(); ++i) out[j++] = A[i];
      for (int i = 0; i < B.size(); ++i) out[j++] = B[i];
      return out;
    }
    
    // It concatenates A and B into 'out'.
    // The vector out should be of size A+B
    template<typename T>
    void concat_emplace(std::vector<T> &out, const std::vector<T> A, const std::vector<T> B) {
      massert(out.size() == (A.size() + B.size()),
	      "Error in concatenating arryas of different sizes " +
	      std::to_string(A.size()) + " and " +
	      std::to_string(B.size()) + " with " +
	      std::to_string(out.size()));
      
      int j = 0;
      for (int i = 0; i < A.size(); ++i) out[j++] = A[i];
      for (int i = 0; i < B.size(); ++i) out[j++] = B[i];
    }
    
    template<typename T>
    std::vector<std::vector<T> > transpose(std::vector<T> vec) {
      std::vector<std::vector<T> > res(vec.size());
      for (int i = 0; i < vec.size(); ++i)
	res[i].push_back(vec[i]);
      return res;
    }
    
    template<typename T>
    void cartesian_product(std::vector<std::vector<T> > &CP, std::vector<T> A) {
      if (CP.empty()) {
	CP = utils::transpose(A);
	return;
      }
      if (A.empty()) { return; }
      
      std::vector<std::vector<T> > res;
      for (std::vector<T> vec : CP) {
	int e = vec.size();
	std::vector<T> new_vec(e + 1);
	std::copy(vec.begin(), vec.end(), new_vec.begin());
	for (T t : A) {
	  new_vec[e] = t;
	  res.push_back(new_vec);
	}
      }
      CP.swap(res);
    }
    
    
    // It returns the description of the array content given as a parameter.
    template<typename T, typename U>
    std::vector<std::pair<T, U> > make_pairs
    (const std::vector<T> array_first, const std::vector<U> array_second) {
      static_assert(array_first.size() == array_second.size(), "Error in making pairs");
      std::vector<std::pair<T, U> > res;
      for (int i = 0; i < array_first.size(); ++i) {
	res.push_back(std::make_pair(array_first[i], array_second[i]));
      }
      return res;
    }
   
    template<typename T>
    std::string numberToString(T Number) {
      std::stringstream ss;
      ss << Number;
      return ss.str();
    }
    
    template<typename T>
    T max(const std::vector<T> array) {
      int idx = 0;
      for (int i = 1; i < array.size(); ++i)
	if (array[i] > array[idx]) idx = i;
      return array[idx];
    }
    
    template<typename T>
    T min(const std::vector<T> array) {
      int idx = 0;
      for (int i = 1; i < array.size(); ++i)
	if (array[i] < array[idx]) idx = i;
      return array[idx];
    }
    
    template<typename T>
    bool find(T elem, const std::vector<T> array) {
      return (std::find(array.begin(), array.end(), elem) != array.end());
    }
    
    template<typename T>
    bool find(T elem, const std::set<T> array) {
      return (array.find(elem) != array.end());
    }
    
    template<typename T>
    int findIdx(const std::vector<T> A, T query) {
      for (int i = 0; i < A.size(); ++i)
	if (A[i] == query)
	  return i;
      return -1;
    }
    
    template<typename T, typename U>
    bool findFirst(const std::vector<std::pair<T, U> > A, T query) {
      for (int i = 0; i < A.size(); ++i)
	if (A[i].first == query)
	  return true;
      return false;
    }
    
    template<typename T, typename U>
    int findIdxFirst(const std::vector<std::pair<T, U> > A, T query) {
      for (int i = 0; i < A.size(); ++i)
	if (A[i].first == query)
	  return i;
      return -1;
    }
    
    template<typename T, typename U>
    bool findSecond(const std::vector<std::pair<T, U> > A, T query) {
      for (int i = 0; i < A.size(); ++i)
	if (A[i].second == query)
	  return true;
	        return false;
    }
    
    template<typename T, typename U>
    int findIdxSecond(const std::vector<std::pair<T, U> > A, T query) {
      for (int i = 0; i < A.size(); ++i)
	if (A[i].second == query)
	  return i;
      return -1;
    }
    
    template<typename T, typename U>
    std::vector<T> extractFirst(const std::vector<std::pair<T, U> > array) {
      std::vector<T> res(array.size());
      for (int i = 0; i < array.size(); ++i)
	res[i] = array[i].first;
      return res;
    }
    
    template<typename T, typename U>
    std::vector<U> extractSecond(const std::vector<std::pair<T, U> > array) {
      std::vector<U> res(array.size());
      for (int i = 0; i < array.size(); ++i)
	res[i] = array[i].second;
      return res;
    }

    
    template<typename K, typename V>
    typename std::map<K,V>::iterator
    findFirstKey(std::map<K, V>& container, K query) {
      for (auto it = container.begin(); it != container.end(); ++it) {
	if (it->first == query)
	  return it;
      }
      return container.end();
    }

    template<typename K, typename V>
    typename std::map<K,V>::iterator 
    findFirstValue(std::map<K, V>& container, V query) {
      for (auto it = container.begin(); it != container.end(); ++it) {
	if (it->second == query)
	  return it;
      }
      return container.end();
    }


    template<typename K, typename V>
    std::vector<K> extractKeys(std::map<K,V> container) {
      std::vector<K> keys;
      for (auto& kv : container) {
        keys.push_back(kv.first);
      }
      return keys;
    }

    template<typename K, typename V>
    std::vector<K> extractValues(std::map<K,V> container) {
      std::vector<V> values;
      for (auto& kv : container) {
        values.push_back(kv.second);
      }
      return values;
    }

    template<typename T>
    int countEq(std::vector<T> array, T value) {
      int count = 0;
      for(auto& a : array){
        if (a == value) count++;
      }
      return count;
    }
        
    template<typename T>
    void findAndRemove(T elem, std::vector<T> &array) {
      typename std::vector<T>::iterator it =
	std::find(array.begin(), array.end(), elem);
      if (it != array.end())
	array.erase(it);
    }
    
    template<typename T>
    void insertOnce(T elem, std::vector<T> &array) {
      if (!utils::find(elem, array))
	array.push_back(elem);
    }
    
    // Generates the next combination of k elements of a vector of size n >= k.
    template<typename Iterator>
    bool next_combination(const Iterator first, Iterator k, const Iterator last) {
      /* Credits: Thomas Draper */
      if ((first == last) || (first == k) || (last == k))
	return false;
      Iterator itr1 = first;
      Iterator itr2 = last;
      ++itr1;
      if (last == itr1)
	return false;
      itr1 = last;
      --itr1;
      itr1 = k;
      --itr2;
      while (first != itr1) {
	if (*--itr1 < *itr2) {
	  Iterator j = k;
	  while (!(*itr1 < *j)) ++j;
	  std::iter_swap(itr1, j);
	  ++itr1;
	  ++j;
	  itr2 = k;
	  std::rotate(itr1, j, last);
	  while (last != j) {
	    ++j;
	    ++itr2;
	  }
	  std::rotate(k, itr2, last);
	  return true;
	}
      }
      std::rotate(first, k, last);
      return false;
    }
    
    
    inline void massert(bool expr, std::basic_string<char, std::char_traits<char>, std::allocator<char>> msg) {
      if(!expr) {
	std::cerr << msg << std::endl;
	exit(1);
      }
    }
    
    inline void warning(bool expr, std::basic_string<char, std::char_traits<char>, std::allocator<char>> msg) {
      if(!expr)
	std::cerr << msg << std::endl;
    }
    
    
    inline void create_dir(std::string dir) {
      struct stat st = {0};
      
      if (stat(dir.c_str(), &st) == -1) {
	mkdir(dir.c_str(), 0700);
      }
    }
    
  }
}

#endif // ULYSSES_UTILITIES__UTILS_H_
