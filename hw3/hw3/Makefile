# this makefile is used for self-made disambig
SRIPATH ?= /root/srilm-1.5.10
MACHINE_TYPE ?= i686-m64

CXX = g++
CXXFLAGS = -O3 -I$(SRIPATH)/include -w 
vpath lib%.a $(SRIPATH)/lib/$(MACHINE_TYPE)

TARGET = mydisambig
SRC = mydisambig.cpp
OBJ = $(SRC:.cpp=.o)
FROM ?= Big5-ZhuYin.map
TO ?= ZhuYin-Big5.map
.PHONY: all clean map

$(TARGET): $(OBJ) -loolm -ldstruct -lmisc
	$(CXX) $(LDFLAGS) -o $@ $^	
%.o: %.cpp;\
	$(CXX) $(CXXFLAGS) -c $<

all: $(TARGET)

map:
	@echo "---------mapping ZhuYin to Big5-----------"
	python3 mapping.py $(FROM) $(TO) 
	@echo "-------------mapping complete-------------"

clean:
	$(RM) $(OBJ) $(TARGET)
