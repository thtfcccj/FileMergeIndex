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
  unsigned long us = *data++;
  us |= (unsigned long)*data << 8;
  return us;
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


  //======================================ͼ����========================================
  QDataStream pic(picFile);  //ͼƬ��������������ƴ���
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




