//
// Created by Ferdinando Fioretto on 11/8/15.
//
#ifndef CUDA_DBE_INPUTPARAMS_HPP
#define CUDA_DBE_INPUTPARAMS_HPP

#include <memory>
#include <vector>
#include <string>
#include <string_utils.hpp>
#include <utils.hpp>

#include "Types.hpp"
#include "Preferences.hpp"
//#include "Utils/GpuInfo.cuh"

using namespace misc_utils;

class InputParams {
public:
  enum format_t {xml, wcsp, uai};
  enum agent_t  {cpuBE, gpuBE, cpuMiniBE, gpuMiniBE};

  typedef std::shared_ptr<InputParams> ptr;

  static void parse(int argc, char** argv) {
    std::vector<std::string> inputs;
    for (int i=0; i < argc; i++) {
      inputs.push_back(argv[i]);
    }
    
    // Input File in .dat format
    int idx_xml  = utils::findIdx(inputs, std::string("--format=xml"));
    int idx_wcsp = utils::findIdx(inputs, std::string("--format=wcsp"));
    int idx_uai  = utils::findIdx(inputs, std::string("--format=uai"));
    if (idx_xml != -1) {
      fileName = inputs[idx_xml + 1];
      format = xml;
    } else if (idx_wcsp != -1) {
      fileName = inputs[idx_wcsp + 1];
      format = wcsp;
    } else if (idx_uai != -1) {
      fileName = inputs[idx_uai + 1];
      format = uai;
    } else {
      std::cout << usage();
      exit(-1);
    }

    int idx_m1 = utils::findIdx(inputs, std::string("--agt=cpuBE"));
    int idx_m2 = utils::findIdx(inputs, std::string("--agt=gpuBE"));
    int idx_m3 = utils::findIdx(inputs, std::string("--agt=cpuMiniBE"));
    int idx_m4 = utils::findIdx(inputs, std::string("--agt=gpuMiniBE"));
    if (idx_m1 != -1) {
      agtType = cpuBE;
      agtParam = 0;
    } else if (idx_m2 != -1) {
      agtType = gpuBE;
      agtParam = 0;
    } else if (idx_m3 != -1) {
      agtType  = cpuMiniBE;
      agtParam = std::stoi(inputs[idx_m3+1]); 
    } else if (idx_m4 != -1) {
      agtType  = gpuMiniBE;
      agtParam = std::stoi(inputs[idx_m4+1]); 
    } else {
      std::cout << usage();
      exit(-1);
    }


    int idx = strutils::findSubstrIdx(inputs, std::string("--root="));
    if (idx != -1) {
      Preferences::ptRoot = std::stoi(inputs[idx].substr(7));
    } else {
      Preferences::ptRoot = Preferences::default_ptRoot;;
    }

    idx = strutils::findSubstrIdx(inputs, std::string("--heur="));
    if (idx != -1) {
      Preferences::ptHeuristic = std::stoi(inputs[idx].substr(7));
    } else {
      Preferences::ptHeuristic = Preferences::default_ptHeuristic;;
    }

    
    idx = strutils::findSubstrIdx(inputs, std::string("--maxMB="));
    if (idx != -1) {
      Preferences::maxDevMemory = std::stod(inputs[idx].substr(8));
    } else {
      Preferences::maxDevMemory = Preferences::default_maxDevMemory;
    }

    idx = strutils::findSubstrIdx(inputs, std::string("--maxGB="));
    if (idx != -1) {
      Preferences::maxDevMemory = 1000 * std::stod(inputs[idx].substr(8));
    } else {
      Preferences::maxDevMemory = Preferences::default_maxDevMemory;
    }

    idx = strutils::findSubstrIdx(inputs, std::string("--devID="));
    if (idx != -1) {
      Preferences::gpuDevID = std::stoi(inputs[idx].substr(8));
    } else {
      Preferences::gpuDevID = Preferences::default_gpuDevID;
    }
    
    Preferences::maxHostMemory = Preferences::default_maxHostMemory;
  }

  static std::string usage() {
    std::string ret = "ProgramName\n";
    ret += "--format=[xml|wcsp|uai] inputFile\n";
    ret += "--agt=[cpuBE|gpuBE|cpuMiniBE z|gpuMiniBE z]\n";
    ret += "       where z is the maximal size of the mini-bucket\n";
    ret += "[--root=X]      : The agent with id=X is set to be the root of the pseudoTree\n";
    ret += "[--heur={1,2,3,4}] : The PseudoTree construction will use heuristic=X\n";
    ret += "[--max[MB|GB=X]: X is the maximum amount of memory used by the GPU\n";
    return ret;
  }

    static std::string &getFileName() {
        return fileName;
    }
    static format_t getFormat() {
      return format;
    }

    static agent_t getAgentType() {
      return agtType;
    }

    static int getAgentParam() {
      return agtParam;
    }

    static std::string to_string() {
      std::string ret = "Problem : " + fileName + " format=";
      if (format == xml) ret+="xml";
      if (format == uai) ret+="uai";
      return ret;
    }
    
private:
    static std::string fileName;
    static format_t format;
    static agent_t agtType;
    static int agtParam;
};


#endif //CUDA_DBE_INPUTPARAMS_H
