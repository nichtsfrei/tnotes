src = $(wildcard ../src/*.c)
test_src = $(wildcard *.c)
obj = $(src:.c=.o) $(test_src:.c=.o)

LDFLAGS = -lcgreen

.PHONY: clean

all_tests: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(obj) all_tests
