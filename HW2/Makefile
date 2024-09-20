c = gcc
f = -Wall -Wextra -Iinclude -pthread  

# here we create variables for the directories
sd = src
bd = build
bi_d = bin
id = include

# here we createa a list of source files and a list for object files
s = $(wildcard $(sd)/*.c)
o = $(patsubst $(sd)/%.c, $(bd)/%.o, $(s))

# jobExecutorServer executable path
jes = $(bi_d)/jobExecutorServer

# jobCommander executable path
jc = $(bi_d)/jobCommander


all: $(jes) $(jc)

# here we link jobExecutorServer and jobCommander
$(jes): $(bd)/jobExecutorServer.o $(bd)/function.o
	$(c) $(f) -o $@ $^


$(jc): $(bd)/jobCommander.o $(bd)/function.o
	$(c) $(f) -o $@ $^

# Compile source files and making sure that they rely on function.h header file
$(bd)/%.o: $(sd)/%.c $(id)/function.h
	$(c) $(f) -c $< -o $@

# this is for when we use the make clean command to delete the object files and executables
clean:
	rm -f $(bd)/*.o $(jes) $(jc)


.PHONY: all clean
