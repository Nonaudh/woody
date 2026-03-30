# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: cblonde <marvin@42.fr>                     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/10/23 16:57:56 by cblonde           #+#    #+#              #
#    Updated: 2024/11/25 08:12:17 by cblonde          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = cc

CFLAGS = -g #-Werror -Wextra -Wall

DLIB = lib/libft

INCLUDE = -Iinc -I$(DLIB)

LIBRARIES = -L$(DLIB) -lft

DSRC64 = src/64
DSRC32 = src/32
DSRCSHARED = src/shared

LIB = $(DLIB)/libft.a

SRC = src/main.c src/utils.c $(DSRC64)/woody.c $(DSRC64)/utils_elf.c $(DSRC64)/insert_payload.c

SRCPAYLOAD = src/asm/payload.s

NAME = woody_woodpacker

all : $(NAME)

DBUILD = build/

OBJ = $(addprefix $(DBUILD),$(SRC:%.c=%.o))

$(DBUILD)%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(NAME) : $(LIB) $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDE) $(OBJ) $(LIBRARIES) -o $(NAME)

$(LIB) :
	@make -C $(DLIB)

clean :
	@make clean -C $(DLIB)
	@rm -rf $(DBUILD)

fclean : clean
	@rm -rf $(NAME)
	@rm -rf $(LIB)

re : fclean $(NAME)

payload :
	nasm -f elf64 $(SRCPAYLOAD) -o payload.o
	objcopy -O binary payload.o payload.bin
	xxd -p payload.bin | tr -d '\n' | sed 's/../\\x&/g' > payload
# rm payload.o payload.bin

# copy :
# 	mkdir -p src/32
# 	cp src/64/* src/32/
# 	cp inc/nm64.h inc/nm32.h
# 	sed -i "s/64/32/g" src/32/*
# 	sed -i "s/64/32/g" inc/nm32.h

.PHONY: all clean fclean re
