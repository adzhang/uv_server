TARGET=libnet.a

SOURCES += $(wildcard ./*.cpp)

LIBDIR = ../../lib/linux
OBJDIR = ./obj
OBJS = $(addprefix $(OBJDIR)/, $(subst .c,.o,$(SOURCES:.cpp=.o)))
OUTPUT = ./bin/$(TARGET)
DEBUG_OUTPUT = ./bin_debug/$(TARGET)

.SUFFIXES: .o .cpp .c
.PRECIOUS: $(OBJS)

C = /usr/bin/gcc
CC = /usr/bin/g++
STRIP=/usr/bin/strip
AR=/usr/bin/ar

CFLAGS += -I/usr/include \
		  -I../../include

CFLAGS	+= -g -Wall -O0 -D_DEBUG -D__linux__
#CFLAGS	+= -g -O0 -fno-rtti -D_DEBUG -lrt -ldl -lgcc_s

LDFLAGS=\
		-L/usr/lib \
		-L/usr/lib64 \
		-L../../lib/linux \
		-luv -lpthread -lrt

$(DEBUG_OUTPUT): $(OBJS) bin_debug

$(LIBDIR) /$(TARGET):$(OBJS)

all: $(LIBDIR)/$(TARGET)

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
	rm -rf  $(OBJDIR) $(OBJS) $(LIBDIR)/$(TARGET)
