# VideoPlayer
## 项目说明
1.程序代码`videoPlayer.cpp`借鉴雷神博客：https://blog.csdn.net/leixiaohua1020/article/details/47068015  
2.仓库中为编译成功后的所有项目文件  
创建工程：CMake  
编译工程：VS 2019  
项目中包括FFmpeg和SDL的库文件：`.dll`文件，`include`文件夹和`lib`文件夹  
3.程序目前只能实现简单的画面播放功能，无声音播放功能，窗口播放画面过程中无法进行其他操作，否则会导致画面播放非正常停止  
4.上述问题的解决方案为实现视音频同步播放和SDL多线程功能，程序持续改进中...  
## 使用说明
1.项目编译成功后，`build`文件夹中会生成`Debug`文件夹  
2.`Debug`文件夹中会生成`videoPlayer.exe`应用程序文件  
3.在该应用程序文件所在目录下打开cmd窗口  
4.输入命令`videoPlayer Frozen.mp4`  
5.`Frozen.mp4`是预先准备好的mp4文件，将其作为启动参数传递给程序  
6.用户也可以利用程序播放自己准备的mp4文件  
7.传递给程序的启动参数是mp4文件的文件路径，若mp4文件与`videoPlayer.exe`文件在同一文件夹下，则可直接将mp4文件名作为启动参数  
8.若输入正确的命令后遇到`查找不到.dll文件`错误信息，请将所需的`.dll`文件复制到`C:\Windows\System32`目录下即可解决  
