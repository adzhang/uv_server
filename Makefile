TARGET=uv_server

LIB_DIRS += ./src/net

SOURCES += $(wildcard src/*.cpp)
SOURCES += $(wildcard src/common/*.c)
SOURCES += $(wildcard src/common/*.cpp)

OBJDIR = ./obj
OBJS = $(addprefix $(OBJDIR)/, $(subst .c,.o,$(SOURCES:.cpp=.o)))
OUTPUT = ./bin/$(TARGET)
DEBUG_OUTPUT = ./bin_debug/$(TARGET)

.SUFFIXES: .o .cpp .c
.PRECIOUS: $(OBJS)

all: clean dep_libs $(DEBUG_OUTPUT) $(TARGET)

C = /usr/bin/gcc
CC = /usr/bin/g++
STRIP=/usr/bin/strip
AR=/usr/bin/ar

CFLAGS += -I/usr/include \
		  -Isrc/common \
		  -Iinclude

CFLAGS	+= -g -Wall -O0 -D_DEBUG -D__linux__
#CFLAGS	+= -g -O0 -fno-rtti -D_DEBUG -lrt -ldl -lgcc_s

LDFLAGS=\
		-L/usr/lib \
		-L/usr/lib64 \
		-Llib/linux \
		-lnet -luv -lpthread -lrt

dep_libs:
	for dir in $(LIB_DIRS); do \
		make all -C $$dir; \
	done

$(DEBUG_OUTPUT): $(OBJS) bin_debug


bin:
	@rm -rf ./bin
	@mkdir ./bin

bin_debug:
	@rm -rf ./bin_debug
	@mkdir ./bin_debug

$(TARGET):	$(OBJS)
	@echo "Linking debug lib $@"
	@$(CC) $(OBJS) $(LDFLAGS) $(CFLAGS) -o ./bin_debug/$@
	
$(OBJDIR):
	@mkdir $(OBJDIR)


#对于发行版本可以删除符号信息
#$(OUTPUT): $(DEBUG_OUTPUT) bin bin_debug
#	@echo "Stripping symbols for release target $@"
#	@cp $(DEBUG_OUTPUT) $(OUTPUT)
#	@$(STRIP) --strip-debug $(OUTPUT)
#	@echo "Build done!"

%.a: $(OBJS)
	@echo "Archiving $@"
	@$(AR) scr $@ $(OBJS)
	@echo "Done!"

#生成DEBUGb
$(OBJDIR)/%.o: ../../../%.cpp
	@echo "Compiling $<"
	@mkdir -p $(dir $@)
	@$(CC) -fno-rtti $(CFLAGS) -c $< -o $@
	
$(OBJDIR)/%.o: ../../%.cpp
	@echo "Compiling $<"
	@mkdir -p $(dir $@)
	@$(CC) -fno-rtti $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: ../../%.c
	@echo "Compiling $<"
	@mkdir -p $(dir $@)
	@$(C) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: ../%.cpp
	@echo "Compiling $<"
	@mkdir -p $(dir $@)
	@$(CC) -fno-rtti $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: ./%.cpp
	@echo "Compiling. $<"
	@mkdir -p $(dir $@)
	@$(CC) -fno-rtti $(CFLAGS) -c $< -o $@
	
$(OBJDIR)/%.o: ./%.c
	@echo "Compiling. $<"
	@mkdir -p $(dir $@)
	@$(C) $(CFLAGS) -c $< -o $@

	
clean:
	rm -rf  $(OBJDIR) $(OBJS) $(OUTPUT) ./bin ./bin_debug
