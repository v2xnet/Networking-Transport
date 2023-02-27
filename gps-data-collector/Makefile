all: gps_position_collector

gps_position_collector: gps_position_collector.o
	gcc -o gps_position_collector gps_position_collector.o -lgps

gps_position_collector.o: gps_position_collector.c
	gcc -o gps_position_collector.o -c gps_position_collector.c -lgps -W -Wall -ansi -pedantic

clean:
	rm -rf *.o

mrproper: clean
	rm -rf gps_position_collector

