#设置libmsc.so库搜索路径
#编译32位可执行文件
make clean;
export LD_LIBRARY_PATH=$(pwd)/libs/x86/:/usr/local/lib/;
make;

