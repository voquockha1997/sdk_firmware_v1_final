CC ?= gcc
CFLAGS ?= -O3 -Wall -fPIC
QUIRC_CFLAGS = -I. $(CFLAGS)
LIB_OBJ = \
	StateMachine.o \
	Led.o

libStateMachine.a: $(LIB_OBJ)
	rm -f $@
	ar cru $@ $(LIB_OBJ)
	ranlib $@

.c.o:
	$(CC) $(QUIRC_CFLAGS) -o $@ -c $<