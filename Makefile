all:
	gcc -g tcpclient.c -o tcpclient
	gcc -g tcpserver.c -o tcpserver
	gcc -g multi-tcpserver.c -o multi-tcpserver -pthread

clean:
	rm tcpclient tcpserver multi-tcpserver
