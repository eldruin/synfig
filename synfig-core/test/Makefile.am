# $Id$

MAINTAINERCLEANFILES=Makefile.in
AM_CXXFLAGS=@CXXFLAGS@ @ETL_CFLAGS@ -I$(top_builddir) -I$(top_srcdir)/src
AM_LDFLAGS=@LDFLAGS@ @SYNFIG_LIBS@ @GTEST_LDFLAGS@
check_PROGRAMS=$(TESTS)

if HAVE_GTEST
  TESTS=bone unit
else
  TESTS=bone
endif

bone_SOURCES=bone.cpp

include gtest/Makefile_insert
