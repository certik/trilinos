#@HEADER
# ************************************************************************
#       phdMesh : Parallel Heterogneous Dynamic unstructured Mesh
#                 Copyright (2007) Sandia Corporation
# 
# Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
# license for use of this work by or on behalf of the U.S. Government.
# 
# This library is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 2.1 of the
# License, or (at your option) any later version.
#  
# This library is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#  
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
# USA
# ************************************************************************
#@HEADER

include $(top_builddir)/Makefile.export.phdmesh

bin_PROGRAMS = test_util.exe

test_util_exe_SOURCES = $(srcdir)/test_comm_all.cpp \
    $(srcdir)/test_comm_bounds.cpp \
    $(srcdir)/test_octtree.cpp $(srcdir)/test_comm.cpp \
    $(srcdir)/test_containers.cpp \
    $(srcdir)/test_array.cpp \
    $(srcdir)/test_util.cpp

test_util_exe_DEPENDENCIES = $(top_builddir)/src/libphdmesh_util.a

test_util_exe_LDADD = $(PHDMESH_LIBS)

AM_CPPFLAGS = $(PHDMESH_INCLUDES)

EXTRA_DIST = $(srcdir)/test_util.i
