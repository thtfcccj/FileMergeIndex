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
            new QPushButton(tr("�򿪽ű��ļ�..."));

     directoryLabel = new QLabel;
     directoryLabel->setFrameStyle(frameStyle);
     directoryButton =
             new QPushButton(tr("�򿪴������ļ�Ŀ¼..."));

     QPushButton *saveFileNameButton =
             new QPushButton(tr("ִ�в�������..."));

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

    setWindowTitle(tr("CCJ�๦���ļ��ϲ������� V1.10       thtfcccj��������"));
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
        openFileNameLabel->setText(fileName);
	    Pro(true);//����Ԥ����
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
									tr("Text Files (*.csv)"),
									&selectedFilter,
									options);
		 if (!fileName.isEmpty())
		  	directoryLabel->setText(fileName);
	 }
	 if(Fun == 4){//��֧�ֵ�ͼ���ļ�
		 QFileDialog::Options options;
		 QString selectedFilter;
		 QString fileName = QFileDialog::getOpenFileName(this,
									tr("֧�ֵ�ͼ���ļ�..."),
									openFileNameLabel->text(),
									tr("֧�ֵ�ͼƬ (*.wbm;*.bmp;*.gif);;wbmp Files (*.wbm);;bmp Files (*.bmp);;gif Files (*.gif)"),
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
 }


void Dialog::setSaveFileName()
{
  Pro(false);//����
}

//�����ļ�
void Dialog::Pro(bool isIdent)//�Ƿ�Ϊʶ��
{
  //�򿪽ű��ļ���������Ϣ
  QFile *descFile = new QFile(openFileNameLabel->text());
  if(descFile->open(QIODevice::ReadOnly) == false){//�ļ���ʧ��

	QMessageBox finalmsgBox;
	QString finalMsg = tr("�ű��ļ���ʧ��,������ָ�����!");
	finalmsgBox.setText(finalMsg);
	finalmsgBox.exec();

    delete descFile;
    return;
  }
  QTextStream t(descFile);//�ű�Ϊ�ļ���

  //��ȡ��������нű�������Ϣ���ֱ���
  QString Line = t.readLine();//
  QStringList Para = Line.split(';'); //;��Ϊע��

  bool Resume = false;//ʶ��ʱ����ʾ
  IsDisFinal = false;//Ĭ��������ʾ
  if((Para[0] == tr("ccj�ļ��ϲ������ű�V1.00")) || //��Դ�ϲ��� ԭ��������
     (Para[0] == tr("ccj��Դ�ļ��ϲ��ű�V1.00"))){  //������
     if(Fun != 1){//�ű��л�ʱ
       Fun = 1;
       directoryLabel->setText("");
	     directoryButton->setText(tr("�򿪴��ϲ��ļ�����Ŀ¼..."));
	   }
     if(isIdent == false) Resume = Pro_ResourceMerge(t); 
  }

  else if(Para[0] == tr("ccj bin�ļ��ϲ������ű�V1.00")){//
    if(Fun != 2){//�ű��л�ʱ
      Fun = 2;
      directoryLabel->setText("");
	    directoryButton->setText(tr("�򿪴��ϲ��ļ�����Ŀ¼..."));
	  }
    if(isIdent == false) Resume = Pro_BinMerge(t); //bin�ļ��ϲ���
  }

  else if(Para[0] == tr("ccj ���ñ���ѡ��V1.00")){//
    if(Fun != 3){//�ű��л�ʱ
      Fun = 3;
      directoryLabel->setText("");
	    directoryButton->setText(tr("��������csv��ʽ�ļ�..."));
	  }
    if(isIdent == false) Resume = Pro_CfgCompile(t); //bin�ļ��ϲ���
  }
  else if(Para[0] == tr("ccj ePicͼƬת������V1.00")){//
    if(Fun != 4){//�ű��л�ʱ
      Fun = 4;
      directoryLabel->setText("");
	    directoryButton->setText(tr("����ת����ͼ��..."));
	  }
    if(isIdent == false) Resume = Pro_ePicTrans(t); 
  }
  else{
    QMessageBox msgBox;
    msgBox.setText(tr("�����ļ���������ʶ���������ȷ�Ľű��ļ�!"));
    msgBox.exec();

	Resume = false;
  }

   descFile->close();
   delete descFile;

  //����ɹ�����
  if((Resume == true) &&  (IsDisFinal == false)){
    QMessageBox finalmsgBox;
    QString finalMsg = tr("����ɹ���                   ");
    finalmsgBox.setText(finalMsg);
    finalmsgBox.exec();
  }
}
