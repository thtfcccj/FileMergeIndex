/************************************************************************************************

                   ccj资源文件合并脚本处理模块

************************************************************************************************/


#include "dialog.h"


//向数据流填充指长度字符,超限时返回1，否则返回0
int Dialog::Pro_fullLenData(QDataStream &dest,
                              quint32 Data,
                              int Len)
{
  if(Len >= 4){
    dest << (quint32)Data;
    return 0;
  }
  if(Len >= 3){
    if(dest.byteOrder() == QDataStream::BigEndian){//高位在前
      dest << (quint16)(Data >> 8);
      dest << (quint8)Data;
    }
    else{ //低位在前
      dest << (quint16)Data;
      dest << (quint8)(Data >> 16);
    }
    if(Data > 0xffffff) return 1;
    return 0;
  }
  if(Len >= 2){
    dest << (quint16)Data;
    if(Data > 0xffff) return 1;
    return 0;
  }
  dest << (quint8)Data;
  if(Data > 0xff) return 1;
  return 0;
}


bool  Dialog::Pro_ResourceMerge(QTextStream &t) //返回true处理完成
{
  //=======================================读取配置信息========================================
  //第二行指定目标起始位置，需以十六进制表示
  QString Line = t.readLine();
  QStringList Para = Line.split(';'); //;后为注释
  bool OK;
  unsigned int Base = Para[0].toLongLong(&OK,16);
  if(OK == false){
    QMessageBox msgBox;
    msgBox.setText(tr("起始位置指定无效，应以“0xnnnnnnnn”方式表达!"));
    msgBox.exec();
    return false;
  }


  //第三行得到合并文件个数,决定数据头长度,后续指定的文件多出部分不合并，不够部分补0x00000000
  Line = t.readLine();
  Para = Line.split(';'); //;后为注释
  Para = Para[0].split(','); //,分割多个参数
  //得到合并文件个数
  int binFileCount = Para[0].toInt();
  if((binFileCount <= 0) || (binFileCount > 1000)){
    QMessageBox msgBox;
    msgBox.setText(tr("合并文件个数描述错误，第二行应为1~1000之间的数字，并以“;”结尾"));
    msgBox.exec();
    return false;
  }

  //第三行指定多字节时组合方式
  bool isMsb = true; //默认大端
  if(Para.count() >= 2){
    bool isLsb = Para[1].toInt(&OK);
    if(OK == true) isMsb = !isLsb;//相反
  }
  //第三行指定多字节时组合方式
  int indexLen = 4; 
  if(Para.count() >= 3){
    int len = Para[2].toInt(&OK);
    if((OK == true) && (len <= 4) && (len >= 0)) indexLen = len;
  }

  //=======================================获取并缓存得路径位置========================================
  //第四行起，为需合并文件绝对路径,以;空行结尾,先获得路径位置
  QStringList listPath;
  int ValidCount = 0;
  for(; ValidCount < binFileCount; ValidCount++){
	  Line = t.readLine();
	  Para = Line.split(';'); //;后为注释
    if(Para[0].isEmpty()) break; //结束了
    QString Pos;
	  if(Para[0][0] == ' ') Pos = ' '; //空格表示中间预留
    else{
      QString curPara = Para[0].simplified(); //去除前后空格
      if(curPara[1] != ':')//当前工作路径
        Pos = directoryLabel->text() + '\\' + curPara; //组合成绝对目录
      else //绝路路径
        Pos = curPara;
    }
    listPath << Pos;
  }
  if(ValidCount < 1){
    QMessageBox msgBox;
    msgBox.setText(tr("第三行起，未找到需合并文件位置"));
    msgBox.exec();
    return false;
  }

  //============================================得到临时目标文件===================================
  QTemporaryFile distFile;
  if(!distFile.open()) {//临时文件创建失败
	QMessageBox finalmsgBox;
	QString finalMsg = tr("文件处理所需的临时文件申请空间失败，请检查系统空间否满!");
	finalmsgBox.setText(finalMsg);
	finalmsgBox.exec();

    return false;
  }

  //=======================================填充目标文件前部的数据头索引========================================
  QDataStream dest(&distFile);  //结果为数据流，需二进制处理
  if(isMsb) dest.setByteOrder(QDataStream::BigEndian);//大端高位在前
  else dest.setByteOrder(QDataStream::LittleEndian);//小端低位在前

  //为空预留，有后续不满时，直接填充0x00000000
  unsigned long curPos;
  if(indexLen){//有数据头时
    curPos = (binFileCount + 1) * indexLen + Base;//用于检查文件容量超限情况
    int ErrCount = 0;
    for(int Pos = 0; Pos < ValidCount; Pos++){
       //空文件预留
	    if(listPath[Pos][0] == ' '){
        ErrCount += Pro_fullLenData(dest, curPos, indexLen);
	      continue;
	    }
	    //获取文件信息中的大小
      QFileInfo FileInfo(listPath[Pos]);
      if(FileInfo.exists() == false){//文件不存在时
	    QMessageBox finalmsgBox;
	    QString finalMsg = listPath[Pos] + tr(" 未被找到,索引填充已中止！");
	    finalmsgBox.setText(finalMsg);
	    finalmsgBox.exec();

        distFile.close();
        return false;
      };
      qint64 Size = FileInfo.size();
      if(Size >= (qint64)(0xffffffff - curPos)){
	      QMessageBox finalmsgBox;
	      QString finalMsg = listPath[Pos] + tr(" 合并后文件过大,索引填充已中止！");
	      finalmsgBox.setText(finalMsg);
	      finalmsgBox.exec();

        distFile.close();
        return false;
      }
	    //填充充当前数据起始位置
      ErrCount += Pro_fullLenData(dest, curPos, indexLen);
	  //更新下个数据起始位置
      curPos += Size; 
    }
    //后续不满时，直接填充最后值,并在最后加上结束位置
    for(int Pos = ValidCount; Pos <= binFileCount; Pos++){
      ErrCount += Pro_fullLenData(dest, curPos, indexLen);
    }
    if(ErrCount){
	    QMessageBox finalmsgBox;
	    QString finalMsg =  tr("共有 ") + QString::number(ErrCount)  + tr(" 个索引值超过索引长度表达范围，索引填充已中止！");
	      finalmsgBox.setText(finalMsg);
	      finalmsgBox.exec();

        distFile.close();
        return false;
      }
    curPos = (binFileCount + 1) * indexLen + Base;//填充时用于检查文件容量超限情况
  }
  else{//无数据头
    curPos = 0;//填充时用于检查文件容量超限情况
  }

  //=======================================填充目标数据========================================
  for(int Pos = 0; Pos < ValidCount; Pos++){
    //空文件跳过
	  if(listPath[Pos][0] == ' '){
	    continue;
	  }
    //加载文件
    QFile *curFile = new QFile(listPath[Pos]);
    if(curFile->open(QIODevice::ReadOnly) == false){//文件打开失败
	  QMessageBox finalmsgBox;
	  QString finalMsg = listPath[Pos] + tr(" 未找到或打开失败,合并已中止！");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

      delete curFile;
      distFile.close();
      return false;
    };
	  qint64 curSize = curFile->size();
    if(curSize > (qint64)(0xffffffff - curPos)){
	  QMessageBox finalmsgBox;
	  QString finalMsg = listPath[Pos] + tr(" 合并后文件过大,合并已中止！");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

      delete curFile;
      curFile->close();
      distFile.close();
      return false;
    }
	  //加载数据流
    QDataStream source(curFile);  //读数据流
    char *raw = new char[curSize];
	  source.readRawData(raw, curSize);
	 //合并入数据流
    dest.writeRawData(raw ,curSize);//合并
	 //更新下个数据起始位置
    curPos += curSize; 
    delete raw;

    curFile->close();
    delete curFile;
  }

  //======================================正确时最后保存数据========================================
  distFile.flush();//保存
  QString fileName;
  if(BatNestDeep == 0) //当前提定
    fileName =QFileDialog::getSaveFileName(0, tr("保存成功生成的文件..."),QDir::currentPath(),tr("Bin格式(*.Bin)"));
  else fileName = BatNestOutFile[BatNestDeep - 1]; //批处理指定

  QFile::remove (fileName); //强制先删除
  if(!distFile.copy(fileName)){
	QMessageBox finalmsgBox;
	QString finalMsg =  fileName + tr("\n 目标文件错误或加载处理异常!");
	finalmsgBox.setText(finalMsg);
	finalmsgBox.exec();

    distFile.close();
    return false;
  }

  distFile.close();
  return true; //处理完成
 
}




