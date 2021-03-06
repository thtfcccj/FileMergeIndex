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
    settings = new QSettings("Settings.ini",QSettings::IniFormat);


    openFileNameLabel = new QLabel;
    openFileNameLabel->setFrameStyle(frameStyle);
    openFileNameButton =
            new QPushButton(tr("????????????..."));

     directoryLabel = new QLabel;
     directoryLabel->setFrameStyle(frameStyle);
     directoryButton =
             new QPushButton(tr("??????????????????..."));

     saveFileNameButton =
             new QPushButton(tr("??????????????..."));

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

    setWindowTitle(tr("CCJ?????????????????????? V2.1   thtfcccj????????"));
}

void Dialog::setOpenFileName()
{
    QFileDialog::Options options;
    QString selectedFilter;
    QString Path = openFileNameLabel->text();
    if(Path.isEmpty()){
      Path = settings->value("LastScriptFile").toString();
    }
    QString fileName = QFileDialog::getOpenFileName(this,
                                tr("????????..."),
                                Path,
                                tr("Text Files (*.txt)"),
                                &selectedFilter,
                                options);
	if (!fileName.isEmpty()){
      settings->setValue("LastScriptFile",fileName);
      noteLabel->setText(tr(""));
      openFileNameLabel->setText(fileName);
      BatNestDeep = 0;//????????
	    Pro(true);//??????????
      BatNestDeep = 0;//????????
	}
}

 void Dialog::setExistingDirectory()
 {
	 if(Fun == 3){//????????????
		 QFileDialog::Options options;
		 QString selectedFilter;
		 QString fileName = QFileDialog::getOpenFileName(this,
									tr("????????csv????????..."),
									settings->value("LastStringCompileFile").toString(),
									tr("csv Files (*.csv)"),
									&selectedFilter,
									options);
     if (!fileName.isEmpty()){
		  	directoryLabel->setText(fileName);
        settings->setValue("LastStringCompileFile",fileName);
     }
	 }
	 else if(Fun == 4){//??????????????????
		 QFileDialog::Options options;
		 QString selectedFilter;
		 QString fileName = QFileDialog::getOpenFileName(this,
									tr("??????????????..."),
									settings->value("LastImageFile").toString(),
									tr("?????????? (*.png;*.wbm;*.bmp;*.gif);;png Files (*.png);;wbmp Files (*.wbm);;bmp Files (*.bmp);;gif Files (*.gif)"),
									&selectedFilter,
									options);
     if (!fileName.isEmpty()){
		  	directoryLabel->setText(fileName);
        settings->setValue("LastImageFile",fileName);
     }
	 }
	 else if(Fun == 5){//??????????????????
		 QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
		 QString directory = QFileDialog::getExistingDirectory(this,
									 tr("??????????????????..."),
									 settings->value("LastBatProPath").toString(),
									 options);
     if (!directory.isEmpty()){
       settings->setValue("LastBatProPath",directory);
			 directoryLabel->setText(directory);
       WorkDir = directory;
     }
	 }
	 else if(Fun == 6){//??????????????
		 QFileDialog::Options options;
		 QString selectedFilter;
		 QString fileName = QFileDialog::getOpenFileName(this,
									tr("????????txt??????????????..."),
									settings->value("LastStringFile").toString(),
									tr("txt Files (*.txt)"),
									&selectedFilter,
									options);
     if (!fileName.isEmpty()){
       settings->setValue("LastStringFile", fileName);
		   directoryLabel->setText(fileName);
     }
   }
   else if(Fun == 7){//????????????????
		 QFileDialog::Options options;
		 QString selectedFilter;
		 QString fileName = QFileDialog::getOpenFileName(this,
									tr("????????????..."),
									settings->value("LastBinSplitFile").toString(),
									tr("*.* Files (*.*)"),
									&selectedFilter,
									options);
     if (!fileName.isEmpty()){
		  	directoryLabel->setText(fileName);
        settings->setValue("LastBinSplitFile", fileName);
     }
	 }
	 else if(Fun == 9){//PNG??RGBA
		 QFileDialog::Options options;
		 QString selectedFilter;
		 QString fileName = QFileDialog::getOpenFileName(this,
									tr("PNG????????..."),
									settings->value("LastPngFile").toString(),
									tr("PNG???? (*.png)"),
									&selectedFilter,
									options);
     if (!fileName.isEmpty()){
		  	directoryLabel->setText(fileName);
        settings->setValue("LastPngFile", fileName);
     }
	 }
	 else{//????????
		 QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
     QString Path;
     if(Fun == 1) Path = settings->value("LastResourceMergePath").toString();
     else if(Fun == 2) Path = settings->value("LastBinMergePath").toString();
     else if(Fun == 8) Path = settings->value("LastBinLogicPath").toString();
     else Path = settings->value("LastDefaultPath").toString();
		 QString directory = QFileDialog::getExistingDirectory(this,
									 tr("??????????????..."),
									 Path,
									 options);
     if (!directory.isEmpty()){
			 directoryLabel->setText(directory);
       if(Fun == 1)settings->setValue("LastResourceMergePath", directory);
       else if(Fun == 2) settings->setValue("LastBinMergePath", directory);
       else if(Fun == 8) settings->setValue("LastBinLogicPath", directory);
       else settings->setValue("LastDefaultPath", directory);
     }
	 }

   noteLabel->setText(tr(""));
 }


void Dialog::setSaveFileName()
{
  //????????????
  directoryButton->setEnabled(false);
  openFileNameButton->setEnabled(false);
  saveFileNameButton->setEnabled(false);

  Pro(false);//????

  //????????????
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

//????????
bool Dialog::Pro(bool isIdent)//??????????
{
  //??????????????????????
  QFile *descFile = new QFile(openFileNameLabel->text());
    if(descFile->open(QIODevice::ReadOnly) == false){//????????????
	  QMessageBox finalmsgBox;
	  QString finalMsg = openFileNameLabel->text() + tr("\n????????????,????????????????!");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

    delete descFile;
    return false;
  }
  QTextStream t(descFile);//????????????

  //????????????????????????????????????
  QString Line = t.readLine();//
  QStringList Para = Line.split(';'); //;????????

  bool Resume = true;//????????????
  IsDisFinal = false;//????????????
  if((Para[0] == tr("ccj????????????????V1.00")) || //?????????? ??????????
     (Para[0] == tr("ccj????????????????V1.00"))){  //??????
     if(Fun != 1){//??????????
       Fun = 1;
       directoryLabel->setText("");
       if(BatNestDeep == 0) directoryButton->setEnabled(true);
	     directoryButton->setText(tr("??????????????????????..."));
	   }
     if(isIdent == false){
       if(BatNestDeep == 0) noteLabel->setText(tr("????????????????????..."));
       Resume = Pro_ResourceMerge(t); 
     }
  }

  else if(Para[0] == tr("ccj bin????????????????V1.00")){//
    if(Fun != 2){//??????????
      Fun = 2;
      directoryLabel->setText("");
      if(BatNestDeep == 0) directoryButton->setEnabled(true);
	    directoryButton->setText(tr("??????????????????????..."));
	  }
     if(isIdent == false){
       if(BatNestDeep == 0) noteLabel->setText(tr("bin????????????????..."));
       Resume = Pro_BinMerge(t); //bin??????????
     }
  }

  else if(Para[0] == tr("ccj ????????????V1.00")){//
    if(Fun != 3){//??????????
      Fun = 3;
      directoryLabel->setText("");
      if(BatNestDeep == 0) directoryButton->setEnabled(true);
	    directoryButton->setText(tr("????????????csv????????..."));
	  }
     if(isIdent == false){
       if(BatNestDeep == 0) noteLabel->setText(tr("????????????????..."));
       Resume = Pro_CfgCompile(t);
     }
  }
  else if(Para[0] == tr("ccj ePic????????????V1.00")){//
    if(Fun != 4){//??????????
      Fun = 4;
      directoryLabel->setText("");
      if(BatNestDeep == 0) directoryButton->setEnabled(true);
	    directoryButton->setText(tr("????????????????..."));
	  }
     if(isIdent == false){
       if(BatNestDeep == 0) noteLabel->setText(tr("????????????????..."));
       Resume = Pro_ePicTrans(t); 
     }
  }
  else if(Para[0] == tr("??????????????V1.00")){//
    if(Fun != 5){//??????????
      Fun = 5;
      directoryLabel->setText("");
	    if(BatNestDeep == 0) directoryButton->setEnabled(true);
      directoryButton->setText(tr("????????????????????..."));
	  }
    if(isIdent == false){
      BatNestDeep++;
      Resume = Pro_BatPro(t);
      BatNestDeep--;
      //????????????????
      Fun = 5;
      directoryButton->setText(tr("????????????????..."));
    }
  }
  else if(Para[0] == tr("ccj??????????????????????V1.00")){//
    if(Fun != 6){//??????????
      Fun = 6;
      directoryLabel->setText("");
      if(BatNestDeep == 0) directoryButton->setEnabled(true);
	    directoryButton->setText(tr("??????????????????????..."));
	  }
     if(isIdent == false){
       if(BatNestDeep == 0) noteLabel->setText(tr("??????????????????..."));
       Resume = Pro_StringCompile(t);
     }
  }
  else if(Para[0] == tr("ccj bin????????????????V1.00")){//
    if(Fun != 7){//??????????
      Fun = 7;
      directoryLabel->setText("");
      if(BatNestDeep == 0) directoryButton->setEnabled(true);
	    directoryButton->setText(tr("??????????..."));
	  }
     if(isIdent == false){
       if(BatNestDeep == 0) noteLabel->setText(tr("????????????..."));
       Resume = Pro_BinSplit(t);
     }
  }
  else if(Para[0] == tr("ccj Bin????????????????V1.00")){//
    if(Fun != 8){//??????????
      Fun = 8;
      directoryLabel->setText("");
      if(BatNestDeep == 0) directoryButton->setEnabled(true);
	    directoryButton->setText(tr("????????????..."));
	  }
     if(isIdent == false){
       if(BatNestDeep == 0) noteLabel->setText(tr("????????????..."));
       Resume = Pro_BinLogic(t);
     }
  }
  else if(Para[0] == tr("ccj PNG??????RGBA????????????V1.00")){//
    if(Fun != 9){//??????????
      Fun = 9;
      directoryLabel->setText("");
      if(BatNestDeep == 0) directoryButton->setEnabled(true);
	    directoryButton->setText(tr("????PNG????????..."));
	  }
     if(isIdent == false){
       if(BatNestDeep == 0) noteLabel->setText(tr("????????..."));
       Resume = Pro_Png2RGBA(t);
     }
  }


  else{
    QMessageBox msgBox;
    msgBox.setText(tr("??????????????????????????????????????????????!"));
    msgBox.exec();
	  Resume = false;
  }
  descFile->close();
  delete descFile;

  if(isIdent == true) return Resume; //????????
  else{
     if(Resume == true) noteLabel->setText(tr("????????????????"));
     else if(Fun != 5)  noteLabel->setText(tr("??????????????????"));
  }

  //????????????
  if((BatNestDeep == 0) && (Resume == true) &&  (IsDisFinal == false)){
    noteLabel->setText(tr("??????????????????????????????"));
    QMessageBox finalmsgBox;
    QString finalMsg = tr("??????????                   ");
    finalmsgBox.setText(finalMsg);
    finalmsgBox.exec();
  }

  return Resume;
}
