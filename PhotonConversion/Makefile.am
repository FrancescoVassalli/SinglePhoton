AUTOMAKE_OPTIONS = foreign

INCLUDES = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I`root-config --incdir`

libConvertedPhotonReconstructor_la_LDFLAGS = \
  -nodefaultlibs \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  `root-config --libs`

lib_LTLIBRARIES = \
  libConvertedPhotonReconstructor.la

include_HEADERS = \
  ConvertedPhotonReconstructor.h \
  ConvertedPhotonReconstructorLinkDef.h \
  RecoConversionEval.h \
  RecoConversionEvalLinkDef.h \
	RaveVertexingAux.h \
	RaveVertexingAuxLinkDef.h

libConvertedPhotonReconstructor_la_SOURCES = \
  ConvertedPhotonReconstructor.cc \
  ConvertedPhotonReconstructor_Dict.C \
  RecoConversionEval.cc \
  RecoConversionEval_Dict.C \
	RaveVertexingAux.C

libConvertedPhotonReconstructor_la_LIBADD = \
	-lphool\
	-lg4eval
BUILT_SOURCES = \
  testexternals.C

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libConvertedPhotonReconstructor.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

##############################################
# please add new classes in alphabetical order
# Rule for generating CINT dictionaries from class headers.
%_Dict.C: %.h %LinkDef.h
	rootcint -f $@ -c $(CINTFLAGS) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $^

clean-local:
	rm -f *Dict* testexternals.C

testexternals_SOURCES = testexternals.C
