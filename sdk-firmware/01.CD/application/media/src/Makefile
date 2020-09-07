CC ?= gcc
LIB_VERSION = 1.0

CFLAGS ?= -O3 -Wall -fPIC
QUIRC_CFLAGS = -Ilib $(CFLAGS)
LIB_OBJ = \
    ../lib/decode.o \
    ../lib/identify.o \
    ../lib/quirc.o \
    ../lib/version_db.o \
    ../lib/dbgutil.o \

# all: libquirc.so inspect

inspect: ../lib/dbgutil.o inspect.o ../lib/libquirc.a
	$(CC) -o $@ ../lib/dbgutil.o inspect.o ../lib/libquirc.a $(LDFLAGS) -lm  ../../../3rd.party/lib/libjpeg.so
libquirc.a: $(LIB_OBJ)
	rm -f $@
	ar cru $@ $(LIB_OBJ)
	ranlib $@

.c.o:
	$(CC) $(QUIRC_CFLAGS) -o $@ -c $<



clean:
	rm -f *.o
	rm -f *.lo
	rm -f libquirc.a
	rm -f libquirc.so.$(LIB_VERSION)
	rm -f inspect