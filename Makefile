CC = g++
CFLAGS = -Wall -Wextra -I./$(INCLDIR) -g -O0 -fsanitize=address
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

PROG = $(BUILDDIR)/cell.elf
MODULES = field main draw generator
OBJ = $(addsuffix .o, $(MODULES))
SRC = $(addsuffix .cpp, $(MODULES))

BUILDDIR = build
SRCDIR = src
INCLDIR = include

all : $(PROG)

$(PROG) : $(addprefix $(BUILDDIR)/, $(OBJ))
		$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(addprefix $(BUILDDIR)/, $(OBJ)) : $(BUILDDIR)/%.o : $(SRCDIR)/%.c $(INCLDIR)/%.h
		$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
	.PHONY: depend

clean :
	    $(RM) $(BUILDDIR)/*.o

depend :
	    $(CC) $(CFLAGS) $(addprefix $(SRCDIR)/, $(SRC)) -MM > $(BUILDDIR)/.depend

-include $(BUILDDIR)/.depend
