1. nvidia-smi
    查看n卡驱动
2. cuda安装包
    https://developer.nvidia.com/cuda-toolkit-archive
3. 安装依赖
    sudo apt-get install freeglut3-dev build-essential libx11-dev libxmu-dev libxi-dev libgl1-mesa-glx libglu1-mesa libglu1-mesa-dev
4. 导入环境变量
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/cuda/lib64
    export PATH=$PATH:/usr/local/cuda/bin
    export CUDA_HOME=$CUDA_HOME:/usr/local/cuda
5. cudnn
    https://docs.nvidia.com/deeplearning/cudnn/install-guide/index.html
5. opencv
    sudo apt-get install build-essential
    sudo apt-get install cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
    sudo apt-get install python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff5-dev libdc1394-22-dev # 处理图像所需的包
    sudo apt-get install libavcodec-dev libavformat-dev libswscale-dev libv4l-dev liblapacke-dev
    sudo apt-get install libxvidcore-dev libx264-dev # 处理视频所需的包
    sudo apt-get install libatlas-base-dev gfortran # 优化opencv功能
    sudo apt-get install ffmpeg

    mkdir ~/opencv_build && cd ~/opencv_build
    git clone https://github.com/opencv/opencv.git
    git clone https://github.com/opencv/opencv_contrib.git

    cd ~/opencv_build/opencv
    mkdir -p build && cd build
<!-- https://pyimagesearch.com/2020/02/03/how-to-use-opencvs-dnn-module-with-nvidia-gpus-cuda-and-cudnn/ -->
<!-- CUDA_ARCH_BIN在下面的链接里可以找到 -->
<!-- https://developer.nvidia.com/cuda-gpus -->
cmake -D CMAKE_BUILD_TYPE=RELEASE \
	-D CMAKE_INSTALL_PREFIX=/usr/local \
	-D INSTALL_PYTHON_EXAMPLES=ON \
	-D INSTALL_C_EXAMPLES=OFF \
	-D OPENCV_ENABLE_NONFREE=ON \
	-D WITH_CUDA=ON \
	-D WITH_CUDNN=ON \
	-D OPENCV_DNN_CUDA=ON \
	-D ENABLE_FAST_MATH=1 \
	-D CUDA_FAST_MATH=1 \
	-D CUDA_ARCH_BIN=6.1 \
	-D WITH_CUBLAS=1 \
	-D OPENCV_EXTRA_MODULES_PATH=/home/wdxm/tools/opencv/opencv_contrib-4.7.0/modules \
	-D HAVE_opencv_python3=ON \
	-D PYTHON_DEFAULT_EXECUTABLE=/home/wdxm/miniconda3/envs/pango/bin/python \
	-D PYTHON_EXECUTABLE=/home/wdxm/miniconda3/envs/pango/bin/python \
	-D BUILD_EXAMPLES=ON ..

    make -j8
    sudo make install
    pkg-config --modversion opencv4

