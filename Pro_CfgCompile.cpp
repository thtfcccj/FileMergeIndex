/************************************************************************************************

                   ccj 配置文件编译模块

************************************************************************************************/


#include "dialog.h"

bool  Dialog::Pro_CfgCompile(QTextStream &t) //返回true处理完成
{
  //=======================================读取配置信息========================================
  //第二行指定未被填充区域(字节为单位)的填充字符(一搬为0xff)，需以十六进制表示
  QString Line = t.readLine();
  QStringList Para = Line.split(';'); //;后为注释
  bool OK;
  unsigned char nullData = Para[0].toInt(&OK,16);
  if((OK == false) || (nullData > 255)){
    QMessageBox msgBox;
    msgBox.setText(tr("填充字符指定无效或值超255，应以“0xnn”方式表达!"));
    msgBox.exec();
    return false;
  }

  //第三行指定多字节时组合方式
  Line = t.readLine();
  Para = Line.split(';'); //;后为注释
  bool isMsb = Para[0].toInt(&OK);
  if(OK == false){
    QMessageBox msgBox;
    msgBox.setText(tr("对齐方式指定无效，应为0 或 1!"));
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

  /*/======================================编译目标数据========================================
  QDataStream dest(&distFile);  //结果为数据流，需二进制处理
  unsigned long curPos = 0;
  for(int Pos = 0; Pos < binFileCount; Pos++){
    //空文件跳过
	if(listPath[Pos][0] == ' '){
	  continue;
	}
    //当前位置至有效数据区没到填充位，填充空字符
    unsigned long curBase = baseAry[Pos]; //本次处理文件基址
	if(curPos < curBase){
		for( ;curPos < curBase; curPos++){
		  dest << (quint8)nullData;
		}
	 }
	else if(curBase < curPos){
	  QMessageBox finalmsgBox;
	  QString finalMsg = listPath[Pos] + tr(" 与下个有效文件目标空间有交集,\n可能本文件过大，或两个文件某个基址定义有误，合并已中止！");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

      distFile.close();
      return false;
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
    if(curSize > (qint64)(fileSize - curPos)){
	  QMessageBox finalmsgBox;
	  QString finalMsg = listPath[Pos] + tr(" 合并后的文件超过指定大小,注意检查目标文件大小是否设置正确，合并已中止！");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

      curFile->close();
      delete curFile;
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

  //最后填充至指定文件大小
  for( ;curPos < fileSize; curPos++){
    dest << (quint8)nullData;
  }

  //======================================正确时最后保存数据========================================
  distFile.flush();//保存
  QString fileName = QFileDialog::getSaveFileName(0, tr("保存成功生成的文件..."),QDir::currentPath(),tr("Bin格式(*.Bin)"));
  QFile::remove (fileName); //强制先删除
  if(!distFile.copy(fileName)){
	QMessageBox finalmsgBox;
	QString finalMsg = tr("未指定保存文件或加载处理异常!");
	finalmsgBox.setText(finalMsg);
	finalmsgBox.exec();

    distFile.close();
    return false;
  }*/

  distFile.close();
  return true; //处理完成
}




