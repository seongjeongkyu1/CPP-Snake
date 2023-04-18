CC = g++
CCFLAGS = -g

snake: snake.o
	$(CC) $(CCFLAGS) -o snake snake.o -lncursesw

clean:
	rm -f *.o

%.o : %.cpp
	$(CC) $(CCFLAGS) -c $<

% : %.cpp
	$(CC) $(CCFLAGS) -o $@ $<
