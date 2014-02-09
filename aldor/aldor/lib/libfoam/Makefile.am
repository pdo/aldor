SUBDIRS = al

aldorsrcdir = $(top_srcdir)/aldor/src

runtime_ALDOR = al/runtime.c
runtime_CSOURCES =			\
	$(aldorsrcdir)/btree.c		\
	$(aldorsrcdir)/compopt.c	\
	$(aldorsrcdir)/dword.c		\
	$(aldorsrcdir)/foam_c.c		\
	$(aldorsrcdir)/foam_cfp.c	\
	$(aldorsrcdir)/foamopt.c	\
	$(aldorsrcdir)/opsys.c		\
	$(aldorsrcdir)/output.c		\
	$(aldorsrcdir)/stdc.c		\
	$(aldorsrcdir)/store.c		\
	$(aldorsrcdir)/table.c		\
	$(aldorsrcdir)/timer.c		\
	$(aldorsrcdir)/util.c		\
	$(aldorsrcdir)/xfloat.c  	

AM_CFLAGS = -I$(aldorsrcdir) -DFOAM_RTS

lib_LTLIBRARIES =


#############################################################################
# :: Built-in bigint
#############################################################################

lib_LTLIBRARIES += libfoam.la

libfoam_la_SOURCES =			\
	$(aldorsrcdir)/bigint.c		\
	$(aldorsrcdir)/foam_i.c		\
	$(runtime_ALDOR)		\
	$(runtime_CSOURCES)


#############################################################################
# :: GMP for bigint
#############################################################################

if GMP
lib_LTLIBRARIES += libfoam-gmp.la
endif

# TODO: get rid of bigint.c in here
libfoam_gmp_la_SOURCES =			\
	../../contrib/gmp/fm_gmp.c	\
	$(aldorsrcdir)/bigint.c		\
	$(runtime_ALDOR)		\
	$(runtime_CSOURCES)


#############################################################################
# :: Common portable byte-code library
#############################################################################

datalibdir = $(datadir)/lib
datalib_DATA = al/libfoam.al
