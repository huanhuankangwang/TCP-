all:packet.o client.o server.o fileoperation.o parser_list_file.o
	gcc -o client client.o packet.o fileoperation.o -lpthread
	gcc -o server server.o packet.o fileoperation.o parser_list_file.o -lpthread
packet_manage.o:packet_manage.c packet_manage.h
	gcc -c packet_manage.c
packet.o:packet.c packet.h
	gcc -c packet.c
parser_list_file.o:parser_list_file.c parser_list_file.h
	gcc -c parser_list_file.c
client.o:client.c packet.h config.h
	gcc -c client.c
server.o:server.c packet.h config.h
	gcc -c server.c
fileoperation:fileoperation.c fileoperation.h
	gcc -c fileoperation.c
	
file:
	gcc -o file fileoperation.c
	
clean:
	rm client server *.o *.bak write/* -rf