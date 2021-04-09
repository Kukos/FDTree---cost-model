CC := gcc

CCWARNINGS := 	-Wall -Wextra -pedantic -Wcast-align \
				-Winit-self -Wlogical-op -Wmissing-include-dirs \
				-Wredundant-decls -Wshadow -Wstrict-overflow=5 -Wundef  \
				-Wwrite-strings -Wpointer-arith -Wmissing-declarations \
				-Wuninitialized -Wold-style-definition -Wstrict-prototypes \
				-Wmissing-prototypes -Wswitch-default -Wbad-function-cast \
				-Wnested-externs -Wconversion -Wunreachable-code

CFLAGS := -std=gnu99 $(CCWARNINGS) -O3

PROJECT_DIR := $(shell pwd)

IDIR := $(PROJECT_DIR)/include
SDIR := $(PROJECT_DIR)/src

SRCS := $(wildcard $(SDIR)/*.c)
OBJS := $(SRCS:%.c=%.o)
DEPS := $(wildcard $(IDIR)/*.h)

LIBS := -lm

EXEC := main.out

ifeq ("$(origin V)", "command line")
  VERBOSE = $(V)
endif

ifndef VERBOSE
  VERBOSE = 0
endif

ifeq ($(VERBOSE),1)
  Q =
else
  Q = @
endif

define print_info
	$(if $(Q), @echo "$(1)")
endef

define print_make
	$(if $(Q), @echo "[MAKE]    $(1)")
endef

define print_cc
	$(if $(Q), @echo "[CC]      $(1)")
endef

define print_bin
	$(if $(Q), @echo "[BIN]     $(1)")
endef

all: $(EXEC)

%.o: %.c
	$(call print_cc, $<)
	$(Q)$(CC) $(CFLAGS) -I$(IDIR) -c $< -o $@

$(EXEC): $(OBJS)
	$(call print_bin, $@)
	$(Q)$(CC) $(CFLAGS) -L$(LDIR) -I$(IDIR) $(OBJS) $(LIBS) -o $@

clean:
	$(call print_info,Cleaning)
	$(Q)rm -f $(OBJS)
	$(Q)rm -f $(EXEC)
	$(Q)rm -f *.png
	$(Q)rm -f *.txt
	$(Q)rm -f *.pdf