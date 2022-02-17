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

#include <QtGui>

#include "dialog.h"


Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
{
    int frameStyle = QFrame::Sunken | QFrame::Panel;

    openFileNameLabel = new QLabel;
    openFileNameLabel->setFrameStyle(frameStyle);
    QPushButton *openFileNameButton =
            new QPushButton(tr("�򿪽ű��ļ�.."));

     directoryLabel = new QLabel;
     directoryLabel->setFrameStyle(frameStyle);
     QPushButton *directoryButton =
             new QPushButton(tr("�򿪴������ļ�Ŀ¼"));

     QPushButton *saveFileNameButton =
             new QPushButton(tr("������.."));

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

    setWindowTitle(tr("CCJ�๦���ļ��ϲ���"));
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
    if (!fileName.isEmpty())
        openFileNameLabel->setText(fileName);
}

 void Dialog::setExistingDirectory()
 {
     QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
     QString directory = QFileDialog::getExistingDirectory(this,
                                 tr("�������ļ�Ŀ¼..."),
                                 directoryLabel->text(),
                                 options);
     if (!directory.isEmpty())
         directoryLabel->setText(directory);
 }


void Dialog::setSaveFileName()
{
  //============================================�򿪽ű��ļ���������Ϣ===================================
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

  //============================================��ȡ�����ǰ������Ϣ===================================
  //����Ϊ���ļ��ϲ�V1.00;��
  QString Line = t.readLine();//
  QStringList Para = Line.split(';'); //;��Ϊע��
  if(Para[0] != tr("ccj�ļ��ϲ������ű�V1.00")){ //������������
    QMessageBox msgBox;
    msgBox.setText(tr("�ļ������������������Ϊ��ccj�ļ��ϲ������ű�V1.00;����txt�ļ�"));
    msgBox.exec();

    descFile->close();
    delete descFile;
    return;

  }
  //�ڶ���ָ��Ŀ����ʼλ�ã�����ʮ�����Ʊ�ʾ
  Line = t.readLine();
  Para = Line.split(';'); //;��Ϊע��
  bool OK;
  unsigned int Base = Para[0].toInt(&OK,16);
  if(OK == false){
    QMessageBox msgBox;
    msgBox.setText(tr("��ʼλ��ָ����Ч��Ӧ�ԡ�0x00000000����ʽ���!"));
    msgBox.exec();

    descFile->close();
    delete descFile;
    return;
  }


  //�����еõ��ϲ��ļ�����,��������ͷ����,����ָ�����ļ�������ֲ��ϲ����������ֲ�0x00000000
  Line = t.readLine();
  Para = Line.split(';'); //;��Ϊע��
  //�õ��ϲ��ļ�����
  int binFileCount = Para[0].toInt();
  if((binFileCount <= 0) || (binFileCount > 1000)){
    QMessageBox msgBox;
    msgBox.setText(tr("�ϲ��ļ������������󣬵ڶ���ӦΪ1~1000֮������֣����ԡ�;����β"));
    msgBox.exec();

    descFile->close();
    delete descFile;
    return;
  }
  //=======================================��ȡ�������·��λ��========================================
  //��������Ϊ��ϲ��ļ�����·��,��;���н�β,�Ȼ��·��λ��
  QStringList listPath;
  int ValidCount = 0;
  for(; ValidCount < binFileCount; ValidCount++){
	Line = t.readLine();
	Para = Line.split(';'); //;��Ϊע��
    if(Para[0].isEmpty()) break; //������
    QString Pos;
	if(Para[0][0] == ' ') Pos = ' '; //�ո��ʾ�м�Ԥ��
    else Pos = directoryLabel->text() + '\\' + Para[0]; //��ϳɾ���Ŀ¼
    listPath << Pos;
  }
  if(ValidCount < 1){
    QMessageBox msgBox;
    msgBox.setText(tr("��������δ�ҵ���ϲ��ļ�λ��"));
    msgBox.exec();

    descFile->close();
    delete descFile;
    return;
  }

  //============================================�õ�Ŀ���ļ�===================================
  QString fileName = QFileDialog::getSaveFileName(0, tr("�����ļ�..."),QDir::homePath(),tr("Bin��ʽ(*.Bin)"));
  QFile *distFile = new QFile(fileName);
  if(distFile->open(QIODevice::WriteOnly) == false){//�ļ���ʧ��

	QMessageBox finalmsgBox;
	QString finalMsg = tr("δָ�������ļ�������ش����쳣!");
	finalmsgBox.setText(finalMsg);
	finalmsgBox.exec();

    delete distFile;
    return;
  }

  //=======================================���Ŀ���ļ�ǰ��������ͷ����========================================
  //Ϊ��Ԥ�����к�������ʱ��ֱ�����0x00000000
  unsigned long curPos = (binFileCount + 1) * 4 + Base;//���ڼ���ļ������������
  QDataStream dest(distFile);  //���Ϊ��������������ƴ���
  for(int Pos = 0; Pos < ValidCount; Pos++){
   //���ļ�Ԥ��
	if(listPath[Pos][0] == ' '){
      dest << (qint32)curPos;
	  continue;
	}
	//��ȡ�ļ���Ϣ�еĴ�С
    QFileInfo FileInfo(listPath[Pos]);
    if(FileInfo.exists() == false){//�ļ�������ʱ
	  QMessageBox finalmsgBox;
	  QString finalMsg = listPath[Pos] + tr(" δ���ҵ�,Ԥ��������ֹ��");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

	  descFile->close();
      delete descFile;
      //distFile->close();
      delete distFile;
      return;
    };
    qint64 Size = FileInfo.size();
    if(Size >= (qint64)(0xffffffff - curPos)){
	  QMessageBox finalmsgBox;
	  QString finalMsg = listPath[Pos] + tr(" �ϲ����ļ�����,Ԥ��������ֹ��");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

	  descFile->close();
      delete descFile;
      //distFile->close();
      delete distFile;
      return;
    }
	//���䵱ǰ������ʼλ��
    dest << (qint32)curPos;
	//�����¸�������ʼλ��
    curPos += Size; 
  }
  //��������ʱ��ֱ��������ֵ,���������Ͻ���λ��
  for(int Pos = ValidCount; Pos <= binFileCount; Pos++){
    dest << (qint32)curPos;
  }

  //=======================================���Ŀ������========================================
  curPos = (binFileCount + 1) * 4 + Base;//���ڼ���ļ������������
  for(int Pos = 0; Pos < ValidCount; Pos++){
   //���ļ�����
	if(listPath[Pos][0] == ' '){
	  continue;
	}
    //�����ļ�
    QFile *curFile = new QFile(listPath[Pos]);
    if(curFile->open(QIODevice::ReadOnly) == false){//�ļ���ʧ��
	  QMessageBox finalmsgBox;
	  QString finalMsg = listPath[Pos] + tr(" δ�ҵ����ʧ��,�ϲ�����ֹ��");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

      delete curFile;
	  descFile->close();
      delete descFile;
      //distFile->close();
      delete distFile;
      return;
    };
	qint64 curSize = curFile->size();
    if(curSize > (qint64)(0xffffffff - curPos)){
	  QMessageBox finalmsgBox;
	  QString finalMsg = listPath[Pos] + tr(" �ϲ����ļ�����,�ϲ�����ֹ��");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

      delete curFile;
	  descFile->close();
      delete descFile;
      //distFile->close();
      delete distFile;
      return;
    }
	//����������
    QDataStream source(curFile);  //��������
    char *raw = new char[curSize];
	source.readRawData(raw, curSize);
	//�ϲ���������
    dest.writeRawData(raw ,curSize);//�ϲ�
	//�����¸�������ʼλ��
    curPos += curSize; 
    delete raw;
    delete curFile;
  }


  //=======================================��󱣴��ļ�����ʾ�ɹ�==============================
  descFile->close();
  delete descFile;
  distFile->close();
  delete distFile;
  //��ʾ�ɹ�
  QMessageBox finalmsgBox;
  QString finalMsg = tr("�ļ��ϲ��ɹ�!");
  finalmsgBox.setText(finalMsg);
  finalmsgBox.exec();
 
}
