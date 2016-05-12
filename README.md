# playOpenCV

##  编译
```shell```
cd ~/playOpenCV/
mkdir build && cd build
cmake ..
make
```

## 功能

- canny_detector.cpp

  使用canny边缘探测来寻找轮廓, 根据轮廓的层级数来判断是否为当前自定义标记点。后续可以根据自定义标记点，读取区域中的数据,例如：二维码的应用。
  ```shell``
  ./canny_detector ../data/canny_test3.jpg
  ```
  
  

