## FFmpeg Tutorial

昨天才搜到 FFmpeg 这这么一个教程: [https://github.com/mpenkov/ffmpeg-tutorial](https://github.com/mpenkov/ffmpeg-tutorial)，因此决定先暂停 [StudyFFmpeg](https://github.com/debugly/StudyFFmpeg) 教程，我需要回过头来重新学习下！

本人之前对 GNU 的那套编译环境并不熟悉，因此这个仓库用来边学习 FFmpeg 边学习 GCC，为以后做跨平台开发做个铺垫。

### 

### 工程目录结构

- assets : 测试资源
- vendor : 依赖的三方类库
- Ccode : c 语言工程
	- source  : c 源代码
	- tools : c 编译脚本
	- build : 编译产物和运行产物
- Xcode : Xcode工程
	- targets : 每个target对应一个教程

### 使用介绍

1. 对于 iOS/macOS 开发者，可以使用 Xcode 工程学习，选择不同的 scheme 运行即可！
2. 对于熟悉 gcc、clang 编译环境的可使用 Ccode/tools 里面的脚本编译执行，脚本已经对路径做了处理，所以你可以在**任何目录**里执行 shell 命令，比如:

	```
	# 进入 Ccode 目录下
	qianlongxu$ cd /Users/qianlongxu/Documents/GitWorkSpace/FFmpeg-Tutorial/Ccode
	# 编译
	qianlongxu$ sh tools/compile_tutorial_01.sh
	# 带上 run 参数运行
	qianlongxu$ sh tools/compile_tutorial_01.sh run
	# 查看 Ccode/build/tutorial01 目录可看到最终效果
	```
3. 对于 VSCode 用户，可打开 FFmpeg-Tutorial 目录，需要先运行 mkdir 任务生成目录，然后运行 build 任务就能完成编译；也可以使用 lldb 去调试。

