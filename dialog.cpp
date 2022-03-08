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
            new QPushButton(tr("�򿪽ű��ļ�..."));

     directoryLabel = new QLabel;
     directoryLabel->setFrameStyle(frameStyle);
     directoryButton =
             new QPushButton(tr("�򿪴������ļ�Ŀ¼..."));

     saveFileNameButton =
             new QPushButton(tr("ִ�в�������..."));

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

    setWindowTitle(tr("CCJ�๦���ļ������������ V2.0   thtfcccj��������"));
}

void Dialog::setOpenFileName()
{
    QFileDialog::Options options;
    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName(this,
                                tr("�ű��ļ�..."),
                                openFileNameLabel->text(),
                                tr("Text Files (*.txt)"),
                                &selectedFilter,
                                options);
	if (!fileName.isEmpty()){
      noteLabel->setText(tr(""));
      openFileNameLabel->setText(fileName);
      BatNestDeep = 0;//��ͷ��ʼ
	    Pro(true);//����Ԥ����
      BatNestDeep = 0;//���λ
	}
}

 void Dialog::setExistingDirectory()
 {
	 if(Fun == 3){//�򿪱����ļ�
		 QFileDialog::Options options;
		 QString selectedFilter;
		 QString fileName = QFileDialog::getOpenFileName(this,
									tr("������csv��ʽ�ļ�..."),
									openFileNameLabel->text(),
									tr("csv Files (*.csv)"),
									&selectedFilter,
									options);
		 if (!fileName.isEmpty())
		  	directoryLabel->setText(fileName);
	 }
	 else if(Fun == 4){//��֧�ֵ�ͼ���ļ�
		 QFileDialog::Options options;
		 QString selectedFilter;
		 QString fileName = QFileDialog::getOpenFileName(this,
									tr("֧�ֵ�ͼ���ļ�..."),
									openFileNameLabel->text(),
									tr("֧�ֵ�ͼƬ (*.png;*.wbm;*.bmp;*.gif);;png Files (*.png);;wbmp Files (*.wbm);;bmp Files (*.bmp);;gif Files (*.gif)"),
									&selectedFilter,
									options);
		 if (!fileName.isEmpty())
		  	directoryLabel->setText(fileName);
	 }
	 else if(Fun == 5){//��������ű��ļ�
		 QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
		 QString directory = QFileDialog::getExistingDirectory(this,
									 tr("ѡ��ű�������Ŀ¼..."),
									 directoryLabel->text(),
									 options);
     if (!directory.isEmpty()){
			 directoryLabel->setText(directory);
       WorkDir = directory;
     }
	 }
	 else if(Fun == 6){//���ַ����ļ�
		 QFileDialog::Options options;
		 QString selectedFilter;
		 QString fileName = QFileDialog::getOpenFileName(this,
									tr("������txt��ʽ�ַ����ļ�..."),
									openFileNameLabel->text(),
									tr("txt Files (*.txt)"),
									&selectedFilter,
									options);
		 if (!fileName.isEmpty())
		  	directoryLabel->setText(fileName);
   }
   else if(Fun == 7){//�����ֵ��ļ�
		 QFileDialog::Options options;
		 QString selectedFilter;
		 QString fileName = QFileDialog::getOpenFileName(this,
									tr("���ֵ��ļ�..."),
									openFileNameLabel->text(),
									tr("*.* Files (*.*)"),
									&selectedFilter,
									options);
		 if (!fileName.isEmpty())
		  	directoryLabel->setText(fileName);
	 }
	 else if(Fun == 9){//PNGתRGBA
		 QFileDialog::Options options;
		 QString selectedFilter;
		 QString fileName = QFileDialog::getOpenFileName(this,
									tr("PNGͼ���ļ�..."),
									openFileNameLabel->text(),
									tr("PNGͼƬ (*.png)"),
									&selectedFilter,
									options);
		 if (!fileName.isEmpty())
		  	directoryLabel->setText(fileName);
	 }

	 else{//��Ŀ¼
		 QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
		 QString directory = QFileDialog::getExistingDirectory(this,
									 tr("�������ļ�Ŀ¼..."),
									 directoryLabel->text(),
									 options);
		 if (!directory.isEmpty())
			 directoryLabel->setText(directory);
	 }

   noteLabel->setText(tr(""));
 }


void Dialog::setSaveFileName()
{
  //�ر�ȫ����ť
  directoryButton->setEnabled(false);
  openFileNameButton->setEnabled(false);
  saveFileNameButton->setEnabled(false);

  Pro(false);//����

  //����ȫ����ť
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

//�����ļ�
bool Dialog::Pro(bool isIdent)//�Ƿ�Ϊʶ��
{
  //�򿪽ű��ļ���������Ϣ
  QFile *descFile = new QFile(openFileNameLabel->text());
    if(descFile->open(QIODevice::ReadOnly) == false){//�ļ���ʧ��
	  QMessageBox finalmsgBox;
	  QString finalMsg = openFileNameLabel->text() + tr("\n�ű���ʧ��,������ָ�����!");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

    delete descFile;
    return false;
  }
  QTextStream t(descFile);//�ű�Ϊ�ļ���

  //��ȡ��������нű�������Ϣ���ֱ���
  QString Line = t.readLine();//
  QStringList Para = Line.split(';'); //;��Ϊע��

  bool Resume = true;//������ʾ��ʾ
  IsDisFinal = false;//Ĭ��������ʾ
  if((Para[0] == tr("ccj�ļ��ϲ������ű�V1.00")) || //��Դ�ϲ��� ԭ��������
     (Para[0] == tr("ccj��Դ�ļ��ϲ��ű�V1.00"))){  //������
     if(Fun != 1){//�ű��л�ʱ
       Fun = 1;
       directoryLabel->setText("");
       if(BatNestDeep == 0) directoryButton->setEnabled(true);
	     directoryButton->setText(tr("�򿪴��ϲ��ļ�����Ŀ¼..."));
	   }
     if(isIdent == false){
       if(BatNestDeep == 0) noteLabel->setText(tr("��Դ�ļ��ϲ����ڴ���..."));
       Resume = Pro_ResourceMerge(t); 
     }
  }

  else if(Para[0] == tr("ccj bin�ļ��ϲ������ű�V1.00")){//
    if(Fun != 2){//�ű��л�ʱ
      Fun = 2;
      directoryLabel->setText("");
      if(BatNestDeep == 0) directoryButton->setEnabled(true);
	    directoryButton->setText(tr("�򿪴��ϲ��ļ�����Ŀ¼..."));
	  }
     if(isIdent == false){
       if(BatNestDeep == 0) noteLabel->setText(tr("bin�ļ��ϲ����ڴ���..."));
       Resume = Pro_BinMerge(t); //bin�ļ��ϲ���
     }
  }

  else if(Para[0] == tr("ccj ���ñ���ѡ��V1.00")){//
    if(Fun != 3){//�ű��л�ʱ
      Fun = 3;
      directoryLabel->setText("");
      if(BatNestDeep == 0) directoryButton->setEnabled(true);
	    directoryButton->setText(tr("��������csv��ʽ�ļ�..."));
	  }
     if(isIdent == false){
       if(BatNestDeep == 0) noteLabel->setText(tr("�����ļ����ڱ���..."));
       Resume = Pro_CfgCompile(t);
     }
  }
  else if(Para[0] == tr("ccj ePicͼƬת������V1.00")){//
    if(Fun != 4){//�ű��л�ʱ
      Fun = 4;
      directoryLabel->setText("");
      if(BatNestDeep == 0) directoryButton->setEnabled(true);
	    directoryButton->setText(tr("����ת����ͼ��..."));
	  }
     if(isIdent == false){
       if(BatNestDeep == 0) noteLabel->setText(tr("���ڴ���ͼƬ����..."));
       Resume = Pro_ePicTrans(t); 
     }
  }
  else if(Para[0] == tr("������ִ�нű�V1.00")){//
    if(Fun != 5){//�ű��л�ʱ
      Fun = 5;
      directoryLabel->setText("");
	    if(BatNestDeep == 0) directoryButton->setEnabled(true);
      directoryButton->setText(tr("�ű���ָ���ļ���Ŀ¼..."));
	  }
    if(isIdent == false){
      BatNestDeep++;
      Resume = Pro_BatPro(t);
      BatNestDeep--;
      //���������ػָ�
      Fun = 5;
      directoryButton->setText(tr("�ű����ļ���Ŀ¼..."));
    }
  }
  else if(Para[0] == tr("ccj�ַ�����Դ�������ýű�V1.00")){//
    if(Fun != 6){//�ű��л�ʱ
      Fun = 6;
      directoryLabel->setText("");
      if(BatNestDeep == 0) directoryButton->setEnabled(true);
	    directoryButton->setText(tr("�ַ�����Դ�ļ�����Ŀ¼..."));
	  }
     if(isIdent == false){
       if(BatNestDeep == 0) noteLabel->setText(tr("�ַ�����Դ���ڱ���..."));
       Resume = Pro_StringCompile(t);
     }
  }
  else if(Para[0] == tr("ccj bin�ļ���������ű�V1.00")){//
    if(Fun != 7){//�ű��л�ʱ
      Fun = 7;
      directoryLabel->setText("");
      if(BatNestDeep == 0) directoryButton->setEnabled(true);
	    directoryButton->setText(tr("������ļ�..."));
	  }
     if(isIdent == false){
       if(BatNestDeep == 0) noteLabel->setText(tr("�ļ����ڲ��..."));
       Resume = Pro_BinSplit(t);
     }
  }
  else if(Para[0] == tr("ccj Bin�ļ��߼�����ű�V1.00")){//
    if(Fun != 8){//�ű��л�ʱ
      Fun = 8;
      directoryLabel->setText("");
      if(BatNestDeep == 0) directoryButton->setEnabled(true);
	    directoryButton->setText(tr("�߼��ļ�Ŀ¼..."));
	  }
     if(isIdent == false){
       if(BatNestDeep == 0) noteLabel->setText(tr("�ļ������߼�..."));
       Resume = Pro_BinLogic(t);
     }
  }
  else if(Para[0] == tr("ccj PNG�ļ�תRGBA���ݴ���ű�V1.00")){//
    if(Fun != 9){//�ű��л�ʱ
      Fun = 9;
      directoryLabel->setText("");
      if(BatNestDeep == 0) directoryButton->setEnabled(true);
	    directoryButton->setText(tr("��PNG��ʽͼ��..."));
	  }
     if(isIdent == false){
       if(BatNestDeep == 0) noteLabel->setText(tr("����ת��..."));
       Resume = Pro_Png2RGBA(t);
     }
  }


  else{
    QMessageBox msgBox;
    msgBox.setText(tr("���нű�������������ʶ���������ȷ�Ľű��ļ�!"));
    msgBox.exec();
	  Resume = false;
  }
  descFile->close();
  delete descFile;

  if(isIdent == true) return Resume; //ʶ�𷵻�
  else{
     if(Resume == true) noteLabel->setText(tr("�����ѳɹ���ɣ�"));
     else if(Fun != 5)  noteLabel->setText(tr("������̷�������"));
  }

  //����ɹ�����
  if((BatNestDeep == 0) && (Resume == true) &&  (IsDisFinal == false)){
    noteLabel->setText(tr("�ɹ�������ɣ��Ѳ�������ļ���"));
    QMessageBox finalmsgBox;
    QString finalMsg = tr("����ɹ���                   ");
    finalmsgBox.setText(finalMsg);
    finalmsgBox.exec();
  }

  return Resume;
}
