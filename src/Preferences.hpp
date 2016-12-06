#ifndef CUDA_DBE_PREFERENCES_H_
#define CUDA_DBE_PREFERENCES_H_

//MINIMIZE | MAXIMIZE
#define MAP

//////////////////////////////////
// Maximum Aposteriori Probability
#ifdef MAP    // (*, max)
#define JoinOP 		*
#define cudaJoinOP 	*
// ProjOP = Max
#define cudaProjOP	fmax
#define ProjOP(a,b)           \
  ({ __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b);  \
      _a > _b ? _a : _b; })

#define RelOP  >
#define MAXIMIZE
#define ZERO (util_t)1
#endif
//////////////////////////////////
// Most Probable Explanation
#ifdef MPE    // (*, +)
#define JoinOP 		*
#define cudaJoinOP 	*
// ProjOP = Sum
#define cudaProjOP(a,b)	({(a) + (b);})
#define 	ProjOP(a,b)	({(a) + (b);})

#define MAXIMIZE
#define ZERO (util_t)1
#endif
//////////////////////////////////
// WCSP maximize
#ifdef WCSPmax    // (+, max)
#define JoinOP		+
#define cudaJoinOP 	+
// ProjOP = Max
#define cudaProjOP max
#define ProjOP(a,b)           \
  ({ __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b);  \
      _a > _b ? _a : _b; })
		  
#define RelOP  >
#define MAXIMIZE
#define ZERO (util_t)0
#endif
//////////////////////////////////
// WCSP minimize
#ifdef WCSPmin	  // (+, min)
#define JoinOP 		+
#define cudaJoinOP 	+
// ProjOP = Min
#define cudaProjOP min
#define ProjOP(a,b)           \
  ({ __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b);  \
      _a > _b ? _b : _a; })
		  
#define RelOP  <
#define ZERO (util_t)0
#define MINIMIZE
#endif


class Preferences {
public:

  // Print and Report preferences
  static constexpr bool verbose = false;
  static constexpr bool verboseDevInit = false;
  static constexpr bool silent = false;
  static constexpr bool csvFormat = true;
  
  // PseudoTree Construction (Default parameters - 
  // can be superseeded by input)
  static constexpr int default_ptRoot      = 0;
  static constexpr int default_ptHeuristic = 2;
  static int ptRoot     ;
  static int ptHeuristic;

  // CUDA Memory preferences
  // default: when singleAgent=F, usePinned=T
  // default: when singleAgent=T, usePinned=F
  static constexpr bool usePinnedMemory = true;
  static constexpr bool singleAgent     = true;
  static constexpr float streamSizeMB = 50;
  
  // Host and Device Memory Limits (in bytes)
  static constexpr float default_maxHostMemory = 0  /*GB*/ * 1e+9;
  static constexpr float default_maxDevMemory  = 12 /*GB*/ * 1e+9; // 0 for unbounded
  static constexpr int   default_gpuDevID      = 0;

  static float maxHostMemory;
  static float maxDevMemory;
  static int   gpuDevID;
};

#endif
