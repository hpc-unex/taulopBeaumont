# what flags you want to pass to the C compiler & linker
AM_CXXFLAGS = -I$(top_srcdir)/include  --pedantic -Wall -std=c++11 -O2
AM_LDFLAGS = -L$(with_taulop)/lib

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
bin_PROGRAMS = beaumont


######   BEAUMONT 

beaumont_SOURCES = main_beaumont.cpp  algorithm.cpp  algorithm_basic.cpp  algorithm_malik.cpp  algorithm_MxM.cpp  algorithm_taulop_pbc.cpp  algorithm_taulop_pbr.cpp  algorithm_W1D.cpp  algorithm_W2D.cpp arrangement.cpp column.cpp cost.cpp heuristic_beaumont.cpp heuristic_malik.cpp heuristic.cpp matrix.cpp rectangle.cpp 

beaumont_CXXFLAGS = $(AM_CXXFLAGS) -I$(includedir)
beaumont_LDADD = -ltaulop

######   MALIK

#malik_SOURCES = main_malik.cpp  algorithm.cpp  algorithm_basic.cpp  algorithm_malik.cpp  algorithm_MxM.cpp  algorithm_taulop_pbc.cpp  algorithm_taulop_pbr.cpp  algorithm_W1D.cpp  algorithm_W2D.cpp arrangement.cpp column.cpp cost.cpp heuristic_beaumont.cpp heuristic_malik.cpp heuristic.cpp matrix.cpp rectangle.cpp 

#malik_CXXFLAGS = $(AM_CXXFLAGS) -I$(includedir)
#malik_LDADD = -ltaulop

######   DELTA

#delta_SOURCES = main_delta.cpp  algorithm.cpp  algorithm_basic.cpp  algorithm_malik.cpp  algorithm_MxM.cpp  algorithm_taulop_pbc.cpp  algorithm_taulop_pbr.cpp  algorithm_W1D.cpp  algorithm_W2D.cpp arrangement.cpp column.cpp cost.cpp heuristic_beaumont.cpp heuristic_malik.cpp heuristic.cpp matrix.cpp rectangle.cpp 

#delta_CXXFLAGS = $(AM_CXXFLAGS) -I$(includedir)
#delta_LDADD = -ltaulop
