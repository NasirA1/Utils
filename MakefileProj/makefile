INCDIR 		:= include
SRCDIR 		:= source
BINDIR 		:= bin
LIBDIR 		:= lib
OBJDIR 		:= obj
TARGETNAME 	:= myapp
INCLUDES	:=
LIBS		:=


#Detects compiler platform
#More details here: http://stackoverflow.com/questions/714100/os-detecting-makefile
$(info )
$(info ******************************************************************)
$(info *                                                                *)
ifeq ($(OS),Windows_NT)
$(info *                     Building for Windows                       *)
TARGET = $(TARGETNAME).exe
INCLUDES += /I$(INCDIR)
CC = cl.exe
CFLAGS = /D WIN32 /EHsc /nologo /WX
else
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
$(info *                      Building for Linux                        *)
TARGET = $(TARGETNAME)
INCLUDES += -I$(INCDIR)
CC = g++
CFLAGS = -D LINUX -g -Wall -std=c++14
else
$(error ERROR: Unknown Platform $(UNAME_S))
false
endif
endif
$(info *                                                                *)
$(info ******************************************************************)
$(info )




.PHONY: default dirs all clean
.PRECIOUS: $(BINDIR)/$(TARGET) $(OBJECTS)



default: dirs $(BINDIR)/$(TARGET)
	$(info )
	$(info Build Complete.)



#returns empty string if BINDIR and OBJDIR don't exist
dirs_exist := $(addsuffix ,$(wildcard $(BINDIR)) $(wildcard $(OBJDIR)))

dirs:
ifeq ($(dirs_exist),)
	@echo "Creating working directories..."
endif
ifeq ($(OS),Windows_NT)
	@if not exist $(BINDIR) mkdir $(BINDIR)
	@if not exist $(OBJDIR) mkdir $(OBJDIR)
else
	@mkdir -p $(BINDIR)
	@mkdir -p $(OBJDIR)
endif



all: default



HEADERS = $(wildcard $(INCDIR)/*.h)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(wildcard $(SRCDIR)/*.c))
OBJECTS += $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.obj, $(wildcard $(SRCDIR)/*.cpp))


$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERS)
ifeq ($(OS),Windows_NT)
	@$(CC) $(CFLAGS) $(INCLUDES) /c $< /Fo$@
else
	@echo Compiling $<...
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
endif


$(OBJDIR)/%.obj: $(SRCDIR)/%.cpp $(HEADERS)
ifeq ($(OS),Windows_NT)
	@$(CC) $(CFLAGS) $(INCLUDES) /c $< /Fo$@
else
	@echo Compiling $<...
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
endif


$(BINDIR)/$(TARGET): $(OBJECTS)
	@echo Linking $@...
ifeq ($(OS),Windows_NT)
	@$(CC) $(OBJECTS) $(LIBS) /nologo /Fe$@
else
	@$(CC) $(OBJECTS) -Wall $(LIBS) -o $@
endif



clean:
	@echo Cleaning...
ifeq ($(OS),Windows_NT)
	@del /f /s $(OBJDIR)\*.obj
	@del /f /s $(OBJDIR)\*.o
	@del /f /s $(BINDIR)\$(TARGET)
else
	@-rm -f $(OBJDIR)/*.obj
	@-rm -f $(OBJDIR)/*.o
	@-rm -f $(BINDIR)/$(TARGET)
endif

