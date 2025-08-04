CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -I.
LDFFLAGS := -lstdc++fs

SRC := sources/main.cpp sources/cmdline.cpp
Algorithm_SRC := sources/algorithm/algorithm.cpp sources/algorithm/FanoutAnalyzer.cpp sources/algorithm/CellReplacer.cpp 
DEF_SRC := sources/def/DEFReader.cpp sources/def/DEFWriter.cpp
LEF_SRC := sources/lef/LEF.cpp sources/lef/LEFReader.cpp
Verilog_SRC := sources/verilog/Verilog.cpp sources/verilog/VerilogReader.cpp

OBJ := $(SRC:.cpp=.o) $(Algorithm_SRC:.cpp=.o) $(DEF_SRC:.cpp=.o) $(LEF_SRC:.cpp=.o) $(Verilog_SRC:.cpp=.o)
TARGET := cads1079

all: $(TARGET)

$(TARGET): $(OBJ) 
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFFLAGS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean: 
	rm -f $(OBJ) $(TARGET)
	
.PHONY: all clean