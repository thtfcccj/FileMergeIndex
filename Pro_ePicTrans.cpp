/************************************************************************************************

                   ePicת��ģ��

************************************************************************************************/


#include "dialog.h"

//---------------------------------�ڲ�����----------------------------------------------
unsigned long Dialog::Lsb2Ul(const char *raw)
{
  const unsigned char *data = (const unsigned char *)raw;
  unsigned long ul = *data++;
  ul |= (unsigned long)*data << 8;
  data++;
  ul |= (unsigned long)*data << 16;
  data++;
  ul |= (unsigned long)*data << 24;
  return ul;
}
unsigned short Dialog::Lsb2Us(const char *raw)
{
  const unsigned char *data = (const unsigned char *)raw;
  unsigned short us = *data++;
  us |= (unsigned short)*data << 8;
  return us;
}

unsigned long Dialog::Msb2Ul(const char *raw)
{
  const unsigned char *data = (const unsigned char *)raw;
  unsigned long ul = (unsigned long)*data << 24;
  data++;
  ul |= (unsigned long)*data << 16;
  data++;
  ul |= (unsigned long)*data << 8;
  data++;
  ul |= (unsigned long)*data << 0;
  return ul;
}
unsigned short Dialog::Msb2Us(const char *raw)
{
  const unsigned char *data = (const unsigned char *)raw;
  unsigned short us = (unsigned short)*data << 8;
  data++;
  us |= (unsigned short)*data << 0;
  return us;
}

unsigned long Dialog::RGB24toUl(const char *raw)//RGB����ת��
{
  const unsigned char *data = (const unsigned char *)raw;
  unsigned long ul = (unsigned long)*data << 16;
  data++;
  ul |= (unsigned long)*data << 8;
  data++;
  ul |= (unsigned long)*data << 0;
  return ul;
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
  //��6��PNGʱ���ݿ�����
  PngDataMask = 0xFFEF1FDC; //Ĭ��ɾ��PNGͷ��ʶ��飬��ɫ�壬ʱ�䣬����ı�����Ϣ,�޽����顣
  Line = t.readLine();
  if(t.status() == QTextStream::Ok){
    Para = Line.split(';'); //;��Ϊע��
    unsigned long Mask = Para[0].toUInt(&OK, 16);
    if(OK == true) PngDataMask = Mask;
  }
  //��7��GIFʱ���ݿ�����
  GifDataMask = 0x800C; //Ĭ��0x800C,������һ֡���ͼ�����ݼ�����С���볤��λ
  Line = t.readLine();
  if(t.status() == QTextStream::Ok){
    Para = Line.split(';'); //;��Ϊע��
    unsigned long Mask = Para[0].toUInt(&OK, 16);
    if(OK == true) GifDataMask = Mask;
  }
  //��8��;//GIF�ȶ�ͼʱ��ͼ������λ��
  PicFramePos = 0; //Ĭ��0
  Line = t.readLine();
  if(t.status() == QTextStream::Ok){
    Para = Line.split(';'); //;��Ϊע��
    unsigned long Mask = Para[0].toUInt(&OK);
    if(OK == true) PicFramePos = Mask;
  }

 //======================================����֧�ֵ�ͼ���ļ�=======================================
  QFile *picFile = new QFile(directoryLabel->text());
  if(picFile->open(QIODevice::ReadOnly) == false){//�ļ���ʧ��
    QMessageBox finalmsgBox;
    finalmsgBox.setText(directoryLabel->text() + tr("\n ͼ���ļ�δ�ҵ�,��������ֹ��"));
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


  //======================================ͼ����========================================
  QDataStream pic(picFile);  //ͼƬ��������������ƴ���
  laterDelRaw = NULL; //��ʼ��
  QString Resume;
  if(PicType == "wbm")
    Resume = Wbmp2epic(pic,dest, picFile->size(), FunMask,HeaderMask);
  else if(PicType == "bmp")
    Resume = Bmp2epic(pic,dest, picFile->size(), FunMask,HeaderMask,toColorType);
  else if(PicType == "gif")
    Resume = Gif2epic(pic,dest, picFile->size(), FunMask,HeaderMask,toColorType);
  else if(PicType == "png")
    Resume = Png2epic(pic,dest, picFile->size(), FunMask,HeaderMask,toColorType);
  else Resume = QString( tr("ͼ�������쳣"));

  if(laterDelRaw != NULL) delete laterDelRaw;//�Ժ�ɾ��
  //��������
  if(!Resume.isEmpty()){
	    QMessageBox msgBox;
      QString note = tr("ͼ���ʽ��֧�֣�");
      note += Resume;
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
   QString fileName;
  if(BatNestDeep == 0) //��ǰ�ᶨ
    fileName = QFileDialog::getSaveFileName(0, tr("����ɹ����ɵ��ļ�..."),QDir::currentPath(),tr("Ƕ��ʽͼ���ʽ(*.ePic);;������ʽ (*.*)"));
  else fileName = BatNestOutFile[BatNestDeep - 1]; //������ָ��

  QFile::remove (fileName); //ǿ����ɾ��
  if(!distFile.copy(fileName)){
	  QMessageBox finalmsgBox;
	  QString finalMsg = fileName + tr("\n Ŀ���ļ��������ش����쳣!");
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




