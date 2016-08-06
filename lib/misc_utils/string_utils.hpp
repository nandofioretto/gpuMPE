#ifndef MISC_UTILS_STRING_UTILS_HPP
#define MISC_UTILS_STRING_UTILS_HPP

#include <algorithm>
#include <string>
#include <vector>
#include <sstream>

namespace misc_utils
{
  namespace strutils
  {
    inline std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
      std::stringstream ss(s);
      std::string item;
      while (getline(ss, item, delim)) {
        if (!item.empty())
          elems.push_back(item);
      }
      return elems;
    }
    
    
    inline std::vector<std::string> split(const std::string &s, char delim = ' ') {
      std::vector<std::string> elems;
      split(s, delim, elems);
      return elems;
    }
    
    
    inline void replace_all(std::string &str, const std::string &from, const std::string &to) {
      if (from.empty())
        return;
      size_t start_pos = 0;
      while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
      }
    }
    
    // Remove directory if present.
    // Do this before extension removal incase directory has a period character.
    inline std::vector<std::string> split_path_file(std::string pathfile, bool remove_extension = false) {
      std::string path = pathfile;
      std::string file = pathfile;
      const size_t last_slash_idx = pathfile.find_last_of("\\/");
      if (std::string::npos != last_slash_idx) {
        path.erase(last_slash_idx, pathfile.length());
        file.erase(0, last_slash_idx + 1);
      }
      if (remove_extension) {
        const size_t period_idx = file.rfind('.');
        if (std::string::npos != period_idx) {
          file.erase(period_idx);
        }
      }
      std::vector<std::string> ret;
      ret.push_back(path);
      ret.push_back(file);
      return ret;
      //return {path, file};
    }
    
    
    inline std::string get_file_extension(const std::string &filename) {
      return filename.substr(filename.find_last_of(".") + 1);
    }
    

    inline int findSubstrIdx(const std::vector<std::string> A, std::string query) {
      for (int i = 0; i < A.size(); ++i) {
        size_t found = A[i].find(query);
        if (found != std::string::npos)
          return i;
      }
      return -1;
    }

    
    inline std::vector<int> vstoi(std::vector<std::string> pValues) {
      std::vector<int> result;
      for (auto val : pValues)
        result.push_back(stoi(val));
      return result;
    }

    inline std::vector<double> vstod(std::vector<std::string> pValues) {
      std::vector<double> result;
      for (auto val : pValues)
        result.push_back(stod(val));
      return result;
    }
    
    // trim from start
    inline std::string &ltrim(std::string &s) {
      s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
      return s;
    }
    
    // trim from end
    inline std::string &rtrim(std::string &s) {
      s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
      return s;
    }
    
    // trim from both ends
    inline std::string &trim(std::string &s) {
      return ltrim(rtrim(s));
    }
    
    
    inline std::string to_string(const std::vector<std::string> &pValues) {
      std::string result = "";
      for (auto val : pValues)
        result += val + ", ";
      return result;
    }
    
    template<typename T, typename U>
    std::string to_string(const std::pair<T,U>& p) {
      return "(" + std::to_string(p.first) + ", " + std::to_string(p.second) + ")";
    }
    
    template <class T>
    std::string to_string(const std::tuple<T>& tuple) {
      std::string res = "(";
      for (auto t : tuple)
        res += to_string(t) + ",";
      res += ")";
      return res;
    }
    
    // It returns the description of the array content given as a parameter.
    template<typename T>
    std::string to_string(const std::vector<T> &array) {
      std::string res = "{";
      for (int i = 0; i < array.size(); ++i) {
        res += std::to_string(((int)(array[i] * 100 + .5) / 100.0));
        if (i < array.size() - 1) res += ", ";
      }
      res += "}";
      return res;
    }
    
    template<typename T>
    std::string to_string(const std::vector<std::vector<T> > pMatrix) {
      std::string res;
      for (int i = 0; i < pMatrix.size(); ++i) {
        for (int j = 0; j < pMatrix[i].size(); ++j) {
          res += std::to_string(pMatrix[i][j]);
          if (j < pMatrix[i].size() - 1) res += ", ";
        }
        res += "\n";
      }
      return res;
    }
    
    // It returns the description of the array content given as a parameter.
    template<typename T, typename U>
    std::string to_string(const std::vector<std::pair<T, U> > pArray) {
      std::string res = "{";
      for (int i = 0; i < pArray.size(); ++i) {
        res += "(" + std::to_string(pArray[i].first) + ", "
          + std::to_string(pArray[i].second) + ")";
        if (i < pArray.size() - 1) res += ", ";
      }
      res += "}";
      return res;
    }
  }
}

#endif
