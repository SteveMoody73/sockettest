all:
	g++ server.cpp -o server -lpugixml
	gcc client.cpp -o client

clean:
	rm server client

