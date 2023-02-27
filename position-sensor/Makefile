all: position_sensor

position_sensor: position_sensor.o
	gcc -o position_sensor position_sensor.o -lgps

position_sensor.o: position_sensor.c
	gcc -o position_sensor.o -c position_sensor.c -lgps -W -Wall -ansi 

clean:
	rm -rf *.o

mrproper: clean
	rm -rf position_sensor
install:
	cp position_sensor /usr/bin/
