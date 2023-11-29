一些工具类的实现，对Windows API、标准库、第三方库的进行封装，简化使用

## TODO:

跨平台

## 窗口操作
查找窗口
```cpp
bool FindWindow(const std::wstring& class_name, const std::wstring& window_name);
```

置项窗口
```cpp
bool TopWindow(const HWND& hWnd);
```



## 文件、文件夹操作类


根据文件路径，获取文件信息
```c++
C:/A/B/test.txt
C:/A/test.txt
C:/test.txt

文件路径
struct FilePath
{
std::wstring drive; //C:
std::wstring dir; //当前目录
std::wstring parent_dir;//上一层目录
std::wstring extension; //.txt
std::wstring filename; //test
};
```
获取文件大小
```cpp
UINT64 GetFileSize(const std::wstring& file_path);
```
获取文件名称(是否带后缀）

```cpp
UINT64 GetFileName(const std::wstring& file_path, bool with_suffix);
```



## 图像操作类

图片信息

```c++
struct ImageInfo
{
UINT width;
UINT height;
UINT64 size;
std::wstring type; //jpg/jpeg、png、gif
};

struct ImageAction
{
bool zoom_in;
bool zoom_out;
INT rotate; //旋转角度
INT scale; //0~300 放大倍率 ratio
};
```

获取图像大小

```cpp
UINT64 GetImageSize(const std::wstring& file_path);
```

图像操作

```cpp
bool OperateImage();
```



## 文件下载类

下载文件

```cpp
bool DownloadFile();
```



## `MD5`,  `Base64`



## 进程相关
