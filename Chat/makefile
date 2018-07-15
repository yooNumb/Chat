server : *.cpp
	g++ -o  $@ $^  -ljson -lpthread -levent -lmysqlclient -L/usr/lib/mysql -L/usr/local/lib -lhiredis -g
clean:
	rm -rf server
