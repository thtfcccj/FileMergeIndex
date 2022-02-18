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
            new QPushButton(tr("�򿪽ű��ļ�..."));

     directoryLabel = new QLabel;
     directoryLabel->setFrameStyle(frameStyle);
     QPushButton *directoryButton =
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

    setWindowTitle(tr("CCJ�๦���ļ��ϲ��� V1.10       thtfcccj��������"));
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

  bool Resume;
  if(Para[0] == tr("ccj�ļ��ϲ������ű�V1.00")){//��Դ�ϲ��� ԭ�������� 
    Resume = Pro_ResourceMerge(t); 
  }
  else if(Para[0] == tr("ccj��Դ�ļ��ϲ��ű�V1.00")){//������
    Resume = Pro_ResourceMerge(t); //��Դ�ϲ���
  }
  //else if(Para[0] == tr("ccj bin�ļ��ϲ������ű�V1.00")){//
  //  Resume = Pro_BinMerge(t); //bin�ļ��ϲ���
  //}
  else{
    QMessageBox msgBox;
    msgBox.setText(tr("�����ļ���������ʶ���������ȷ�Ľű��ļ�!"));
    msgBox.exec();

	Resume = false;
  }

   descFile->close();
   delete descFile;

  //����ɹ�����
  if(Resume == true){
    QMessageBox finalmsgBox;
    QString finalMsg = tr("����ɹ���                   ");
    finalmsgBox.setText(finalMsg);
    finalmsgBox.exec();
  }
}

/************************************************************************************************
                   ccj��Դ�ļ��ϲ��ű��������
************************************************************************************************/
bool  Dialog::Pro_ResourceMerge(QTextStream &t) //����true�������
{
  //=======================================��ȡ������Ϣ========================================
  //�ڶ���ָ��Ŀ����ʼλ�ã�����ʮ�����Ʊ�ʾ
  QString Line = t.readLine();
  QStringList Para = Line.split(';'); //;��Ϊע��
  bool OK;
  unsigned int Base = Para[0].toInt(&OK,16);
  if(OK == false){
    QMessageBox msgBox;
    msgBox.setText(tr("��ʼλ��ָ����Ч��Ӧ�ԡ�0x00000000����ʽ���!"));
    msgBox.exec();
    return false;
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
    return false;
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
    return false;
  }

  //============================================�õ�Ŀ���ļ�===================================
  QString fileName = QFileDialog::getSaveFileName(0, tr("�����ļ�..."),QDir::currentPath(),tr("Bin��ʽ(*.Bin)"));
  QFile *distFile = new QFile(fileName);
  if(distFile->open(QIODevice::ReadWrite) == false){//�ļ���ʧ��

	QMessageBox finalmsgBox;
	QString finalMsg = tr("δָ�������ļ�������ش����쳣!");
	finalmsgBox.setText(finalMsg);
	finalmsgBox.exec();

    delete distFile;
    return false;
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

      //distFile->close();
      delete distFile;
      return false;
    };
    qint64 Size = FileInfo.size();
    if(Size >= (qint64)(0xffffffff - curPos)){
	  QMessageBox finalmsgBox;
	  QString finalMsg = listPath[Pos] + tr(" �ϲ����ļ�����,Ԥ��������ֹ��");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

      //distFile->close();
      delete distFile;
      return false;
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
      //distFile->close();
      delete distFile;
      return false;
    };
	qint64 curSize = curFile->size();
    if(curSize > (qint64)(0xffffffff - curPos)){
	  QMessageBox finalmsgBox;
	  QString finalMsg = listPath[Pos] + tr(" �ϲ����ļ�����,�ϲ�����ֹ��");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

      delete curFile;
      curFile->close();
      //distFile->close();
      delete distFile;
      return false;
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

    curFile->close();
    delete curFile;
  }


  distFile->close(); //����
  delete distFile;
  return true; //�������
 
}




