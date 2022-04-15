/************************************************************************************************

                   ePic转换-转bmp子模块

************************************************************************************************/


#include "dialog.h"

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
  laterDelRaw = raw;
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
  if(Data == 256) biCompression |= 0x80;//标识为256
  unsigned short biClrUsed = Data;
  delete raw; laterDelRaw = NULL;

   //得到调色板空间大小
   unsigned char Mask = FunMask & 0x60;
   unsigned short PaletteSize = 0;//颜色查找表大小
   if(biBitCount <= 8){//8位以下有查找表
     PaletteSize = (unsigned short)1 << biBitCount;
     if(biClrUsed != 0){//0使用全部
       //if(biClrUsed > (PaletteSize + 1)) //实测4位图像保存为8位时，也是0xff 256色调色板,故注解掉
       //  return  QString(tr("调色板数量异常"));//允许1种透明色
       //else 
         PaletteSize = biClrUsed;//使用局部
     }
     PaletteSize *= 4;//查找表转换为字节
   }

  //检查数据长度是否正确
  if(picSize <= (PaletteSize + 54))  return  QString(tr("文件大小异常，无图像数据域"));

  //填充数据头
  if(FunMask & 0x01){//需要数据头时
    unsigned char vLenMask = FunMask & 0x80; //可变长度位
    if(HeaderMask & 0x01) {
      char Flag = 'b';//前缀
      if(vLenMask) Flag |= 0x80; //压缩数据头标志
      dest << (quint8)Flag;
    }
    if(HeaderMask & 0x02) dest << (quint8)biBitCount; //色深
    PushWH(dest, biWidth, (HeaderMask &0x0C) | vLenMask);//宽度
    PushWH(dest, biHeight, (HeaderMask &0x30) | vLenMask);//高度
    if(HeaderMask & 0x40) dest << (quint8)biCompression;//压缩方式
    if(HeaderMask & 0x80) dest << (quint8)biClrUsed;//调色板个数
  }

   //有时无条件提取调色板数据
   char *color = NULL;
   if(PaletteSize){//
     color = new char[PaletteSize];
     pic.readRawData(color, PaletteSize); //读取原数据调色板
     laterDelRaw = color;
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

   }//end if
   if(color != NULL) {delete color;   laterDelRaw = NULL;}

  //填充数据
  if(FunMask & 0x08){//需要数据时
    picSize -= (PaletteSize + 54);
    if(picSize != biSizeImages)  return  QString(tr("图像数据大小域异常"));//需为4的倍数

    char *rawData = new char[picSize];
    pic.readRawData(rawData, picSize);
    dest.writeRawData(rawData ,picSize);//合并
    delete rawData;
  }
  
  return QString();//空
}