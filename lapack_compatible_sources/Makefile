
# Defintions of variables.

CC      = gcc
CCFLAGS = -O -fopenmp
LD      = gcc
LDFLAGS = -O -fopenmp

LAPACK_DIR = /opt/AMD/libflame/mt
BLAS_DIR   = /opt/AMD/blis/mt

CCFLAGS += -I$(LAPACK_DIR)/include -I$(BLAS_DIR)/include/blis

# Defintions of rules.

simple_test.x : simple_test.o NoFLA_HQRRP_WY_blk_var4.o
	$(LD) $(LDFLAGS) \
            -o simple_test.x \
            simple_test.o \
            NoFLA_HQRRP_WY_blk_var4.o -lm  \
            -L$(LAPACK_DIR)/lib -lflame-mt \
            -L$(BLAS_DIR)/lib -lblis-mt \
            -lgfortran

simple_test.o : simple_test.c
	$(CC) $(CCFLAGS) -O -fopenmp -c simple_test.c

%.o : %.c
	$(CC) $(CCFLAGS) -c $< -o $@

clean: 
	rm -f a.out *.x *.o *~ core

