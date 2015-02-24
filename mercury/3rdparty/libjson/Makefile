CXX?=g++

CXX_FLAGS:=$(CXX_FLAGS) -DNDEBUG
TGT?=x86_64

OBS = \
    _$(TGT)_obs/internalJSONNode.o\
    _$(TGT)_obs/JSONAllocator.o\
    _$(TGT)_obs/JSONChildren.o\
    _$(TGT)_obs/JSONDebug.o\
    _$(TGT)_obs/JSONIterators.o\
    _$(TGT)_obs/JSONMemory.o\
    _$(TGT)_obs/JSONNode.o\
    _$(TGT)_obs/JSONNode_Mutex.o\
    _$(TGT)_obs/JSONPreparse.o\
    _$(TGT)_obs/JSONStream.o\
    _$(TGT)_obs/JSONValidator.o\
    _$(TGT)_obs/JSONWorker.o\
    _$(TGT)_obs/JSONWriter.o\
    _$(TGT)_obs/libjson.o

default: init | all

init:
	mkdir -p _$(TGT)_obs


_$(TGT)_obs/internalJSONNode.o: src/internalJSONNode.cpp
	$(CXX) $(CXX_FLAGS) -o $@ -c $^

_$(TGT)_obs/JSONAllocator.o: src/JSONAllocator.cpp
	$(CXX) $(CXX_FLAGS) -o $@ -c $^

_$(TGT)_obs/JSONChildren.o: src/JSONChildren.cpp
	$(CXX) $(CXX_FLAGS) -o $@ -c $^

_$(TGT)_obs/JSONDebug.o: src/JSONDebug.cpp
	$(CXX) $(CXX_FLAGS) -o $@ -c $^

_$(TGT)_obs/JSONIterators.o: src/JSONIterators.cpp
	$(CXX) $(CXX_FLAGS) -o $@ -c $^

_$(TGT)_obs/JSONMemory.o: src/JSONMemory.cpp
	$(CXX) $(CXX_FLAGS) -o $@ -c $^

_$(TGT)_obs/JSONNode.o: src/JSONNode.cpp
	$(CXX) $(CXX_FLAGS) -o $@ -c $^

_$(TGT)_obs/JSONNode_Mutex.o: src/JSONNode_Mutex.cpp
	$(CXX) $(CXX_FLAGS) -o $@ -c $^

_$(TGT)_obs/JSONPreparse.o: src/JSONPreparse.cpp
	$(CXX) $(CXX_FLAGS) -o $@ -c $^

_$(TGT)_obs/JSONStream.o: src/JSONStream.cpp
	$(CXX) $(CXX_FLAGS) -o $@ -c $^

_$(TGT)_obs/JSONValidator.o: src/JSONValidator.cpp
	$(CXX) $(CXX_FLAGS) -o $@ -c $^

_$(TGT)_obs/JSONWorker.o: src/JSONWorker.cpp
	$(CXX) $(CXX_FLAGS) -o $@ -c $^

_$(TGT)_obs/JSONWriter.o: src/JSONWriter.cpp
	$(CXX) $(CXX_FLAGS) -o $@ -c $^

_$(TGT)_obs/libjson.o: src/libjson.cpp
	$(CXX) $(CXX_FLAGS) -o $@ -c $^

clean:
	rm -rf _$(TGT)_obs/

archive: $(OBS)
	$(AR) rcs _$(TGT)_obs/libjson.a $(OBS)

all: archive
