demo2: demo2.cpp draw.cpp display.cpp gpio.o
	g++ -o demo2 demo2.cpp draw.cpp display.cpp gpio.o -std=gnu++0x -lpthread -g

demo1: demo.cpp display.cpp gpio.o
	g++ -o demo1 demo.cpp display.cpp gpio.o -std=gnu++0x -lpthread -g

demo0: demo0.cpp gpio.o
	g++ -o demo0 demo0.cpp gpio.o -std=gnu++0x -lpthread -g

gpio.o: gpio.c
	gcc -c gpio.c

clean:
	rm -f *.o demo0 demo1
