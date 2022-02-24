/************************************************************************************************

                   ccj bin文件拆分脚本处理模块

************************************************************************************************/


#include "dialog.h"

bool  Dialog::Pro_BinSplit(QTextStream &t) //返回true处理完成
{
  //============================================得到并打开要拆分的文件========================================
  //加载文件
  QFile *inFile = new QFile(directoryLabel->text());
  if(inFile->open(QIODevice::ReadOnly) == false){//文件打开失败
	  QMessageBox finalmsgBox;
	  QString finalMsg = directoryLabel->text() + tr(" 未找到或打开失败,合并已中止！");
	  finalmsgBox.setText(finalMsg);
    finalmsgBox.exec();

    inFile->close();
    delete inFile;
     return false;
  }
  //=======================================获取输出文件所在目录========================================
  QString outDir;
  if(BatNestDeep == 0){//当前确定
    outDir = QFileDialog::getExistingDirectory(this,
	  								 tr("指定输出文件存放目录..."),
		  							 directoryLabel->text(),
		  							 QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly);
  }
  else outDir = BatNestOutFile[BatNestDeep - 1]; //批处理指定

  if (outDir.isEmpty()){
	  QMessageBox finalmsgBox;
	  QString finalMsg = tr(" 未指定文件输出位置,处理已中止！");
	  finalmsgBox.setText(finalMsg);
    finalmsgBox.exec();

    inFile->close();
    delete inFile;
     return false;
  }
  //=======================================获取并缓存存放位置与路径位置========================================
  unsigned long inSize = inFile->size();
  int LinePos = 0;
  do{
    if(t.atEnd()) break; //结束了
	  QString Line = t.readLine();
    LinePos++;
	  QStringList LineData = Line.split(';'); //;后为注释
	  QStringList Para = LineData[0].split(','); //,区分三个参数
    QString fileName = Para[0].simplified(); //去除前后空格
    if((fileName[0] == '/') && ((fileName[1] == '/') )) continue; //“//”不处理 
    if(Para.count() < 3){
      QMessageBox msgBox;
      msgBox.setText(QString::number(LinePos)  + tr("行：缺少参数, 处理中止！"));
      msgBox.exec();
      inFile->close();
      delete inFile;
      return false;
    }
    //得到输出文件位置
    if(fileName[1] != ':'){//当前工作路径
        fileName = outDir + '\\' +fileName; //提取位置并组合成绝对目录
    }
    //得到起始位置
    QString string = Para[1].simplified(); //去除前后空格
    bool OK;
    unsigned long Start = string.toULong(&OK,10);
    if(OK == false){
      Start = string.toULong(&OK,16);
      if(OK == false){
        QMessageBox msgBox;
        msgBox.setText(QString::number(LinePos)  + tr("行：不能识别的起始位置, 处理中止！"));
        msgBox.exec();
        inFile->close();
        delete inFile;
        return false;
      }
    }
    //得到总数
    string = Para[2].simplified(); //去除前后空格
    unsigned long Count = string.toULong(&OK,10);
    if(OK == false){
      Count = string.toULong(&OK,16);
      if(OK == false){
        QMessageBox msgBox;
        msgBox.setText(QString::number(LinePos)  + tr("行：不能识别的读取总数, 处理中止！"));
        msgBox.exec();
        inFile->close();
        delete inFile;
        return false;
      }
    }
    //检查总数起始位置合法性
    QString Err = QString();
    if(Start >= inSize)  Err =  tr("行：起始位置小于文件大小, 处理中止！");
    else{
      if(Count == 0) Count = inSize - Start;//读后面的所有
      else if(Count >= (inSize - Start))  Err =  tr("行：读取总数超过文件末尾，处理中止！");
    }
    if(!Err.isNull()){
        QMessageBox msgBox;
        msgBox.setText(QString::number(LinePos)  + Err);
        msgBox.exec();
        inFile->close();
        delete inFile;
        return false;
    }
    //打开要保存的文件
    QFile *curFile = new QFile(fileName);
    if(curFile->open(QIODevice::WriteOnly | QIODevice::Truncate	) == false){//文件打开失败
	    QMessageBox finalmsgBox;
	    finalmsgBox.setText(QString::number(LinePos)  + tr("未找到或打开失败,处理已中止!"));
	    finalmsgBox.exec();

      delete curFile;
      inFile->close();
      delete inFile;
      return false;
    };

    //填充输出文件
    inFile->seek(Start);//到需要的位置
	  //加载数据流
    QDataStream source(inFile);  //读数据流
    char *raw = new char[Count];
	  source.readRawData(raw, Count);
	  //合并入数据流
    QDataStream dest(curFile);  //读数据流
    dest.writeRawData(raw,Count);//合并
    delete raw;
    curFile->close();
    delete curFile;
  }while(1);

  //处理完成返回
  inFile->close();
  delete inFile;

  return true; //处理完成
}




