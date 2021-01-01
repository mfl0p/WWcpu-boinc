CC = g++
LD = $(CC)

.SUFFIXES:
.SUFFIXES: .o .c .h .cpp

APP = WWcpu-win64

SRC = main.cpp wwcpu.cpp
OBJ =

OCL_INC = 
OCL_LIB = -L .

OCL_INC = 
OCL_LIB = 

BOINC_DIR = C:/mingwbuilds/boinc
BOINC_INC = -I$(BOINC_DIR)/lib -I$(BOINC_DIR)/api -I$(BOINC_DIR) -I$(BOINC_DIR)/win_build
BOINC_LIB = -L$(BOINC_DIR)/lib -L$(BOINC_DIR)/api -L$(BOINC_DIR) -lboinc_api -lboinc -lstdc++ -lpthread

CFLAGS  = -I . -O3 -m64 -Wall
LDFLAGS = $(CFLAGS) -static

all : clean $(APP)

$(APP) : $(SRC)
	$(LD) $(LDFLAGS) $^ $(OCL_LIB) $(BOINC_LIB) $(BOINC_INC) -o $@


clean :
	del *.o
	del $(APP).exe

