# NOTE: add path to Eigen and libigl
INC_DIRS := /usr/local/include/eigen3 /Users/sagejenson/dev/libigl/include /Users/sagejenson/dev/cellgrow_test/external

NANOGUI_DIRS = /Users/sagejenson/dev/libigl/external/nanogui/include /Users/sagejenson/dev/libigl/external/nanogui/ext/nanovg/src
NANOGUI_DIRS += /Users/sagejenson/dev/libigl/external/nanogui/ext/glfw/include /Users/sagejenson/dev/libigl/external/nanogui/src
# INC_DIRS += $(NANOGUI_DIRS)

CC=clang++
TARGET_EXEC ?= simulation

BUILD_DIR ?= bin
SRC_DIRS ?= src
SRC_DIRS += external

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_FLAGS := $(addprefix -I,$(INC_DIRS))
LDFLAGS = $(INC_FLAGS)

CPPFLAGS ?= $(INC_FLAGS) -O3 -std=c++11 -lglfw -framework OpenGL -framework GLUT -Wno-undefined-inline -MMD -MP

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(CPPFLAGS) $(OBJS) -o $@

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
