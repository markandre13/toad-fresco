PRGFILE		= main

HTML		= html/parser.cc html/polygon.cc html/element.cc \
		  html/htmlview.cc html/urlstream.cc

SRCS		= main.cc \
		  MainWindow.cc MenuBar.cc helper.cc \
		  toad/toad.cc \
		  $(HTML)

# CC		= /opt/gcc-2.7.2.3/bin/g++
CC		= g++
MAKEDEP		= $(CC) -M

DEFINES		=
CFLAGS		= -Wall -Wno-unused -g -I./html -I.
LFLAGS		=

INCDIRS		= -I/opt/Fresco
LIBDIRS		= -L/opt/Fresco/Build/lib -L/usr/X11R6/lib
LIBS		= -lX11 -lpthread -ldl \
		  -locs -lbase -ldefs -lfdisplay -lwidgets -lfigures

SRCEXT		= .cc
OBJEXT		= .o

OBJS      	= $(SRCS:$(SRCEXT)=$(OBJEXT))
MAKE		= make -j 1

all:
	$(MAKE) $(PRGFILE)

#---------------------------------------------------------------------------
# Compiling
#---------------------------------------------------------------------------
.SUFFIXES: $(SRCEXT)

$(SRCEXT)$(OBJEXT):
	@echo compiling $*$(SRCEXT) ...
	@$(CC) $(CFLAGS) $(DEFINES) $(INCDIRS) $*$(SRCEXT) -c -o $*$(OBJEXT)

#---------------------------------------------------------------------------
# Linking
#---------------------------------------------------------------------------
$(PRGFILE): $(OBJS)
	@echo linking $(PRGFILE) ...
	@$(CC) $(LFLAGS) $(OBJS) -o $(PRGFILE) $(LIBDIRS) $(LIBS)
	@echo Ok

#---------------------------------------------------------------------------
# Xtras
#---------------------------------------------------------------------------
clean::
	@echo removing generated files in `pwd`
	@rm -f $(PRGFILE) $(OBJS) *.bak *~ .depend core DEADJOE $(CLEAN)
	@touch .depend
  
depend:
	@echo creating dependencies in `pwd`
	makedepend -Y -I. $(SRCS) 2> /dev/null	

.depend:
	touch .depend

include .depend
# DO NOT DELETE

main.o: help.hh toad/toad.hh ./toad/types.hh html/htmlview.hh html/state.hh
main.o: MainWindow.hh
MainWindow.o: help.hh MainWindow.hh MenuBar.hh ToolBox.hh DPicture.hh
MainWindow.o: ToolButton.hh CTool.hh action.hh html/htmlview.hh toad/toad.hh
MainWindow.o: ./toad/types.hh
MenuBar.o: help.hh MenuBar.hh
helper.o: help.hh
toad/toad.o: toad/toad.hh ./toad/types.hh help.hh
html/parser.o: html/parser.hh html/element.hh toad/toad.hh ./toad/types.hh
html/parser.o: ./toad/pointer.hh html/string.hh
html/polygon.o: html/polygon.hh ./toad/types.hh
html/element.o: toad/toad.hh ./toad/types.hh ./toad/pointer.hh
html/element.o: html/element.hh html/string.hh html/state.hh
html/element.o: html/lex_special_char.cc
html/htmlview.o: toad/toad.hh ./toad/types.hh ./toad/pointer.hh
html/htmlview.o: ./toad/scrollbar.hh html/htmlview.hh html/element.hh
html/htmlview.o: html/parser.hh html/string.hh html/state.hh html/polygon.hh
