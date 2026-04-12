CC = gcc
CFLAGS = -Wall -Wextra -Iincludes
LDFLAGS = -lsqlite3
TARGET = bin/aviation_app
SRCDIR = src
OBJDIR = build
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	mkdir -p bin
	$(CC) $^ -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) bin

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run