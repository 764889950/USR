CC = cc
AR = ar
CCC = g++ -std=c++11

#-L ./表示作为第一个寻找库文件的目录，寻找的顺序是：./ -->/lib-->/usr/lib-->/usr/local/lib    -lworld表示在上面的lib的路径中寻找libworld.so动态库文件
#LIB = -L ./ -lpthread 表示在上面的./的路径中寻找libpthread.so动态库文件（如果gcc编译选项中加入了“-static”表示寻找libpthread.a静态库文件）
#-I ./目录作为第一个寻找头文件的目录，寻找的顺序是：./ -->/lib-->/usr/lib-->/usr/local/lib
#-c 只编译不链接
INC = -c -I./cJSON-master/ -I./libcrc-master/include/ -I./pthread/ -I./octoModule/ -I./mana-svr/tcp/ $(shell pkg-config --cflags opencv) 
CXXFLAGS = -Wall -fopenmp -lpthread -ldl -D_GNU_SOURCE -lm -pthread  $(shell pkg-config --libs --static opencv)
ARCOMMAD = -rcs
  
TARGET = auto
 
CSRCS = $(wildcard ./cJSON-master/cJSON.c)
CPPSRCS = $(wildcard ./*.cpp ./pthread/*.cpp ./octoModule/*.cpp)
CLIB = $(wildcard ./mana-svr/tcp/*.c ./mana-svr/3rd/crypt/*.c ./mana-svr/3rd/http_parser/*.c)
 
COBJS = $(patsubst %.c, %.o, $(CSRCS))
CPPOBJS = $(patsubst %.cpp, %.o, $(CPPSRCS))
CLIB_OBJS = $(patsubst %.c, %.o, $(CLIB))
 
$(TARGET): $(COBJS) $(CPPOBJS) $(CLIB_OBJS)
	$(AR) $(ARCOMMAD) ./mana-svr/tcp/libmana-svr.a $(CLIB_OBJS)
	$(CCC) $^ -o $@ $(CXXFLAGS)

$(COBJS):%.o: %.c
	$(CC) $(CXXFLAGS) $(INC) -o $@ -c $<

$(CPPOBJS):%.o: %.cpp
	$(CCC) $(CXXFLAGS) $(INC) -o $@ -c $<

$(CLIB_OBJS):%.o: %.c
	$(CC) $(CXXFLAGS) $(INC) -o $@ -c $<
clean:
	rm -f *.o ./cJSON-master/cJSON.o ./pthread/*.o ./octoModule/*.o
	rm -f ./mana-svr/tcp/*.o ./mana-svr/3rd/crypt/*.o ./mana-svr/3rd/http_parser/*.o

clean-all:
	rm -f *.o ./cJSON-master/cJSON.o ./pthread/*.o ./octoModule/*.o 
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


