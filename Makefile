all: boot_menu.o bootenv.o main.o keypress
	cc boot_menu.o bootenv.o main.o -o main
keypress: keypress.o
%.c:%.o

clean:
	rm -f boot_menu.o bootenv.o main.o main keypress.o keypress
