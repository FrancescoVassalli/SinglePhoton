AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
	-I$(MY_INSTALL)/include \
  -I$(OFFLINE_MAIN)/include \
  -I$(OFFLINE_MAIN)/include/eigen3 \
  -I`root-config --incdir`

#libPVReco_la_LDFLAGS = \
#  -L$(libdir) \
#  -L$(OFFLINE_MAIN)/lib

lib_LTLIBRARIES = \
   libPVReco.la

AM_CXXFLAGS = -Wall -Werror -msse2 -DRAVE -DRaveDllExport=

AM_LDFLAGS = \
	-L$(libdir) \
	-L$(MY_INSTALL)/lib \
	-L$(OFFLINE_MAIN)/lib

pkginclude_HEADERS = \
	PVReco.h\
	PVRecoLinkDef.h

libPVReco_la_SOURCES = \
  PVReco.C 

libPVReco_la_LIBADD = \
  -lfun4all \
	-lg4detectors \
  -lg4dst \
  -lg4eval \
  -lg4jets \
	-lgenfit2 \
	-lgenfit2exp \
	-lPHGenFit \
	-lg4tpc \
	-lg4intt \
	-lg4mvtx \
  -lphhepmc

#pkginclude_HEADERS = $(include_HEADERS)

# Rule for generating table CINT dictionaries.
%_Dict.C: %.h %LinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(INCLUDES) $^

%_Dict.cpp: %.h %LinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(INCLUDES) $^

################################################
# linking tests

noinst_PROGRAMS = testexternals

BUILT_SOURCES = \
  testexternals.C

testexternals_LDADD = \
  libPVReco.la

testexternals_SOURCES = testexternals.C

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f *Dict* testexternals.C
