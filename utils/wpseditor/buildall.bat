@echo off
echo qmake...
qmake 
echo Building gui...
mingw32-make 1>nul
echo Building libs...
cd libwps
call buildall.bat 2>nul
cd ..
