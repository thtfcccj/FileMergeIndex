/************************************************************************************************

                   ccj 配置文件编译模块

************************************************************************************************/


#include "dialog.h"

//标准色转RGB565,四舍五入
unsigned short Dialog::toRGB565(unsigned long ARGB)
{
  quint8 u8 = (ARGB >> (16 + 3)) & 0x1f;//R丢低3位
  if((u8 < 0x1f) && (ARGB & (1 << (16 + 2)))) u8++;//颜色低位四舍五入
  quint16 u16 = u8 << 11;
  u8 = (ARGB >> (8 + 2)) & 0x3f;//G丢低2位
  if((u8 < 0x3f) && (ARGB & (1 << (8 + 1)))) u8++;//颜色低位四舍五入
  u16 |= u8 << 5;
  u8 = (ARGB >> (0 + 3)) & 0x1f;//B丢低3位
  if((u8 < 0x1f) && (ARGB & (1 << (0 + 2)))) u8++;//颜色低位四舍五入
  u16 |= u8 << 0;
  return u16;
}


//单种色转换为6色,四舍五入
unsigned char Dialog::Scolor256To6(unsigned char sColor)
{
  if(sColor < (0x00 + 25)) return 0;  
  if(sColor < (0x33 + 25)) return 1;  
  if(sColor < (0x66 + 25)) return 2;
  if(sColor < (0x99 + 25)) return 3;  
  if(sColor < (0xCC + 25)) return 4;
  return 5;
}

unsigned char Dialog::toRGBM666(unsigned long ARGB)//标准色转RGBM666,四舍五入
{
  quint8 u8 = Scolor256To6((ARGB >> 16) & 0xff);
  quint8 rgb8 = u8 * 6 * 6;
  u8 = Scolor256To6((ARGB >> 8) & 0xff);
  rgb8 += u8 * 6;
  u8 = Scolor256To6((ARGB >> 0) & 0xff);
  rgb8 += u8 * 1;

  return rgb8;
}

//编译一个变量数据
int Dialog::Pro_CfgCompileData(QDataStream &dest, QString &DescOrg, QString &DataOrg) 
{
    QString Desc = DescOrg.simplified(); //去除前后空格
    QString Data = DataOrg.simplified(); //去除前后空格

    //以最大方式预读
    bool bs64,bu64, bh64;
    qint64 s64 =  Data. toLongLong (&bs64,10);
    quint64 u64 =  Data. toULongLong (&bu64,10);
    quint64 h64 =  Data. toULongLong (&bh64,16);
    int Len = 0; ///不填充,负表示错误
    //根据类型标识检查并移入数据
    if(Desc == "U8"){
      if(bu64 == false) Len = -1; //变量值表达错误
      else if((u64 < 0) || (u64 > 0xff)) Len = -2;//变量值超限
      else{ dest << (quint8)u64; Len = 1; }
    }
    else if(Desc == "U16"){
      if(bu64 == false) Len = -1; //变量值表达错误
      else if((u64 < 0) || (u64 > 0xffff)) Len = -2;//变量值超限
      else{ dest << (quint16)u64; Len = 2; }
    }
    else if(Desc == "U32"){
      if(bu64 == false) Len = -1; //变量值表达错误
      else if((u64 < 0) || (u64 > 0xffffffff)) Len = -2;//变量值超限
      else{ dest << (quint32)u64; Len = 4; }
    }
    else if(Desc == "U64"){
      if(bu64 == false) Len = -1; //变量值表达错误
      else{ dest << (quint64)u64; Len = 8; }
    }
    else if(Desc == "S8"){
      if(bs64 == false) Len = -1; //变量值表达错误
      else if((s64 < -128) || (s64 > 127)) Len = -2;//变量值超限
      else{ dest << (qint8)s64; Len = 1; }
    }
    else if(Desc == "S16"){
      if(bs64 == false) Len = -1; //变量值表达错误
      else if((s64 < -32768) || (s64 > 32767)) Len = -2;//变量值超限
      else{ dest << (qint16)s64; Len = 2; }
    }
    else if(Desc == "S32"){
      if(bs64 == false) Len = -1; //变量值表达错误
      else if((s64 < ((qint64)-21474836480 / 10)) || (s64 > (qint64)2147483647)) Len = -2;//变量值超限
      else{ dest << (qint32)s64; Len = 4; }
    }
    else if(Desc == "S64"){
      if(bs64 == false) Len = -1; //变量值表达错误
      else{ dest << (qint64)s64; Len = 8; }
    }
    else if(Desc == "BIN"){
      bool bf;
      quint32 b =  Data.toUInt(&bf, 2);
      int len =  Data.size();
      if(bf == false) Len = -1; //变量值表达错误
      else if(len <=8){dest << (quint8)b;  Len = 1;}
      else if(len <=16){dest << (quint16)b;  Len = 2;}
      else {dest << (quint32)b;  Len = 4;}
    }
    else if(Desc == "FLOAT"){
      bool bf;
      float f =  Data.toFloat(&bf);
      if(bf == false) Len = -1; //变量值表达错误
      else{dest << f;  Len = 4;}
    }
    else if(Desc == "DOUBLE"){
      bool bf;
      double f =  Data.toDouble(&bf);
      if(bf == false) Len = -1; //变量值表达错误
      else{dest << f;  Len = 8;}
    }
    else if(Desc == "HEX8"){
      if(bh64 == false) Len = -1; //变量值表达错误
      else if((h64 < 0) || (h64 > 0xff)) Len = -2;//变量值超限
      else{ dest << (quint8)h64; Len = 1; }
    }
    else if(Desc == "HEX16"){
      if(bh64 == false) Len = -1; //变量值表达错误
      else if((h64 < 0) || (h64 > 0xffff)) Len = -2;//变量值超限
      else{ dest << (quint16)h64; Len = 2; }
    }
    else if(Desc == "HEX32"){
      if(bh64 == false) Len = -1; //变量值表达错误
      else if((h64 < 0) || (h64 > 0xffffffff)) Len = -2;//变量值超限
      else{ dest << (quint32)h64; Len = 4; }
    }
    else if(Desc == "HEX64"){
      if(bh64 == false) Len = -1; //变量值表达错误
      else{ dest << h64; Len = 8; }
    }
    else if(Desc == "RGB"){
      if(Data[0] != '#') Len = -1; //变量值表达错误
      else{
        bool bu32;
        QString rgb = Data.right(Data.size() - 1);
        quint32 u32 =  rgb.toInt(&bu32,16);
        if(bu32 == false) Len = -1; //变量值表达错误
        else{dest << (u32 & 0x00ffffff);  Len = 4;}
      }
    }
    else if(Desc == "RGB24"){
      if(Data[0] != '#') Len = -1; //变量值表达错误
      else{
        bool bu32;
        QString rgb = Data.right(Data.size() - 1);
        quint32 u32 =  rgb.toInt(&bu32,16);
        if(bu32 == false) Len = -1; //变量值表达错误
        else{//数组RGB排列
          dest << (quint8)((u32 >> 16) & 0xff);
          dest << (quint8)((u32 >> 8) & 0xff);
          dest << (quint8)(u32 & 0xff);  Len = 3;
        }
      }
    }
    else if(Desc == "ARGB"){
      if(Data[0] != '#') Len = -1; //变量值表达错误
      else{
        bool bu32;
        QString rgb = Data.right(Data.size() - 1);
        quint32 u32 =  rgb.toInt(&bu32,16);
        if(bu32 == false) Len = -1; //变量值表达错误
        else{dest << u32;  Len = 4;}
      }
    }
    else if(Desc == "RGB2RGB565"){
      if(Data[0] != '#') Len = -1; //变量值表达错误
      else{
        bool bu32;
        QString rgb = Data.right(Data.size() - 1);
        quint32 u32 =  rgb.toInt(&bu32,16);
        if(bu32 == false) Len = -1; //变量值表达错误
        else{dest << (quint16)toRGB565(u32);  Len = 2;}
      }
    }
    else if(Desc == "RGB2M666"){
      if(Data[0] != '#') Len = -1; //变量值表达错误
      else{
        bool bu32;
        QString rgb = Data.right(Data.size() - 1);
        quint32 u32 =  rgb.toInt(&bu32,16);
        if(bu32 == false) Len = -1; //变量值表达错误
        else{dest << (quint8)toRGBM666(u32);  Len = 1;}
      }
    }
    else if(Desc == "NULL"){Len = 0; }//不填充
    else Len = -10;  //不能识别的类型标识

    return Len;
}

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
  bool isMsb = !Para[0].toInt(&OK);//相反
  if(OK == false){
    QMessageBox msgBox;
    msgBox.setText(tr("对齐方式指定无效，应为0 或 1"));
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
  int DataCount = 0;//编译变量个数
  do{
    if(csvStream.atEnd()) break; //结束了
    lineCount++;
    Line = csvStream.readLine();
    if(Line.isEmpty()) continue; //本行无数据
    Line = Line.simplified(); //去除前后空格
    if((Line[0] == '"') || (Line[0] == '/') && (Line[1] == '/')){//注解处理
      continue; //继续下一行
    }
    Para = Line.split(','); //csv格式分割符

	  if(Para.count() < 4){//文件格式不对了
	    QMessageBox finalmsgBox;
	    QString finalMsg = QString::number(lineCount)  + tr("行：格式错误,“,”分隔符少于4个，编译中止!"); 
	    finalmsgBox.setText(finalMsg);
	    finalmsgBox.exec();

      distFile.close();
	    csvFile->close();
      delete csvFile;
      return false;	  
    }
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
    if(Para[1] == "STRING"){//字符串为单变量
      //暂不支持转义字符,批量转换方式测试中文字符乱码，用单个转换方式实现：
      QByteArray ba =  Para[2].toLocal8Bit(); 
      int Len = ba.size();
      for(int i = 0; i < Len; i++) dest << (quint8)ba[i];
      curPos += Len;//目标数据位置计数
      DataCount++;  //变量个数
    }
    else{//字符串外其它变量，去除其所有空格，并区分变量组
      Para[2].remove(QRegExp("\\s"));//去除其所有空格
      //行内数据编译
      QStringList ParaAry = Para[1].split('|'); //|分割符区分行内变量
      QStringList DataAry =  Para[2].split('|'); //|分割符区分行内变量
      int vCount = ParaAry.size();
      if(vCount !=  DataAry.size()){
	      QMessageBox msgBox;
        msgBox.setText(QString::number(lineCount)  + tr("行内变量描述与变量值数量不符，编译中止!"));
	      msgBox.exec();

        distFile.close();
	      csvFile->close();
        delete csvFile;
        return false;
      }
      for(int vPos = 0; vPos < vCount; vPos++){
        int Len = Pro_CfgCompileData(dest, ParaAry[vPos], DataAry[vPos]); 
        if(Len < 0){//数据有误
	        QMessageBox msgBox;
          QString note = QString::number(lineCount)  + tr("行：第 ") + QString::number(vPos + 1) ;
          if(Len == -10) note +=  tr("个变量：类型标识不能被识别,注意全部为大写");
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
        DataCount++;  //变量个数
      }//end for vPos
    }//end 符串外其它变量

    ValidLine++;//有效行计数
  }while(1); //end do 一行

  //======================================正确时最后保存数据========================================
  distFile.flush();//保存
  QString fileName;
  if(BatNestDeep == 0) //当前提定
    fileName = QFileDialog::getSaveFileName(0, tr("保存成功生成的文件..."),QDir::currentPath(),tr("Bin格式(*.Bin)"));
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


  //未禁止提示时，这里提示编译情况
  if((BatNestDeep == 0) && (IsDisFinal == false)){
    IsDisFinal = true;//返回后不需要提示了
	  QMessageBox finalmsgBox;
	  QString finalMsg = tr("编译成功完成!\n 共扫描 ") + QString::number(lineCount - 1)  + tr(" 行数据\n 有效数据 ") + 
                          QString::number(ValidLine)  + tr(" 行！\n 编译变量 ") + QString::number(DataCount) + 
                          tr(" 个！\n 生成 ") + QString::number(curPos) + tr(" 字节！") ;
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();
  }


  csvFile->close();
  delete csvFile;
  distFile.close();




  return true; //处理完成
}




