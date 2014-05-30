#makefile 


CC   =   g++

#UCFLAGS = -O0 -g3 -Wall -gstabs+  
UCFLAGS = -O3 -Wall -gstabs+ -std=c++0x


RUCFLAGS := $(shell root-config --cflags) -I./include/ -I../Core/include/
LIBS :=  $(shell root-config --libs) -lTreePlayer
GLIBS := $(shell root-config --glibs)

VPATH = ../Core/src:./src/

SRCPP = main.cpp\
	HistoManager.cpp\
	IAnalysis.cpp\
	IEvent.cpp\
	ParReader.cpp\
	Utilities.cpp\
	Analysis.cpp\
	Event.cpp


	
         
#OBJCPP = $(SRCPP:.cpp=.o)
OBJCPP = $(patsubst %.cpp,obj/%.o,$(SRCPP))


all : analysis.exe

obj/%.o : %.cpp
	@echo "> compiling $*"
	@mkdir -p obj/
	@$(CC) -c $< $(UCFLAGS) $(RUCFLAGS) -o $@

analysis.exe : $(OBJCPP) 
	@echo "> linking"
	@$(CC) $^ $(ACLIBS) $(LIBS) $(GLIBS)  -o $@

clean:
	@echo "> Cleaning object files and executable"
	@rm  -f obj/*.o
	@rm -f analysis.exe
