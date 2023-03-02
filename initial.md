# conda initial
```
conda create -n yolov5 python=3.8

conda activate yolov5

conda install pytorch torchvision cudatoolkit -c pytorch
```

# yolov8 initial
```
git clone git@github.com:ultralytics/yolov5.git

cd yolov5

pip install -r requirements.txt -i https://pypi.tuna.tsinghua.edu.cn/simple

mkdir weights
# download model from github into weights floder
<!-- https://github.com/ultralytics/assets/releases/download/v0.0.0/yolov5s.pt -->

python detect.py --source ./data/images/ --weights weights/yolov5s.pt --conf 0.4
```
