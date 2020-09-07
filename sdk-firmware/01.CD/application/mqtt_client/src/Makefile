CC=gcc
CFLAGS  = -I.
LIBS    = ../../../3rd.party/lib/

run_sub: ex_mqtt.o
	$(CC) -L$(LIBS) -o run_sub ex_mqtt.o -lpaho-mqtt3c -lcjson -lpthread


ex_mqtt.o: ex_mqtt.c
	$(CC) -c ex_mqtt.c ex_mqtt.c
clean:
	rm ex_mqtt.o  run_sub
