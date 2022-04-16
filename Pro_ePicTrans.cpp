/************************************************************************************************

                   ePic转换模块

************************************************************************************************/


#include "dialog.h"

//---------------------------------内部函数----------------------------------------------
unsigned long Dialog::Lsb2Ul(const char *raw)
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
unsigned short Dialog::Lsb2Us(const char *raw)
{
  const unsigned char *data = (const unsigned char *)raw;
  unsigned short us = *data++;
  us |= (unsigned short)*data << 8;
  return us;
}

unsigned long Dialog::Msb2Ul(const char *raw)
{
  const unsigned char *data = (const unsigned char *)raw;
  unsigned long ul = (unsigned long)*data << 24;
  data++;
  ul |= (unsigned long)*data << 16;
  data++;
  ul |= (unsigned long)*data << 8;
  data++;
  ul |= (unsigned long)*data << 0;
  return ul;
}
unsigned short Dialog::Msb2Us(const char *raw)
{
  const unsigned char *data = (const unsigned char *)raw;
  unsigned short us = (unsigned short)*data << 8;
  data++;
  us |= (unsigned short)*data << 0;
  return us;
}

unsigned long Dialog::RGB24toUl(const char *raw)//RGB数组转换
{
  const unsigned char *data = (const unsigned char *)raw;
  unsigned long ul = (unsigned long)*data << 16;
  data++;
  ul |= (unsigned long)*data << 8;
  data++;
  ul |= (unsigned long)*data << 0;
  return ul;
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
  //第6行PNG时数据块掩码
  PngDataMask = 0xFFEF1FDC; //默认删除PNG头，识别块，调色板，时间，相关文本等信息,无结束块。
  Line = t.readLine();
  if(t.status() == QTextStream::Ok){
    Para = Line.split(';'); //;后为注释
    unsigned long Mask = Para[0].toUInt(&OK, 16);
    if(OK == true) PngDataMask = Mask;
  }
  //第7行GIF时数据块掩码
  GifDataMask = 0x800C; //默认0x800C,即仅含一帧表基图像数据及其最小代码长度位
  Line = t.readLine();
  if(t.status() == QTextStream::Ok){
    Para = Line.split(';'); //;后为注释
    unsigned long Mask = Para[0].toUInt(&OK, 16);
    if(OK == true) GifDataMask = Mask;
  }
  //第8行;//GIF等动图时，图像数据位置
  PicFramePos = 0; //默认0
  Line = t.readLine();
  if(t.status() == QTextStream::Ok){
    Para = Line.split(';'); //;后为注释
    unsigned long Mask = Para[0].toUInt(&OK);
    if(OK == true) PicFramePos = Mask;
  }

 //======================================加载支持的图像文件=======================================
  QFile *picFile = new QFile(directoryLabel->text());
  if(picFile->open(QIODevice::ReadOnly) == false){//文件打开失败
    QMessageBox finalmsgBox;
    finalmsgBox.setText(directoryLabel->text() + tr("\n 图像文件未找到,编译已中止！"));
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
  laterDelRaw = NULL; //初始化
  QString Resume;
  if(PicType == "wbm")
    Resume = Wbmp2epic(pic,dest, picFile->size(), FunMask,HeaderMask);
  else if(PicType == "bmp")
    Resume = Bmp2epic(pic,dest, picFile->size(), FunMask,HeaderMask,toColorType);
  else if(PicType == "gif")
    Resume = Gif2epic(pic,dest, picFile->size(), FunMask,HeaderMask,toColorType);
  else if(PicType == "png")
    Resume = Png2epic(pic,dest, picFile->size(), FunMask,HeaderMask,toColorType);
  else Resume = QString( tr("图像类型异常"));

  if(laterDelRaw != NULL) delete laterDelRaw;//稍后删除
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
   QString fileName;
  if(BatNestDeep == 0) //当前提定
    fileName = QFileDialog::getSaveFileName(0, tr("保存成功生成的文件..."),QDir::currentPath(),tr("嵌入式图像格式(*.ePic);;其它格式 (*.*)"));
  else fileName = BatNestOutFile[BatNestDeep - 1]; //批处理指定

  QFile::remove (fileName); //强制先删除
  if(!distFile.copy(fileName)){
	  QMessageBox finalmsgBox;
	  QString finalMsg = fileName + tr("\n 目标文件错误或加载处理异常!");
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




