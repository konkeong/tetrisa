# ---------------------------------------------------------------------------
VERSION = BCB.01
# ---------------------------------------------------------------------------
!ifndef BCB
BCB = $(MAKEDIR)\..
!endif
# ---------------------------------------------------------------------------
PROJECT = tetrisa.exe
OBJFILES = tetrisa.obj fmMain.obj fmAbout.obj fmDocu.obj \
   mlogo.obj mloopq.obj fmOption.obj fmScoreA.obj fmScoreD.obj \
   mscore.obj Kutil.obj
RESFILES = tetrisa.res
RESDEPEN = $(RESFILES) fmMain.dfm fmAbout.dfm fmDocu.dfm \
   fmOption.dfm fmScoreA.dfm fmScoreD.dfm
LIBFILES = 
DEFFILE = 
# ---------------------------------------------------------------------------
CFLAG1 = -O2 -Hc -w -k- -r -vi -c -a4 -b- -w-par -w-inl -Vx -Ve \
   -x
CFLAG2 = -I$(BCB)\projects;$(BCB)\include;$(BCB)\include\vcl;d:\bcb\splitter \
   -H=$(BCB)\lib\vcl.csm 
PFLAGS = -AWinTypes=Windows;WinProcs=Windows;DbiTypes=BDE;DbiProcs=BDE;DbiErrs=BDE \
   -U$(BCB)\projects;$(BCB)\lib\obj;$(BCB)\lib;d:\bcb\splitter \
   -I$(BCB)\projects;$(BCB)\include;$(BCB)\include\vcl;d:\bcb\splitter \
   -$Q -$L- -$D- -JPHNV -M     
RFLAGS = -i$(BCB)\projects;$(BCB)\include;$(BCB)\include\vcl;d:\bcb\splitter 
LFLAGS = -L$(BCB)\projects;$(BCB)\lib\obj;$(BCB)\lib;d:\bcb\splitter \
   -aa -Tpe -x -V4.0 
IFLAGS = 
LINKER = tlink32
# ---------------------------------------------------------------------------
ALLOBJ = c0w32.obj $(OBJFILES)
ALLRES = $(RESFILES)
ALLLIB = $(LIBFILES) vcl.lib import32.lib cp32mt.lib 
# ---------------------------------------------------------------------------
.autodepend

$(PROJECT): $(OBJFILES) $(RESDEPEN) $(DEFFILE)
    $(BCB)\BIN\$(LINKER) @&&!
    $(LFLAGS) +
    $(ALLOBJ), +
    $(PROJECT),, +
    $(ALLLIB), +
    $(DEFFILE), +
    $(ALLRES) 
!

.pas.hpp:
    $(BCB)\BIN\dcc32 $(PFLAGS) { $** }

.pas.obj:
    $(BCB)\BIN\dcc32 $(PFLAGS) { $** }

.cpp.obj:
    $(BCB)\BIN\bcc32 $(CFLAG1) $(CFLAG2) -o$* $* 

.c.obj:
    $(BCB)\BIN\bcc32 $(CFLAG1) $(CFLAG2) -o$* $**

.rc.res:
    $(BCB)\BIN\brcc32 $(RFLAGS) $<
#-----------------------------------------------------------------------------
