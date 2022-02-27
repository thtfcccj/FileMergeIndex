/************************************************************************************************

                   ccj字符串文件合并脚本处理模块

************************************************************************************************/


#include "dialog.h"

bool  Dialog::Pro_StringCompile(QTextStream &t) //返回true处理完成
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


  //第三行得到字符串行数,决定数据头长度,后续指定的文件多出部分不合并，不够部分补0x00000000
  Line = t.readLine();
  Para = Line.split(';'); //;后为注释
  //得到合并文件个数
  int txtLineCount = Para[0].toInt();
  if((txtLineCount < 0) || (txtLineCount > 1000)){
    QMessageBox msgBox;
    msgBox.setText(tr("合并文件个数描述错误，第二行应为0~1000之间的数字，并以“;”结尾"));
    msgBox.exec();
    return false;
  }

  //第四行指定多字节时组合方式
  Line = t.readLine();
  bool isMsb = true; //默认大端
  if(!Line.isEmpty()){
    Para = Line.split(';'); //;后为注释
    bool isLsb = Para[0].toInt(&OK);
    if(OK == true) isMsb = !isLsb;//相反
  }

  //第五行指定多字节时组合方式
  Line = t.readLine();
  int indexLen = 4; 
  if(!Line.isEmpty()){
    Para = Line.split(';'); //;后为注释
    int len = Para[0].toInt(&OK);
    if((OK == true) && (len <= 4) && (len >= 1)) indexLen = len;
  }
  //第6行， 是否在字符串末尾插入结束字符，0时无
  Line = t.readLine();
  bool haveEnd = false; //默认无
  if(!Line.isEmpty()){
    Para = Line.split(';'); //;后为注释
    bool end = Para[0].toInt(&OK);
    if(OK == true) haveEnd = end;
  }


  //=======================================获取并缓存得到路径位置========================================
  QFile *txtFile = new QFile(directoryLabel->text());
  if(txtFile->open(QIODevice::ReadOnly) == false){//文件打开失败
    QMessageBox finalmsgBox;
    finalmsgBox.setText(directoryLabel->text() + tr("\n 未找到待编译的txt文件,编译已中止！"));
	  finalmsgBox.exec();

    delete txtFile;
    return false;
  };

  //============================================得到临时目标文件===================================
  QTemporaryFile distFile;
  if(!distFile.open()) {//临时文件创建失败
	  QMessageBox finalmsgBox;
	  QString finalMsg = tr("文件处理所需的临时文件申请空间失败，请检查系统空间否满!");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

    return false;
  }

  //=======================================填充目标文件前部的数据头索引及文件================================
  QDataStream dest(&distFile);  //结果为数据流，需二进制处理
  if(isMsb) dest.setByteOrder(QDataStream::BigEndian);//大端高位在前
  else dest.setByteOrder(QDataStream::LittleEndian);//小端低位在前

  //为空预留，有后续不满时，直接填充0x00000000
  int ErrCount = 0;
  int realLintCount = 0;
  unsigned long curPos; //数据区偏移量
  if(!txtLineCount){//自动时，先获取并填充行数
     QTextStream txtLen(txtFile);//文本流
     while(!txtLen.atEnd()){txtLen.readLine(); realLintCount++; };
     txtFile->seek(0);//回到开始
     ErrCount += Pro_fullLenData(dest, realLintCount, indexLen);
     curPos = (realLintCount + 2) * indexLen;//首+尾
  }
  else{
    realLintCount = txtLineCount;
    curPos = (realLintCount + 1) *indexLen;//尾
  }

  int ValidCount = 0;
  curPos +=  Base; //填充位置
  QTextStream txt(txtFile);//文本流
  do{
    if(txt.atEnd()) break; //结束了
    Line = txt.readLine();
    ValidCount++;
    if(ValidCount > realLintCount){
	    QMessageBox finalmsgBox;
	    QString finalMsg = tr(" 指定行数小于实际文件行数， 索引填充已中止！");
	    finalmsgBox.setText(finalMsg);
	    finalmsgBox.exec();

      distFile.close();
      return false;
    }
    QByteArray ba =  Line.toLocal8Bit(); //用单个转换方式实现以支持中文，不支持转义字符
    unsigned long  Size = ba.size();
    if(haveEnd) Size++;//插入结束字符
    if(Size >= (0xffffffff - curPos)){
	    QMessageBox finalmsgBox;
	    QString finalMsg = tr(" 合并后文件过大,索引填充已中止！");
	    finalmsgBox.setText(finalMsg);
	    finalmsgBox.exec();

      distFile.close();
      return false;
    }
	  //填充充当前数据起始位置
    ErrCount += Pro_fullLenData(dest, curPos, indexLen);
	//更新下个数据起始位置
    curPos += Size; 
  }while(1);
  //后续不满时，直接填充最后值,并在最后加上结束位置
  if(txtLineCount){
    for(int Pos = ValidCount; Pos <= txtLineCount; Pos++){
      ErrCount += Pro_fullLenData(dest, curPos, indexLen);
    }
  }
  else  ErrCount += Pro_fullLenData(dest, curPos, indexLen);//最后加上结束位置

  if(ErrCount){
	  QMessageBox finalmsgBox;
	  QString finalMsg =  tr("共有 ") + QString::number(ErrCount)  + tr(" 个索引值超过索引长度表达范围，索引填充已中止！");
	    finalmsgBox.setText(finalMsg);
	    finalmsgBox.exec();

      distFile.close();
      return false;
  }

  //=======================================填充目标数据========================================
  txtFile->seek(0);//回到开始
  QTextStream txt2(txtFile);//新的文本流
  do{
    if(txt2.atEnd()) break; //结束了
    Line = txt2.readLine();
    QByteArray ba =  Line.toLocal8Bit(); //用单个转换方式实现以支持中文，不支持转义字符
    unsigned long  Size = ba.size();
	  //合并入数据流
    for(unsigned long i = 0; i < Size; i++) {
      dest << (quint8)(ba.at(i));
    }
    if(haveEnd) dest << (quint8)'\0';//插入结束字符
  }while(1);

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




