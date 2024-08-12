CC := gcc
# CFLAGS := -fsanitize=leak,address -g -lm
CFLAGS := -O3
SRCDIR := src
BUILDDIR := build
BINDIR := bin
TARGET := $(BINDIR)/program
SRCEXT := c
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
DEPS := $(OBJECTS:.o=.d)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $^ -o $(TARGET) -lm

-include $(DEPS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

clean:
	@echo "Cleaning...";
	rm -rf $(BUILDDIR) $(BINDIR)

run: bin/program
	@echo "\033[34m./bin/program\033[0m"
	@./bin/program

.PHONY: clean run

