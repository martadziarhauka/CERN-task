generate: generate.cc
	clang++ -std=c++17 -I$(shell pythia8-config --includedir) generate.cc -o generate $(shell pythia8-config --libs)

CXX = clang++
CXXFLAGS = -std=c++17
PYTHIA_INCLUDE = $(shell pythia8-config --includedir)
PYTHIA_LIBS = $(shell pythia8-config --libs)

generate: generate.cc
	$(CXX) $(CXXFLAGS) -I$(PYTHIA_INCLUDE) generate.cc -o generate $(PYTHIA_LIBS)
