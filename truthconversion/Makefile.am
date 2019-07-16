AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(OFFLINE_MAIN)/include/eigen3 \
  -I`root-config --incdir`

libtruthconversion_la_LDFLAGS = \
  -nodefaultlibs \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
	-L$(HOME)/install/lib \
 `root-config --libs`

lib_LTLIBRARIES = \
	libtruthconversion.la
  
pkginclude_HEADERS = \
	ConversionClusterTreeMaker.h \
	ConversionClusterTreeMakerLinkDef.h \
	TruthConversionEval.h \
	TruthConversionEvalLinkDef.h \
	Conversion.h \
	ConversionLinkDef.h 

libtruthconversion_la_SOURCES = \
	ConversionClusterTreeMaker.C \
	TruthConversionEval.C \
	Conversion.cc 

libtruthconversion_la_LIBADD = \
  -lphool\
  -lg4eval\
  -lg4dst\
  -lcalotrigger\
	-lcalo_io\
	-lcalo_util

BUILT_SOURCES = \
	testexternals.C \
	gcc_test.c

noinst_PROGRAMS = \
	testexternals

testexternals_LDADD = \
	libtruthconversion.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@
%_Dict.C: %.h %LinkDef.h
		rootcint -f $@ -c $(CINTFLAGS) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $^
##############################################
# please add new classes in alphabetical order
# Rule for generating CINT dictionaries from class headers.

clean-local:
	rm -f *Dict* testexternals.C
