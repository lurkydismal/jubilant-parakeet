SRCDIR                = .
SUBDIRS               =
DLLS                  =
LIBS                  = libobject_t.a
EXES                  =



### Common settings

CEXTRA                = -g
CXXEXTRA              =
RCEXTRA               =
DEFINES               =
INCLUDE_PATH          = -I include \
			-I ../stdfunc/include \
			-I ../state_t/include \
			-I ../animation_t/include \
			-I ../boxes_t/include \
			-I ../color_t/include
DLL_PATH              =
DLL_IMPORTS           =
LIBRARY_PATH          =
LIBRARIES             = -lSDL3


### libobject_t.a sources and settings

libobject_t_a_MODULE       = libobject_t.a
libobject_t_a_C_SRCS       = src/object_t.c
libobject_t_a_CXX_SRCS     =
libobject_t_a_RC_SRCS      =
libobject_t_a_LDFLAGS      =
libobject_t_a_ARFLAGS      = rc
libobject_t_a_DLL_PATH     =
libobject_t_a_DLLS         =
libobject_t_a_LIBRARY_PATH =
libobject_t_a_LIBRARIES    =

libobject_t_a_OBJS         = $(libobject_t_a_C_SRCS:.c=.o) \
			$(libobject_t_a_CXX_SRCS:.cpp=.o) \
			$(libobject_t_a_RC_SRCS:.rc=.res)



### Global source lists

C_SRCS                = $(libobject_t_a_C_SRCS)
CXX_SRCS              = $(libobject_t_a_CXX_SRCS)
RC_SRCS               = $(libobject_t_a_RC_SRCS)


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

$(libobject_t_a_MODULE): $(libobject_t_a_OBJS)
	$(AR) $(libobject_t_a_ARFLAGS) $@ $(libobject_t_a_OBJS)


