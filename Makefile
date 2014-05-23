
####################
### LIB EXTERNES ###
####################

OS=$(shell uname -s)

# Linux ########################################################
ifeq ($(OS), Linux)

VIEWER_LIBPATH = -L/usr/X11R6/lib64 -L/usr/lib/x86_64-linux-gnu
VIEWER_INCLUDEPATH = -I/usr/include/Qt -I/usr/include/QtCore -I/usr/include/QtGui -I/usr/share/qt4/mkspecs/linux-g++-64 -I/usr/include/QtOpenGL -I/usr/include/QtXml -I/usr/X11R6/include -I/usr/include/qt4/ $(foreach dir, $(shell ls /usr/include/qt4 | xargs), -I/usr/include/qt4/$(dir))
VIEWER_LIBS = -lGLU -lglut -lGL -lQtXml -lQtOpenGL -lQtGui -lQtCore -lpthread -lGLEW
VIEWER_DEFINES = -D_REENTRANT -DQT_NO_DEBUG -DQT_XML_LIB -DQT_OPENGL_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED

CUDA_INCLUDEPATH = -I/usr/local/cuda-5.5/include -I/usr/local/cuda-6.0/include
CUDA_LIBPATH = -L/usr/local/cuda-5.5/lib64 -L/usr/lib/nvidia-331 -L/usr/local/cuda-6.0/lib64
CUDA_LIBS = -lcuda -lcudart
NVCC=nvcc

OPENCL_INCLUDEPATH =
OPENCL_LIBPATH =
OPENCL_LIBS = -lOpenCL

DISTRIB=$(filter-out Distributor ID:, $(shell lsb_release -i))
#Ubuntu
ifeq ($(DISTRIB), Ubuntu)
else #Centos
endif

endif
###############################################################

#Compilateurs
LINK= g++
LINKFLAGS= -W -Wall -Wextra -pedantic -std=c++0x
LDFLAGS= $(VIEWER_LIBS) $(CUDA_LIBS) $(OPENCL_LIBS) -llog4cpp
INCLUDE = -Ilocal/include/ -I$(SRCDIR) $(foreach dir, $(call subdirs, $(SRCDIR)), -I$(dir)) $(VIEWER_INCLUDEPATH) $(CUDA_INCLUDEPATH) $(OPENAl_INCLUDEPATH)
LIBS = $(VIEWER_LIBPATH) $(CUDA_LIBPATH) $(OPENCL_LIBPATH)
DEFINES= $(VIEWER_DEFINES) $(OPT)


CC=gcc
CFLAGS= -W -Wall -Wextra -pedantic -std=c99 -m64

CXX=g++
CXXFLAGS= -W -Wall -Wextra -Wno-unused-parameter -pedantic -std=c++0x -m64 -Wshadow -Wstrict-aliasing -Weffc++ -Werror

#preprocesseur QT
MOC=moc
MOCFLAGS=

NVCCFLAGS= -Xcompiler -Wall -m64 -arch sm_$(NARCH) -O3

AS = nasm
ASFLAGS= -f elf64

# Autres flags
DEBUGFLAGS= -g -O0
CUDADEBUGFLAGS= -Xcompiler -Wall -m64 -G -g -arch sm_$(NARCH) -Xptxas="-v"
PROFILINGFLAGS= -pg
RELEASEFLAGS= -O3

# Source et destination des fichiers
TARGET = main

SRCDIR = $(realpath .)/src
OBJDIR = $(realpath .)/obj
EXCL=#excluded dirs in src
EXCLUDED_SUBDIRS = $(foreach DIR, $(EXCL), $(call subdirs, $(SRCDIR)/$(DIR)))
SUBDIRS =  $(filter-out $(EXCLUDED_SUBDIRS), $(call subdirs, $(SRCDIR)))

SRC_EXTENSIONS = c C cc cpp s S asm cu
WEXT = $(addprefix *., $(SRC_EXTENSIONS))

MOCSRC = $(shell grep -rlw $(SRCDIR)/ -e 'Q_OBJECT' --include=*.h | xargs) #need QT preprocessor
MOCOUTPUT = $(addsuffix .moc, $(basename $(MOCSRC)))
SRC = $(foreach DIR, $(SUBDIRS), $(foreach EXT, $(WEXT), $(wildcard $(DIR)/$(EXT))))
OBJ = $(subst $(SRCDIR), $(OBJDIR), $(addsuffix .o, $(basename $(SRC))))

include rules.mk
