CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -O2 -I./include
LDFLAGS  =

# Affichage temps réel. Désactivable quand SFML n'est pas installé :
#   make USE_SFML=0
USE_SFML ?= 1
ifeq ($(USE_SFML), 1)
	CXXFLAGS += -DUSE_SFML
	LDFLAGS  += -lsfml-graphics -lsfml-window -lsfml-system
endif

# Parallélisation du rendu. Chaque pixel étant indépendant, la boucle sur les
# lignes se parallélise sans synchronisation. Désactivable :
#   make USE_OPENMP=0
USE_OPENMP ?= 1
ifeq ($(USE_OPENMP), 1)
	CXXFLAGS += -fopenmp
	LDFLAGS  += -fopenmp
endif

SRC_DIR   = src
BUILD_DIR = build
NAME      = retsu

SRCS = $(shell find $(SRC_DIR) -name "*.cpp")
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

# -MMD -MP génère les dépendances d'en-têtes : toucher un .hpp recompile ce qui
# l'inclut, au lieu de laisser un build silencieusement incohérent.
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

clean:
	rm -rf $(BUILD_DIR)
	find . -type f -name "*.ppm" -delete

fclean: clean
	rm -f $(NAME)

re: fclean all

# Rend les trois scènes fournies.
demo: $(NAME)
	./$(NAME) scenes/demo_scene.rts
	./$(NAME) scenes/spheres_hd.rts
	./$(NAME) scenes/architecture_hd.rts

.PHONY: all clean fclean re demo
