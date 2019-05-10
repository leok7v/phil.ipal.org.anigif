##############################################################################
DIRI	= ./Install_
BINI	= $(DIRI)bin
INCI	= $(DIRI)inc
LIBI	= $(DIRI)lib

INCD	= -I. -I$(INCI)
LIBD	= -L. -L$(LIBI)

BINL	= -langif -lc
LIBL	= -lc -lm
PNML	= -lpnm -lppm -lpgm -lpbm

##############################################################################
FUNS	= \
	angif_init_header.o\
	angif_init_image.o\
	angif_init_stream.o\
	angif_init_text.o\
	angif_put_animate.o\
	angif_put_comment.o\
	angif_put_header.o\
	angif_put_image.o\
	angif_put_index.o\
	angif_put_linear.o\
	angif_put_prelzw.o\
	angif_put_rgb.o\
	angif_put_text.o\
	angif_put_trailer.o\
	angif_set_file.o\
	angif_set_function.o\
	angif_set_memory.o\
	angif_take_memory.o\
	angif_write.c\

##############################################################################
PGMS	= \
	gifscan\
	pnmtogif\
	rawtogif\
	solidgif\

##############################################################################
PIPE	= -pipe
COPT	= \
	-Wall\
	-Wpointer-arith\
	-Wcast-qual\
	-Wcast-align\
	-Wwrite-strings\
	-Wnested-externs\
	-fomit-frame-pointer\
	-finline-functions\
	-O3\
	-funsigned-char\
	-fPIC\

##############################################################################
.SUFFIXES:

##############################################################################
all:			library programs
		ls -dl angif.h
		ls -dl libangif.so.1.0.0 libangif.so.1 libangif.so
		ls -dl gifscan pnmtogif rawtogif solidgif

install:		install_header install_library install_programs
		ls -dl `cd $(INCI);/bin/pwd`/angif.h
		ls -dl `cd $(LIBI);/bin/pwd`/libangif.so.1.0.0
		ls -dl `cd $(LIBI);/bin/pwd`/libangif.so.1
		ls -dl `cd $(LIBI);/bin/pwd`/libangif.so
		ls -dl `cd $(BINI);/bin/pwd`/gifscan
		ls -dl `cd $(BINI);/bin/pwd`/pnmtogif
		ls -dl `cd $(BINI);/bin/pwd`/rawtogif
		ls -dl `cd $(BINI);/bin/pwd`/solidgif

install_local:
		-ln -s /usr/local/bin $(BINI)
		-ln -s /usr/local/include $(INCI)
		-ln -s /usr/local/lib $(LIBI)

over:			distclean all

##############################################################################
angif_lib.h:		angif.h

install_header:		angif.h
		-install -m 0755 -d $(INCI)
		test -d $(INCI) -a -w $(INCI) -a -x $(INCI)
		install -m 0644 angif.h $(INCI)

##############################################################################
angif_init_header.o:	angif_init_header.c angif_lib.h angif.h
		gcc $(COPT) $(PIPE) -c $(INCD) -o angif_init_header.o angif_init_header.c

angif_init_image.o:	angif_init_image.c angif_lib.h angif.h
		gcc $(COPT) $(PIPE) -c $(INCD) -o angif_init_image.o angif_init_image.c

angif_init_stream.o:	angif_init_stream.c angif_lib.h angif.h
		gcc $(COPT) $(PIPE) -c $(INCD) -o angif_init_stream.o angif_init_stream.c

angif_init_text.o:	angif_init_text.c angif_lib.h angif.h
		gcc $(COPT) $(PIPE) -c $(INCD) -o angif_init_text.o angif_init_text.c

angif_put_animate.o:	angif_put_animate.c angif_lib.h angif.h
		gcc $(COPT) $(PIPE) -c $(INCD) -o angif_put_animate.o angif_put_animate.c

angif_put_comment.o:	angif_put_comment.c angif_lib.h angif.h
		gcc $(COPT) $(PIPE) -c $(INCD) -o angif_put_comment.o angif_put_comment.c

angif_put_header.o:	angif_put_header.c angif_lib.h angif.h
		gcc $(COPT) $(PIPE) -c $(INCD) -o angif_put_header.o angif_put_header.c

angif_put_image.o:	angif_put_image.c angif_lib.h angif.h
		gcc $(COPT) $(PIPE) -c $(INCD) -o angif_put_image.o angif_put_image.c

angif_put_index.o:	angif_put_index.c angif_lib.h angif.h
		gcc $(COPT) $(PIPE) -c $(INCD) -o angif_put_index.o angif_put_index.c

angif_put_linear.o:	angif_put_linear.c angif_lib.h angif.h
		gcc $(COPT) $(PIPE) -c $(INCD) -o angif_put_linear.o angif_put_linear.c

angif_put_prelzw.o:	angif_put_prelzw.c angif_lib.h angif.h
		gcc $(COPT) $(PIPE) -c $(INCD) -o angif_put_prelzw.o angif_put_prelzw.c

angif_put_rgb.o:	angif_put_rgb.c angif_lib.h angif.h
		gcc $(COPT) $(PIPE) -c $(INCD) -o angif_put_rgb.o angif_put_rgb.c

angif_put_text.o:	angif_put_text.c angif_lib.h angif.h
		gcc $(COPT) $(PIPE) -c $(INCD) -o angif_put_text.o angif_put_text.c

angif_put_trailer.o:	angif_put_trailer.c angif_lib.h angif.h
		gcc $(COPT) $(PIPE) -c $(INCD) -o angif_put_trailer.o angif_put_trailer.c

angif_set_file.o:	angif_set_file.c angif_lib.h angif.h
		gcc $(COPT) $(PIPE) -c $(INCD) -o angif_set_file.o angif_set_file.c

angif_set_function.o:	angif_set_function.c angif_lib.h angif.h
		gcc $(COPT) $(PIPE) -c $(INCD) -o angif_set_function.o angif_set_function.c

angif_set_memory.o:	angif_set_memory.c angif_lib.h angif.h
		gcc $(COPT) $(PIPE) -c $(INCD) -o angif_set_memory.o angif_set_memory.c

angif_take_memory.o:	angif_take_memory.c angif_lib.h angif.h
		gcc $(COPT) $(PIPE) -c $(INCD) -o angif_take_memory.o angif_take_memory.c

angif_write.o:	angif_write.c angif_lib.h angif.h
		gcc $(COPT) $(PIPE) -c $(INCD) -o angif_write.o angif_write.c

##############################################################################
library:		libangif.so.1.0.0

libangif.so.1.0.0:	$(FUNS)
		gcc -shared -Wl,-soname,libangif.so.1 -o libangif.so.1.0.0 $(FUNS) $(LIBL)
		ln -sf libangif.so.1.0.0 libangif.so.1
		ln -sf libangif.so.1 libangif.so

install_library:	libangif.so.1.0.0
		-install -m 0755 -d $(LIBI)
		test -d $(LIBI) -a -w $(LIBI) -a -x $(LIBI)
		install -m 0644 libangif.so.1.0.0 $(LIBI)
		ln -sf libangif.so.1.0.0 $(LIBI)/libangif.so.1
		ln -sf libangif.so.1 $(LIBI)/libangif.so

##############################################################################
programs:		gifscan pnmtogif rawtogif solidgif

gifscan:		angif.h
		gcc $(COPT) $(PIPE) $(INCD) $(LIBD) -o gifscan gifscan.c $(BINL)

pnmtogif:		angif.h
		gcc $(COPT) $(PIPE) $(INCD) $(LIBD) -o pnmtogif pnmtogif.c $(BINL) $(PNML)

rawtogif:		angif.h
		gcc $(COPT) $(PIPE) $(INCD) $(LIBD) -o rawtogif rawtogif.c $(BINL)

solidgif:		angif.h
		gcc $(COPT) $(PIPE) $(INCD) $(LIBD) -o solidgif solidgif.c $(BINL)

install_programs:	gifscan pnmtogif rawtogif solidgif
		-install -m 0755 -d $(BINI)
		test -d $(BINI) -a -w $(BINI) -a -x $(BINI)
		install -m 0755 gifscan pnmtogif rawtogif solidgif $(BINI)

##############################################################################
test:			pnmtogif solidgif
		./pnmtogif money.ppm > money.gif
		./solidgif 1 1 -1 > transdot.gif

##############################################################################
clean:
		-sh -c 'rm -f *.cgi'
		-sh -c 'rm -f *.cpp'
		-sh -c 'rm -f *.exe'
		-sh -c 'rm -f *.i'
		-sh -c 'rm -f *.o'
		-sh -c 'rm -f *.s'
		-sh -c 'rm -f *~'
		-sh -c 'rm -f #*#'
		-sh -c 'rm -f *.OLD'
		-sh -c 'rm -f core'
		-sh -c 'rm -f *.gif'

distclean:		clean
		-sh -c 'rm -f gifscan pnmtogif rawtogif solidgif'
		-sh -c 'rm -f libangif.so libangif.so.1 libangif.so.1.0.0'

virgin:			distclean

permissions:
		-sh -c 'chmod go=u-w M* [a-z]*'

##############################################################################
