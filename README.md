# AVRemoteControl
## 简述
最初由[AVPlayer社区](http://avplayer.org)成员共同讨论提出的一种全新概念的`远程控制解决方案`，特点在客户端为浏览器，无需额外下载安装控制软件，随时随地想连接受控端均可。

## 主要功能
查看受控端实时桌面、控制受控端（传输全部鼠标、键盘事件）、查看受控端摄像头

## 项目组成
分为`服务端（受控端）`和`客户端（控制端）`两个子项目

>### 服务端（受控端）
由社区成员讨论一致决定 服务端 采用`C++`编写，可能依赖的库有：`ffmpeg`、`boost`，ffmpeg主要会用到的是获取桌面图像以及编码为视频流，boost主要会用到asio库用于与客户端网络数据传输以及实现简易webservice功能。

>### 客户端（控制端）
预计目前客户端有几套方案：

>>#### HTML5
需要用到`HTML`+`CSS`+`JavaScript`，采用HTML5最潮流的技术——[`WebRTC`](http://www.webrtc.org)，目前主要问题是需寻找是否有现有库方便实现其协议，该方案用户体验特好，开启浏览器输入对应地址即可实现远程控制客户端所需的全部功能。

>>#### Flash
需要用到`ActionScript`语言采用`Flex`框架做界面，同样借助浏览器实现控制，相比前者此方案有缺陷，因为Flash Player的右键事件并未开放，故使用者会无法以正常的习惯控制受控机；另该方案需要用户浏览器已安装Flash Player Plugin；
视频流协议可参考`red5`和`fms`类似的技术，开发途中还需分析Flash流媒体协议，可能对服务端开发要求较高。

>>#### QT
需要依赖`QT`框架，客户端要自行用`ffmpeg`解码并处理接收到的视频流数据，控制音视频同步、断线重连等问题，相对前者对客户端开发人员编程素养要求较高。

## 服务端启动命令设计
>### Options:
*  `-b` [BindIP]:[BindPort]  (defalut:0.0.0.0:8848)
*  `-camera` [enable|disable]  (defalut:enable)
*  `-control` [enable|disable]  (defalut:enable)
*  `/?`
*  `-h`
*  `-help`
*  `--help`
*  `-v`
*  `-version`