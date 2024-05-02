# NCM2DUMP

Linux命令行工具，将网易云音乐下载的NCM加密文件转化为MP3/Flac文件。支持批量转换，支持多线程。

# 编译

1. 下载本项目

   ```bash
   git clone git@github.com:luchao2424631502/ncm2dump.git
   ```

2. 编译

   ```bash
   cd ncm2dump
   make
   ```

# 使用

1. 直接使用 ./ncm2dump xxx.ncm

   * 转换单个ncm文件 

     `./ncm2dump test.ncm`

   * 转换多个ncm文件 

     `./ncm2dump test1.ncm test2.ncm test3.ncm`

   * 批量转换目录下ncm文件 

     `./ncm2dump -d ~/Downloads/`

   * 开启多线程转换

     `./ncm2dump -j8 -d ~/Downloads/`

```bash
Usage ./ncm2dump [OPTIONS] FILES
Convert CloudMusic ncm files to mp3/flac files

Examples:
        ncm2dump test.ncm
        ncm2dump test1.ncm test2.ncm test3.ncm
        ncm2dump -d ./download/
        ncm2dump -j 6 -d ./download/

Options:
        -h                   display HELP and EXIT
        -j [N]               start N threads to convert
        -d <Directory>       batch convert ncm in a specified <directory>
        -o <file>            place out file in <file>
```

2. 或者自己将得到二进制加入bin目录，更新PATH，长期使用。

# 3-rd LIB使用

* [cJSON库](https://github.com/DaveGamble/cJSON/tree/master)
* [base64库](https://github.com/jwerle/b64.c/tree/master)
* [AES库](https://github.com/kokke/tiny-AES-c)
* [C线程池库](https://github.com/Pithikos/C-Thread-Pool/tree/master)

# NCM解密参考

1. ncmdump.py https://github.com/QCloudHao/ncmdump/blob/master/ncmdump.py
2. ncm文件格式 https://github.com/charlotte-xiao/NCM2MP3
3. [blog](https://www.cnblogs.com/cyx-b/p/13443003.html#%E9%9F%B3%E9%A2%91%E7%9F%A5%E8%AF%86%E7%AE%80%E4%BB%8B)

# 注

:为了我在linux上用，于是参考别人实现用C在linux下改写了一下，方便使用。
>>>>>>> v 0.1
