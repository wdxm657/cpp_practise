# yolov8/yolov5 Inference C++

```
mkdir build
cd build
cmake ..
make
./app
```

main分支我merge了trans分支之后没试过，  如果不行的话用trans即可

py 目录下为网络发送视频数据的py脚本，安装opencv-python应该就可以使用了

app程序的主线程使用方法为先打开MAP，在打开DMA，在打开INFERENCE
关闭app之前务必要关闭MAP，否则无法再次运行APP，必须关闭电脑 复位FPGA重来
遇到APP直接 崩溃不需要重启电脑  按照正常打开方式继续运行即可
这里的崩溃问题我一直没找到问题，  pcie的数据是一直在接受的   就是GTK的图像显示会显示不出来会出现app崩溃的情况
<!-- 
yolov8s.onnx:

![image](https://user-images.githubusercontent.com/40023722/217356132-a4cecf2e-2729-4acb-b80a-6559022d7707.png)

yolov5s.onnx:

![image](https://user-images.githubusercontent.com/40023722/217357005-07464492-d1da-42e3-98a7-fc753f87d5e6.png) -->
