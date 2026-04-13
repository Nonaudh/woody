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

LIB = $(DLIB)/libft.a

CDIR = src/c

SRC = $(CDIR)/main.c $(CDIR)/utils.c $(CDIR)/payload.c $(CDIR)/utils_elf.c $(CDIR)/get_injection_address.c \
		$(CDIR)/injection.c $(CDIR)/copy.c $(CDIR)/patch_payload.c $(CDIR)/xor.c $(CDIR)/encryption_text.c $(CDIR)/table_huafman.c $(CDIR)/verif_encryption.c $(CDIR)/init_elf.c

SRCPAYLOAD = src/asm/payload.s

NAME = woody_woodpacker

all : $(NAME)

DBUILD = build/

OBJ = $(addprefix $(DBUILD),$(SRC:%.c=%.o))

$(DBUILD)%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(NAME) : payload $(LIB) $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDE) $(OBJ) $(LIBRARIES) -o $(NAME)

$(LIB) :
	@make -C $(DLIB)

clean :
	@make clean -C $(DLIB)
	@rm -rf $(DBUILD)

fclean : clean
	@rm -rf $(NAME)
	@rm -rf $(LIB)
	@rm -f payload

re : fclean $(NAME)

payload :
	nasm -f elf64 $(SRCPAYLOAD) -o payload.o
	objcopy -O binary payload.o payload.bin
	xxd -p payload.bin | tr -d '\n' > payload
	rm payload.o payload.bin

.PHONY: all clean fclean re
