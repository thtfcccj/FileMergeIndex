/************************************************************************************************

                                  Png2RGBA转换模块
主要用于测度与lodepng：源码出处：
    https://github.com/lvandeve/lodepng.git
************************************************************************************************/


#include "dialog.h"
#include "lodepng.h"


bool  Dialog::Pro_Png2RGBA(QTextStream &t) //返回true处理完成
{
  (void)t;
  //======================================图像处理========================================
  std::vector<unsigned char> image; //the raw pixels
  unsigned width, height;

  //std::string str = directoryLabel->text().toStdString();
  //onst char* filename = str.c_str();
  QByteArray ba = directoryLabel->text().toLocal8Bit();  // toLocal8Bit 支持中文
  const char* filename = ba.data();

  //decode inflateHuffmanBlock 是关键解压处， ZIP压缩算法参考头条作者：yuanwufan： ZIP压缩算法详细分析及解压实例解释
  //https://www.toutiao.com/i6675160779332780557
  unsigned error = lodepng::decode(image, width, height, filename);

  //if there's an error, display it
  if(error){
	    QMessageBox msgBox;
      QString note = tr("PNG图像解码错误：\n 错误码");
      note += QString::number(error) + tr("：") + lodepng_error_text(error);
      note +=  tr("\n转换中止!");
      msgBox.setText(note);
	    msgBox.exec();

      return false;
  }

  //the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
  //======================================正确时最后保存数据========================================
   QString fileName;
  if(BatNestDeep == 0) //当前提定
    fileName = QFileDialog::getSaveFileName(0, tr("保存成功生成的文件..."),QDir::currentPath(),tr("嵌入式图像格式(*.ePic);;其它格式 (*.*)"));
  else fileName = BatNestOutFile[BatNestDeep - 1]; //批处理指定

  //打开要保存的文件
  QFile *curFile = new QFile(fileName);
  if(curFile->open(QIODevice::WriteOnly | QIODevice::Truncate	) == false){//文件打开失败
    QMessageBox finalmsgBox;
    finalmsgBox.setText( tr("未找到或打开失败,处理已中止!"));
    finalmsgBox.exec();

    delete curFile;
    return false;
  };

  QDataStream dest(curFile);  //读数据流

  //保存文件(后可将RGBA数据转换为其它格式)
  qint64 count = image.size();
  for(qint64 pos = 0; pos < count; pos++){
    dest << (qint8)image[pos];
  }
  curFile->close();
  delete curFile;

  return true; //处理完成
}




