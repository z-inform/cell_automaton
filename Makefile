CC = gcc
CFLAGS = -Wall -Wextra -I./$(INCLDIR) -fsanitize=address -g -O0

PROG = $(BUILDDIR)/cell.elf
MODULES = field main
OBJ = $(addsuffix .o, $(MODULES))
SRC = $(addsuffix .cpp, $(MODULES))

BUILDDIR = build
SRCDIR = src
INCLDIR = include

all : $(PROG)

$(PROG) : $(addprefix $(BUILDDIR)/, $(OBJ))
	    $(CC) $(CFLAGS) $^ -o $@

$(addprefix $(BUILDDIR)/, $(OBJ)) : $(BUILDDIR)/%.o : $(SRCDIR)/%.c $(INCLDIR)/%.h
	    $(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
	.PHONY: depend

clean :
	    $(RM) $(BUILDDIR)/*.o

depend :
	    $(CC) $(CFLAGS) $(addprefix $(SRCDIR)/, $(SRC)) -MM > $(BUILDDIR)/.depend

-include $(BUILDDIR)/.depend
