

**本demo是基于ffmpeg、x264、rtmp库，编译并移植到android中，完成能够进行直播的demo，通过本demo及相关的文档，你能学习到直播所需的基础知识**

Demo 参考了若干库，包括：

- [small-video-record](https://github.com/mabeijianxi/small-video-record)
- [MediaPlus](https://github.com/javandoc/MediaPlus)
- [libyuv-android](https://github.com/illuspas/libyuv-android)
- [dlib-android-app](https://github.com/tzutalin/dlib-android-app)

**测试环境**

坚果Pro2、MAC VLC播放器

**目前已经完成的功能：**

- [x] 音视频编码
- [x] RTMP推流

**尚未完成的功能：**

- [ ] dlib人脸识别
- [ ] 人脸识别贴图
- [ ] OpenGL雪花飘落
- [ ] 添加滤镜
- [ ] 完善各功能


## 初次编码时的步骤

### 步骤1：下载

**版本**： [FFmpeg 3.0.11 "Einstein"](http://ffmpeg.org/download.html#releases)

### 步骤2：编译


可参考 [手把手图文并茂教你用Android Studio编译FFmpeg库并移植](https://blog.csdn.net/hejjunlin/article/details/52661331)

但是上面的文章只编译了 arm下的so库，如果想编译 arm64v8a、x86等需要其他的脚本，在github上发现了这个库 [FFmpeg4Android/build_script](https://github.com/mabeijianxi/FFmpeg4Android),里面包含了各个版本的so库的编译脚本。

**注意NDK和TOOLCHAIN变量的设置**

**[TODO]** 编译各平台的so库

### 步骤3：Cmake 如何添加第三方so库

- [NDK学习( 二)，在NDK开发中引入第三方库（AndroidStudio Cmake）](https://blog.csdn.net/mxw3755/article/details/56676923)
- [Android studio 多个so库配置 ffmpeg库配置 cmake编译](https://blog.csdn.net/m0_37677536/article/details/78561085)
- [Android Studio NDK 入门教程（4）--优雅的在C++中输出Logcat](https://blog.csdn.net/venusic/article/details/52294815)


### 步骤4：验证ffmpeg已打进APK中


[fmpeg中第三方库的编译_libx264和librtmp](https://blog.csdn.net/jiandanjiuhao_88/article/details/54694029)

[编译Android下可用的全平台FFmpeg(包含libx264与libfdk-aac)](https://blog.csdn.net/mabeijianxi/article/details/74544879)
