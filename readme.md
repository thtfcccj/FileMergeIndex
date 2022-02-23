### CCJ多功能文件处理与编译器 V2.0
  目标可执行文件下载地址：https://download.csdn.net/download/thtfcccj/81188414

#### 说明
  此应用可根据脚本文件不同，实现不同功能，主要有：
  * 资源文件合并:
     + 功能：将不同类型与大小的文件如各种图像，合并成一个bin文件，并在头部生成位置数组(可理解为其指针)，供嵌入式开发调用不同大小资源。
     + 说明及示例：见“资源文件合并功能说明及示例”子目录及其readme.md

  * bin文件合并：
    + 将多个指定bin文件， 按给定的存放位置合并为一个文件，如：烧写程序，字库，其它资源等组合成一个bin用于最终的硬件烧录
    + 说明及示例：见“bin文件合并功能说明及示例”子目录及其readme.md

  * 配置功能编译 即常量编译器 功能：
    + 如图片在界面中的位置信息，颜色信息，要显示的常量字符串，常量信息等，编译为一个bin文件，以供MCU程序调用。
    + 此功能可代替使用嵌入式程序的编译器编译常量信息，也用于不同场景下，对EEPROM初始化不同的值。
    + 说明及示例：见”配置功能编译说明及示例”子目录及其readme.md

  * 图片转换与图片数据提换功能：
    + 提取.wbm/.bmp/.gif/.png格式中的，纯图像数据，如bmp中的颜色信息。
    + 对上术图片其进行精简，转换为最优化(空间，时间)嵌入式使用的.ePic格式，以供资源整合后，合并入MCU程序调用。
    + 提取png文件中的数据块。
    + 提取各图片的调色板
    + 去除png文件中无用的数据块，以节省空间。
    + 说明及示例：见”ePic图片转换说明及示例”子目录及其readme.md

  * 上述所有子功能模块的，批处理编译功能：
    + 此功能可用于自动化地调用上述各子功能模块，编译成最终嵌入式系统需要的资源常量文件以供烧定入FLASH中。
    + 此资源文件内可含：字库，图片，各种位置信息常量，字符常量等，也可与烧写程序，BOOT程序一起合并成一个烧写bin文件
    + 说明及示例：见”合并批处理功能说明及示例”子目录及其readme.md， 生成的文件在“批处理示例输出”子目录！