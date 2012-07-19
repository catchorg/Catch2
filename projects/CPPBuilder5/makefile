ROOT = $(MAKEDIR)\..
BRCC = $(ROOT)\bin\brcc32.exe 

.silent
default: clean all

clean: 
	echo ========== cleaning object files ======
	-@DEL /F /Q /S *.obj *.tds *.exe 1>nul
	
		
# this override of the default implicit rule is a hack for not getting a ".bpr.exe" rule working correctly
# to override the default .cpp.exe rule
# currently relying upon BCB convention of a main cpp file named for the project listing project files
.cpp.exe: 
	echo ========== build for $@ ======
	bpr2mak -q $&.bpr
	make -a -f $&.mak

Tutorial1.exe: 

TARGETS= Tutorial1.exe 

all: $(TARGETS) 

help:
	@echo targets built by this makefile: $(TARGETS)
	@echo.
	@echo help - shows this output
	@echo all - builds all targets
	@echo clean - cleans all targets
	@echo tests - performs some tests on targets
	@echo default - clean all
	
	
pre-tests: 
	echo ========== running tests ======

post-tests: 
	echo ========== completed tests ======

tests: Tutorial1.exe pre-tests unit-tests post-tests
		
	
unit-tests: Tutorial1.exe
	-Tutorial1.exe



	