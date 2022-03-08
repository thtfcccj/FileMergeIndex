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
: QDialog(parent),Fun(-1),BatNestOutFile(),WorkDir()
{
    int frameStyle = QFrame::Sunken | QFrame::Panel;

    openFileNameLabel = new QLabel;
    openFileNameLabel->setFrameStyle(frameStyle);
    openFileNameButton =
            new QPushButton(tr("打开脚本文件..."));

     directoryLabel = new QLabel;
     directoryLabel->setFrameStyle(frameStyle);
     directoryButton =
             new QPushButton(tr("打开待处理文件目录..."));

     saveFileNameButton =
             new QPushButton(tr("执行并保存结果..."));

     noteLabel = new QLabel;
     directoryLabel->setFrameStyle(frameStyle);

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

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addLayout(layout);
    vlayout->addWidget(noteLabel);

    setLayout(vlayout);

    setWindowTitle(tr("CCJ多功能文件处理与编译器 V2.0   thtfcccj倾情制作"));
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
      noteLabel->setText(tr(""));
      openFileNameLabel->setText(fileName);
      BatNestDeep = 0;//从头开始
	    Pro(true);//编译预处理
      BatNestDeep = 0;//最后复位
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
									tr("csv Files (*.csv)"),
									&selectedFilter,
									options);
		 if (!fileName.isEmpty())
		  	directoryLabel->setText(fileName);
	 }
	 else if(Fun == 4){//打开支持的图像文件
		 QFileDialog::Options options;
		 QString selectedFilter;
		 QString fileName = QFileDialog::getOpenFileName(this,
									tr("支持的图像文件..."),
									openFileNameLabel->text(),
									tr("支持的图片 (*.png;*.wbm;*.bmp;*.gif);;png Files (*.png);;wbmp Files (*.wbm);;bmp Files (*.bmp);;gif Files (*.gif)"),
									&selectedFilter,
									options);
		 if (!fileName.isEmpty())
		  	directoryLabel->setText(fileName);
	 }
	 else if(Fun == 5){//打开批处理脚本文件
		 QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
		 QString directory = QFileDialog::getExistingDirectory(this,
									 tr("选择脚本内文总目录..."),
									 directoryLabel->text(),
									 options);
     if (!directory.isEmpty()){
			 directoryLabel->setText(directory);
       WorkDir = directory;
     }
	 }
	 else if(Fun == 6){//打开字符串文件
		 QFileDialog::Options options;
		 QString selectedFilter;
		 QString fileName = QFileDialog::getOpenFileName(this,
									tr("需编译的txt格式字符串文件..."),
									openFileNameLabel->text(),
									tr("txt Files (*.txt)"),
									&selectedFilter,
									options);
		 if (!fileName.isEmpty())
		  	directoryLabel->setText(fileName);
   }
   else if(Fun == 7){//打开需拆分的文件
		 QFileDialog::Options options;
		 QString selectedFilter;
		 QString fileName = QFileDialog::getOpenFileName(this,
									tr("需拆分的文件..."),
									openFileNameLabel->text(),
									tr("*.* Files (*.*)"),
									&selectedFilter,
									options);
		 if (!fileName.isEmpty())
		  	directoryLabel->setText(fileName);
	 }
	 else if(Fun == 9){//PNG转RGBA
		 QFileDialog::Options options;
		 QString selectedFilter;
		 QString fileName = QFileDialog::getOpenFileName(this,
									tr("PNG图像文件..."),
									openFileNameLabel->text(),
									tr("PNG图片 (*.png)"),
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

   noteLabel->setText(tr(""));
 }


void Dialog::setSaveFileName()
{
  //关闭全部按钮
  directoryButton->setEnabled(false);
  openFileNameButton->setEnabled(false);
  saveFileNameButton->setEnabled(false);

  Pro(false);//编译

  //开启全部按钮
  directoryButton->setEnabled(true);
  openFileNameButton->setEnabled(true);
  saveFileNameButton->setEnabled(true);

}

void Dialog::Delay_MSec(unsigned int msec)
{
    QTime _Timer = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < _Timer )
      QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

//处理文件
bool Dialog::Pro(bool isIdent)//是否为识别
{
  //打开脚本文件并处理信息
  QFile *descFile = new QFile(openFileNameLabel->text());
    if(descFile->open(QIODevice::ReadOnly) == false){//文件打开失败
	  QMessageBox finalmsgBox;
	  QString finalMsg = openFileNameLabel->text() + tr("\n脚本打开失败,请重新指定或打开!");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

    delete descFile;
    return false;
  }
  QTextStream t(descFile);//脚本为文件流

  //获取并检查首行脚本类型信息并分别处理
  QString Line = t.readLine();//
  QStringList Para = Line.split(';'); //;后为注释

  bool Resume = true;//用于提示提示
  IsDisFinal = false;//默认允许提示
  if((Para[0] == tr("ccj文件合并描述脚本V1.00")) || //资源合并器 原描述兼容
     (Para[0] == tr("ccj资源文件合并脚本V1.00"))){  //新描述
     if(Fun != 1){//脚本切换时
       Fun = 1;
       directoryLabel->setText("");
       if(BatNestDeep == 0) directoryButton->setEnabled(true);
	     directoryButton->setText(tr("打开待合并文件所在目录..."));
	   }
     if(isIdent == false){
       if(BatNestDeep == 0) noteLabel->setText(tr("资源文件合并正在处理..."));
       Resume = Pro_ResourceMerge(t); 
     }
  }

  else if(Para[0] == tr("ccj bin文件合并描述脚本V1.00")){//
    if(Fun != 2){//脚本切换时
      Fun = 2;
      directoryLabel->setText("");
      if(BatNestDeep == 0) directoryButton->setEnabled(true);
	    directoryButton->setText(tr("打开待合并文件所在目录..."));
	  }
     if(isIdent == false){
       if(BatNestDeep == 0) noteLabel->setText(tr("bin文件合并正在处理..."));
       Resume = Pro_BinMerge(t); //bin文件合并器
     }
  }

  else if(Para[0] == tr("ccj 配置编译选项V1.00")){//
    if(Fun != 3){//脚本切换时
      Fun = 3;
      directoryLabel->setText("");
      if(BatNestDeep == 0) directoryButton->setEnabled(true);
	    directoryButton->setText(tr("打开需编译的csv格式文件..."));
	  }
     if(isIdent == false){
       if(BatNestDeep == 0) noteLabel->setText(tr("配置文件正在编译..."));
       Resume = Pro_CfgCompile(t);
     }
  }
  else if(Para[0] == tr("ccj ePic图片转换配置V1.00")){//
    if(Fun != 4){//脚本切换时
      Fun = 4;
      directoryLabel->setText("");
      if(BatNestDeep == 0) directoryButton->setEnabled(true);
	    directoryButton->setText(tr("打开需转换的图像..."));
	  }
     if(isIdent == false){
       if(BatNestDeep == 0) noteLabel->setText(tr("正在处理图片数据..."));
       Resume = Pro_ePicTrans(t); 
     }
  }
  else if(Para[0] == tr("批处理执行脚本V1.00")){//
    if(Fun != 5){//脚本切换时
      Fun = 5;
      directoryLabel->setText("");
	    if(BatNestDeep == 0) directoryButton->setEnabled(true);
      directoryButton->setText(tr("脚本内指定文件总目录..."));
	  }
    if(isIdent == false){
      BatNestDeep++;
      Resume = Pro_BatPro(t);
      BatNestDeep--;
      //其它处理返回恢复
      Fun = 5;
      directoryButton->setText(tr("脚本内文件总目录..."));
    }
  }
  else if(Para[0] == tr("ccj字符串资源编译配置脚本V1.00")){//
    if(Fun != 6){//脚本切换时
      Fun = 6;
      directoryLabel->setText("");
      if(BatNestDeep == 0) directoryButton->setEnabled(true);
	    directoryButton->setText(tr("字符串资源文件所在目录..."));
	  }
     if(isIdent == false){
       if(BatNestDeep == 0) noteLabel->setText(tr("字符串资源正在编译..."));
       Resume = Pro_StringCompile(t);
     }
  }
  else if(Para[0] == tr("ccj bin文件拆分描述脚本V1.00")){//
    if(Fun != 7){//脚本切换时
      Fun = 7;
      directoryLabel->setText("");
      if(BatNestDeep == 0) directoryButton->setEnabled(true);
	    directoryButton->setText(tr("被拆分文件..."));
	  }
     if(isIdent == false){
       if(BatNestDeep == 0) noteLabel->setText(tr("文件正在拆分..."));
       Resume = Pro_BinSplit(t);
     }
  }
  else if(Para[0] == tr("ccj Bin文件逻辑处理脚本V1.00")){//
    if(Fun != 8){//脚本切换时
      Fun = 8;
      directoryLabel->setText("");
      if(BatNestDeep == 0) directoryButton->setEnabled(true);
	    directoryButton->setText(tr("逻辑文件目录..."));
	  }
     if(isIdent == false){
       if(BatNestDeep == 0) noteLabel->setText(tr("文件正在逻辑..."));
       Resume = Pro_BinLogic(t);
     }
  }
  else if(Para[0] == tr("ccj PNG文件转RGBA数据处理脚本V1.00")){//
    if(Fun != 9){//脚本切换时
      Fun = 9;
      directoryLabel->setText("");
      if(BatNestDeep == 0) directoryButton->setEnabled(true);
	    directoryButton->setText(tr("打开PNG格式图像..."));
	  }
     if(isIdent == false){
       if(BatNestDeep == 0) noteLabel->setText(tr("正在转换..."));
       Resume = Pro_Png2RGBA(t);
     }
  }


  else{
    QMessageBox msgBox;
    msgBox.setText(tr("首行脚本功能描述不能识别，请加载正确的脚本文件!"));
    msgBox.exec();
	  Resume = false;
  }
  descFile->close();
  delete descFile;

  if(isIdent == true) return Resume; //识别返回
  else{
     if(Resume == true) noteLabel->setText(tr("处理已成功完成！"));
     else if(Fun != 5)  noteLabel->setText(tr("处理过程发生错误！"));
  }

  //处理成功返回
  if((BatNestDeep == 0) && (Resume == true) &&  (IsDisFinal == false)){
    noteLabel->setText(tr("成功处理完成，已产生输出文件！"));
    QMessageBox finalmsgBox;
    QString finalMsg = tr("处理成功！                   ");
    finalmsgBox.setText(finalMsg);
    finalmsgBox.exec();
  }

  return Resume;
}
