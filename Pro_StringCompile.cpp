/************************************************************************************************

                   ccj�ַ����ļ��ϲ��ű�����ģ��

************************************************************************************************/


#include "dialog.h"

bool  Dialog::Pro_StringCompile(QTextStream &t) //����true�������
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


  //�����еõ��ַ�������,��������ͷ����,����ָ�����ļ�������ֲ��ϲ����������ֲ�0x00000000
  Line = t.readLine();
  Para = Line.split(';'); //;��Ϊע��
  //�õ��ϲ��ļ�����
  int txtLineCount = Para[0].toInt();
  if((txtLineCount < 0) || (txtLineCount > 1000)){
    QMessageBox msgBox;
    msgBox.setText(tr("�ϲ��ļ������������󣬵ڶ���ӦΪ0~1000֮������֣����ԡ�;����β"));
    msgBox.exec();
    return false;
  }

  //������ָ�����ֽ�ʱ��Ϸ�ʽ
  Line = t.readLine();
  bool isMsb = true; //Ĭ�ϴ��
  if(!Line.isEmpty()){
    Para = Line.split(';'); //;��Ϊע��
    bool isLsb = Para[0].toInt(&OK);
    if(OK == true) isMsb = !isLsb;//�෴
  }

  //������ָ�����ֽ�ʱ��Ϸ�ʽ
  Line = t.readLine();
  int indexLen = 4; 
  if(!Line.isEmpty()){
    Para = Line.split(';'); //;��Ϊע��
    int len = Para[0].toInt(&OK);
    if((OK == true) && (len <= 4) && (len >= 1)) indexLen = len;
  }
  //��6�У� �Ƿ����ַ���ĩβ��������ַ���0ʱ��
  Line = t.readLine();
  bool haveEnd = false; //Ĭ����
  if(!Line.isEmpty()){
    Para = Line.split(';'); //;��Ϊע��
    bool end = Para[0].toInt(&OK);
    if(OK == true) haveEnd = end;
  }


  //=======================================��ȡ������õ�·��λ��========================================
  QFile *txtFile = new QFile(directoryLabel->text());
  if(txtFile->open(QIODevice::ReadOnly) == false){//�ļ���ʧ��
    QMessageBox finalmsgBox;
    finalmsgBox.setText(directoryLabel->text() + tr("\n δ�ҵ��������txt�ļ�,��������ֹ��"));
	  finalmsgBox.exec();

    delete txtFile;
    return false;
  };

  //============================================�õ���ʱĿ���ļ�===================================
  QTemporaryFile distFile;
  if(!distFile.open()) {//��ʱ�ļ�����ʧ��
	  QMessageBox finalmsgBox;
	  QString finalMsg = tr("�ļ������������ʱ�ļ�����ռ�ʧ�ܣ�����ϵͳ�ռ����!");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

    return false;
  }

  //=======================================���Ŀ���ļ�ǰ��������ͷ�������ļ�================================
  QDataStream dest(&distFile);  //���Ϊ��������������ƴ���
  if(isMsb) dest.setByteOrder(QDataStream::BigEndian);//��˸�λ��ǰ
  else dest.setByteOrder(QDataStream::LittleEndian);//С�˵�λ��ǰ

  //Ϊ��Ԥ�����к�������ʱ��ֱ�����0x00000000
  int ErrCount = 0;
  int realLintCount = 0;
  unsigned long curPos; //������ƫ����
  if(!txtLineCount){//�Զ�ʱ���Ȼ�ȡ���������
     QTextStream txtLen(txtFile);//�ı���
     while(!txtLen.atEnd()){txtLen.readLine(); realLintCount++; };
     txtFile->seek(0);//�ص���ʼ
     ErrCount += Pro_fullLenData(dest, realLintCount, indexLen);
     curPos = (realLintCount + 2) * indexLen;//��+β
  }
  else{
    realLintCount = txtLineCount;
    curPos = (realLintCount + 1) *indexLen;//β
  }

  int ValidCount = 0;
  curPos +=  Base; //���λ��
  QTextStream txt(txtFile);//�ı���
  do{
    if(txt.atEnd()) break; //������
    Line = txt.readLine();
    ValidCount++;
    if(ValidCount > realLintCount){
	    QMessageBox finalmsgBox;
	    QString finalMsg = tr(" ָ������С��ʵ���ļ������� �����������ֹ��");
	    finalmsgBox.setText(finalMsg);
	    finalmsgBox.exec();

      distFile.close();
      return false;
    }
    QByteArray ba =  Line.toLocal8Bit(); //�õ���ת����ʽʵ����֧�����ģ���֧��ת���ַ�
    unsigned long  Size = ba.size();
    if(haveEnd) Size++;//��������ַ�
    if(Size >= (0xffffffff - curPos)){
	    QMessageBox finalmsgBox;
	    QString finalMsg = tr(" �ϲ����ļ�����,�����������ֹ��");
	    finalmsgBox.setText(finalMsg);
	    finalmsgBox.exec();

      distFile.close();
      return false;
    }
	  //���䵱ǰ������ʼλ��
    ErrCount += Pro_fullLenData(dest, curPos, indexLen);
	//�����¸�������ʼλ��
    curPos += Size; 
  }while(1);
  //��������ʱ��ֱ��������ֵ,���������Ͻ���λ��
  if(txtLineCount){
    for(int Pos = ValidCount; Pos <= txtLineCount; Pos++){
      ErrCount += Pro_fullLenData(dest, curPos, indexLen);
    }
  }
  else  ErrCount += Pro_fullLenData(dest, curPos, indexLen);//�����Ͻ���λ��

  if(ErrCount){
	  QMessageBox finalmsgBox;
	  QString finalMsg =  tr("���� ") + QString::number(ErrCount)  + tr(" ������ֵ�����������ȱ�ﷶΧ�������������ֹ��");
	    finalmsgBox.setText(finalMsg);
	    finalmsgBox.exec();

      distFile.close();
      return false;
  }

  //=======================================���Ŀ������========================================
  txtFile->seek(0);//�ص���ʼ
  QTextStream txt2(txtFile);//�µ��ı���
  do{
    if(txt2.atEnd()) break; //������
    Line = txt2.readLine();
    QByteArray ba =  Line.toLocal8Bit(); //�õ���ת����ʽʵ����֧�����ģ���֧��ת���ַ�
    unsigned long  Size = ba.size();
	  //�ϲ���������
    for(unsigned long i = 0; i < Size; i++) {
      dest << (quint8)(ba.at(i));
    }
    if(haveEnd) dest << (quint8)'\0';//��������ַ�
  }while(1);

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




