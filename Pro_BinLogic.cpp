/************************************************************************************************

                   ccj bin文件逻辑处理脚本处理模块

************************************************************************************************/


#include "dialog.h"

bool  Dialog::Pro_BinLogic(QTextStream &t) //返回true处理完成
{
  //==========================================第2行打A文件=================================
	QString Line = t.readLine();
	QStringList Para = Line.split(';'); //;后为注释
  QString curPara = Para[0].simplified(); //去除前后空格
  QString Pos;
  if(curPara[1] != ':')//当前工作路径
    Pos = directoryLabel->text() + '\\' + curPara; //提取位置并组合成绝对目录
  else //绝路路径
    Pos = curPara;
  //加载文件
  QFile *aFile = new QFile(Pos);
  if(aFile->open(QIODevice::ReadOnly) == false){//文件打开失败
	  QMessageBox finalmsgBox;
	  QString finalMsg = Pos + tr(" 即逻辑A文件未找到或打开失败,合并已中止！");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

    delete aFile;
    return false;
  }

  //==========================================第3行打B文件=================================
	Line = t.readLine();
	Para = Line.split(';'); //;后为注释
  curPara = Para[0].simplified(); //去除前后空格
  if(curPara[1] != ':')//当前工作路径
    Pos = directoryLabel->text() + '\\' + curPara; //提取位置并组合成绝对目录
  else //绝路路径
    Pos = curPara;
  //加载文件
  QFile *bFile = new QFile(Pos);
  if(bFile->open(QIODevice::ReadOnly) == false){//文件打开失败
	  QMessageBox finalmsgBox;
	  QString finalMsg = Pos + tr(" 即逻辑B文件未找到或打开失败,合并已中止！");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

    aFile->close();
    delete aFile;
    delete bFile;
    return false;
  }
  //======================================第4行读取逻辑类型========================================
  //第二行指定逻辑类型ID
  Line = t.readLine();
  Para = Line.split(';'); //;后为注释
  bool OK;
  unsigned char type = Para[0].toInt(&OK,10);
  if(OK == false){
    QMessageBox msgBox;
    msgBox.setText(tr("逻辑类型无法识别，指定无效!"));
    msgBox.exec();
    return false;
  }

  //============================================得到临时目标文件===================================
  QTemporaryFile cFile;
  if(!cFile.open()) {//临时文件创建失败
	  QMessageBox finalmsgBox;
	  QString finalMsg = tr("文件处理所需的临时文件申请空间失败，请检查系统空间否满!");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

    aFile->close();
    delete aFile;
    bFile->close();
    delete bFile;
    return false;
  }

  //=======================================逻辑处理========================================
  QDataStream A(aFile);  //结果为数据流，需二进制处理
  QDataStream B(bFile);  //结果为数据流，需二进制处理
  QDataStream C(&cFile);  //结果为数据流，需二进制处理

  qint64 aSize = aFile->size();
  qint64 bSize = bFile->size();
  qint64 size = (aSize > bSize) ? bSize : aSize; //取小值

  quint8 a,b,c;
  for(; size > 0; size--){//若文件较大，可优化改为小循环。
     A >> a; B >> b;//读出数值
     switch(type){
       case 0: c = a ^ b; break;
       case 1: c = a | b; break;
       case 2: c = a & b; break;
       case 3: c = a + b; break;
       case 4: c = a + b; if(c < a){c = 0xff;} break;
       case 5: c = a - b; break;
       case 6: c = b - a; break;
       case 10: if(a != 0) c = b; else c = 0; break;
       case 11: if(a != 0) c = 0; else c = b; break;
       case 12: if(a != 0) c = b; else c = 0xff; break;
       case 13: if(a != 0) c = 0xff; else c = b; break;
       default: c = b; break;
     }
     C << c; //写入文件
  }//end for
  //C=B处理：
  if(bSize > aSize){
    qint64 curSize = bSize - aSize;
    char *raw = new char[curSize];
	  B.readRawData(raw, curSize);
    C.writeRawData(raw ,curSize);//合并
  }

  //======================================正确时最后保存数据========================================
  cFile.flush();//保存
  QString fileName;
  if(BatNestDeep == 0) //当前提定
    fileName = QFileDialog::getSaveFileName(0, tr("保存成功生成的文件..."),QDir::currentPath(),tr("Bin格式(*.Bin)"));
  else fileName = BatNestOutFile[BatNestDeep - 1]; //批处理指定

  QFile::remove (fileName); //强制先删除
  if(!cFile.copy(fileName)){
	  QMessageBox finalmsgBox;
	  QString finalMsg = fileName + tr("\n 目标文件错误或加载处理异常!");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();


    aFile->close();
    delete aFile;
    bFile->close();
    delete bFile;
    cFile.close();
    return false;
  }

  aFile->close();
  delete aFile;
  bFile->close();
  delete bFile;
  cFile.close();
  return true; //处理完成
}




