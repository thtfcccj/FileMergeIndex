/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the example classes of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/


#include "dialog.h"


Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
{
    int frameStyle = QFrame::Sunken | QFrame::Panel;

    openFileNameLabel = new QLabel;
    openFileNameLabel->setFrameStyle(frameStyle);
    QPushButton *openFileNameButton =
            new QPushButton(tr("打开脚本文件..."));

     directoryLabel = new QLabel;
     directoryLabel->setFrameStyle(frameStyle);
     QPushButton *directoryButton =
             new QPushButton(tr("打开待处理文件目录..."));

     QPushButton *saveFileNameButton =
             new QPushButton(tr("执行并保存结果..."));

    connect(openFileNameButton, SIGNAL(clicked()),
            this, SLOT(setOpenFileName()));

     connect(directoryButton, SIGNAL(clicked()),
             this, SLOT(setExistingDirectory()));

     connect(saveFileNameButton, SIGNAL(clicked()),
             this, SLOT(setSaveFileName()));

#ifndef Q_WS_WIN
#ifndef Q_OS_MAC
    native->hide();
#endif
#endif
    QGridLayout *layout = new QGridLayout;
    layout->setColumnStretch(1, 1);
    layout->setColumnMinimumWidth(1, 250);

    layout->addWidget(openFileNameButton, 1, 0);
    layout->addWidget(openFileNameLabel, 1, 1);

	layout->addWidget(directoryButton, 2, 0);
    layout->addWidget(directoryLabel, 2, 1);

    layout->addWidget(saveFileNameButton, 3, 0);

    setLayout(layout);

    setWindowTitle(tr("CCJ多功能文件合并器 V1.10       thtfcccj倾情制作"));
}

void Dialog::setOpenFileName()
{
    QFileDialog::Options options;
    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName(this,
                                tr("脚本文件..."),
                                openFileNameLabel->text(),
                                tr("Text Files (*.txt)"),
                                &selectedFilter,
                                options);
    if (!fileName.isEmpty())
        openFileNameLabel->setText(fileName);
}

 void Dialog::setExistingDirectory()
 {
     QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
     QString directory = QFileDialog::getExistingDirectory(this,
                                 tr("待处理文件目录..."),
                                 directoryLabel->text(),
                                 options);
     if (!directory.isEmpty())
         directoryLabel->setText(directory);
 }


void Dialog::setSaveFileName()
{
  //打开脚本文件并处理信息
  QFile *descFile = new QFile(openFileNameLabel->text());
  if(descFile->open(QIODevice::ReadOnly) == false){//文件打开失败

	QMessageBox finalmsgBox;
	QString finalMsg = tr("脚本文件打开失败,请重新指定或打开!");
	finalmsgBox.setText(finalMsg);
	finalmsgBox.exec();

    delete descFile;
    return;
  }
  QTextStream t(descFile);//脚本为文件流

  //获取并检查首行脚本类型信息并分别处理
  QString Line = t.readLine();//
  QStringList Para = Line.split(';'); //;后为注释

  bool Resume;
  if(Para[0] == tr("ccj文件合并描述脚本V1.00")){//资源合并器 原描述兼容 
    Resume = Pro_ResourceMerge(t); 
  }
  else if(Para[0] == tr("ccj资源文件合并脚本V1.00")){//新描述
    Resume = Pro_ResourceMerge(t); //资源合并器
  }
  //else if(Para[0] == tr("ccj bin文件合并描述脚本V1.00")){//
  //  Resume = Pro_BinMerge(t); //bin文件合并器
  //}
  else{
    QMessageBox msgBox;
    msgBox.setText(tr("首行文件描述不能识别，请加载正确的脚本文件!"));
    msgBox.exec();

	Resume = false;
  }

   descFile->close();
   delete descFile;

  //处理成功返回
  if(Resume == true){
    QMessageBox finalmsgBox;
    QString finalMsg = tr("处理成功！                   ");
    finalmsgBox.setText(finalMsg);
    finalmsgBox.exec();
  }
}

/************************************************************************************************
                   ccj资源文件合并脚本处理程序
************************************************************************************************/
bool  Dialog::Pro_ResourceMerge(QTextStream &t) //返回true处理完成
{
  //=======================================读取配置信息========================================
  //第二行指定目标起始位置，需以十六进制表示
  QString Line = t.readLine();
  QStringList Para = Line.split(';'); //;后为注释
  bool OK;
  unsigned int Base = Para[0].toInt(&OK,16);
  if(OK == false){
    QMessageBox msgBox;
    msgBox.setText(tr("起始位置指定无效，应以“0x00000000”方式表达!"));
    msgBox.exec();
    return false;
  }


  //第三行得到合并文件个数,决定数据头长度,后续指定的文件多出部分不合并，不够部分补0x00000000
  Line = t.readLine();
  Para = Line.split(';'); //;后为注释
  //得到合并文件个数
  int binFileCount = Para[0].toInt();
  if((binFileCount <= 0) || (binFileCount > 1000)){
    QMessageBox msgBox;
    msgBox.setText(tr("合并文件个数描述错误，第二行应为1~1000之间的数字，并以“;”结尾"));
    msgBox.exec();
    return false;
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
    else Pos = directoryLabel->text() + '\\' + Para[0]; //组合成绝对目录
    listPath << Pos;
  }
  if(ValidCount < 1){
    QMessageBox msgBox;
    msgBox.setText(tr("第三行起，未找到需合并文件位置"));
    msgBox.exec();
    return false;
  }

  //============================================得到目标文件===================================
  QString fileName = QFileDialog::getSaveFileName(0, tr("保存文件..."),QDir::currentPath(),tr("Bin格式(*.Bin)"));
  QFile *distFile = new QFile(fileName);
  if(distFile->open(QIODevice::ReadWrite) == false){//文件打开失败

	QMessageBox finalmsgBox;
	QString finalMsg = tr("未指定保存文件，或加载处理异常!");
	finalmsgBox.setText(finalMsg);
	finalmsgBox.exec();

    delete distFile;
    return false;
  }

  //=======================================填充目标文件前部的数据头索引========================================
  //为空预留，有后续不满时，直接填充0x00000000
  unsigned long curPos = (binFileCount + 1) * 4 + Base;//用于检查文件容量超限情况
  QDataStream dest(distFile);  //结果为数据流，需二进制处理
  for(int Pos = 0; Pos < ValidCount; Pos++){
   //空文件预留
	if(listPath[Pos][0] == ' '){
      dest << (qint32)curPos;
	  continue;
	}
	//获取文件信息中的大小
    QFileInfo FileInfo(listPath[Pos]);
    if(FileInfo.exists() == false){//文件不存在时
	  QMessageBox finalmsgBox;
	  QString finalMsg = listPath[Pos] + tr(" 未被找到,预处理已中止！");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

      //distFile->close();
      delete distFile;
      return false;
    };
    qint64 Size = FileInfo.size();
    if(Size >= (qint64)(0xffffffff - curPos)){
	  QMessageBox finalmsgBox;
	  QString finalMsg = listPath[Pos] + tr(" 合并后文件过大,预处理已中止！");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

      //distFile->close();
      delete distFile;
      return false;
    }
	//填充充当前数据起始位置
    dest << (qint32)curPos;
	//更新下个数据起始位置
    curPos += Size; 
  }
  //后续不满时，直接填充最后值,并在最后加上结束位置
  for(int Pos = ValidCount; Pos <= binFileCount; Pos++){
    dest << (qint32)curPos;
  }

  //=======================================填充目标数据========================================
  curPos = (binFileCount + 1) * 4 + Base;//用于检查文件容量超限情况
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
      //distFile->close();
      delete distFile;
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
      //distFile->close();
      delete distFile;
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


  distFile->close(); //保存
  delete distFile;
  return true; //处理完成
 
}




