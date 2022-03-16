CC = gcc
CFLAGS = -Wall -Wextra -I./$(INCLDIR)

PROG = $(BUILDDIR)/snake.elf
MODULES = shapegen field main
OBJ = $(addsuffix .o, $(MODULES))
SRC = $(addsuffix .cpp, $(MODULES))

BUILDDIR = build
SRCDIR = src
INCLDIR = inlude

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
