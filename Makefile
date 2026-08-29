# C Compiler
CC := gcc

# Program Name
TARGET := cool

#The Directories, Source, Includes, Objects, Binary
SRCDIRS   := src src/lexer src/library src/parser
INCDIR    := src/inc
BUILDDIR  := obj
TARGETDIR := bin

#Includes Flag
INC       := -I$(INCDIR)

SOURCES   := $(wildcard $(addsuffix /*.c, $(SRCDIRS)))
OBJECTS   := $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SOURCES))

#Defauilt Make
all: $(TARGET)

#Remake
remake: cleaner all

#Clean only Objects
clean:
	@$(RM) -r $(BUILDDIR)/*

#Full Clean, Objects and Binaries
cleaner: clean
	@$(RM) -r $(TARGETDIR)/*

#Link
$(TARGET): $(OBJECTS)
	$(CC) $(INC) -o $(TARGETDIR)/$(TARGET) $^

#Compile
$(BUILDDIR)/%.o : $(SRCDIRS)/%.c
	$(CC) $(INC) -c $< -o $@

#Non-File Targets
.PHONY: all remake clean cleaner resources
