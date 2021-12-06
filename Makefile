# Copyright 2020 Baiatu Bianca <baiatu.daniela2001@gmail.com>

# compiler setup
CC=gcc
CFLAGS=-Wall -Wextra -std=c99

# define targets
TARGETS= tema3

build: $(TARGETS)

tema3: pics.c
	$(CC) $(CFLAGS) pics.c -o image_editor

pack:
	zip -FSr 313CA_BiancaBaiatu_Tema2.zip README Makefile *.c *.h

clean:
	rm -f $(TARGETS)

.PHONY: pack clean
