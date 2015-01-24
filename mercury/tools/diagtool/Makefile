TGT?=x86_64
ENV?=devel
ROOT_DIR=../../
include $(ROOT_DIR)/targets/$(TGT)/$(ENV).mk

all:
	$(CXX) -o udcat udcat.cpp ../../_$(TGT)_obs/libmercury.a -I ../../include
