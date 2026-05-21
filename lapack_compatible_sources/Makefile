# Defintions of variables.

CC      = icc
CCFLAGS = -O2 -qopenmp -DUSE_MKL
LD      = icc
LDFLAGS = -O2 -qopenmp

MKL_DIR = /work/opt/local/x86_64/cores/intel/2023.2.0/mkl/latest

CCFLAGS += -I$(MKL_DIR)/include

MKL_LIBS = -L$(MKL_DIR)/lib/intel64 \
           -lmkl_intel_lp64 -lmkl_intel_thread -lmkl_core \
           -lpthread -lm

# Defintions of rules.
simple_test.x : simple_test.o NoFLA_HQRRP_WY_blk_var4.o
	$(LD) $(LDFLAGS) \
		-o simple_test.x \
		simple_test.o \
		NoFLA_HQRRP_WY_blk_var4.o \
		$(MKL_LIBS)

simple_test.o : simple_test.c
	$(CC) $(CCFLAGS) -c simple_test.c

%.o : %.c
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	rm -f a.out *.x *.o *~ core
