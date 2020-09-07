CC ?= gcc
LIB_VERSION = 1.0

CFLAGS ?= -O3 -Wall -fPIC
QUIRC_CFLAGS = -I../../../3rd.party/include $(CFLAGS)
LIB_OBJ = \
    decode.o \
    identify.o \
    quirc.o \
    version_db.o \
    dbgutil.o \

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