/************************************************************************************************

                   ccj��Դ�ļ��ϲ��ű�����ģ��

************************************************************************************************/


#include "dialog.h"

bool  Dialog::Pro_ResourceMerge(QTextStream &t) //����true�������
{
  //=======================================��ȡ������Ϣ========================================
  //�ڶ���ָ��Ŀ����ʼλ�ã�����ʮ�����Ʊ�ʾ
  QString Line = t.readLine();
  QStringList Para = Line.split(';'); //;��Ϊע��
  bool OK;
  unsigned int Base = Para[0].toLongLong(&OK,16);
  if(OK == false){
    QMessageBox msgBox;
    msgBox.setText(tr("��ʼλ��ָ����Ч��Ӧ�ԡ�0xnnnnnnnn����ʽ���!"));
    msgBox.exec();
    return false;
  }


  //�����еõ��ϲ��ļ�����,��������ͷ����,����ָ�����ļ�������ֲ��ϲ����������ֲ�0x00000000
  Line = t.readLine();
  Para = Line.split(';'); //;��Ϊע��
  Para = Para[0].split(','); //,�ָ�������
  //�õ��ϲ��ļ�����
  int binFileCount = Para[0].toInt();
  if((binFileCount <= 0) || (binFileCount > 1000)){
    QMessageBox msgBox;
    msgBox.setText(tr("�ϲ��ļ������������󣬵ڶ���ӦΪ1~1000֮������֣����ԡ�;����β"));
    msgBox.exec();
    return false;
  }

  //������ָ�����ֽ�ʱ��Ϸ�ʽ
  bool isMsb = true; //Ĭ�ϴ��
  if(Para.count() >= 2){
    bool curMsb = Para[1].toInt(&OK);
    if(OK == true) isMsb = curMsb;
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

  //============================================�õ���ʱĿ���ļ�===================================
  QTemporaryFile distFile;
  if(!distFile.open()) {//��ʱ�ļ�����ʧ��
	QMessageBox finalmsgBox;
	QString finalMsg = tr("�ļ������������ʱ�ļ�����ռ�ʧ�ܣ�����ϵͳ�ռ����!");
	finalmsgBox.setText(finalMsg);
	finalmsgBox.exec();

    return false;
  }

  //=======================================���Ŀ���ļ�ǰ��������ͷ����========================================
  //Ϊ��Ԥ�����к�������ʱ��ֱ�����0x00000000
  unsigned long curPos = (binFileCount + 1) * 4 + Base;//���ڼ���ļ������������
  QDataStream dest(&distFile);  //���Ϊ��������������ƴ���
  if(isMsb) dest.setByteOrder(QDataStream::BigEndian);//��˸�λ��ǰ
  else dest.setByteOrder(QDataStream::LittleEndian);//С�˵�λ��ǰ

  for(int Pos = 0; Pos < ValidCount; Pos++){
   //���ļ�Ԥ��
	if(listPath[Pos][0] == ' '){
      dest << (quint32)curPos;
	  continue;
	}
	//��ȡ�ļ���Ϣ�еĴ�С
    QFileInfo FileInfo(listPath[Pos]);
    if(FileInfo.exists() == false){//�ļ�������ʱ
	  QMessageBox finalmsgBox;
	  QString finalMsg = listPath[Pos] + tr(" δ���ҵ�,Ԥ��������ֹ��");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

      distFile.close();
      return false;
    };
    qint64 Size = FileInfo.size();
    if(Size >= (qint64)(0xffffffff - curPos)){
	  QMessageBox finalmsgBox;
	  QString finalMsg = listPath[Pos] + tr(" �ϲ����ļ�����,Ԥ��������ֹ��");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

      distFile.close();
      return false;
    }
	//���䵱ǰ������ʼλ��
    dest << (quint32)curPos;
	//�����¸�������ʼλ��
    curPos += Size; 
  }
  //��������ʱ��ֱ��������ֵ,���������Ͻ���λ��
  for(int Pos = ValidCount; Pos <= binFileCount; Pos++){
    dest << (quint32)curPos;
  }

  //=======================================���Ŀ������========================================
  curPos = (binFileCount + 1) * 4;//���ڼ���ļ������������
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
      distFile.close();
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
      distFile.close();
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

  //======================================��ȷʱ��󱣴�����========================================
  distFile.flush();//����
  QString fileName = QFileDialog::getSaveFileName(0, tr("����ɹ����ɵ��ļ�..."),QDir::currentPath(),tr("Bin��ʽ(*.Bin)"));
  QFile::remove (fileName); //ǿ����ɾ��
  if(!distFile.copy(fileName)){
	QMessageBox finalmsgBox;
	QString finalMsg = tr("δָ�������ļ�����ش����쳣!");
	finalmsgBox.setText(finalMsg);
	finalmsgBox.exec();

    distFile.close();
    return false;
  }

  distFile.close();
  return true; //�������
 
}




