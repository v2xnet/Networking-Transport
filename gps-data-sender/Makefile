all: gps_position_sender

gps_position_sender: gps_position_sender.o
	gcc  gps_position_sender.o -o gps_position_sender -lc -W -Wall -ansi

gps_position_sender.o: gps_position_sender.c
	gcc  -c gps_position_sender.c  

clean:
	rm -rf *.o

mrproper: clean
	rm -rf gps_position_sender

