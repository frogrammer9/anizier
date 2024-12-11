CC = clang

CFLAGS = -Wall -Wextra -Iinclude

LDFLAGS = -Llib -lGLEW -lGL -lGLU -lm -lglfw

SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.c)

OBJDIR = obj
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))

TARGET = anizier

all: $(TARGET)

$(TARGET): clean $(OBJECTS)
	@echo "Linking..."
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	@mkdir -p $(OBJDIR)

clean:
	@echo "Cleaning..."
	rm -rf $(OBJDIR) $(TARGET)

run: all
	@clear
	@./$(TARGET)
	@echo "Program exited with code $$?"

gdb: all
	@gdb $(TARGET)

.PHONY: all clean run gdb

