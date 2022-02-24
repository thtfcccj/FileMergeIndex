/************************************************************************************************

                   ePic转换-转png子模块

************************************************************************************************/


#include "dialog.h"

//---------------------------------png格式转ePic----------------------------------------------
//返回空格符转换正确，否则描述错识位置
QString  Dialog::Png2epic(QDataStream &pic,
                                      QDataStream &dest,
                                      qint64 picSize,
                                      unsigned char FunMask,
                                      unsigned char HeaderMask,
                                      unsigned char toColorType)
{
  //PNG Signature（PNG签名块，包含PNG类型的标识） + 紧跟的文件头数据块IHDR(header chunk，13个字节)
  //数据块由4Byte数据区长度n+4Byte定义好的数据块标识+数据区n个+4ByteCRC校验组成
  
  //缓冲整个图像以方便扫描查找调色板
  char *raw = new char[picSize]; 
  laterDelRaw = raw;
  pic.readRawData(raw, picSize);

  //图像标识域（首位判断出错注解掉了）
  if(/*(raw[0] != 0x89) || */(raw[1] != 'P') || (raw[2] != 'N') || (raw[3] != 'G') || (raw[4] !=  0x0d) || (raw[5] != 0x0a) || (raw[6] != 0x1a) || (raw[7] != 0x0a))
    return  QString(tr("图像标识域错误"));

  //==========================数据块IHDR(header chunk，13个字节)，含分辨率、比特深度、色彩模式、压缩方法
  if((raw[12] != 'I') || (raw[13] != 'H') || (raw[14] != 'D') || (raw[15] !=  'R'))
    return  QString(tr("未找到IHDR标识"));
  unsigned long Data = Msb2Ul(&raw[8]);
  if(Data != 13) return  QString(tr("IHDR长度域错误"));
  Data = Msb2Ul(&raw[16]);
  if(Data >= 16384) return  QString(tr("不支持宽超16384像素的图像"));
  quint16 biWidth = Data;
  Data = Msb2Ul(&raw[20]);
  if(Data >= 16384) return  QString(tr("不支持高超16384像素的图像"));
  quint16 biHeight = Data;
  quint8 colorDeep = raw[24];
  if((colorDeep != 1) && (colorDeep != 2) && (colorDeep != 4) && (colorDeep != 8) && (colorDeep != 16))
    return  QString(tr("颜色深度域表达异常"));
  quint8 colorType = raw[25];
  if(colorType > 6) return  QString(tr("颜色类型域表达异常"));
  quint8 Compression = raw[26]; //压缩方法，0为默认
  quint8 Filter = raw[27];//滤波器方法，0为5种滤波器中默认自适应
  quint8 Scan = raw[28];//扫描方法，0为5种滤波器中默认自适应

 //=======================扫描整个数据，以获取相关信息
 //准备MAP
  QMap<QString, int> mapHeader; //由数据头找到类型,b7:在PLTE和IDAT之前,b6在IDAT之前,b5:不可去除数据块， 低5bit对应使用掩码
  //已处理mapHeader[" PNG"] = 0xc8 |0;//PNG头(ePic默认去除)
  //已处理mapHeader["IHDR"] = 0xc8 |1;//文件头数据块(ePic默认去除)
  mapHeader["cHRM"] = 0xe0 |2;//基色和白色点数据块	
  mapHeader["gAMA"] = 0xc0 |3;//图像γ数据块
  mapHeader["sBIT"] = 0xc0 |4;//样本有效位数据块
  mapHeader["PLTE"] = 0x60 |5;//调色板数据块(ePic默认去除)
  mapHeader["bKGD"] = 0xc0 |6;//背景颜色数据块
  mapHeader["hIST"] = 0xc0 |7;//图像直方图数据块
  mapHeader["tRNS"] = 0xc0 |8;//图像透明数据块
  mapHeader["oFFs"] = 0x40 |9;//(专用公共数据块)
  mapHeader["pHYs"] = 0x40 |10;//物理像素尺寸数据块
  mapHeader["sCAL"] = 0x40 |11;//(专用公共数据块)
  mapHeader["IDAT"] = 0x20 |12;//图像数据块
  mapHeader["tIME"] = 0x00 |13;//图像最后修改时间数据块(ePic默认去除)
  mapHeader["tEXt"] = 0x00 |14;//文本信息数据块(ePic默认去除)
  mapHeader["zTXt"] = 0x00 |15;//压缩文本数据块(ePic默认去除)
  mapHeader["fRAc"] = 0x00 |16;//(专用公共数据块)
  mapHeader["gIFg"] = 0x00 |17;//(专用公共数据块)
  mapHeader["gIFt"] = 0x00 |18;//(专用公共数据块)
  mapHeader["gIFx"] = 0x00 |19;//(专用公共数据块)
  mapHeader["IEND"] = 0x00 |20;//最后一个数据块(ePic默认去除)

  //开始查找最后图像数据块位置个数，查找表位置
  qint64 RdPos = 33;//从长度域开始
  unsigned long RdSize = 0; 
  unsigned char headerType = 0;
  unsigned long FulledMask = 0;
  qint64 pltePos = 0;
  qint64 idatPos = 0;
  int idatCount = 0;//出现IDAT总数
  do{
    RdSize = Msb2Ul(&raw[RdPos]) + 4 * 3; //含长度4，标识域4+ 本次数据+校验码4
    QString Header(raw[RdPos + 4]); Header.append(raw[RdPos + 5]); Header.append(raw[RdPos + 6]); Header.append(raw[RdPos + 7]);
    headerType = mapHeader[Header] & 0x1f;
    unsigned long Mask = (unsigned long)1 << headerType;
    if(headerType > 0){//有值时
      if(headerType == 12){//图像数据块
        idatCount++; 
        idatPos = RdPos;
      }
      else{
        if(headerType == 5) pltePos = RdPos; //有调色板了
        if(Mask & PngDataMask) FulledMask |= Mask;//允许填充时
      }
    }
    RdPos += RdSize; //下次位置
    if(headerType == 20) break;//最后一个数据块了。
  }while(RdPos < picSize);
  if(headerType != 20) return  QString(tr("无效的文件结束符"));

  //获取标志
  bool isOnlyData;
  if((FunMask & 0x20) && (FulledMask == 0) && (idatCount == 1))//仅有结束与1个IDAT时，查找表后为纯IDAT数据
    isOnlyData = true;
  else isOnlyData = false;

  //=======================需要时查找调色板
  quint16 PaletteCount; //调色板数
  if((FunMask & 0x02) && (colorType == 3)){
    if(pltePos == 0) return  QString(tr("未找到调色板"));
    PaletteCount = Msb2Ul(&raw[pltePos]) / 3;//RGB24
  }
  else PaletteCount = 0; //无调色板

  //====================填充数据头
  if(FunMask & 0x01){//需要数据头时
    unsigned char vLenMask = FunMask & 0x80; //可变长度位
    if(HeaderMask & 0x01) {
      char Flag = 'n';//n前缀
      if(vLenMask) Flag |= 0x80; //压缩数据头标志
      dest << (quint8)Flag;
    }
    if(HeaderMask & 0x02) dest << (quint8)(((colorType & 0x07) << 5) | colorDeep); //颜色类型b5~7与色深信息b4~0
    PushWH(dest, biWidth, (HeaderMask &0x0C) | vLenMask);//宽度
    PushWH(dest, biHeight, (HeaderMask &0x30) | vLenMask);//高度
    quint8 Data = (Scan & 0x03) << 5;//扫描方法(b6~5)
    Data |= (Filter & 0x07) << 2;//滤波器方法(b4~2)
    if(isOnlyData) Data |= 0x80; //纯数据时置最高位b7
    if(HeaderMask & 0x40) dest << (quint8)(Data | Compression & 0x03); //压缩方法(b1~0), 
    if(HeaderMask & 0x80) dest << (quint8)PaletteCount;//256时为0
  }

   //=====================填充调色板
  if(PaletteCount){
    unsigned short PaletteSize = PaletteCount * 3; ////调色板数字节数
    pltePos += 8;//移至调色板位置了
    if(!(FunMask & 0x04) || (toColorType <= 1)){//原调色板或RGB24时，直接对应
      dest.writeRawData(&raw[pltePos] ,PaletteSize);//并入
    }
    else{//其它需要一个个转换
      for(; PaletteSize > 0; PaletteSize-= 3, pltePos+= 3){
        unsigned long ARGB = RGB24toUl(&raw[pltePos]);
        if(toColorType == 2) dest << (quint16)toRGB565(ARGB);
        else if(toColorType == 3)  dest << (quint8)toRGBM666(ARGB);
        else return  QString(tr("不支持的目标色系"));
     }//end for
   }//end if
  }

  //======================填充内容================================
  if(!(FunMask & 0x08)) return QString(); //无需填充内容
  if(PngDataMask & 0x01){//强制填充头，此功能可用于独立提取PNG信息
    dest.writeRawData(&raw[0] ,8);//并入
  }
  if(PngDataMask & 0x02){//填充IHDR，此功能可用于独立提取PNG信息
    dest.writeRawData(&raw[8] ,13 + 12);//并入
  }
  if(isOnlyData && (PngDataMask & (1 << 12))){//纯数据时仅并入数据
    dest.writeRawData(&raw[idatPos + 8] ,Msb2Ul(&raw[idatPos]));//并入
    return QString();//空
  }

  RdPos = 33;//从长度域开始，填充需要的数据
  do{
    RdSize = Msb2Ul(&raw[RdPos]) + 4 * 3; //含长度4，标识域4+ 本次数据+校验码4
    QString Header(raw[RdPos + 4]); Header.append(raw[RdPos + 5]); Header.append(raw[RdPos + 6]); Header.append(raw[RdPos + 7]);
    headerType = mapHeader[Header] & 0x1f;
    unsigned long Mask = (unsigned long)1 << headerType;
    if((headerType > 0) && (Mask & PngDataMask)){//有值且允许时填充
      if(FunMask & 0x40) dest.writeRawData(&raw[RdPos + 8] ,RdSize - 12);//去头去尾并入
      else dest.writeRawData(&raw[RdPos] ,RdSize);//全部并入
    }
    RdPos += RdSize; //下次位置
    if(headerType == 20) break;//最后一个数据块了。
  }while(RdPos < picSize);

  return QString();//空
}
