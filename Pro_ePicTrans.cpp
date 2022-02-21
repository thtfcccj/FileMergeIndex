/************************************************************************************************

                   ePicת��ģ��

************************************************************************************************/


#include "dialog.h"

//---------------------------------wbmp��ʽתePic----------------------------------------------
//���ؿո��ת����ȷ������������ʶλ��
const QString  Dialog::Wbmp2epicHeader(QDataStream &pic,
                                      QDataStream &dest,
                                      qint64 picSize,
                                      unsigned char FunMask,
                                      unsigned char HeaderMask)
{
  //���岢��������ͷ
  char *raw = new char[6];
  pic.readRawData(raw, 6);
  if(raw[0] != 0) return tr("ͼ���ʶ0����");
  else if(raw[1] != 0) return tr("ͼ���ʶ1����");
  
  unsigned char DataPos = 3;
  quint16 w = raw[2];
  if(w & 0x80){//���λ��λ��ʾ������һλ
    w <<= 7; w += raw[3]; DataPos = 4;
  }
  quint16 h = raw[DataPos++];
  if(h & 0x80){//���λ��λ��ʾ������һλ
    h <<= 7; h += raw[DataPos++];
  }
  if((w >= 16384) || (h >= 16384)) return tr("��֧�ֿ��߳�16384���ص�ͼ��");

  //������ݳ����Ƿ���ȷ
  qint64 bitSize = (unsigned long)w * h;
  qint64 size = bitSize >> 3; //�ֽڴ�С
  if(size & 0x07) size++; //�ֽڶ���
  if(picSize != (size + DataPos))  return tr("�ļ������쳣��������wbmp����!");


  //�������ͷ
  if(FunMask & 0x01){//��Ҫ����ͷʱ
    if(HeaderMask & 0x01) dest << (quint8)'w';//wǰ׺
    if(HeaderMask & 0x02) dest << (quint8)1; //1bitɫ��

    unsigned char Mask = HeaderMask & 0x0c;
    if(Mask == 0x0c) dest << (quint16)w;   //˫�ֽڿ��
    else if(Mask) dest << (quint8)w;      //0x04,0x08Ϊ���ֽڿ��

    Mask = HeaderMask & 0x30;
    if(Mask == 0x30) dest << (quint16)h;   //˫�ֽڸ߶�
    else if(Mask) dest << (quint8)h;       //0x10,0x10Ϊ���ֽڸ߶�
    if(HeaderMask & 0x40) dest << (quint8)0;//ѹ����ʽ����ѹ��
    if(HeaderMask & 0x80) dest << (quint8)0;//�޵�ɫ��
  }
  //�����ұ�(��)

  //�������
  if(FunMask & 0x04){//��Ҫ����ʱ
    for(; DataPos < 6; DataPos++) dest << (quint8)raw[DataPos];//����ͷ���²���
    delete raw;
    picSize -= 6; //���ݳ�����
    char *rawData = new char[picSize];
    pic.readRawData(rawData, picSize);
    dest.writeRawData(rawData ,picSize);//�ϲ�
  }
  
  return QString();//��
}

bool  Dialog::Pro_ePicTrans(QTextStream &t) //����true�������
{
  //=======================================��ȡ������Ϣ========================================
  //�ڶ���ָ�����ֽ�ʱ��Ϸ�ʽ
  QString Line = t.readLine();
  QStringList Para = Line.split(';'); //;��Ϊע��
  bool OK;
  bool isMsb = !Para[0].toInt(&OK);//�෴
  if(OK == false){
    QMessageBox msgBox;
    msgBox.setText(tr("���뷽ʽָ����Ч��ӦΪ0 �� 1"));
    msgBox.exec();
    return false;
  }
  //�����й�����Ч����
  Line = t.readLine();
  Para = Line.split(';'); //;��Ϊע��
  unsigned char FunMask = Para[0].toInt(&OK, 16);
  if(OK == false){
    QMessageBox msgBox;
    msgBox.setText(tr("������Ч����ָ����Ч��ӦΪ0xnn��ʽ��"));
    msgBox.exec();
    return false;
  }
  //����������ͷ��Ӧ�ֽ���Ч����λ
  Line = t.readLine();
  Para = Line.split(';'); //;��Ϊע��
  unsigned char HeaderMask = Para[0].toInt(&OK, 16);
  if(OK == false){
    QMessageBox msgBox;
    msgBox.setText(tr("����ͷ��Ӧ�ֽ���Ч����ָ����Ч��ӦΪ0xnn��ʽ��"));
    msgBox.exec();
    return false;
  }
  //������Ŀ���ɫ���ʽ
  Line = t.readLine();
  Para = Line.split(';'); //;��Ϊע��
  unsigned char toColorType = 0; //��ת��
  if((FunMask & 0x06) == 0x06){//֧��ת��ΪĿ���ɫ��ʱ
     if(Para[0] == "RGB24")  toColorType = 1;
     else if(Para[0] == "RGB565")  toColorType = 2;
     else if(Para[0] == "RGBM666")  toColorType = 3;
     else {
      QMessageBox msgBox;
      msgBox.setText(tr("��֧�ֵ�Ŀ���ɫ���ʽ��"));
      msgBox.exec();
      return false;
     }
  }

 //======================================����֧�ֵ�ͼ���ļ�=======================================
  QFile *picFile = new QFile(directoryLabel->text());
  if(picFile->open(QIODevice::ReadOnly) == false){//�ļ���ʧ��
    QMessageBox finalmsgBox;
    finalmsgBox.setText( tr(" δ�ҵ���ת����ͼ���ļ�,��������ֹ��"));
	  finalmsgBox.exec();

    delete picFile;
    return false;
  };

  //��ȡ�ļ���ʽ��׺
  Para = directoryLabel->text().split('.'); //
  QString PicType = Para[Para.count() - 1];//���һ����
  PicType.toLower();//תСд
  //===============================�õ���ʱĿ���ļ�=============================================
  QTemporaryFile distFile;
  if(!distFile.open()) {//��ʱ�ļ�����ʧ��
	  QMessageBox finalmsgBox;
	  QString finalMsg = tr("�ļ������������ʱ�ļ�����ռ�ʧ�ܣ�����ϵͳ�ռ����!");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

	  picFile->close();
    delete picFile;
    return false;
  }
  QDataStream dest(&distFile);  //���Ϊ��������������ƴ���
  if(isMsb) dest.setByteOrder(QDataStream::BigEndian);//��˸�λ��ǰ
  else dest.setByteOrder(QDataStream::LittleEndian);//С�˵�λ��ǰ


  //======================================wbmpͼ���ʽ����========================================
  QDataStream pic(picFile);  //ͼƬ��������������ƴ���
  const QString *resume;
  if(PicType == "wbm")
    resume = &Wbmp2epicHeader(pic,dest, picFile->size(), FunMask,HeaderMask);
  else resume = &QString( tr("ͼ�������쳣"));

  //��������
  if(!resume->isEmpty()){
	    QMessageBox msgBox;
      QString note = tr("ͼ���ʽ��֧�֣�");
      note += *resume;
      note +=  tr(",ת����ֹ!");
      msgBox.setText(note);
	    msgBox.exec();

      distFile.close();
	    picFile->close();
      delete picFile;
      return false;
    }

  //======================================��ȷʱ��󱣴�����========================================
  distFile.flush();//����
  QString fileName = QFileDialog::getSaveFileName(0, tr("����ɹ����ɵ��ļ�..."),QDir::currentPath(),tr("Ƕ��ʽͼ���ʽ(*.ePic);;������ʽ (*.*)"));
  QFile::remove (fileName); //ǿ����ɾ��
  if(!distFile.copy(fileName)){
	  QMessageBox finalmsgBox;
	  QString finalMsg = tr("δָ�������ļ�����ش����쳣!");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

    distFile.close();
    return false;
  }

  picFile->close();
  delete picFile;
  distFile.close();

  return true; //�������
}




