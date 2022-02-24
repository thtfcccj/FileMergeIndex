/************************************************************************************************

                   ccj��Դ�ļ��ϲ��ű�����ģ��

************************************************************************************************/


#include "dialog.h"


//�����������ָ�����ַ�,����ʱ����1�����򷵻�0
int Dialog::Pro_fullLenData(QDataStream &dest,
                              quint32 Data,
                              int Len)
{
  if(Len >= 4){
    dest << (quint32)Data;
    return 0;
  }
  if(Len >= 3){
    if(dest.byteOrder() == QDataStream::BigEndian){//��λ��ǰ
      dest << (quint16)(Data >> 8);
      dest << (quint8)Data;
    }
    else{ //��λ��ǰ
      dest << (quint16)Data;
      dest << (quint8)(Data >> 16);
    }
    if(Data > 0xffffff) return 1;
    return 0;
  }
  if(Len >= 2){
    dest << (quint16)Data;
    if(Data > 0xffff) return 1;
    return 0;
  }
  dest << (quint8)Data;
  if(Data > 0xff) return 1;
  return 0;
}


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
    bool isLsb = Para[1].toInt(&OK);
    if(OK == true) isMsb = !isLsb;//�෴
  }
  //������ָ�����ֽ�ʱ��Ϸ�ʽ
  int indexLen = 4; 
  if(Para.count() >= 3){
    int len = Para[2].toInt(&OK);
    if((OK == true) && (len <= 4) && (len >= 0)) indexLen = len;
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
    else{
      QString curPara = Para[0].simplified(); //ȥ��ǰ��ո�
      if(curPara[1] != ':')//��ǰ����·��
        Pos = directoryLabel->text() + '\\' + curPara; //��ϳɾ���Ŀ¼
      else //��··��
        Pos = curPara;
    }
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
  QDataStream dest(&distFile);  //���Ϊ��������������ƴ���
  if(isMsb) dest.setByteOrder(QDataStream::BigEndian);//��˸�λ��ǰ
  else dest.setByteOrder(QDataStream::LittleEndian);//С�˵�λ��ǰ

  //Ϊ��Ԥ�����к�������ʱ��ֱ�����0x00000000
  unsigned long curPos;
  if(indexLen){//������ͷʱ
    curPos = (binFileCount + 1) * indexLen + Base;//���ڼ���ļ������������
    int ErrCount = 0;
    for(int Pos = 0; Pos < ValidCount; Pos++){
       //���ļ�Ԥ��
	    if(listPath[Pos][0] == ' '){
        ErrCount += Pro_fullLenData(dest, curPos, indexLen);
	      continue;
	    }
	    //��ȡ�ļ���Ϣ�еĴ�С
      QFileInfo FileInfo(listPath[Pos]);
      if(FileInfo.exists() == false){//�ļ�������ʱ
	    QMessageBox finalmsgBox;
	    QString finalMsg = listPath[Pos] + tr(" δ���ҵ�,�����������ֹ��");
	    finalmsgBox.setText(finalMsg);
	    finalmsgBox.exec();

        distFile.close();
        return false;
      };
      qint64 Size = FileInfo.size();
      if(Size >= (qint64)(0xffffffff - curPos)){
	      QMessageBox finalmsgBox;
	      QString finalMsg = listPath[Pos] + tr(" �ϲ����ļ�����,�����������ֹ��");
	      finalmsgBox.setText(finalMsg);
	      finalmsgBox.exec();

        distFile.close();
        return false;
      }
	    //���䵱ǰ������ʼλ��
      ErrCount += Pro_fullLenData(dest, curPos, indexLen);
	  //�����¸�������ʼλ��
      curPos += Size; 
    }
    //��������ʱ��ֱ��������ֵ,���������Ͻ���λ��
    for(int Pos = ValidCount; Pos <= binFileCount; Pos++){
      ErrCount += Pro_fullLenData(dest, curPos, indexLen);
    }
    if(ErrCount){
	    QMessageBox finalmsgBox;
	    QString finalMsg =  tr("���� ") + QString::number(ErrCount)  + tr(" ������ֵ�����������ȱ�ﷶΧ�������������ֹ��");
	      finalmsgBox.setText(finalMsg);
	      finalmsgBox.exec();

        distFile.close();
        return false;
      }
    curPos = (binFileCount + 1) * indexLen + Base;//���ʱ���ڼ���ļ������������
  }
  else{//������ͷ
    curPos = 0;//���ʱ���ڼ���ļ������������
  }

  //=======================================���Ŀ������========================================
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
  QString fileName;
  if(BatNestDeep == 0) //��ǰ�ᶨ
    fileName =QFileDialog::getSaveFileName(0, tr("����ɹ����ɵ��ļ�..."),QDir::currentPath(),tr("Bin��ʽ(*.Bin)"));
  else fileName = BatNestOutFile[BatNestDeep - 1]; //������ָ��

  QFile::remove (fileName); //ǿ����ɾ��
  if(!distFile.copy(fileName)){
	QMessageBox finalmsgBox;
	QString finalMsg =  fileName + tr("\n Ŀ���ļ��������ش����쳣!");
	finalmsgBox.setText(finalMsg);
	finalmsgBox.exec();

    distFile.close();
    return false;
  }

  distFile.close();
  return true; //�������
 
}




