/************************************************************************************************

                   ePic转换模块

************************************************************************************************/


#include "dialog.h"

//---------------------------------内部函数----------------------------------------------
static unsigned long Lsb2Ul(const char *raw)
{
  const unsigned char *data = (const unsigned char *)raw;
  unsigned long ul = *data++;
  ul |= (unsigned long)*data << 8;
  data++;
  ul |= (unsigned long)*data << 16;
  data++;
  ul |= (unsigned long)*data << 24;
  return ul;
}
static unsigned short Lsb2Us(const char *raw)
{
  const unsigned char *data = (const unsigned char *)raw;
  unsigned long us = *data++;
  us |= (unsigned long)*data << 8;
  return us;
}

//---------------------------------wbmp格式转ePic----------------------------------------------
//返回空格符转换正确，否则描述错识位置
QString  Dialog::Wbmp2epic(QDataStream &pic,
                                      QDataStream &dest,
                                      qint64 picSize,
                                      unsigned char FunMask,
                                      unsigned char HeaderMask)
{
  //缓冲并处理数据头
  char *raw = new char[6];
  pic.readRawData(raw, 6);
  if(raw[0] != 0) return  QString(tr("图像标识0错误"));
  else if(raw[1] != 0) return  QString(tr("图像标识1错误"));
  
  unsigned char DataPos = 3;
  quint16 w = raw[2];
  if(w & 0x80){//最高位置位表示还有下一位
    w <<= 7; w += raw[3]; DataPos = 4;
  }
  quint16 h = raw[DataPos++];
  if(h & 0x80){//最高位置位表示还有下一位
    h <<= 7; h += raw[DataPos++];
  }
  if((w >= 16384) || (h >= 16384)) return  QString(tr("不支持宽或高超16384像素的图像"));

  //检查数据长度是否正确
  qint64 bitSize = (unsigned long)w * h;
  qint64 size = bitSize >> 3; //字节大小
  if(size & 0x07) size++; //字节对齐
  if(picSize != (size + DataPos))  return  QString(tr("文件大小异常，不符合wbmp规则!"));


  //填充数据头
  if(FunMask & 0x01){//需要数据头时
    if(HeaderMask & 0x01) dest << (quint8)'w';//w前缀
    if(HeaderMask & 0x02) dest << (quint8)1; //1bit色深

    unsigned char Mask = HeaderMask & 0x0c;
    if(Mask == 0x0c) dest << (quint16)w;   //双字节宽度
    else if(Mask) dest << (quint8)w;      //0x04,0x08为单字节宽度

    Mask = HeaderMask & 0x30;
    if(Mask == 0x30) dest << (quint16)h;   //双字节高度
    else if(Mask) dest << (quint8)h;       //0x10,0x10为单字节高度
    if(HeaderMask & 0x40) dest << (quint8)0;//压缩方式：无压缩
    if(HeaderMask & 0x80) dest << (quint8)0;//无调色板
  }
  //0x02,0x04: 填充查找表(无)

  //填充数据
  if(FunMask & 0x08){//需要数据时
    for(; DataPos < 6; DataPos++) dest << (quint8)raw[DataPos];//数据头余下部分
    picSize -= 6; //数据长度了
    char *rawData = new char[picSize];
    pic.readRawData(rawData, picSize);
    dest.writeRawData(rawData ,picSize);//合并
  }
  delete raw;
  return QString();//空
}

//---------------------------------bmp格式转ePic----------------------------------------------

//返回空格符转换正确，否则描述错识位置
QString  Dialog::Bmp2epic(QDataStream &pic,
                                      QDataStream &dest,
                                      qint64 picSize,
                                      unsigned char FunMask,
                                      unsigned char HeaderMask,
                                      unsigned char toColorType)
{
  //处理bmp文件头,一共14字节
  char *raw = new char[54];
  pic.readRawData(raw, 54);
  if((raw[0] != 'B') && (raw[0] != 'b')) return  QString(tr("图像标识域0错误"));
  if((raw[1] != 'M') && (raw[1] != 'm')) return  QString(tr("图像标识域1错误"));
  unsigned long size = Lsb2Ul(&raw[2]);
  if(size != picSize)   return  QString(tr("文件大小域异常，不符合bmp规则"));

  unsigned long bfOffBits = Lsb2Ul(&raw[10]); //从头到位图数据偏移
  if(bfOffBits < 54) return  QString(tr("bmp文件头图像起始域异常"));


  //处理位图信息头,一共40字节
  if(Lsb2Ul(&raw[14 + 0]) != 40) return  QString(tr("位图信息头长度域错误"));
  unsigned long Data = Lsb2Ul(&raw[14 + 4]);
  if(Data >= 16384) return  QString(tr("不支持宽超16384像素的图像"));
  quint16 biWidth = Data;
  Data = Lsb2Ul(&raw[14 + 8]);
  if(Data >= 16384) return  QString(tr("不支持高超16384像素,或正向排列的图像"));
  quint16 biHeight = Data;
  if(Lsb2Us(&raw[14 + 12]) != 1) return  QString(tr("目标设备说明颜色平面数不为1"));
  unsigned short biBitCount = Lsb2Us(&raw[14 + 14]); //比特数/像素数
  if((biBitCount != 1) && (biBitCount != 2) && (biBitCount != 4) && (biBitCount != 8) && (biBitCount != 16) && (biBitCount != 24) && (biBitCount !=32))
    return  QString(tr("不支持的颜色深度"));

  Data = Lsb2Ul(&raw[14 + 16]);
  if(Data > 3) return  QString(tr("不支持的图像压缩方式")); //只有3种
  quint8 biCompression = Data;

  unsigned long biSizeImages = Lsb2Ul(&raw[14 + 20]);//4字节，说明位图数据的大小，当用BI_RGB格式时，可以设置为0；

  Data = Lsb2Ul(&raw[14 + 32]);//说明位图使用的调色板中的颜色索引数，为0说明使用所有；
  if(Data > 257) return  QString(tr("调色板数量域异常"));//允许1种透明色
  unsigned short biClrUsed = Data;
  delete raw;

   //得到调色板空间大小
   unsigned char Mask = FunMask & 0x60;
   unsigned short PaletteSize = 0;//颜色查找表大小
   if(biBitCount <= 8){//8位以下有查找表
     PaletteSize = (unsigned short)1 << biBitCount;
     if(biClrUsed != 0){//0使用全部
       if(biClrUsed > (PaletteSize + 1)) return  QString(tr("调色板数量异常"));//允许1种透明色
       else PaletteSize = biClrUsed;//使用局部
     }
     PaletteSize *= 4;//查找表转换为字节
   }

  //检查数据长度是否正确
  if(picSize <= (PaletteSize + 54))  return  QString(tr("文件大小异常，无图像数据域"));

  //填充数据头
  if(FunMask & 0x01){//需要数据头时
    if(HeaderMask & 0x01) dest << (quint8)'b';//b前缀
    if(HeaderMask & 0x02) dest << (quint8)biBitCount; //色深

    unsigned char Mask = HeaderMask & 0x0c;
    if(Mask == 0x0c) dest << (quint16)biWidth;   //双字节宽度
    else if(Mask) dest << (quint8)biWidth;      //0x04,0x08为单字节宽度

    Mask = HeaderMask & 0x30;
    if(Mask == 0x30) dest << (quint16)biHeight;   //双字节高度
    else if(Mask) dest << (quint8)biHeight;       //0x10,0x10为单字节高度
    if(HeaderMask & 0x40) dest << (quint8)biCompression;//压缩方式
    if(HeaderMask & 0x80) dest << (quint8)biClrUsed;//调色板个数
  }

   //有时无条件提取调色板数据
   char *color = NULL;
   if(PaletteSize){//
     color = new char[PaletteSize];
     pic.readRawData(color, PaletteSize); //读取原数据调色板
   }

   Mask = FunMask & 0x06;
   if((color != NULL) && Mask){//使用调色板时
     //得到调色板数据
     if((toColorType != 0) && (Mask == 0x06)){//转换为目标调色板
       for(unsigned short pos = 0; pos < PaletteSize; pos += 4){
         unsigned long ARGB = Lsb2Ul(&color[pos]);
         if(toColorType == 1){ //数组RGB排列
          dest << (quint8)((ARGB >> 16) & 0xff);
          dest << (quint8)((ARGB >> 8) & 0xff);
          dest << (quint8)(ARGB & 0xff);
         }
         else if(toColorType == 2){ //RGB565
           dest << (quint16)toRGB565(ARGB);
         }
         else if(toColorType == 3){ //RGBM666
           dest << (quint8)toRGBM666(ARGB);
         }
         else return  QString(tr("不支持的目标色系"));
       }//end for
     }//endif
     else dest.writeRawData(color ,PaletteSize);//使用数据调色板
     delete color;
   }//end if

  //填充数据
  if(FunMask & 0x08){//需要数据时
    picSize -= (PaletteSize + 54);
    if(picSize != biSizeImages)  return  QString(tr("图像数据大小域异常"));//需为4的倍数
    char *rawData = new char[picSize];
    pic.readRawData(rawData, picSize);
    dest.writeRawData(rawData ,picSize);//合并
  }
  
  return QString();//空
}

//---------------------------------git格式转ePic----------------------------------------------

//返回空格符转换正确，否则描述错识位置
QString  Dialog::Gif2epic(QDataStream &pic,
                                      QDataStream &dest,
                                      qint64 picSize,
                                      unsigned char FunMask,
                                      unsigned char HeaderMask,
                                      unsigned char toColorType)
{
  //处理bmp文件头,一共13字节
  char *raw = new char[6+7];
  pic.readRawData(raw, 6+7);
  if((raw[0] != 'G') || (raw[1] != 'I') || (raw[2] != 'F') || (raw[3] != '8') /*|| (raw[4] != '9')*/ || (raw[5] != 'a'))
    return  QString(tr("图像标识域错误, 仅支持GIF87a 或 GIF89a 格式"));


  //处理逻辑屏幕标识符(Logical Screen Descriptor) 由7个字节组成
  unsigned char biWidth = Lsb2Us(&raw[6 + 0]);
  unsigned char biHeight = Lsb2Us(&raw[6 + 2]);
  quint8 mcrspixel = Lsb2Us(&raw[6 + 4]);//m(b7有全局调色板)cr(b6~b4,颜色深度-1)s(b3) pixel(b3~0全局调色板个数-1)域
  quint8 bgColor = Lsb2Us(&raw[6 + 5]);//背景颜色(在全局颜色列表中的索引，如果没有全局颜色列表，该值没有意义)

  //全局颜色列表空间大小
  unsigned short colorDeep =   (unsigned short)1 << (((mcrspixel >> 4) & 0x07) + 1);//色深
  unsigned short PaletteSize = (unsigned short)1 << (((mcrspixel >> 0) & 0x07) + 1); //调色板个数
  if(PaletteSize > colorDeep) return  QString(tr("mcrspixel内调色板数量异常"));//允许1种透明色


  //填充数据头
  if(FunMask & 0x01){//需要数据头时
    if(HeaderMask & 0x01) dest << (quint8)'g';//g前缀
    if(HeaderMask & 0x02) dest << (quint8)mcrspixel; //色深查找表等信息

    unsigned char Mask = HeaderMask & 0x0c;
    if(Mask == 0x0c) dest << (quint16)biWidth;   //双字节宽度
    else if(Mask) dest << (quint8)biWidth;      //0x04,0x08为单字节宽度

    Mask = HeaderMask & 0x30;
    if(Mask == 0x30) dest << (quint16)biHeight;   //双字节高度
    else if(Mask) dest << (quint8)biHeight;       //0x10,0x10为单字节高度
    if(HeaderMask & 0x40) dest << (quint8)bgColor;//压背景颜色
    if(HeaderMask & 0x80) dest << (quint8)PaletteSize;//调色板个数(256时为0)
  }

   //有时无条件提取调色板数据
   char *color = NULL;
   if((mcrspixel & 0x80)){//
     PaletteSize *= 3;//RGB
     color = new char[PaletteSize];
     pic.readRawData(color, PaletteSize); //读取原数据调色板
   }
   else PaletteSize = 0;//无全局调色板
  //填充调色板
   unsigned char Mask = FunMask & 0x06;
   if((color != NULL) && Mask){//使用全局调色板时
     if((toColorType != 0) && (Mask == 0x06)){//转换为目标调色板
       for(unsigned short pos = 0; pos < PaletteSize; pos += 3){
         if(toColorType == 1){ //数组RGB排列,直接对应
          dest << (quint8)color[pos + 0];
          dest << (quint8)color[pos + 1];
          dest << (quint8)color[pos + 2];
         }
         else{
           unsigned long ARGB = Lsb2Ul(&color[pos]);
           if(toColorType == 2){ //RGB565
             dest << (quint16)toRGB565(ARGB);
           }
           else if(toColorType == 3){ //RGBM666
             dest << (quint8)toRGBM666(ARGB);
           }
           else return  QString(tr("不支持的目标色系"));
         }
       }//end for
     }//endif
     else dest.writeRawData(color ,PaletteSize);//使用数据调色板
     delete color;
   }//end if


  //紧跟着应该是图形控制扩展(Graphic Control Extension)，0x21,0xf0,0x04开头，0x04表示跟4个数据，0结尾即共8个数:
  //0x21: 扩展块标志
  //0xf0标签类型： 图形控制扩展标签; 或0xfe注释块标签; 或0x01图形文本扩展块; 或0xff应用程序扩展块
  //0x04: 此标签长度，后跟数据
  //标答内容对应4个 (单图时为0x00 0x00 0x00 0x00)
  //0结尾
 //->图形控制扩展后就是LZW压缩格式图片了
  //gif文件以固定值0x3B结束

  //填充数据
  if(FunMask & 0x08){//需要数据时
    PaletteSize += (6+7); //含头了
    if(picSize <= PaletteSize)  return  QString(tr("图像数据大小域异常"));
    picSize -= PaletteSize;
    char *rawData = new char[picSize];
    pic.readRawData(rawData, picSize);
    dest.writeRawData(rawData ,picSize);//合并
  }
  
  return QString();//空
}



bool  Dialog::Pro_ePicTrans(QTextStream &t) //返回true处理完成
{
  //=======================================读取配置信息========================================
  //第二行指定多字节时组合方式
  QString Line = t.readLine();
  QStringList Para = Line.split(';'); //;后为注释
  bool OK;
  bool isMsb = !Para[0].toInt(&OK);//相反
  if(OK == false){
    QMessageBox msgBox;
    msgBox.setText(tr("对齐方式指定无效，应为0 或 1"));
    msgBox.exec();
    return false;
  }
  //第三行功能有效掩码
  Line = t.readLine();
  Para = Line.split(';'); //;后为注释
  unsigned char FunMask = Para[0].toInt(&OK, 16);
  if(OK == false){
    QMessageBox msgBox;
    msgBox.setText(tr("功能有效掩码指定无效，应为0xnn格式！"));
    msgBox.exec();
    return false;
  }
  //第四行数据头对应字节有效掩码位
  Line = t.readLine();
  Para = Line.split(';'); //;后为注释
  unsigned char HeaderMask = Para[0].toInt(&OK, 16);
  if(OK == false){
    QMessageBox msgBox;
    msgBox.setText(tr("数据头对应字节有效掩码指定无效，应为0xnn格式！"));
    msgBox.exec();
    return false;
  }
  //第五行目标调色板格式
  Line = t.readLine();
  Para = Line.split(';'); //;后为注释
  unsigned char toColorType = 0; //不转换
  if((FunMask & 0x06) == 0x06){//支持转换为目标调色板时
     if(Para[0] == "RGB24")  toColorType = 1;
     else if(Para[0] == "RGB565")  toColorType = 2;
     else if(Para[0] == "RGBM666")  toColorType = 3;
     else {
      QMessageBox msgBox;
      msgBox.setText(tr("不支持的目标调色板格式！"));
      msgBox.exec();
      return false;
     }
  }

 //======================================加载支持的图像文件=======================================
  QFile *picFile = new QFile(directoryLabel->text());
  if(picFile->open(QIODevice::ReadOnly) == false){//文件打开失败
    QMessageBox finalmsgBox;
    finalmsgBox.setText( tr(" 未找到需转换的图像文件,编译已中止！"));
	  finalmsgBox.exec();

    delete picFile;
    return false;
  };

  //提取文件格式后缀
  Para = directoryLabel->text().split('.'); //
  QString PicType = Para[Para.count() - 1];//最后一个是
  PicType.toLower();//转小写
  //===============================得到临时目标文件=============================================
  QTemporaryFile distFile;
  if(!distFile.open()) {//临时文件创建失败
	  QMessageBox finalmsgBox;
	  QString finalMsg = tr("文件处理所需的临时文件申请空间失败，请检查系统空间否满!");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

	  picFile->close();
    delete picFile;
    return false;
  }
  QDataStream dest(&distFile);  //结果为数据流，需二进制处理
  if(isMsb) dest.setByteOrder(QDataStream::BigEndian);//大端高位在前
  else dest.setByteOrder(QDataStream::LittleEndian);//小端低位在前


  //======================================图像处理========================================
  QDataStream pic(picFile);  //图片数据流，需二进制处理
  QString Resume;
  if(PicType == "wbm")
    Resume = Wbmp2epic(pic,dest, picFile->size(), FunMask,HeaderMask);
  else if(PicType == "bmp")
    Resume = Bmp2epic(pic,dest, picFile->size(), FunMask,HeaderMask,toColorType);
  else if(PicType == "gif")
    Resume = Gif2epic(pic,dest, picFile->size(), FunMask,HeaderMask,toColorType);
  else Resume = QString( tr("图像类型异常"));

  //数据有误
  if(!Resume.isEmpty()){
	    QMessageBox msgBox;
      QString note = tr("图像格式不支持：");
      note += Resume;
      note +=  tr(",转换中止!");
      msgBox.setText(note);
	    msgBox.exec();

      distFile.close();
	    picFile->close();
      delete picFile;
      return false;
    }

  //======================================正确时最后保存数据========================================
  distFile.flush();//保存
  QString fileName = QFileDialog::getSaveFileName(0, tr("保存成功生成的文件..."),QDir::currentPath(),tr("嵌入式图像格式(*.ePic);;其它格式 (*.*)"));
  QFile::remove (fileName); //强制先删除
  if(!distFile.copy(fileName)){
	  QMessageBox finalmsgBox;
	  QString finalMsg = tr("未指定保存文件或加载处理异常!");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

    distFile.close();
    return false;
  }

  picFile->close();
  delete picFile;
  distFile.close();

  return true; //处理完成
}




