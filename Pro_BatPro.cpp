/************************************************************************************************

                   ccj 批处理模块

************************************************************************************************/


#include "dialog.h"

bool  Dialog::Pro_BatPro(QTextStream &t) //返回true处理完成
{
  //扫描前处理嵌套
  if(BatNestDeep > _BAT_NEST_DEEP){
		QMessageBox msgBox;
		msgBox.setText( tr("批处理嵌套深度不应大于3，编译中止！"));
		msgBox.exec();
		return false;
  }

  QString prvScpriptName(openFileNameLabel->text());
  QString prvDirName(directoryLabel->text());
  QString curUnitName;

  int UnitState = 0;//一个处理单元的状态
  int LinePos = 1;//正在扫描的行位置,从1开始
  int curFunType = -1;//当前编译类型
  do{
    if(t.atEnd()) break; //结束了
    LinePos++; //从1开始
    QString Line = t.readLine();
    if(Line.isEmpty()) continue; //本行无数据
    Line = Line.simplified(); //去除前后空格
    if((Line[0] == '/') && ((Line[1] == '/'))){//注解处理
      continue; //继续下一行
    }

    QStringList linePara = Line.split(';'); //;后为注释
    if(linePara[0].isEmpty()) continue; //本行无有效数据，继续下一行

    //得到到以“:”间隔的首后字符
    QStringList subPara = linePara[0].split(':'); //:后为名称
    if(subPara.size() < 2){
	    QMessageBox msgBox;
      msgBox.setText(QString::number(LinePos)  + tr("行：未找到以“:”符号，编译中止"));
	    msgBox.exec();
      openFileNameLabel->setText(prvScpriptName);
      directoryLabel->setText(prvDirName);
      noteLabel->setText(tr("在第 ") + QString::number(LinePos)  + tr("  行出现编译错误，已中止！"));
      return false;
    }
    QString para0 = subPara[0].simplified(); //去除前后空格
    QString para1 = subPara[1].simplified(); //去除前后空格
    //================================单元头处理======================================
    //格式示例：>>cfgCompile：编译全部配置文件;
    if((para0[0] == '>') && ((para0[1] == '>'))){
      UnitState = 1; //单元状态有编译器了
      curUnitName = QString(para1);//curUnitName.clear(); curUnitName.append(para1);//得到名称
      //得到功能ID
      if(para0 == ">>resourceMerge")curFunType = 1;
      else if(para0 == ">>bineMerge")curFunType = 2;
      else if(para0 == ">>cfgCompile")curFunType = 3;
      else if(para0 == ">>ePicTrans")curFunType = 4;
      else if(para0 == ">>stringCompile")curFunType = 6;
      else if(para0 == ">>binSplit")curFunType = 7;
      else{
	      QMessageBox msgBox;
        msgBox.setText(QString::number(LinePos)  + tr("行：不能识别的编译器名称，注意区分大小写，编译中止"));
	      msgBox.exec();
        openFileNameLabel->setText(prvScpriptName);
        directoryLabel->setText(prvDirName);
        noteLabel->setText(tr("在第 ") + QString::number(LinePos)  + tr("  行出现编译错误，已中止！"));
        return false;
      }
      continue; //继续下一行
    }
    //================================脚本名称处理===================================
    //指定或重替换脚本名称
    if(para0 == "scriptFile"){
      if(UnitState < 1){
	      QMessageBox msgBox;
        msgBox.setText(QString::number(LinePos)  + tr("行：脚本文件前未指定编译器，编译中止"));
	      msgBox.exec();
        openFileNameLabel->setText(prvScpriptName);
        directoryLabel->setText(prvDirName);
        noteLabel->setText(tr("在第 ") + QString::number(LinePos)  + tr("  行出现编译错误，已中止！"));
        return false;
      }
      UnitState = 2; //单元状态有编译器和脚本文件名了
      if(para1[1] != ':')//当前工作路径
        openFileNameLabel->setText(WorkDir + para1);
      else //绝路路径
        openFileNameLabel->setText(para1);

      if(Pro(true) == false){//编译预处理,更新按键与编译类型,已在内部对话框提示了
        openFileNameLabel->setText(prvScpriptName);
        directoryLabel->setText(prvDirName);
        noteLabel->setText(tr("在第 ") + QString::number(LinePos)  + tr("  行出现编译错误，已中止！"));
        return false;
      }
      if(Fun != curFunType){
	      QMessageBox msgBox;
        msgBox.setText(QString::number(LinePos)  + tr("行：脚本内首行识别码与当前单元编译器不对应，编译中止"));
	      msgBox.exec();
        openFileNameLabel->setText(prvScpriptName);
        directoryLabel->setText(prvDirName);
        noteLabel->setText(tr("在第 ") + QString::number(LinePos)  + tr("  行出现编译错误，已中止！"));
        return false;
      }
      continue; //继续下一行
    }
    //================================路径或文件名称处理===================================
    //指定或重替换脚本名称
    if((curFunType <= 2) && (para0 == "InDir") ||//合并文件时为目录
     (curFunType == 3) && (para0 == "csvFile") ||
     (curFunType == 4) && (para0 == "picFile") ||
     (curFunType == 6) && (para0 == "txtFile") ||
     (curFunType == 7) && (para0 == "splitFile"))
    {
      if(UnitState < 2){
	      QMessageBox msgBox;
        msgBox.setText(QString::number(LinePos)  + tr("行：未指定编译器及脚本，编译中止"));
	      msgBox.exec();
        openFileNameLabel->setText(prvScpriptName);
        directoryLabel->setText(prvDirName);
        noteLabel->setText(tr("在第 ") + QString::number(LinePos)  + tr("  行出现编译错误，已中止！"));
        return false;
      }
      UnitState = 3; //单元状态有编译器和脚本文件名，文件了
      if((para1.size() >  1) || subPara.size() <= 2) //当前工作路径
        directoryLabel->setText(WorkDir + para1);
      else //绝路路径首字为A~F, 中间:被分成了两半
        directoryLabel->setText(para1 + subPara[2]);
      continue; //继续下一行
    }
    //================================输出执行处理===================================
    //指定或重替换脚本名称
    if(para0 == "outFile"){
      if(UnitState < 3){
	      QMessageBox msgBox;
        msgBox.setText(QString::number(LinePos)  + tr("行：输出前未指定编译器及脚本，编译中止"));
	      msgBox.exec();
        openFileNameLabel->setText(prvScpriptName);
        directoryLabel->setText(prvDirName);
        noteLabel->setText(tr("在第 ") + QString::number(LinePos)  + tr("  行出现编译错误，已中止！"));
        return false;
      }

      QString OutFile;
      if((para1.size() >  1) || subPara.size() <= 2) //当前工作路径
        OutFile = WorkDir + para1;
      else //绝路路径首字为A~F, 中间:被分成了两半
        OutFile = para1 + subPara[2];

      BatNestOutFile.insert(BatNestDeep - 1,OutFile);
      //更新提示
      noteLabel->setText(tr("正在编译: ") + curUnitName + tr("  单元,输出:") + OutFile);

      if(Pro(false) == false){//正式处理异常，需退出
        openFileNameLabel->setText(prvScpriptName);
        directoryLabel->setText(prvDirName);
        noteLabel->setText(tr("在第 ") + QString::number(LinePos) + tr("  行出现编译错误，已中止！"));
        return false;
      }
      continue; //继续下一行
    }

	  QMessageBox msgBox;
    msgBox.setText(QString::number(LinePos)  + tr("行：脚本描述有误，编译中止"));
	  msgBox.exec();
    openFileNameLabel->setText(prvScpriptName);
    directoryLabel->setText(prvDirName);
    noteLabel->setText(tr("在第 ") + QString::number(LinePos) + tr("  行出现编译错误，已中止！"));
    return false;
  }
  while(1);
   
  openFileNameLabel->setText(prvScpriptName);
  directoryLabel->setText(prvDirName);
  noteLabel->setText(tr("编译成功完成！"));
  return true; //处理完成
}




