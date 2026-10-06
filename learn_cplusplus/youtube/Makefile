########################################################################
####################### Makefile Template ##############################
########################################################################

# Compiler settings - Can be customized.
CC = g++
# webview 是 header-only 的 C++ 库，编译和链接都要带上 GTK4 + WebKitGTK 6.0 的参数
WEBVIEW_CFLAGS = $(shell pkg-config --cflags gtk4 webkitgtk-6.0)
WEBVIEW_LIBS = $(shell pkg-config --libs gtk4 webkitgtk-6.0) -ldl
CXXFLAGS = -std=c++11 -Wall $(WEBVIEW_CFLAGS)
LDFLAGS = $(WEBVIEW_LIBS)

# Makefile settings - Can be customized.
APPNAME = myapp
EXT = .cpp
SRCDIR = src
OBJDIR = obj

# 由 src/index.html 生成的 C++ 头文件，页面内容会被编进可执行文件
GENHEADER = $(OBJDIR)/index_html.h

CXXFLAGS += -I$(OBJDIR)

############## Do not change anything from here downwards! #############
SRC = $(wildcard $(SRCDIR)/*$(EXT))
OBJ = $(SRC:$(SRCDIR)/%$(EXT)=$(OBJDIR)/%.o)
DEP = $(OBJ:$(OBJDIR)/%.o=%.d)
# UNIX-based OS variables & settings
RM = rm
DELOBJ = $(OBJ) $(GENHEADER)
# Windows OS variables & settings
DEL = del
EXE = .exe
WDELOBJ = $(SRC:$(SRCDIR)/%$(EXT)=$(OBJDIR)\\%.o)

########################################################################
####################### Targets beginning here #########################
########################################################################

all: $(APPNAME)

# Builds the app
$(APPNAME): $(OBJ)
	$(CC) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Creates the dependecy rules
%.d: $(SRCDIR)/%$(EXT) $(GENHEADER)
	@$(CC) $(CXXFLAGS) $< -MM -MT $(@:%.d=$(OBJDIR)/%.o) >$@

# Includes all .h files（clean 时不去重新生成它们）
ifeq ($(filter clean cleandep cleanw cleandepw,$(MAKECMDGOALS)),)
-include $(DEP)
endif

# 把 src/index.html 变成 C++ 头文件：内容是原始字符串字面量
$(GENHEADER): $(SRCDIR)/index.html
	@mkdir -p $(OBJDIR)
	@{ \
		echo '// 本文件由 make 依据 src/index.html 自动生成，请勿手改'; \
		echo ''; \
		echo 'static const char *kIndexHtml = R"WEBVIEW_HTML('; \
		cat $<; \
		echo ')WEBVIEW_HTML";'; \
	} > $@
	@echo "已生成 $@（来自 $<）"

# Building rule for .o files and its .c/.cpp in combination with all .h
$(OBJDIR)/%.o: $(SRCDIR)/%$(EXT) $(GENHEADER)
	$(CC) $(CXXFLAGS) -o $@ -c $<

################### Cleaning rules for Unix-based OS ###################
# Cleans complete project
.PHONY: clean
clean:
	$(RM) -f $(DELOBJ) $(DEP) $(APPNAME)

# Cleans only all files with the extension .d
.PHONY: cleandep
cleandep:
	$(RM) -f $(DEP)

#################### Cleaning rules for Windows OS #####################
# Cleans complete project
.PHONY: cleanw
cleanw:
	$(DEL) $(WDELOBJ) $(DEP) $(APPNAME)$(EXE)

# Cleans only all files with the extension .d
.PHONY: cleandepw
cleandepw:
	$(DEL) $(DEP)
