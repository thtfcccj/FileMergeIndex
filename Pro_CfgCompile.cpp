/************************************************************************************************

                   ccj �����ļ�����ģ��

************************************************************************************************/


#include "dialog.h"

bool  Dialog::Pro_CfgCompile(QTextStream &t) //����true�������
{
  //=======================================��ȡ������Ϣ========================================
  //�ڶ���ָ��δ���������(�ֽ�Ϊ��λ)������ַ�(һ��Ϊ0xff)������ʮ�����Ʊ�ʾ
  QString Line = t.readLine();
  QStringList Para = Line.split(';'); //;��Ϊע��
  bool OK;
  unsigned char nullData = Para[0].toInt(&OK,16);
  if((OK == false) || (nullData > 255)){
    QMessageBox msgBox;
    msgBox.setText(tr("����ַ�ָ����Ч��ֵ��255��Ӧ�ԡ�0xnn����ʽ���!"));
    msgBox.exec();
    return false;
  }

  //������ָ�����ֽ�ʱ��Ϸ�ʽ
  Line = t.readLine();
  Para = Line.split(';'); //;��Ϊע��
  bool isMsb = Para[0].toInt(&OK);
  if(OK == false){
    QMessageBox msgBox;
    msgBox.setText(tr("���뷽ʽָ����Ч��ӦΪ0 �� 1!"));
    msgBox.exec();
    return false;
  }

 //======================================����csv�ļ�============================================
  QFile *csvFile = new QFile(directoryLabel->text());
  if(csvFile->open(QIODevice::ReadOnly) == false){//�ļ���ʧ��
    QMessageBox finalmsgBox;
    finalmsgBox.setText( tr(" δ��csvĿ���ļ�,��������ֹ��"));
	  finalmsgBox.exec();

    delete csvFile;
    return false;
  };


  //============================================�õ���ʱĿ���ļ�===================================
  QTemporaryFile distFile;
  if(!distFile.open()) {//��ʱ�ļ�����ʧ��
	  QMessageBox finalmsgBox;
	  QString finalMsg = tr("�ļ������������ʱ�ļ�����ռ�ʧ�ܣ�����ϵͳ�ռ����!");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

	  csvFile->close();
    delete csvFile;
    return false;
  }
  QDataStream dest(&distFile);  //���Ϊ��������������ƴ���
  if(isMsb) dest.setByteOrder(QDataStream::BigEndian);//��˸�λ��ǰ
  else dest.setByteOrder(QDataStream::LittleEndian);//С�˵�λ��ǰ

  //======================================����Ŀ������׼�������������б���========================================
  QTextStream csvStream(csvFile);  //Դ����
  Line = csvStream.readLine();//1���û����⣬��
  Line = csvStream.readLine();//2���û�ע�⣬��
  Line = csvStream.readLine();//3�б���ͷ����,

  int lineCount = 3; //�ӵ�4�п�ʼΪ����������
  int curPos = 0;//Ŀ������λ�ü���
  int ValidLine = 0;//��Ч�м���
  do{
    lineCount++;
    Line = csvStream.readLine();
    if(Line.isEmpty()) break; //���б�ʾ������

    Para = Line.split(','); //csv��ʽ�ָ��

	  if(Para.count() < 4){//�ļ���ʽ������
	    QMessageBox finalmsgBox;
	    QString finalMsg = QString::number(lineCount)  + tr("�У���ʽ����,������ֹ!"); 
	    finalmsgBox.setText(finalMsg);
	    finalmsgBox.exec();

      distFile.close();
	    csvFile->close();
      delete csvFile;
      return false;	  }

    //����ΪĿ��λ�ã���ַΪ��ʱֱ������
	  if(Para[0][0] == ' '){
	    continue;
	  }
    int curBase = Para[0].toInt(&OK,16);//ʮ�����ƻ�ַ
    if((OK == false) || (curBase < 0)){
      QMessageBox msgBox;
      msgBox.setText(QString::number(lineCount)  + tr("�У����λ�ø�ʽ�����ݴ���,������ֹ!"));
      msgBox.exec();

      distFile.close();
	    csvFile->close();
      delete csvFile;
      return false;
    }

    //��ǰλ������Ч������û�����λ������
	  if(Para[0][0] == ' '){
	    continue;
	  }

    //��ǰλ������Ч������û�����λ�������ַ�
	  if(curPos < curBase){
		  for( ;curPos < curBase; curPos++){
		    dest << (quint8)nullData;
		  }
	  }
	  else if(curBase < curPos){
	    QMessageBox msgBox;
      msgBox.setText(QString::number(lineCount)  + tr("�У����λ�ñ�ǰһ����С, ������ֹ!"));
	    msgBox.exec();

      distFile.close();
	    csvFile->close();
      delete csvFile;
      return false;
	  }
    //======================================�������ڶ�,���б���========================================
    //�����ʽԤ��
    bool bs64,bu64, bh64;
    qint64 s64 =  Para[2].toInt(&bs64,10);
    quint64 u64 =  Para[2].toInt(&bu64,10);
    quint64 h64 =  Para[2].toInt(&bh64,16);
    int Len = 0; ///����ʶ������ͱ�ʶ,����ʾ����
    //�������ͱ�ʶ��鲢��������
    if(Para[1] == "U8"){
      if(bu64 == false) Len = -1; //����ֵ������
      else if((u64 < 0) || (u64 > 0xff)) Len = -2;//����ֵ����
      else{ dest << (quint8)u64; Len = 1; }
    }
    else if(Para[1] == "U16"){
      if(bu64 == false) Len = -1; //����ֵ������
      else if((u64 < 0) || (u64 > 0xffff)) Len = -2;//����ֵ����
      else{ dest << (quint16)u64; Len = 2; }
    }
    else if(Para[1] == "U32"){
      if(bu64 == false) Len = -1; //����ֵ������
      else if((u64 < 0) || (u64 > 0xffffffff)) Len = -2;//����ֵ����
      else{ dest << (quint32)u64; Len = 4; }
    }
    else if(Para[1] == "U64"){
      if(bu64 == false) Len = -1; //����ֵ������
      else{ dest << (quint64)u64; Len = 8; }
    }
    else if(Para[1] == "S8"){
      if(bs64 == false) Len = -1; //����ֵ������
      else if((s64 < -128) || (s64 > 127)) Len = -2;//����ֵ����
      else{ dest << (qint8)u64; Len = 1; }
    }
    else if(Para[1] == "S16"){
      if(bs64 == false) Len = -1; //����ֵ������
      else if((s64 < -32768) || (s64 > 32767)) Len = -2;//����ֵ����
      else{ dest << (qint16)u64; Len = 2; }
    }
    else if(Para[1] == "S32"){
      if(bs64 == false) Len = -1; //����ֵ������
      else if((s64 < ((qint64)-21474836480 / 10)) || (s64 > (qint64)2147483647)) Len = -2;//����ֵ����
      else{ dest << (qint32)u64; Len = 4; }
    }
    else if(Para[1] == "S64"){
      if(bs64 == false) Len = -1; //����ֵ������
      else{ dest << (qint64)u64; Len = 8; }
    }
    else if(Para[1] == "HEX8"){
      if(bh64 == false) Len = -1; //����ֵ������
      else if((h64 < 0) || (h64 > 0xff)) Len = -2;//����ֵ����
      else{ dest << (quint8)h64; Len = 1; }
    }
    else if(Para[1] == "HEX16"){
      if(bh64 == false) Len = -1; //����ֵ������
      else if((h64 < 0) || (h64 > 0xffff)) Len = -2;//����ֵ����
      else{ dest << (quint16)h64; Len = 2; }
    }
    else if(Para[1] == "HEX32"){
      if(bh64 == false) Len = -1; //����ֵ������
      else if((h64 < 0) || (h64 > 0xffffffff)) Len = -2;//����ֵ����
      else{ dest << (quint32)h64; Len = 4; }
    }
    else if(Para[1] == "HEX64"){
      if(bh64 == false) Len = -1; //����ֵ������
      else{ dest << (quint64)h64; Len = 8; }
    }
    else if(Para[1] == "RGB"){
      if(Para[2][0] != '#') Len = -1; //����ֵ������
      else{
        bool bu32;
        QString rgb = Para[2].right(Para[2].size() - 1);
        quint32 u32 =  rgb.toInt(&bu32,16);
        if(bu32 == false) Len = -1; //����ֵ������
        else{
          dest << (quint8)0;  dest << ((quint8)(u32 >> 16) & 0xff);
          dest << ((quint8)(u32 >> 8) & 0xff);
          dest << ((quint8)u32 & 0xff);  Len = 4;
        }
      }
    }
    else if(Para[1] == "RGB24"){
      if(Para[2][0] != '#') Len = -1; //����ֵ������
      else{
        bool bu32;
        QString rgb = Para[2].right(Para[2].size() - 1);
        quint32 u32 =  rgb.toInt(&bu32,16);
        if(bu32 == false) Len = -1; //����ֵ������
        else{
          dest << ((quint8)(u32 >> 16) & 0xff);
          dest << ((quint8)(u32 >> 8) & 0xff);
          dest << ((quint8)u32 & 0xff);  Len = 3;
        }
      }
    }
    else if(Para[1] == "ARGB"){
      if(Para[2][0] != '#') Len = -1; //����ֵ������
      else{
        bool bu32;
        QString rgb = Para[2].right(Para[2].size() - 1);
        quint32 u32 =  rgb.toInt(&bu32,16);
        if(bu32 == false) Len = -1; //����ֵ������
        else{
          dest << ((quint8)(u32 >> 24) & 0xff);
          dest << ((quint8)(u32 >> 16) & 0xff);
          dest << ((quint8)(u32 >> 8) & 0xff);
          dest << ((quint8)u32 & 0xff);  Len = 4;
        }
      }
    }
    else if(Para[1] == "RGB2RGB565"){
      if(Para[2][0] != '#') Len = -1; //����ֵ������
      else{
        bool bu32;
        QString rgb = Para[2].right(Para[2].size() - 1);
        quint32 u32 =  rgb.toInt(&bu32,16);
        if(bu32 == false) Len = -1; //����ֵ������
        else{
          quint8 u8 = (u32 >> (16 + 3)) & 0x1f;//R����3λ
          if(u32 & (1 << (16 + 2))) u8++;//��ɫ��λ��������
          quint32 u16 = u8 << 11;
          u8 = (u32 >> (8 + 2)) & 0x1f;//G����3λ
          if(u32 & (1 << (8 + 1))) u8++;//��ɫ��λ��������
          u16 |= u8 << 5;
          u8 = (u32 >> (0 + 3)) & 0x1f;//B����3λ
          if(u32 & (1 << (0 + 2))) u8++;//��ɫ��λ��������
          u16 |= u8 << 0;
          dest << ((quint8)(u16 >> 8) & 0xff);
          dest << ((quint8)u16 & 0xff);  Len = 2;
        }
      }
    }


    //��������
    if(Len <= 0){
	    QMessageBox msgBox;
      QString note = QString::number(lineCount)  + tr("�У�");
      if(Len == 0) note +=  tr("���ͱ�ʶ���ܱ�ʶ��,ע��ȫ��Ϊ��д");
      if(Len == -1) note +=  tr("����ֵ������");
      if(Len == -2) note +=  tr("����ֵ����");
      note +=  tr(",������ֹ!");
      msgBox.setText(note);
	    msgBox.exec();

      distFile.close();
	    csvFile->close();
      delete csvFile;
      return false;
    }
    //��ȷ����
    curPos += Len;//Ŀ������λ�ü���
    ValidLine++;//��Ч�м���
  }while(1); //end do

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


  csvFile->close();
  delete csvFile;
  distFile.close();
  return true; //�������
}




