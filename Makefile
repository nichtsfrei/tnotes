TEST_LDFLAGS = -lcgreen
OUTPUT_OPTION = -Wall -MMD -MP -o $@
CFLAGS = -Wall


ifeq ($(out),)
    out := /usr/local
endif

src = $(wildcard src/*.c)
src_o = $(src:.c=.o)
test_src = $(wildcard tests/*.c)
tobj = $(filter-out src/main.o,$(src_o)) $(test_src:.c=.o)
deps = $(tobj:.o=.d)
-include ${deps}

.PHONY: clean

all: tnotes

tnotes: $(src_o)
	$(CC) $(OUTPUT_OPTION) $^

tests/all: $(tobj)
	$(CC) -o $@ $^ $(TEST_LDFLAGS)

clean:
	rm -f $(tobj) $(deps) tests/all tnotes

install:
	install -d $(out)/bin
	install -p ./tnotes $(out)/bin/tnotes
