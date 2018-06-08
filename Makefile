
C=gcc
CFLAGS += -Wall -Wno-unused-result -pedantic -Ofast -march=native -std=c99 -g -ggdb -fopenmp
CLIBS = -lm
INCDIRS = -I./src
LIBDIRS =

OBJDIR = obj
SRCDIR = src

DEPS = $(shell find $(SRCDIR) -name '*.h')
SRCS = $(shell find $(SRCDIR) -name '*.c')
OBJS = $(patsubst $(SRCDIR)%.c, $(OBJDIR)%.o, $(SRCS))

all: $(OBJDIR) ipot_solver

$(OBJDIR):
	mkdir -p $(OBJDIR)

ipot_solver: $(OBJS) $(DEPS)
	$(C) $(CFLAGS) $(INCDIRS) $(LIBDIRS) -o ipot_solver $(OBJS) $(CLIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(C) $(CFLAGS) $(INCDIRS) -c -o $@ $<

clean:
	rm $(OBJS) ipot_solver
	rmdir $(OBJDIR)
