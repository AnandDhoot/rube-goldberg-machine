.SUFFIXES: .cpp .hpp

# Programs
SHELL 	= bash
CC     	= g++
LD	= ld
RM 	= rm
ECHO	= /bin/echo
CAT	= cat
PRINTF	= printf
SED	= sed
DOXYGEN = doxygen
######################################
#Variables
STATIC_LIB = FALSE

# Project Name (generate executable with this name)
TARGET = cs251_exe_21

# Project Paths
PROJECT_ROOT=.
EXTERNAL_ROOT=$(PROJECT_ROOT)/external
SRCDIR = $(PROJECT_ROOT)/src
OBJDIR = $(PROJECT_ROOT)/myobjs
BINDIR = $(PROJECT_ROOT)/mybins
DOCDIR = $(PROJECT_ROOT)/doc
LIBDIR = $(PROJECT_ROOT)/mylibs

# Library Paths
BOX2D_ROOT=$(EXTERNAL_ROOT)
GLUI_ROOT=/usr/X11/
GL_ROOT=/usr/X11/include/

#Libraries
LIBS = -lBox2D -lglui -lglut -lGLU -lGL

# Compiler and Linker flags
CPPFLAGS =-g -O3 -Wall -fno-strict-aliasing
CPPFLAGS+=-I $(BOX2D_ROOT)/include -I $(GLUI_ROOT)/include
LDFLAGS+=-L $(BOX2D_ROOT)/lib -L $(GLUI_ROOT)/lib

######################################

NO_COLOR=\e[0m
OK_COLOR=\e[1;32m
ERR_COLOR=\e[1;31m
WARN_COLOR=\e[1;33m
MESG_COLOR=\e[1;34m
FILE_COLOR=\e[1;37m

OK_STRING="[OK]"
ERR_STRING="[ERRORS]"
WARN_STRING="[WARNINGS]"
OK_FMT="${OK_COLOR}%30s\n${NO_COLOR}"
ERR_FMT="${ERR_COLOR}%30s\n${NO_COLOR}"
WARN_FMT="${WARN_COLOR}%30s\n${NO_COLOR}"
######################################

SRCS := $(wildcard $(SRCDIR)/*.cpp)
INCS := $(wildcard $(SRCDIR)/*.hpp)
OBJS := $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
OBJ := $(OBJS:.//myobjs/main.o= )
SRC := $(SRCS:.//src/main.cpp= )

.PHONY: all setup doc clean distclean exelib executable report

all: setup executable report doc

setup:
	@$(ECHO) "Setting up compilation..."
	@mkdir -p myobjs
	@mkdir -p mybins
	@mkdir -p mylibs
	@if !(test -d ./doc); \
	then tar -xvf cs251_g21_project.tar.gz;\
	fi;\
	if !(test -f ./external/lib/libBox2D.a) || !(test -d ./external/include/Box2D); \
	then cd ./external/src ; tar -xvf Box2D.tgz ; \
		cd Box2D ; mkdir -p build251 ; cd build251 ; \
		cmake ../ ; make ; make install; \
	fi;\

executable: setup $(OBJS) $(BINDIR)/$(TARGET)

######################################	

$(BINDIR)/$(TARGET): $(OBJS)
	@$(PRINTF) "$(MESG_COLOR)Building executable:$(NO_COLOR) $(FILE_COLOR) %16s$(NO_COLOR)" "$(notdir $@)"
	@$(CC) -o $@ $(LDFLAGS) $(OBJS) $(LIBS) 2> temp.log || touch temp.err
	@if test -e temp.err; \
	then $(PRINTF) $(ERR_FMT) $(ERR_STRING) && $(CAT) temp.log; \
	elif test -s temp.log; \
	then $(PRINTF) $(WARN_FMT) $(WARN_STRING) && $(CAT) temp.log; \
	else $(PRINTF) $(OK_FMT) $(OK_STRING); \
	fi;
	@$(RM) -f temp.log temp.err

-include $(OBJS:.o=.d)

$(OBJS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(PRINTF) "$(MESG_COLOR)Compiling: $(NO_COLOR) $(FILE_COLOR) %25s$(NO_COLOR)" "$(notdir $<)"
	@$(CC) $(CPPFLAGS) -c -fPIC $< -o $@ -MD 2> temp.log || touch temp.err
	@if test -e temp.err; \
	then $(PRINTF) $(ERR_FMT) $(ERR_STRING) && $(CAT) temp.log; \
	elif test -s temp.log; \
	then $(PRINTF) $(WARN_FMT) $(WARN_STRING) && $(CAT) temp.log; \
	else printf "${OK_COLOR}%30s\n${NO_COLOR}" "[OK]"; \
	fi;
	@$(RM) -f temp.log temp.err

######################################

ifeq ($(STATIC_LIB),TRUE)
libr: statlibr
else
libr: sharelibr
endif

statlibr: $(OBJS)
	@ar rcs $(LIBDIR)/libCS251.a $(OBJ)
	
sharelibr: $(OBJS)
	@$(CC) -shared -o $(LIBDIR)/libCS251.so $(OBJ);

exelib: setup libr
	@$(PRINTF) "$(MESG_COLOR)Build using library:$(NO_COLOR) $(FILE_COLOR) %16s$(NO_COLOR)" "cs251_exelib_21"
	@$(PRINTF) $(OK_FMT) $(OK_STRING)
	@$(CC) -L $(LIBDIR) $(LDFLAGS)  -Wl,-rpath,$(CURDIR)/mylibs -o $(BINDIR)/cs251_exelib_21 $(OBJDIR)/main.o -lCS251 $(LIBS)

######################################
doc:
	@$(ECHO) -n "Generating Doxygen Documentation "
	@$(RM) -rf doc/html
	@$(DOXYGEN) $(DOCDIR)/Doxyfile 2 > /dev/null
	@$(ECHO) "Done"
	
report:
	@$(ECHO) "Generating Report and Presentation ";
	@cd doc;\
	latex -interaction nonstopmode main.tex >/dev/null;\
	bibtex main.aux >/dev/null;\
	latex -interaction nonstopmode main.tex >/dev/null;\
	latex -interaction nonstopmode main.tex >/dev/null;\
	pdflatex -interaction nonstopmode main.tex >/dev/null;\
	pdflatex -interaction nonstopmode report.tex >/dev/null;\
	bibtex report.aux >/dev/null;\
	pdflatex -interaction nonstopmode report.tex >/dev/null ;\
	pdflatex -interaction nonstopmode report.tex >/dev/null ;\
	rm -f *.log *.bak *.aux *.blg *.dvi *.toc *.bbl *.out *.nav *.snm;\

clean:
	@$(ECHO) -n "Cleaning up..."
	@$(RM) -rf  $(LIBDIR) $(OBJDIR) $(BINDIR) *~ $(DEPS) cs251_g21_project.tar.gz *~
	@$(ECHO) "Done"

distclean: clean
	@$(RM) -R -f $(EXTERNAL_ROOT)/lib/* *~ $(EXTERNAL_ROOT)/include/Box2D/ *~ *~ $(EXTERNAL_ROOT)/src/Box2D/ doc/*.pdf doc/html/ doc/*~  cs251_g21_project.tar.gz *~;
	@$(ECHO) -n "Making Distributable Archive.. "
	tar cvzf cs251_g21_project.tar.gz external/ src/ doc/ ;\
	$(RM) -R -f doc/ src/ external/ ;
install: setup
	@rm -f cs251_g21_project.tar.gz;
