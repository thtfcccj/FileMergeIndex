/************************************************************************************************

                   ePic转换-转gif子模块
主要参考：https://blog.csdn.net/Swallow_he/article/details/76165202
************************************************************************************************/


#include "dialog.h"


//---------------------------------得到并填充调色板----------------------------------------------
//返回正调色板占用字节数，负值调色板异常
int Dialog::FullGifPalette(QDataStream &pic,
                           QDataStream &dest,
                           unsigned char mcrspixel, //b8:含调色板，b0~2调色板大小-1;
                           unsigned char EnFullDesc,//是否填充至目标
                           unsigned char FunMask,
                           unsigned char toColorType)
{
  if(!(mcrspixel & 0x80)) return 0;//无调色板
  quint16 PaletteSize = (unsigned short)1 << (((mcrspixel >> 0) & 0x07) + 1); //读取调色板个数
  //读取调色板
  char *color = NULL;
  PaletteSize *= 3;//RGB
  color = new char[PaletteSize];
  pic.readRawData(color, PaletteSize); //读取原数据调色板

  if(color == NULL){
    return -1;//异常
  }
  //无需填充
  if(EnFullDesc == 0){
    delete color;
    return PaletteSize;
  }
  //允许填充至目标时
  if((toColorType == 0) || (!(FunMask & 0x04))){//使用数据调色板
    dest.writeRawData(color ,PaletteSize);
    delete color;
    return PaletteSize;
  }
  //需转换为目标调色板时
  for(unsigned short pos = 0; pos < PaletteSize; pos += 3){
    if(toColorType == 1){ //数组RGB排列,直接对应
      dest << (quint8)color[pos + 0];
      dest << (quint8)color[pos + 1];
      dest << (quint8)color[pos + 2];
    }
    else{
      unsigned long ARGB = RGB24toUl(&color[pos]);
      if(toColorType == 2){ //RGB565
        dest << (quint16)toRGB565(ARGB);
      }
      else if(toColorType == 3){ //RGBM666
        dest << (quint8)toRGBM666(ARGB);
      }
      else{
        delete color;
        return -2;//不支持的目标色系
      }
    }
  }//end for
  
  delete color;
  return PaletteSize;
}


//---------------------------------数据子块处理----------------------------------------------
//返回结束位置
static qint64  _DataSubBlocksPro(QDataStream &pic,
                                 QDataStream &dest,
                                 qint64 Pos,
                                 qint64 picSize,
                                 unsigned char EnMask)//0b压入数据，2b同时压入长度域,3b压入结束符 
{
  char *block = new char[256];
  quint8 Len = 0;
  while(Pos < picSize){
    pic.readRawData((char*)&Len, 1); Pos += 1; //读块中的长度域
    if(Len == 0) break;//结束了
    
    if((EnMask & 0x05) == 0x05){//含含图像数据中的块长度位时
      dest << (quint8)Len;
    }
    pic.readRawData(block, Len); Pos += Len; //读块中的数据
    if((EnMask & 0x01) == 0x01){//含图像数据时
      dest.writeRawData(block ,Len);
    }
  };
  if((EnMask & 0x09) == 0x09){//含含图像数据中的块长度位时
    dest << (quint8)Len;
  }
  return Pos;
}

//------------------------------------------------git格式转ePic-----------------------------------------------------
//返回空格符转换正确，否则描述错识位置
QString  Dialog::Gif2epic(QDataStream &pic,
                                      QDataStream &dest,
                                      qint64 picSize,
                                      unsigned char FunMask,
                                      unsigned char HeaderMask,
                                      unsigned char toColorType)
{
  //================================处理bmp文件头,一共13字节===================================
  char *raw = new char[6+7];
  laterDelRaw = raw;
  pic.readRawData(raw, 6+7);
  //由6个固定字节组成：  由6个固定字节组成
  if((raw[0] != 'G') || (raw[1] != 'I') || (raw[2] != 'F') || (raw[3] != '8') /*|| (raw[4] != '9')*/ || (raw[5] != 'a'))
    return  QString(tr("图像标识域错误, 仅支持GIF87a 或 GIF89a 格式"));


  //逻辑屏幕标识符(Logical Screen Descriptor)：  由7个字节组成
  quint16 biWidth = Lsb2Us(&raw[6 + 0]);
  quint16 biHeight = Lsb2Us(&raw[6 + 2]);
  quint8 mcrspixel = Lsb2Us(&raw[6 + 4]);//包装域： m(b7有全局调色板)cr(b6~b4,颜色深度-1)s(b3) pixel(b3~0全局调色板个数-1)域
  quint8 bgColor = Lsb2Us(&raw[6 + 5]);//背景颜色(在全局颜色列表中的索引，如果没有全局颜色列表，该值没有意义)

  if(GifDataMask & 0x4000){//仅Gif图像处理时
    dest.writeRawData(raw ,6+7); //输出至目标
  }
  delete raw;
  laterDelRaw = NULL; //释放

  //全局颜色列表空间大小
  quint16 colorDeep =   (unsigned short)1 << (((mcrspixel >> 4) & 0x07) + 1);//色深
  quint16 PaletteSize = (unsigned short)1 << (((mcrspixel >> 0) & 0x07) + 1); //调色板个数
  if(PaletteSize > colorDeep) return  QString(tr("mcrspixel内调色板数量异常"));//允许1种透明色


  //填充数据头
  if(!(GifDataMask & 0x4000) && (FunMask & 0x01)){//需要数据头时
    unsigned char vLenMask = FunMask & 0x80; //可变长度位
    if(HeaderMask & 0x01) {
      char Flag = 'g';//前缀
      if(vLenMask) Flag |= 0x80; //压缩数据头标志
      dest << (quint8)Flag;
    }
    if(HeaderMask & 0x02) dest << (quint8)mcrspixel; //色深查找表等信息
    PushWH(dest, biWidth, (HeaderMask &0x0C) | vLenMask);//宽度
    PushWH(dest, biHeight, (HeaderMask &0x30) | vLenMask);//高度
    if(HeaderMask & 0x40) dest << (quint8)bgColor;//压背景颜色
    if(HeaderMask & 0x80) dest << (quint8)PaletteSize;//调色板个数(256时为0)
  }

  //====================================处理紧跟的全局彩色表================================
   // 全局彩色表标志(mcrspixel & 0x80)G用来说明是否有全局彩色表存在。
  //如果G＝1，表示有一个全局彩色表(Global Color Table)将紧跟在这个逻辑屏幕描述块(LogicalScreen Descriptor)之后
  if(GifDataMask & 0x4000) toColorType = 0; //仅Gif图像处理时不转换全局色

  qint64 Pos = FullGifPalette(pic, dest, mcrspixel, FunMask & 0x02, FunMask, toColorType);
  if(Pos < 0) return  QString(tr("全局调色板：不支持的目标色系"));
   
  //====================================处理后续相关数据块================================
  Pos += (6 + 7); //已读取位置
  char *block = new char[16];
  laterDelRaw = block;
  bool isFinal = false;
  int FrameCount = 0;//图像帧总数
  int CurPicFramePos;
  if(GifDataMask & (1 << 15)) CurPicFramePos = PicFramePos;//指定位置
  else CurPicFramePos = 0;//其它为第一帧图像
  while(Pos < picSize){
    pic.readRawData(block, 1); Pos += 1;//先读取块头标志
    //+++++++++0x2c: 图像描述块头(Image Descriptor),0x2c打头处理+++++++++++++++++++++++++++++
    //含可有可无的局部彩色表，总是带有表基图像数据, 此头占10字节
    if((unsigned char)block[0] == 0x2c){
      bool EnFullPalette = true;
      pic.readRawData(&block[1], 9); Pos += 9;//读取块
      if(GifDataMask & (1 << 0)){//图像描述块头允许放时
        if(!(GifDataMask & (1 << 15)) || (FrameCount == CurPicFramePos)){//允行多帧输出时
          if((CurPicFramePos != 0) && (GifDataMask & (1 << 13))) {//输出非0帧时，强制选去除局部颜色查找表
            unsigned char Org = block[9];
            block[9] &= ~0x87;//去局部标志及查找表位
            EnFullPalette = false;//临时不允许了
            dest.writeRawData(block ,10); //输出至目标
            block[9] = Org;//恢复以供FullGifPalette去除
          }
          else dest.writeRawData(block ,10); //输出至目标
        }
      }
      //填充可能的局部查找表
      unsigned char Mask;
      if(EnFullPalette == false) Mask = 0;//强制不允许查找表
      else{
        Mask = GifDataMask & (1 << 1);
        if((GifDataMask & (1 << 15)) && (FrameCount != CurPicFramePos)) Mask = 0; //不允行多帧输出时
      }
      int cCount = FullGifPalette(pic, dest, block[9], Mask, FunMask, toColorType);
      if(cCount < 0) return  QString(tr("局部调色板：不支持的目标色系"));
      Pos += cCount;
      //表基图像数据处理->表基图像数据中的LZW最小代码长度位
      pic.readRawData(block, 1); Pos += 1; //读LZW最小代码长度位
      if((GifDataMask & 0x0c) == 0x0c){//含表基图像数据及其中的LZW最小代码长度位时填充
        if(!(GifDataMask & (1 << 15)) || (FrameCount == CurPicFramePos)){//允行多帧输出时
          dest << (quint8)block[0];
        }
      }
      //表基图像数据处理->数据块
      Mask = GifDataMask >> 2;
      if((GifDataMask & (1 << 15)) && (FrameCount != CurPicFramePos)) Mask = 0; //不允行多帧输出时
      Pos = _DataSubBlocksPro(pic,dest, Pos, picSize, Mask);//压入块
      FrameCount++;//多一帧数据了
      continue; //继续
    }//block[0] == 0x2c

    //+++++++++++++++++++++++++++++++++++0x21打头处理++++++++++++++++++++++++++++++++++++++++
    if((unsigned char)block[0] == 0x21){
      pic.readRawData(&block[1], 1); Pos++;//读取第二标志位
      //图形控制扩展块,跟“块大小1Byte+标志1Byte+延时时间2Byte+透时彩色索引1Byte+0结束1Byte”共8Byte组成 
      if((unsigned char)block[1] == 0xf9){
        pic.readRawData(&block[2], 6); Pos += 6;//读取块
        if(GifDataMask & (1 << 6)){//允许时
          dest.writeRawData(block ,8); //输出至目标
        }
        continue; //继续
      }
      //无格式文本扩展块: 0x21,0x01打头，跟“块大小1Byte固定为13..." 共15字节组成,后跟无格式文本数据
      unsigned char EnMask = 0;
      if((unsigned char)block[1] == 0x01){
        pic.readRawData(&block[2], 13); Pos += 13;//读取块
        if(GifDataMask & (1 << 7)){//允许时
          dest.writeRawData(block ,15); //输出至目标
          EnMask = 0xff;//全部
        }
        //后跟无格式文本数据
        Pos = _DataSubBlocksPro(pic,dest, Pos, picSize, EnMask);//压入块
        continue; //继续
      }
      //注释扩展块:  0x21,0xfe打头，跟“注释数据块0...n" 组成, 以0结尾
      if((unsigned char)block[1] == 0xfe){
        if(GifDataMask & (1 << 8)){//允许时
          dest.writeRawData(block ,2); //头输出至目标
          EnMask = 0xff;//全部
        }
        //注释扩展块
        Pos = _DataSubBlocksPro(pic,dest, Pos, picSize, EnMask);//压入块
        continue; //继续
      }
      //应用扩展块:  0x21,0xff打头，跟“块大小1Byte固定为12..." 共14字节组成,后跟应用数据
      if((unsigned char)block[1] == 0xff){
        pic.readRawData(&block[2], 12); Pos += 12;//读取块
        if(GifDataMask & (1 << 9)){//允许时
          dest.writeRawData(block ,14); //输出至目标
          EnMask = 0xff;//全部
        }
        //后跟应用数据
        Pos = _DataSubBlocksPro(pic,dest, Pos, picSize, EnMask);//压入块
        continue; //继续
      }
      //出错了！
      return  QString(tr("错误位置0x") + QString::number(Pos,16)  + tr("  :不能识别的GIF 0x21后子标识头!"));
    }
    //++++++++++++++++++++++++++++++++++结束符处理++++++++++++++++++++++++++++++++++++++++
    //GIF文件结束块： 0X3b结尾
    if((unsigned char)block[0] == 0x3b){
      if(Pos != picSize){
        //出错了！
        return  QString(tr("错误位置0x") + QString::number(Pos,16)  + tr("  :非GIF文件末尾出现了文件结束块!"));
      }
      isFinal = true;
      break; //正确时退出
    }
    //出错了！
    return  QString(tr("错误位置0x") + QString::number(Pos,16)  + tr("  :不能识别的GIF主标识头!"));
  }//end while

  if(isFinal == false){
    return  QString("GIF文件末尾结束符不是0x3b, 异常结束!");
  }

  //正确结束时填认结束符
  if(GifDataMask & (1 << 10)){//允许时
    dest << (quint8)block[0];
  }
  //正确结束了,多帧图像时提示
  if(FrameCount > 1){
	    QMessageBox msgBox;
      QString note = tr("在GIF动图共发现") + QString::number(FrameCount)  + tr("  帧图像数据！");
      msgBox.setText(note);
	    msgBox.exec();
  }

  //正确结束了
  return QString();//空
}

