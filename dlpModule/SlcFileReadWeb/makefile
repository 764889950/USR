CC = gcc
AR = ar
CCC = g++ -std=c++11


#-L ./表示当前目录作为第一个寻找头文件的目录，寻找的顺序是：./include-->/usr/include-->/usr/local/include
#LIB = -L ./ -lpthread -ldl 
#-I ./作为第一个寻找库文件的目录，寻找的顺序是：./lib-->/lib-->/usr/lib-->/usr/local/lib
#INC = -I ./ -lpthread -ldl
LIB = -lpthread -ldl -L ./mana-svr/tcp/ -lm $(shell pkg-config --libs --static opencv)
INC = -lpthread -ldl -D_GNU_SOURCE -lm -pthread -c -Wall $( shell pkg-config --cflags opencv)
CXXFLAGS = -Wall -fopenmp
ARCOMMAD = -rcs
 
TARGET = SLC_Read
 
CSRCS = $(wildcard ./cJSON-master/cJSON.c)
CPPSRCS = $(wildcard ./*.cpp ./pthread/*.cpp)
CLIB = $(wildcard ./mana-svr/tcp/*.c ./mana-svr/3rd/crypt/*.c ./mana-svr/3rd/http_parser/*.c)
 
COBJS = $(patsubst %.c, %.o, $(CSRCS))
CPPOBJS = $(patsubst %.cpp, %.o, $(CPPSRCS))
CLIB_OBJS = $(patsubst %.c, %.o, $(CLIB))
 
$(TARGET): $(COBJS) $(CPPOBJS) $(CLIB_OBJS)
	$(AR) $(ARCOMMAD) ./mana-svr/tcp/libmana-svr.a $(CLIB_OBJS)
	$(CCC) $(CXXFLAGS) -o $@ $^ $(LIB)

$(COBJS):%.o: %.c
	$(CC) $(CXXFLAGS) $(INC) -o $@ -c $<

$(CPPOBJS):%.o: %.cpp
	$(CCC) $(CXXFLAGS) $(INC) -o $@ -c $<

$(CLIB_OBJS):%.o: %.c
	$(CC) $(CXXFLAGS) $(INC) -o $@ -c $<
clean:
	rm -f *.o ./cJSON-master/cJSON.o ./pthread/*.o 
	rm -f ./mana-svr/tcp/*.o ./mana-svr/3rd/crypt/*.o ./mana-svr/3rd/http_parser/*.o

clean-all:
	rm -f *.o ./cJSON-master/cJSON.o ./pthread/*.o 
	rm -f ./mana-svr/tcp/*.o ./mana-svr/3rd/crypt/*.o ./mana-svr/3rd/http_parser/*.o
	rm -f $(TARGET) ./mana-svr/tcp/*.a


#src=$(wildcard *.cpp *.c ./sub/*.c)
#dir=$(notdir $(src))
#obj=$(patsubst %.c,%.o,$(dir) )
#all:
#	@echo $(src)
#	@echo $(dir)
#	@echo $(obj)
#	@echo "end" 

#https://blog.csdn.net/srw11/article/details/7516712
#wildcard把 指定目录 ./ 和 ./sub/ 下的所有后缀是c的文件全部展开。
#notdir把展开的文件去除掉路径信息
#第三行在$(patsubst %.c,%.o,$(dir) )中，patsubst把$(dir)中的变量符合后缀是.c的全部替换成.o


