本系统依赖Qt5.7以上版本
1、编译qxmpp
（1）cd qxmpp
（2）mkdir build
（3）cd build
（4）cmake ..
（5）cmake --build .
（6）cmake --build . --target install
2、更改Qt工程文件中对应的qxmpp库路径
3、进行整体编译