CC=gcc

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

# Name of executable, source files, object files
EXE := $(CURDIR)/server
SRC = $(filter-out src/queue.c, $(wildcard $(SRC_DIR)/*.c))
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Flags
CPPFLAGS := -Iinclude -MMD -MP    # -I is a preprocessor flag, not a compiler flag
CFLAGS   := -Wall -O3             # some warnings about bad code
LDFLAGS  := -Llib                 # -L is a linker flag
LDLIBS := -lcrypto -lpthread

# Default target
all: $(EXE)
# Make will think we want to actually create a file or folder named all (and clean, see below),
# so let's tell him this is not a real target
.PHONY: all clean

# List the prerequisites for building your executable
# $(CC) is a built-in variable already containing what we need when compiling and linking in C
# To avoid linker errors, it is strongly recommended to put $(LDFLAGS) before our object files and $(LDLIBS) after
# $(CPPFLAGS) and $(CFLAGS) are useless here, the compilation phase is already over, it is the linking phase here
# $(BIN_DIR) might not exist yet so the call to the compiler might fail - > use mkdir
$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BIN_DIR):
	mkdir -p $@

# Since the source and object files don't share the same prefix,
# we need to tell make exactly what to do since its built-in rules don't cover our specific case:
# $(OBJ_DIR) might not exist yet so the call to the compiler might fail -> use mkdir
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

# Simple rule to clean the build artifacts
clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR) # The @ disables the echoing of the

# Include the automatic dependency generation (.d files to .o files with MakeFile rules for us to use)
-include $(OBJ:.o=.d) # The dash silences errors when files don't exist (yet)