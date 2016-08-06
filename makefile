# ---------------------------------------------------------------------
# Includes all the files to be compiled.
# To add a new file, modify the make.inc file.
# ---------------------------------------------------------------------
include sources.inc

# Set your device acompute cabability here
DEVICE_CC=20

# ---------------------------------------------------------------------
# Includes MACROs 
# ---------------------------------------------------------------------

# ---------------------------------------------------------------------
# Paths 
# ---------------------------------------------------------------------
# cudaDBE
cudaDBE_PATH=/home/grad12/ffiorett/git/CUDA/Constraints2016/cudaBE_SumProd
LIB_PATH=$(cudaDBE_PATH)/lib

# Set your Rapid-xml library path here
RAPID_XML_PATH=$(LIB_PATH)/rapidxml-1.13
# Set the FF-misc-util library
MISC_UTIL_PATH=$(LIB_PATH)/misc_utils 
# Set your CUDA paths here
CUDA_PATH=/usr/local/cuda
CUDA_SAMPLES_PATH=$(CUDA_PATH)/samples/common/inc


# ---------------------------------------------------------------------
# Objects
# ---------------------------------------------------------------------
H_SOURCES = src/main.cpp $(H_HEADERS:%.hpp=%.cpp)
D_SOURCES = $(D_HEADERS:%.cuh=%.cu)
H_OBJECTS = $(H_SOURCES:%.cpp=%.o)
D_OBJECTS = $(D_SOURCES:%.cu=%.o)
OBJECTS   = $(H_OBJECTS) $(D_OBJECTS)


# ---------------------------------------------------------------------
# Compiler options 
# ---------------------------------------------------------------------
UNAME_S := $(shell uname -s)
# MAC-OS-X options
ifeq ($(UNAME_S),Darwin)
  CC = clang++
  NVCC=$(CUDA_PATH)/bin/nvcc 
  DEPEND = -std=c++11 -stdlib=libc++
endif
# LINUX optinos
ifeq ($(UNAME_S),Linux)
  CC = g++
  NVCC=$(CUDA_PATH)/bin/nvcc
  DEPEND = -std=c++11
endif

OPTIONS = -O3 -w -gencode arch=compute_$(DEVICE_CC),code=sm_$(DEVICE_CC) 
## Debug info
OPTIONS += -G -g -lineinfo

#LINKOPT=-lm -lpthread

vpath %.o ./.obj
## lib dirs -L...
CCLNDIRS= 
## include dirs -I...
INCLDIRS=-I$(cudaDBE_PATH)/src/ -I$(RAPID_XML_PATH) -I$(MISC_UTIL_PATH) \
	 -I$(CUDA_SAMPLES_PATH) -I$(CUDA_PATH)/include/

#Directives
DFLAGS=-D__cplusplus=201103L

## Compiler Flags
OPTIONS+= $(INCLDIRS) $(CCLNDIRS) $(LINKOPT) $(DFLAGS) $(DEPEND)

DIR_GUARD=@mkdir -p $(cudaDBE_PATH)/.obj/$(@D)

CCOPT=-O3 -w $(INCLDIRS) $(CCLNDIRS) $(DEPEND) $(DFLAGS)
NVCCOPT=-O3 -w $(INCLDIRS) $(CCLNDIRS) $(DEPEND) $(DFLAGS)

all:	cudaDBE


cudaDBE: $(OBJECTS)
	$(NVCC) $(OPTIONS) -o cudaBE_sp $(OBJECTS:%=$(cudaDBE_PATH)/.obj/%)

$(H_OBJECTS): %.o: %.cpp
	$(DIR_GUARD)
	$(CC) $(CCOPT) $< -c -o $(cudaDBE_PATH)/.obj/$@

$(D_OBJECTS): %.o: %.cu
	$(DIR_GUARD)
	$(NVCC) $(NVCCOPT) $< -dc -o $(cudaDBE_PATH)/.obj/$@

clean-gpu:
	rm -f $(D_OBJECTS:%=.obj/%)

clean-cpu:
	rm -f $(H_OBJECTS:%=.obj/%)

clean-tmp:
	rm -f $(H_SOURCES:%=%~) 
	rm -f $(H_HEADERS:%=%~)
	rm -f $(D_SOURCES:%=%~)
	rm -f $(D_HEADERS:%=%~)

clean: clean-cpu clean-gpu clean-tmp 
