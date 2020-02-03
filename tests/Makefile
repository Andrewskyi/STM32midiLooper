# Makefile for gtest examples

GOOGLE_TEST_LIB = gtest
GOOGLE_TEST_INCLUDE = /usr/local/include
GOOGLE_TEST_LIB_DIR = /usr/src/gtest

SOURCES = MidiEventMasterBuffer.cpp MidiEventSlaveBuffer.cpp
TESTS = MidiEventMasterBufferTests.cpp MidiEventSlaveBufferTests.cpp

G++ = g++
G++_FLAGS = -g -std=gnu++11 -c -Wall -I $(GOOGLE_TEST_INCLUDE) -I ../src/looper
LD_FLAGS = -L $(GOOGLE_TEST_LIB_DIR) -l $(GOOGLE_TEST_LIB) -l pthread
BUILD_DIR = obj

OBJECTS = $(BUILD_DIR)/GtestMain.o \
  $(TESTS:%.cpp=$(BUILD_DIR)/%.o) \
  $(SOURCES:%.cpp=$(BUILD_DIR)/%.o)
DEPS = $(OBJECTS:%.o=%.d)
TARGET = tests

all: $(TARGET)
	-./$(TARGET)

$(TARGET): $(OBJECTS)
	g++ -o $(TARGET) $(OBJECTS) $(LD_FLAGS)

-include $(DEPS)

$(BUILD_DIR)/%.o : src/%.cpp
	$(G++) $(G++_FLAGS) -MMD -o $@ $<

$(BUILD_DIR)/%.o : ../src/looper/%.cpp
	$(G++) $(G++_FLAGS) -MMD -o $@ $<

clean:
	-rm -f $(TARGET) $(OBJECTS) $(BUILD_DIR)/*
                    
.PHONY: all clean
