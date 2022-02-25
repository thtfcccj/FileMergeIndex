/************************************************************************************************

                   ePic转换-转wbmp子模块

************************************************************************************************/


#include "dialog.h"

//---------------------------------压入宽度与高度入图像数据头----------------------------------------------
void Dialog::PushWH(QDataStream &dest,
                    unsigned short wh,
                    unsigned char Mask)

{
  if(Mask & 0x80){//可变数据头时
    if(wh < 127) dest << (quint8)wh;      //1位表示
    else{//两位表示
      dest << (quint8)((wh & 0x7f) | 0x80); //低位
      dest << (quint8)(wh >> 7);
    }
  }
  else{//固定长度
    if(Mask == 0x0c) dest << (quint16)wh;   //双字节宽度
    else if(Mask) dest << (quint8)wh;      //0x04,0x08为单字节宽度
  }
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
  laterDelRaw = raw;
  pic.readRawData(raw, 6);
  if(raw[0] != 0) return  QString(tr("图像标识0错误"));
  else if(raw[1] != 0) return  QString(tr("图像标识1错误"));
  
  unsigned char DataPos = 3;
  quint16 w = raw[2];
  if(w & 0x80){//最高位置位表示还有下一位
    if(raw[3] & 0x80) w = 0xffff;//超宽了
    else{w = (w & 0x7f) << 7; w += raw[3]; DataPos = 4;}
  }
  quint16 h = raw[DataPos++];
  if(h & 0x80){//最高位置位表示还有下一位
    if(raw[DataPos] & 0x80) h = 0xffff;//超高了
    else {h = (h & 0x7f) << 7; h += raw[DataPos++];}
  }
  if((w >= 16384) || (h >= 16384)) return  QString(tr("不支持宽或高超16384像素的图像"));

  //检查数据长度是否正确
  unsigned long size = w >> 3;
  if(w & 0x07) size++; //每行像素8字节对齐
  size *= h;
  if(picSize != (size + DataPos))  return  QString(tr("文件大小异常，不符合wbmp规则!"));


  //填充数据头
  if(FunMask & 0x01){//需要数据头时
    unsigned char vLenMask = FunMask & 0x80; //可变长度位
    if(HeaderMask & 0x01) {
      char Flag = 'w';//w前缀
      if(vLenMask) Flag |= 0x80; //压缩数据头标志
      dest << (quint8)Flag;
    }
    if(!vLenMask && (HeaderMask & 0x02)) dest << (quint8)1; //1bit色深
    PushWH(dest, w, (HeaderMask &0x0C) | vLenMask);//宽度
    PushWH(dest, h, (HeaderMask &0x30) | vLenMask);//高度
    if(!vLenMask){//固定长度时
      if(HeaderMask & 0x40) dest << (quint8)0;//压缩方式：无压缩
      if(HeaderMask & 0x80) dest << (quint8)0;//无调色板
    }
  }
  //0x02,0x04: 填充查找表(无)

  //填充数据
  if(FunMask & 0x08){//需要数据时
    for(; DataPos < 6; DataPos++) dest << (quint8)raw[DataPos];//数据头余下部分
    picSize -= 6; //数据长度了
    char *rawData = new char[picSize];
    pic.readRawData(rawData, picSize);
    dest.writeRawData(rawData ,picSize);//合并
    delete rawData;
  }
  delete raw; laterDelRaw = NULL;
  return QString();//空
}
