/************************************************************************************************

                   ePic转换-转png子模块

************************************************************************************************/


#include "dialog.h"

//---------------------------------git格式转ePic----------------------------------------------

//返回空格符转换正确，否则描述错识位置
QString  Dialog::Png2epic(QDataStream &pic,
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
