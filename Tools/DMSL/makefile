# DMSL makefile

# Configurable parameters
TARGET		= libdmsl.so
JAVA_HOME 	= /usr/lib/jvm/java-6-sun-1.6.0.21
#

INCLUDE_DIR	= Include
SRC_DIR 	= Src
BUILD_DIR 	= Obj

UTIL_DIR 		= Utility
PROFILE_DIR 		= Profile
DECISIONMAKER_DIR 	= DecisionMaker
DMSL_DIR 		= DMSL
TESTER_DIR 		= Tester

JNI_INCLUDE = -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux

UTIL_INCLUDES 		= -I$(UTIL_DIR)/$(INCLUDE_DIR)
PROFILE_INCLUDES 	= $(UTIL_INCLUDES) -I$(PROFILE_DIR)/$(INCLUDE_DIR)
DECISIONMAKER_INCLUDES 	= $(UTIL_INCLUDES) $(PROFILE_INCLUDES) -I$(DECISIONMAKER_DIR)/$(INCLUDE_DIR)
DMSL_INCLUDES 		= $(UTIL_INCLUDES) $(PROFILE_INCLUDES) -I$(DECISIONMAKER_DIR)/$(INCLUDE_DIR)
TESTER_INCLUDES 	= $(UTIL_INCLUDES) $(PROFILE_INCLUDES) $(DECISIONMAKER_INCLUDES)

UTIL_BUILD_DIR 		= $(UTIL_DIR)/$(BUILD_DIR)
PROFILE_BUILD_DIR 	= $(PROFILE_DIR)/$(BUILD_DIR)
DECISIONMAKER_BUILD_DIR = $(DECISIONMAKER_DIR)/$(BUILD_DIR)
DMSL_BUILD_DIR 		= $(DMSL_DIR)/$(BUILD_DIR)
TESTER_BUILD_DIR 	= $(TESTER_DIR)/$(BUILD_DIR)

UTIL_FILES 	= 	$(UTIL_BUILD_DIR)/util.o
PROFILE_FILES 	= 	$(PROFILE_BUILD_DIR)/ExprValue.o 		\
			$(PROFILE_BUILD_DIR)/ManageActions.o		\
			$(PROFILE_BUILD_DIR)/Profile.o			\
			$(PROFILE_BUILD_DIR)/ProfileBisonParser.o	\
			$(PROFILE_BUILD_DIR)/ProfileScanner.o
DECISIONMAKER_FILES = 	$(DECISIONMAKER_BUILD_DIR)/DecisionMaker.o		\
			$(DECISIONMAKER_BUILD_DIR)/DecisionMakerBisonParser.o	\
			$(DECISIONMAKER_BUILD_DIR)/DecisionMakerScanner.o	\
			$(DECISIONMAKER_BUILD_DIR)/Expressions.o		\
			$(DECISIONMAKER_BUILD_DIR)/LibfuncExpressions.o		\
			$(DECISIONMAKER_BUILD_DIR)/ManageExpressions.o		\
			$(DECISIONMAKER_BUILD_DIR)/ManageStatements.o		\
			$(DECISIONMAKER_BUILD_DIR)/Statements.o

DMSL_FILES 	=	$(DMSL_BUILD_DIR)/dmsl_java_wrap.o
TESTER_FILES = $(TESTER_BUILD_DIR)/main.o
LIB_FILES = $(UTIL_FILES) $(PROFILE_FILES) $(DECISIONMAKER_FILES) $(DMSL_FILES)
BUILD_DIRS = mkdir $(UTIL_BUILD_DIR) mkdir $(PROFILE_BUILD_DIR) $(DECISIONMAKER_BUILD_DIR) $(DMSL_BUILD_DIR) $(TESTER_BUILD_DIR)

CC = g++
RM = rm
OBJ = .o
CFLAGS = -O1 -Wall -g

all: builddirs $(LIB_FILES) dynamiclink

builddirs:
	if test -d wibble; then; else mkdir $(UTIL_BUILD_DIR); fi
	if test -d wibble; then; else mkdir $(PROFILE_BUILD_DIR); fi
	if test -d wibble; then; else mkdir $(DECISIONMAKER_BUILD_DIR); fi
	if test -d wibble; then; else mkdir $(DMSL_BUILD_DIR); fi
	if test -d wibble; then; else mkdir $(TESTER_BUILD_DIR); fi

# These only have dependency for their headers.
# Change these creating individual rules for each object file.
$(UTIL_BUILD_DIR)/%.o: $(UTIL_DIR)/$(SRC_DIR)/%.cpp $(UTIL_DIR)/$(INCLUDE_DIR)/%.h
	$(CC) $(CFLAGS) $(UTIL_INCLUDES) -c $< -o $@

$(PROFILE_BUILD_DIR)/%.o: $(PROFILE_DIR)/$(SRC_DIR)/%.cpp $(PROFILE_DIR)/$(INCLUDE_DIR)/%.h
	$(CC) $(CFLAGS) $(PROFILE_INCLUDES) -c $< -o $@

$(DECISIONMAKER_BUILD_DIR)/%.o: $(DECISIONMAKER_DIR)/$(SRC_DIR)/%.cpp $(DECISIONMAKER_DIR)/$(INCLUDE_DIR)/%.h
	$(CC) $(CFLAGS) $(DECISIONMAKER_INCLUDES) -c $< -o $@

$(DMSL_BUILD_DIR)/dmsl_java_wrap.o: $(DMSL_DIR)/$(SRC_DIR)/dmsl_java_wrap.cxx
	$(CC) $(CFLAGS) $(DMSL_INCLUDES) $(JNI_INCLUDE) -c $< -o $@

$(TESTER_BUILD_DIR)/%.o: $(TESTER_DIR)/$(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) $(TESTER_INCLUDES) -c $< -o $@

dynamiclink: builddirs $(LIB_FILES)
	$(CC) -shared -fPIC -o $(TARGET) $(LIB_FILES)

statictester: builddirs $(LIB_FILES) $(TESTER_FILES)
	$(CC) $(LIB_FILES) $(TESTER_FILES) -o $@

dynamictester: dynamiclink $(TESTER_FILES)
	$(CC) $(TESTER_FILES) -L. -ldmsl -o $(TESTER_DIR)/$@

clean:
	- $(RM) $(LIB_FILES) $(TESTER_FILES) $(TARGET)
