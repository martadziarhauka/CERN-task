CXX = clang++
CXXFLAGS = -std=c++17
PYTHIA_INCLUDE = $(shell pythia8-config --includedir)
PYTHIA_LIBS = $(shell pythia8-config --libs)
ROOT_CFLAGS = $(filter-out -std=%,$(shell root-config --cflags))
ROOT_LIBS = $(shell root-config --libs)

generate: generate.cc
	$(CXX) $(CXXFLAGS) $(ROOT_CFLAGS) -I$(PYTHIA_INCLUDE) generate.cc -o generate $(ROOT_LIBS) $(PYTHIA_LIBS)
