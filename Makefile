all:
	gcc server.cpp -o server
	gcc client.cpp -o client

clean:
	rm server client

