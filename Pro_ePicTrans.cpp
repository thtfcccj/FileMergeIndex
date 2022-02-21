/************************************************************************************************

                   ePicת��ģ��

************************************************************************************************/


#include "dialog.h"

//---------------------------------�ڲ�����----------------------------------------------
static unsigned long Lsb2Ul(const char *raw)
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
static unsigned short Lsb2Us(const char *raw)
{
  const unsigned char *data = (const unsigned char *)raw;
  unsigned long us = *data++;
  us |= (unsigned long)*data << 8;
  return us;
}

//---------------------------------wbmp��ʽתePic----------------------------------------------
//���ؿո��ת����ȷ������������ʶλ��
QString  Dialog::Wbmp2epic(QDataStream &pic,
                                      QDataStream &dest,
                                      qint64 picSize,
                                      unsigned char FunMask,
                                      unsigned char HeaderMask)
{
  //���岢��������ͷ
  char *raw = new char[6];
  pic.readRawData(raw, 6);
  if(raw[0] != 0) return  QString(tr("ͼ���ʶ0����"));
  else if(raw[1] != 0) return  QString(tr("ͼ���ʶ1����"));
  
  unsigned char DataPos = 3;
  quint16 w = raw[2];
  if(w & 0x80){//���λ��λ��ʾ������һλ
    w <<= 7; w += raw[3]; DataPos = 4;
  }
  quint16 h = raw[DataPos++];
  if(h & 0x80){//���λ��λ��ʾ������һλ
    h <<= 7; h += raw[DataPos++];
  }
  if((w >= 16384) || (h >= 16384)) return  QString(tr("��֧�ֿ��߳�16384���ص�ͼ��"));

  //������ݳ����Ƿ���ȷ
  qint64 bitSize = (unsigned long)w * h;
  qint64 size = bitSize >> 3; //�ֽڴ�С
  if(size & 0x07) size++; //�ֽڶ���
  if(picSize != (size + DataPos))  return  QString(tr("�ļ���С�쳣��������wbmp����!"));


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
  //0x02,0x04: �����ұ�(��)

  //�������
  if(FunMask & 0x08){//��Ҫ����ʱ
    for(; DataPos < 6; DataPos++) dest << (quint8)raw[DataPos];//����ͷ���²���
    picSize -= 6; //���ݳ�����
    char *rawData = new char[picSize];
    pic.readRawData(rawData, picSize);
    dest.writeRawData(rawData ,picSize);//�ϲ�
  }
  delete raw;
  return QString();//��
}

//---------------------------------bmp��ʽתePic----------------------------------------------

//���ؿո��ת����ȷ������������ʶλ��
QString  Dialog::Bmp2epic(QDataStream &pic,
                                      QDataStream &dest,
                                      qint64 picSize,
                                      unsigned char FunMask,
                                      unsigned char HeaderMask,
                                      unsigned char toColorType)
{
  //����bmp�ļ�ͷ,һ��14�ֽ�
  char *raw = new char[54];
  pic.readRawData(raw, 54);
  if((raw[0] != 'B') && (raw[0] != 'b')) return  QString(tr("ͼ���ʶ��0����"));
  if((raw[1] != 'M') && (raw[1] != 'm')) return  QString(tr("ͼ���ʶ��1����"));
  unsigned long size = Lsb2Ul(&raw[2]);
  if(size != picSize)   return  QString(tr("�ļ���С���쳣��������bmp����"));

  unsigned long bfOffBits = Lsb2Ul(&raw[10]); //��ͷ��λͼ����ƫ��
  if(bfOffBits < 54) return  QString(tr("bmp�ļ�ͷͼ����ʼ���쳣"));


  //����λͼ��Ϣͷ,һ��40�ֽ�
  if(Lsb2Ul(&raw[14 + 0]) != 40) return  QString(tr("λͼ��Ϣͷ���������"));
  unsigned long Data = Lsb2Ul(&raw[14 + 4]);
  if(Data >= 16384) return  QString(tr("��֧�ֿ�16384���ص�ͼ��"));
  quint16 biWidth = Data;
  Data = Lsb2Ul(&raw[14 + 8]);
  if(Data >= 16384) return  QString(tr("��֧�ָ߳�16384����,���������е�ͼ��"));
  quint16 biHeight = Data;
  if(Lsb2Us(&raw[14 + 12]) != 1) return  QString(tr("Ŀ���豸˵����ɫƽ������Ϊ1"));
  unsigned short biBitCount = Lsb2Us(&raw[14 + 14]); //������/������
  if((biBitCount != 1) && (biBitCount != 2) && (biBitCount != 4) && (biBitCount != 8) && (biBitCount != 16) && (biBitCount != 24) && (biBitCount !=32))
    return  QString(tr("��֧�ֵ���ɫ���"));

  Data = Lsb2Ul(&raw[14 + 16]);
  if(Data > 3) return  QString(tr("��֧�ֵ�ͼ��ѹ����ʽ")); //ֻ��3��
  quint8 biCompression = Data;

  unsigned long biSizeImages = Lsb2Ul(&raw[14 + 20]);//4�ֽڣ�˵��λͼ���ݵĴ�С������BI_RGB��ʽʱ����������Ϊ0��

  Data = Lsb2Ul(&raw[14 + 32]);//˵��λͼʹ�õĵ�ɫ���е���ɫ��������Ϊ0˵��ʹ�����У�
  if(Data > 257) return  QString(tr("��ɫ���������쳣"));//����1��͸��ɫ
  unsigned short biClrUsed = Data;
  delete raw;

   //�õ���ɫ��ռ��С
   unsigned char Mask = FunMask & 0x60;
   unsigned short PaletteSize = 0;//��ɫ���ұ��С
   if(biBitCount <= 8){//8λ�����в��ұ�
     PaletteSize = (unsigned short)1 << biBitCount;
     if(biClrUsed != 0){//0ʹ��ȫ��
       if(biClrUsed > (PaletteSize + 1)) return  QString(tr("��ɫ�������쳣"));//����1��͸��ɫ
       else PaletteSize = biClrUsed;//ʹ�þֲ�
     }
     PaletteSize *= 4;//���ұ�ת��Ϊ�ֽ�
   }

  //������ݳ����Ƿ���ȷ
  if(picSize <= (PaletteSize + 54))  return  QString(tr("�ļ���С�쳣����ͼ��������"));

  //�������ͷ
  if(FunMask & 0x01){//��Ҫ����ͷʱ
    if(HeaderMask & 0x01) dest << (quint8)'b';//bǰ׺
    if(HeaderMask & 0x02) dest << (quint8)biBitCount; //ɫ��

    unsigned char Mask = HeaderMask & 0x0c;
    if(Mask == 0x0c) dest << (quint16)biWidth;   //˫�ֽڿ��
    else if(Mask) dest << (quint8)biWidth;      //0x04,0x08Ϊ���ֽڿ��

    Mask = HeaderMask & 0x30;
    if(Mask == 0x30) dest << (quint16)biHeight;   //˫�ֽڸ߶�
    else if(Mask) dest << (quint8)biHeight;       //0x10,0x10Ϊ���ֽڸ߶�
    if(HeaderMask & 0x40) dest << (quint8)biCompression;//ѹ����ʽ
    if(HeaderMask & 0x80) dest << (quint8)biClrUsed;//��ɫ�����
  }

   //��ʱ��������ȡ��ɫ������
   char *color = NULL;
   if(PaletteSize){//
     color = new char[PaletteSize];
     pic.readRawData(color, PaletteSize); //��ȡԭ���ݵ�ɫ��
   }

   Mask = FunMask & 0x06;
   if((color != NULL) && Mask){//ʹ�õ�ɫ��ʱ
     //�õ���ɫ������
     if((toColorType != 0) && (Mask == 0x06)){//ת��ΪĿ���ɫ��
       for(unsigned short pos = 0; pos < PaletteSize; pos += 4){
         unsigned long ARGB = Lsb2Ul(&color[pos]);
         if(toColorType == 1){ //����RGB����
          dest << (quint8)((ARGB >> 16) & 0xff);
          dest << (quint8)((ARGB >> 8) & 0xff);
          dest << (quint8)(ARGB & 0xff);
         }
         else if(toColorType == 2){ //RGB565
           dest << (quint16)toRGB565(ARGB);
         }
         else if(toColorType == 3){ //RGBM666
           dest << (quint8)toRGBM666(ARGB);
         }
         else return  QString(tr("��֧�ֵ�Ŀ��ɫϵ"));
       }//end for
     }//endif
     else dest.writeRawData(color ,PaletteSize);//ʹ�����ݵ�ɫ��
     delete color;
   }//end if

  //�������
  if(FunMask & 0x08){//��Ҫ����ʱ
    picSize -= (PaletteSize + 54);
    if(picSize != biSizeImages)  return  QString(tr("ͼ�����ݴ�С���쳣"));//��Ϊ4�ı���
    char *rawData = new char[picSize];
    pic.readRawData(rawData, picSize);
    dest.writeRawData(rawData ,picSize);//�ϲ�
  }
  
  return QString();//��
}

//---------------------------------git��ʽתePic----------------------------------------------

//���ؿո��ת����ȷ������������ʶλ��
QString  Dialog::Gif2epic(QDataStream &pic,
                                      QDataStream &dest,
                                      qint64 picSize,
                                      unsigned char FunMask,
                                      unsigned char HeaderMask,
                                      unsigned char toColorType)
{
  //����bmp�ļ�ͷ,һ��13�ֽ�
  char *raw = new char[6+7];
  pic.readRawData(raw, 6+7);
  if((raw[0] != 'G') || (raw[1] != 'I') || (raw[2] != 'F') || (raw[3] != '8') /*|| (raw[4] != '9')*/ || (raw[5] != 'a'))
    return  QString(tr("ͼ���ʶ�����, ��֧��GIF87a �� GIF89a ��ʽ"));


  //�����߼���Ļ��ʶ��(Logical Screen Descriptor) ��7���ֽ����
  unsigned char biWidth = Lsb2Us(&raw[6 + 0]);
  unsigned char biHeight = Lsb2Us(&raw[6 + 2]);
  quint8 mcrspixel = Lsb2Us(&raw[6 + 4]);//m(b7��ȫ�ֵ�ɫ��)cr(b6~b4,��ɫ���-1)s(b3) pixel(b3~0ȫ�ֵ�ɫ�����-1)��
  quint8 bgColor = Lsb2Us(&raw[6 + 5]);//������ɫ(��ȫ����ɫ�б��е����������û��ȫ����ɫ�б���ֵû������)

  //ȫ����ɫ�б�ռ��С
  unsigned short colorDeep =   (unsigned short)1 << (((mcrspixel >> 4) & 0x07) + 1);//ɫ��
  unsigned short PaletteSize = (unsigned short)1 << (((mcrspixel >> 0) & 0x07) + 1); //��ɫ�����
  if(PaletteSize > colorDeep) return  QString(tr("mcrspixel�ڵ�ɫ�������쳣"));//����1��͸��ɫ


  //�������ͷ
  if(FunMask & 0x01){//��Ҫ����ͷʱ
    if(HeaderMask & 0x01) dest << (quint8)'g';//gǰ׺
    if(HeaderMask & 0x02) dest << (quint8)mcrspixel; //ɫ����ұ����Ϣ

    unsigned char Mask = HeaderMask & 0x0c;
    if(Mask == 0x0c) dest << (quint16)biWidth;   //˫�ֽڿ��
    else if(Mask) dest << (quint8)biWidth;      //0x04,0x08Ϊ���ֽڿ��

    Mask = HeaderMask & 0x30;
    if(Mask == 0x30) dest << (quint16)biHeight;   //˫�ֽڸ߶�
    else if(Mask) dest << (quint8)biHeight;       //0x10,0x10Ϊ���ֽڸ߶�
    if(HeaderMask & 0x40) dest << (quint8)bgColor;//ѹ������ɫ
    if(HeaderMask & 0x80) dest << (quint8)PaletteSize;//��ɫ�����(256ʱΪ0)
  }

   //��ʱ��������ȡ��ɫ������
   char *color = NULL;
   if((mcrspixel & 0x80)){//
     PaletteSize *= 3;//RGB
     color = new char[PaletteSize];
     pic.readRawData(color, PaletteSize); //��ȡԭ���ݵ�ɫ��
   }
   else PaletteSize = 0;//��ȫ�ֵ�ɫ��
  //����ɫ��
   unsigned char Mask = FunMask & 0x06;
   if((color != NULL) && Mask){//ʹ��ȫ�ֵ�ɫ��ʱ
     if((toColorType != 0) && (Mask == 0x06)){//ת��ΪĿ���ɫ��
       for(unsigned short pos = 0; pos < PaletteSize; pos += 3){
         if(toColorType == 1){ //����RGB����,ֱ�Ӷ�Ӧ
          dest << (quint8)color[pos + 0];
          dest << (quint8)color[pos + 1];
          dest << (quint8)color[pos + 2];
         }
         else{
           unsigned long ARGB = Lsb2Ul(&color[pos]);
           if(toColorType == 2){ //RGB565
             dest << (quint16)toRGB565(ARGB);
           }
           else if(toColorType == 3){ //RGBM666
             dest << (quint8)toRGBM666(ARGB);
           }
           else return  QString(tr("��֧�ֵ�Ŀ��ɫϵ"));
         }
       }//end for
     }//endif
     else dest.writeRawData(color ,PaletteSize);//ʹ�����ݵ�ɫ��
     delete color;
   }//end if


  //������Ӧ����ͼ�ο�����չ(Graphic Control Extension)��0x21,0xf0,0x04��ͷ��0x04��ʾ��4�����ݣ�0��β����8����:
  //0x21: ��չ���־
  //0xf0��ǩ���ͣ� ͼ�ο�����չ��ǩ; ��0xfeע�Ϳ��ǩ; ��0x01ͼ���ı���չ��; ��0xffӦ�ó�����չ��
  //0x04: �˱�ǩ���ȣ��������
  //������ݶ�Ӧ4�� (��ͼʱΪ0x00 0x00 0x00 0x00)
  //0��β
 //->ͼ�ο�����չ�����LZWѹ����ʽͼƬ��
  //gif�ļ��Թ̶�ֵ0x3B����

  //�������
  if(FunMask & 0x08){//��Ҫ����ʱ
    PaletteSize += (6+7); //��ͷ��
    if(picSize <= PaletteSize)  return  QString(tr("ͼ�����ݴ�С���쳣"));
    picSize -= PaletteSize;
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


  //======================================ͼ����========================================
  QDataStream pic(picFile);  //ͼƬ��������������ƴ���
  QString Resume;
  if(PicType == "wbm")
    Resume = Wbmp2epic(pic,dest, picFile->size(), FunMask,HeaderMask);
  else if(PicType == "bmp")
    Resume = Bmp2epic(pic,dest, picFile->size(), FunMask,HeaderMask,toColorType);
  else if(PicType == "gif")
    Resume = Gif2epic(pic,dest, picFile->size(), FunMask,HeaderMask,toColorType);
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




