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
: QDialog(parent),Fun(-1)
{
    int frameStyle = QFrame::Sunken | QFrame::Panel;

    openFileNameLabel = new QLabel;
    openFileNameLabel->setFrameStyle(frameStyle);
    QPushButton *openFileNameButton =
            new QPushButton(tr("打开脚本文件..."));

     directoryLabel = new QLabel;
     directoryLabel->setFrameStyle(frameStyle);
     directoryButton =
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

    setWindowTitle(tr("CCJ多功能文件合并编译器 V1.10       thtfcccj倾情制作"));
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
	if (!fileName.isEmpty()){
        openFileNameLabel->setText(fileName);
	    Pro(true);//编译预处理
	}
}

 void Dialog::setExistingDirectory()
 {
	 if(Fun == 3){//打开编译文件
		 QFileDialog::Options options;
		 QString selectedFilter;
		 QString fileName = QFileDialog::getOpenFileName(this,
									tr("需编译的csv格式文件..."),
									openFileNameLabel->text(),
									tr("Text Files (*.csv)"),
									&selectedFilter,
									options);
		 if (!fileName.isEmpty())
		  	directoryLabel->setText(fileName);
	 }
	 if(Fun == 4){//打开支持的图像文件
		 QFileDialog::Options options;
		 QString selectedFilter;
		 QString fileName = QFileDialog::getOpenFileName(this,
									tr("支持的图像文件..."),
									openFileNameLabel->text(),
									tr("支持的图片 (*.wbm;*.bmp;*.gif);;wbmp Files (*.wbm);;bmp Files (*.bmp);;gif Files (*.gif)"),
									&selectedFilter,
									options);
		 if (!fileName.isEmpty())
		  	directoryLabel->setText(fileName);
	 }


	 else{//打开目录
		 QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
		 QString directory = QFileDialog::getExistingDirectory(this,
									 tr("待处理文件目录..."),
									 directoryLabel->text(),
									 options);
		 if (!directory.isEmpty())
			 directoryLabel->setText(directory);
	 }
 }


void Dialog::setSaveFileName()
{
  Pro(false);//编译
}

//处理文件
void Dialog::Pro(bool isIdent)//是否为识别
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

  bool Resume = false;//识别时不提示
  IsDisFinal = false;//默认允许提示
  if((Para[0] == tr("ccj文件合并描述脚本V1.00")) || //资源合并器 原描述兼容
     (Para[0] == tr("ccj资源文件合并脚本V1.00"))){  //新描述
     if(Fun != 1){//脚本切换时
       Fun = 1;
       directoryLabel->setText("");
	     directoryButton->setText(tr("打开待合并文件所在目录..."));
	   }
     if(isIdent == false) Resume = Pro_ResourceMerge(t); 
  }

  else if(Para[0] == tr("ccj bin文件合并描述脚本V1.00")){//
    if(Fun != 2){//脚本切换时
      Fun = 2;
      directoryLabel->setText("");
	    directoryButton->setText(tr("打开待合并文件所在目录..."));
	  }
    if(isIdent == false) Resume = Pro_BinMerge(t); //bin文件合并器
  }

  else if(Para[0] == tr("ccj 配置编译选项V1.00")){//
    if(Fun != 3){//脚本切换时
      Fun = 3;
      directoryLabel->setText("");
	    directoryButton->setText(tr("打开需编译的csv格式文件..."));
	  }
    if(isIdent == false) Resume = Pro_CfgCompile(t); //bin文件合并器
  }
  else if(Para[0] == tr("ccj ePic图片转换配置V1.00")){//
    if(Fun != 4){//脚本切换时
      Fun = 4;
      directoryLabel->setText("");
	    directoryButton->setText(tr("打开需转换的图像..."));
	  }
    if(isIdent == false) Resume = Pro_ePicTrans(t); 
  }
  else{
    QMessageBox msgBox;
    msgBox.setText(tr("首行文件描述不能识别，请加载正确的脚本文件!"));
    msgBox.exec();

	Resume = false;
  }

   descFile->close();
   delete descFile;

  //处理成功返回
  if((Resume == true) &&  (IsDisFinal == false)){
    QMessageBox finalmsgBox;
    QString finalMsg = tr("处理成功！                   ");
    finalmsgBox.setText(finalMsg);
    finalmsgBox.exec();
  }
}
