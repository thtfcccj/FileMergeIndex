/************************************************************************************************

                   ePic转换模块

************************************************************************************************/


#include "dialog.h"

//---------------------------------wbmp格式转ePic----------------------------------------------
//返回空格符转换正确，否则描述错识位置
const QString  Dialog::Wbmp2epicHeader(QDataStream &pic,
                                      QDataStream &dest,
                                      qint64 picSize,
                                      unsigned char FunMask,
                                      unsigned char HeaderMask)
{
  //缓冲并处理数据头
  char *raw = new char[6];
  pic.readRawData(raw, 6);
  if(raw[0] != 0) return tr("图像标识0错误");
  else if(raw[1] != 0) return tr("图像标识1错误");
  
  unsigned char DataPos = 3;
  quint16 w = raw[2];
  if(w & 0x80){//最高位置位表示还有下一位
    w <<= 7; w += raw[3]; DataPos = 4;
  }
  quint16 h = raw[DataPos++];
  if(h & 0x80){//最高位置位表示还有下一位
    h <<= 7; h += raw[DataPos++];
  }
  if((w >= 16384) || (h >= 16384)) return tr("不支持宽或高超16384像素的图像");

  //检查数据长度是否正确
  qint64 bitSize = (unsigned long)w * h;
  qint64 size = bitSize >> 3; //字节大小
  if(size & 0x07) size++; //字节对齐
  if(picSize != (size + DataPos))  return tr("文件长度异常，不符合wbmp规则!");


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
  //填充查找表(无)

  //填充数据
  if(FunMask & 0x04){//需要数据时
    for(; DataPos < 6; DataPos++) dest << (quint8)raw[DataPos];//数据头余下部分
    delete raw;
    picSize -= 6; //数据长度了
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


  //======================================wbmp图像格式处理========================================
  QDataStream pic(picFile);  //图片数据流，需二进制处理
  const QString *resume;
  if(PicType == "wbm")
    resume = &Wbmp2epicHeader(pic,dest, picFile->size(), FunMask,HeaderMask);
  else resume = &QString( tr("图像类型异常"));

  //数据有误
  if(!resume->isEmpty()){
	    QMessageBox msgBox;
      QString note = tr("图像格式不支持：");
      note += *resume;
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




