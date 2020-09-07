CC ?= gcc
# LIBS    = ../../../3rd.party/lib/
state_machine: main.c 
	$(CC) -o state_machine main.c ../lib/libStateMachine.a ../../media/lib/libquirc.a -I. -lpthread -lm ../../../3rd.party/lib/libjpeg.so ../../../3rd.party/lib/libcurl.so

clean:
	rm state_machine
