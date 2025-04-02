CFLAGS=-MD -Werror -Wall -Wextra -Wconversion

all: json.o json-test

json.o: ../json.c
	gcc -o $@ -O2 $(CFLAGS) -c $<

%.g.o: ../%.c
	gcc -o $@ -g $(CFLAGS) -c $<

json-test: json-test.g.o json.g.o
	gcc -o $@ -g $(CFLAGS) $^

.PHONY: run
.PHONY: debug
.PHONY: clean

run: json-test
	./$<

debug: json-test
	gdb $<

clean:
	rm -f json-test *.o *.d
  
-include $(wildcard *.d)
