CC = clang

CFLAGS = -Wall -Wextra -Iinclude

LDFLAGS = -Llib -lGLEW -lGL -lGLU -lm -lglfw

SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.c)

OBJDIR = obj
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))

TARGET = bezier_editor

all: $(TARGET)

$(TARGET): $(OBJECTS)
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

.PHONY: all clean run

