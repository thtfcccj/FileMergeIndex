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

 //======================================加载csv文件============================================
  QFile *csvFile = new QFile(directoryLabel->text());
  if(csvFile->open(QIODevice::ReadOnly) == false){//文件打开失败
    QMessageBox finalmsgBox;
    finalmsgBox.setText( tr(" 未找csv目标文件,编译已中止！"));
	  finalmsgBox.exec();

    delete csvFile;
    return false;
  };


  //============================================得到临时目标文件===================================
  QTemporaryFile distFile;
  if(!distFile.open()) {//临时文件创建失败
	  QMessageBox finalmsgBox;
	  QString finalMsg = tr("文件处理所需的临时文件申请空间失败，请检查系统空间否满!");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

	  csvFile->close();
    delete csvFile;
    return false;
  }
  QDataStream dest(&distFile);  //结果为数据流，需二进制处理
  if(isMsb) dest.setByteOrder(QDataStream::BigEndian);//大端高位在前
  else dest.setByteOrder(QDataStream::LittleEndian);//小端低位在前

  //======================================编译目标数据准备及数据区首行编译========================================
  QTextStream csvStream(csvFile);  //源数据
  Line = csvStream.readLine();//1行用户标题，略
  Line = csvStream.readLine();//2行用户注解，略
  Line = csvStream.readLine();//3行标题头，略,

  int lineCount = 3; //从第4行开始为需编译的数据
  int curPos = 0;//目标数据位置计数
  int ValidLine = 0;//有效行计数
  do{
    lineCount++;
    Line = csvStream.readLine();
    if(Line.isEmpty()) break; //空行表示结束了

    Para = Line.split(','); //csv格式分割符

	  if(Para.count() < 4){//文件格式不对了
	    QMessageBox finalmsgBox;
	    QString finalMsg = QString::number(lineCount)  + tr("行：格式错误,编译中止!"); 
	    finalmsgBox.setText(finalMsg);
	    finalmsgBox.exec();

      distFile.close();
	    csvFile->close();
      delete csvFile;
      return false;	  }

    //首行为目标位置，基址为空时直接跳过
	  if(Para[0][0] == ' '){
	    continue;
	  }
    int curBase = Para[0].toInt(&OK,16);//十六进制基址
    if((OK == false) || (curBase < 0)){
      QMessageBox msgBox;
      msgBox.setText(QString::number(lineCount)  + tr("行：存放位置格式或数据错误，,编译中止!"));
      msgBox.exec();

      distFile.close();
	    csvFile->close();
      delete csvFile;
      return false;
    }

    //当前位置至有效数据区没到填充位，忽略
	  if(Para[0][0] == ' '){
	    continue;
	  }

    //当前位置至有效数据区没到填充位，填充空字符
	  if(curPos < curBase){
		  for( ;curPos < curBase; curPos++){
		    dest << (quint8)nullData;
		  }
	  }
	  else if(curBase < curPos){
	    QMessageBox msgBox;
      msgBox.setText(QString::number(lineCount)  + tr("行：存放位置比前一个还小, 编译中止!"));
	    msgBox.exec();

      distFile.close();
	    csvFile->close();
      delete csvFile;
      return false;
	  }
    //======================================数据区第二,三行编译========================================
    //以最大方式预读
    bool bs64,bu64, bh64;
    qint64 s64 =  Para[2].toInt(&bs64,10);
    quint64 u64 =  Para[2].toInt(&bu64,10);
    quint64 h64 =  Para[2].toInt(&bh64,16);
    int Len = 0; ///不能识别的类型标识,负表示错误
    //根据类型标识检查并移入数据
    if(Para[1] == "U8"){
      if(bu64 == false) Len = -1; //变量值表达错误
      else if((u64 < 0) || (u64 > 0xff)) Len = -2;//变量值超限
      else{ dest << (quint8)u64; Len = 1; }
    }
    else if(Para[1] == "U16"){
      if(bu64 == false) Len = -1; //变量值表达错误
      else if((u64 < 0) || (u64 > 0xffff)) Len = -2;//变量值超限
      else{ dest << (quint16)u64; Len = 2; }
    }
    else if(Para[1] == "U32"){
      if(bu64 == false) Len = -1; //变量值表达错误
      else if((u64 < 0) || (u64 > 0xffffffff)) Len = -2;//变量值超限
      else{ dest << (quint32)u64; Len = 4; }
    }
    else if(Para[1] == "U64"){
      if(bu64 == false) Len = -1; //变量值表达错误
      else{ dest << (quint64)u64; Len = 8; }
    }
    else if(Para[1] == "S8"){
      if(bs64 == false) Len = -1; //变量值表达错误
      else if((s64 < -128) || (s64 > 127)) Len = -2;//变量值超限
      else{ dest << (qint8)u64; Len = 1; }
    }
    else if(Para[1] == "S16"){
      if(bs64 == false) Len = -1; //变量值表达错误
      else if((s64 < -32768) || (s64 > 32767)) Len = -2;//变量值超限
      else{ dest << (qint16)u64; Len = 2; }
    }
    else if(Para[1] == "S32"){
      if(bs64 == false) Len = -1; //变量值表达错误
      else if((s64 < ((qint64)-21474836480 / 10)) || (s64 > (qint64)2147483647)) Len = -2;//变量值超限
      else{ dest << (qint32)u64; Len = 4; }
    }
    else if(Para[1] == "S64"){
      if(bs64 == false) Len = -1; //变量值表达错误
      else{ dest << (qint64)u64; Len = 8; }
    }
    else if(Para[1] == "HEX8"){
      if(bh64 == false) Len = -1; //变量值表达错误
      else if((h64 < 0) || (h64 > 0xff)) Len = -2;//变量值超限
      else{ dest << (quint8)h64; Len = 1; }
    }
    else if(Para[1] == "HEX16"){
      if(bh64 == false) Len = -1; //变量值表达错误
      else if((h64 < 0) || (h64 > 0xffff)) Len = -2;//变量值超限
      else{ dest << (quint16)h64; Len = 2; }
    }
    else if(Para[1] == "HEX32"){
      if(bh64 == false) Len = -1; //变量值表达错误
      else if((h64 < 0) || (h64 > 0xffffffff)) Len = -2;//变量值超限
      else{ dest << (quint32)h64; Len = 4; }
    }
    else if(Para[1] == "HEX64"){
      if(bh64 == false) Len = -1; //变量值表达错误
      else{ dest << (quint64)h64; Len = 8; }
    }
    else if(Para[1] == "RGB"){
      if(Para[2][0] != '#') Len = -1; //变量值表达错误
      else{
        bool bu32;
        QString rgb = Para[2].right(Para[2].size() - 1);
        quint32 u32 =  rgb.toInt(&bu32,16);
        if(bu32 == false) Len = -1; //变量值表达错误
        else{
          dest << (quint8)0;  dest << ((quint8)(u32 >> 16) & 0xff);
          dest << ((quint8)(u32 >> 8) & 0xff);
          dest << ((quint8)u32 & 0xff);  Len = 4;
        }
      }
    }
    else if(Para[1] == "RGB24"){
      if(Para[2][0] != '#') Len = -1; //变量值表达错误
      else{
        bool bu32;
        QString rgb = Para[2].right(Para[2].size() - 1);
        quint32 u32 =  rgb.toInt(&bu32,16);
        if(bu32 == false) Len = -1; //变量值表达错误
        else{
          dest << ((quint8)(u32 >> 16) & 0xff);
          dest << ((quint8)(u32 >> 8) & 0xff);
          dest << ((quint8)u32 & 0xff);  Len = 3;
        }
      }
    }
    else if(Para[1] == "ARGB"){
      if(Para[2][0] != '#') Len = -1; //变量值表达错误
      else{
        bool bu32;
        QString rgb = Para[2].right(Para[2].size() - 1);
        quint32 u32 =  rgb.toInt(&bu32,16);
        if(bu32 == false) Len = -1; //变量值表达错误
        else{
          dest << ((quint8)(u32 >> 24) & 0xff);
          dest << ((quint8)(u32 >> 16) & 0xff);
          dest << ((quint8)(u32 >> 8) & 0xff);
          dest << ((quint8)u32 & 0xff);  Len = 4;
        }
      }
    }
    else if(Para[1] == "RGB2RGB565"){
      if(Para[2][0] != '#') Len = -1; //变量值表达错误
      else{
        bool bu32;
        QString rgb = Para[2].right(Para[2].size() - 1);
        quint32 u32 =  rgb.toInt(&bu32,16);
        if(bu32 == false) Len = -1; //变量值表达错误
        else{
          quint8 u8 = (u32 >> (16 + 3)) & 0x1f;//R丢低3位
          if(u32 & (1 << (16 + 2))) u8++;//颜色低位四舍五入
          quint32 u16 = u8 << 11;
          u8 = (u32 >> (8 + 2)) & 0x1f;//G丢低3位
          if(u32 & (1 << (8 + 1))) u8++;//颜色低位四舍五入
          u16 |= u8 << 5;
          u8 = (u32 >> (0 + 3)) & 0x1f;//B丢低3位
          if(u32 & (1 << (0 + 2))) u8++;//颜色低位四舍五入
          u16 |= u8 << 0;
          dest << ((quint8)(u16 >> 8) & 0xff);
          dest << ((quint8)u16 & 0xff);  Len = 2;
        }
      }
    }


    //数据有误
    if(Len <= 0){
	    QMessageBox msgBox;
      QString note = QString::number(lineCount)  + tr("行：");
      if(Len == 0) note +=  tr("类型标识不能被识别,注意全部为大写");
      if(Len == -1) note +=  tr("变量值表达错误");
      if(Len == -2) note +=  tr("变量值超限");
      note +=  tr(",编译中止!");
      msgBox.setText(note);
	    msgBox.exec();

      distFile.close();
	    csvFile->close();
      delete csvFile;
      return false;
    }
    //正确后处理
    curPos += Len;//目标数据位置计数
    ValidLine++;//有效行计数
  }while(1); //end do

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
  }


  csvFile->close();
  delete csvFile;
  distFile.close();
  return true; //处理完成
}




