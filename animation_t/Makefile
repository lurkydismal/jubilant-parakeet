SRCDIR                = .
SUBDIRS               =
DLLS                  =
LIBS                  = libanimation_t.a
EXES                  =



### Common settings

CEXTRA                =
CXXEXTRA              =
RCEXTRA               =
DEFINES               =
INCLUDE_PATH          = -I include \
			-I ../stdfunc/include
DLL_PATH              =
DLL_IMPORTS           =
LIBRARY_PATH          =
LIBRARIES             = -lSDL3


### libanimation_t.a sources and settings

libanimation_t_a_MODULE       = libanimation_t.a
libanimation_t_a_C_SRCS       = src/animation_t.c
libanimation_t_a_CXX_SRCS     =
libanimation_t_a_RC_SRCS      =
libanimation_t_a_LDFLAGS      =
libanimation_t_a_ARFLAGS      = rc
libanimation_t_a_DLL_PATH     =
libanimation_t_a_DLLS         =
libanimation_t_a_LIBRARY_PATH =
libanimation_t_a_LIBRARIES    =

libanimation_t_a_OBJS         = $(libanimation_t_a_C_SRCS:.c=.o) \
			$(libanimation_t_a_CXX_SRCS:.cpp=.o) \
			$(libanimation_t_a_RC_SRCS:.rc=.res)



### Global source lists

C_SRCS                = $(libanimation_t_a_C_SRCS)
CXX_SRCS              = $(libanimation_t_a_CXX_SRCS)
RC_SRCS               = $(libanimation_t_a_RC_SRCS)


### Tools

CC = ccache gcc
CXX = ccache g++
RC = rcc
AR = ar


### Generic targets

all: $(SUBDIRS) $(DLLS:%=%.so) $(LIBS) $(EXES)

### Build rules

.PHONY: all clean dummy

$(SUBDIRS): dummy
	@cd $@ && $(MAKE)

# Implicit rules

.SUFFIXES: .cpp .cxx .rc .res
DEFINCL = $(INCLUDE_PATH) $(DEFINES) $(OPTIONS)

.c.o:
	$(CC) -c $(CFLAGS) $(CEXTRA) $(DEFINCL) -o $@ $<

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(CXXEXTRA) $(DEFINCL) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(CXXEXTRA) $(DEFINCL) -o $@ $<

.rc.res:
	$(RC) $(RCFLAGS) $(RCEXTRA) $(DEFINCL) -fo$@ $<

# Rules for cleaning

CLEAN_FILES     = y.tab.c y.tab.h lex.yy.c core *.orig *.rej \
                  \\\#*\\\# *~ *% .\\\#*

clean:: $(SUBDIRS:%=%/__clean__) $(EXTRASUBDIRS:%=%/__clean__)
	$(RM) $(CLEAN_FILES) $(RC_SRCS:.rc=.res) $(C_SRCS:.c=.o) $(CXX_SRCS:.cpp=.o)
	$(RM) $(DLLS:%=%.so) $(LIBS) $(EXES) $(EXES:%=%.so)

$(SUBDIRS:%=%/__clean__): dummy
	cd `dirname $@` && $(MAKE) clean

$(EXTRASUBDIRS:%=%/__clean__): dummy
	-cd `dirname $@` && $(RM) $(CLEAN_FILES)

### Target specific build rules
DEFLIB = $(LIBRARY_PATH) $(LIBRARIES) $(DLL_PATH) $(DLL_IMPORTS:%=-l%)

$(libanimation_t_a_MODULE): $(libanimation_t_a_OBJS)
	$(AR) $(libanimation_t_a_ARFLAGS) $@ $(libanimation_t_a_OBJS)


