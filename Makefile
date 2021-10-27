all:
	-mkdir bin
	gcc -g -ludev -Wall -Wextra -pedantic *.c -o bin/moused
install:
	-install -m 644 moused.service /etc/systemd/system/
	-install -m 755 bin/moused /usr/bin
