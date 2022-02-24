/************************************************************************************************

                   ccj bin�ļ���ֽű�����ģ��

************************************************************************************************/


#include "dialog.h"

bool  Dialog::Pro_BinSplit(QTextStream &t) //����true�������
{
  //============================================�õ�����Ҫ��ֵ��ļ�========================================
  //�����ļ�
  QFile *inFile = new QFile(directoryLabel->text());
  if(inFile->open(QIODevice::ReadOnly) == false){//�ļ���ʧ��
	  QMessageBox finalmsgBox;
	  QString finalMsg = directoryLabel->text() + tr(" δ�ҵ����ʧ��,�ϲ�����ֹ��");
	  finalmsgBox.setText(finalMsg);
    finalmsgBox.exec();

    inFile->close();
    delete inFile;
     return false;
  }
  //=======================================��ȡ����ļ�����Ŀ¼========================================
  QString outDir;
  if(BatNestDeep == 0){//��ǰȷ��
    outDir = QFileDialog::getExistingDirectory(this,
	  								 tr("ָ������ļ����Ŀ¼..."),
		  							 directoryLabel->text(),
		  							 QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly);
  }
  else outDir = BatNestOutFile[BatNestDeep - 1]; //������ָ��

  if (outDir.isEmpty()){
	  QMessageBox finalmsgBox;
	  QString finalMsg = tr(" δָ���ļ����λ��,��������ֹ��");
	  finalmsgBox.setText(finalMsg);
    finalmsgBox.exec();

    inFile->close();
    delete inFile;
     return false;
  }
  //=======================================��ȡ��������λ����·��λ��========================================
  unsigned long inSize = inFile->size();
  int LinePos = 0;
  do{
    if(t.atEnd()) break; //������
	  QString Line = t.readLine();
    LinePos++;
	  QStringList LineData = Line.split(';'); //;��Ϊע��
	  QStringList Para = LineData[0].split(','); //,������������
    QString fileName = Para[0].simplified(); //ȥ��ǰ��ո�
    if((fileName[0] == '/') && ((fileName[1] == '/') )) continue; //��//�������� 
    if(Para.count() < 3){
      QMessageBox msgBox;
      msgBox.setText(QString::number(LinePos)  + tr("�У�ȱ�ٲ���, ������ֹ��"));
      msgBox.exec();
      inFile->close();
      delete inFile;
      return false;
    }
    //�õ�����ļ�λ��
    if(fileName[1] != ':'){//��ǰ����·��
        fileName = outDir + '\\' +fileName; //��ȡλ�ò���ϳɾ���Ŀ¼
    }
    //�õ���ʼλ��
    QString string = Para[1].simplified(); //ȥ��ǰ��ո�
    bool OK;
    unsigned long Start = string.toULong(&OK,10);
    if(OK == false){
      Start = string.toULong(&OK,16);
      if(OK == false){
        QMessageBox msgBox;
        msgBox.setText(QString::number(LinePos)  + tr("�У�����ʶ�����ʼλ��, ������ֹ��"));
        msgBox.exec();
        inFile->close();
        delete inFile;
        return false;
      }
    }
    //�õ�����
    string = Para[2].simplified(); //ȥ��ǰ��ո�
    unsigned long Count = string.toULong(&OK,10);
    if(OK == false){
      Count = string.toULong(&OK,16);
      if(OK == false){
        QMessageBox msgBox;
        msgBox.setText(QString::number(LinePos)  + tr("�У�����ʶ��Ķ�ȡ����, ������ֹ��"));
        msgBox.exec();
        inFile->close();
        delete inFile;
        return false;
      }
    }
    //���������ʼλ�úϷ���
    QString Err = QString();
    if(Start >= inSize)  Err =  tr("�У���ʼλ��С���ļ���С, ������ֹ��");
    else{
      if(Count == 0) Count = inSize - Start;//�����������
      else if(Count >= (inSize - Start))  Err =  tr("�У���ȡ���������ļ�ĩβ��������ֹ��");
    }
    if(!Err.isNull()){
        QMessageBox msgBox;
        msgBox.setText(QString::number(LinePos)  + Err);
        msgBox.exec();
        inFile->close();
        delete inFile;
        return false;
    }
    //��Ҫ������ļ�
    QFile *curFile = new QFile(fileName);
    if(curFile->open(QIODevice::WriteOnly | QIODevice::Truncate	) == false){//�ļ���ʧ��
	    QMessageBox finalmsgBox;
	    finalmsgBox.setText(QString::number(LinePos)  + tr("δ�ҵ����ʧ��,��������ֹ!"));
	    finalmsgBox.exec();

      delete curFile;
      inFile->close();
      delete inFile;
      return false;
    };

    //�������ļ�
    inFile->seek(Start);//����Ҫ��λ��
	  //����������
    QDataStream source(inFile);  //��������
    char *raw = new char[Count];
	  source.readRawData(raw, Count);
	  //�ϲ���������
    QDataStream dest(curFile);  //��������
    dest.writeRawData(raw,Count);//�ϲ�
    delete raw;
    curFile->close();
    delete curFile;
  }while(1);

  //������ɷ���
  inFile->close();
  delete inFile;

  return true; //�������
}




