override NAME		:=	webserv

override INCLUDE_DIR	:=	includes/
override TEMPLATE_DIR	:=	includes/
override SOURCE_DIR		:=	src/
INCLUDES				:=
TEMPLATES				:=
SOURCES					:=	main \
							utils \
							Logger \
							Server \
							FileCacheManager \
							http_utils \
							HttpConnection \
							HttpRequest \
							HttpResponse
override INCLUDE		:=	$(addprefix $(INCLUDE_DIR), $(addsuffix .hpp, $(INCLUDES)))
override TEMPLATE		:=	$(addprefix $(TEMPLATE_DIR), $(addsuffix .tpp, $(TEMPLATES)))
override SOURCE			:=	$(addprefix $(SOURCE_DIR), $(addsuffix .cpp, $(SOURCES)))

CPPFLAGS	:=	-Wall -Wextra -Werror -MD -Wshadow -g3 -std=c++98 -pthread
TURBO_FLAGS	:=	-O3 -ffast-math -finline-functions -m64 -pthread
TURBO		?=	0

ifeq ($(TURBO),1)
CPPFLAGS	+= $(TURBO_FLAGS)
endif

override BUILD_DIR	:=	.dist/
override OBJ		:=	$(patsubst $(SOURCE_DIR)%.cpp, $(BUILD_DIR)%.o, $(SOURCE))
override DEPS		:=	$(patsubst %.o, %.d, $(OBJ))
override DIRS		:=	$(sort $(dir $(NAME) $(OBJ) $(DEPS)))

MAKEFLAGS	:=	--no-print-directory
GCC			:=	c++
RM			:=	rm -rf
VG			:=	valgrind
VGFLAGS		:=	--leak-check=full --show-leak-kinds=all --track-origins=yes --show-mismatched-frees=yes --track-fds=yes --trace-children=yes

.PHONY: all
all: $(NAME)

$(NAME): $(OBJ)
	$(GCC) $(CPPFLAGS) $(OBJ) -o $(NAME)

$(BUILD_DIR)%.o: $(SOURCE_DIR)%.cpp $(INCLUDE) $(TEMPLATE) | $(DIRS)
	$(GCC) $(CPPFLAGS) -c -I$(INCLUDE_DIR) -I$(TEMPLATE_DIR) $< -o $@

$(DIRS):
	@mkdir -p $@

.PHONY: bonus
bonus:
	@$(MAKE) TURBO=1

.PHONY: clean
clean:
	$(RM) $(BUILD_DIR)

.PHONY: fclean
fclean: clean
	$(RM) $(NAME)

.PHONY: re
re: fclean
	$(MAKE)

.PHONY: run
run: all
	@clear
	./$(NAME) samples/webserv.conf

.PHONY: vg
vg: all
	@clear
	$(VG) $(VGFLAGS) ./$(NAME) samples/webserv.conf

-include $(DEPS)