# cppnn
Reimplementation of [BashNN](https://github.com/avada-z/BashNN) algorithm but with **C++**

### Compilation:
```sh
g++ train.cpp -std=c++20 -pthread -o train -Ofast -fomit-frame-pointer -flto -march=native -mtune=native -I /path/to/opencv4/ -lopencv_core -lopencv_imgcodecs
```
### Requirements:
[ImageMagick](https://github.com/ImageMagick/ImageMagick) installed

### Data Preparation:
Run:
```sh
for f in *; do convert $f -threshold 30% $f; done
```
Inside the source images folder to convert them to black and white
