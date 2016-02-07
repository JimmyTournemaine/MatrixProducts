CC=gcc
GEN=generator
PROD=product_v1
PROD2=product_fair

# Darwin systems don't support sched functions
# But Linux needs -pthread link.

OS := $(shell uname -s)
ifneq ($(OS),Darwin)
    all: $(GEN) $(PROD) $(PROD2)
    TH=-pthread
else
    all: $(GEN) $(PROD)
endif

$(GEN): matrix.o generator.o
	$(LINK.c) matrix.o generator.o -o $(GEN)

$(PROD): matrix.o product.o product_v1.o
	$(LINK.c) $(TH) matrix.o product.o product_v1.o -o $(PROD)

$(PROD2): matrix.o product.o product_fair.o
	$(LINK.c) $(TH) matrix.o product.o product_fair.o -o $(PROD2)

matrix.o: matrix.c matrix.h
	$(COMPILE.c) matrix.c -o matrix.o

generator.o: generate.c generate.h
	$(COMPILE.c) generate.c -o generator.o

product.o: product.c product.h
	$(COMPILE.c) product.c -o product.o

clean:
	rm -f *.o

mrproper: clean
	rm -f $(GEN)
	rm -f $(PROD)
	rm -f $(PROD2)
	rm -f *.log
	rm -f *.txt