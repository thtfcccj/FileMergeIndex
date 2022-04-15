/************************************************************************************************

                   ePicת��-תbmp��ģ��

************************************************************************************************/


#include "dialog.h"

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
  laterDelRaw = raw;
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
  if(Data == 256) biCompression |= 0x80;//��ʶΪ256
  unsigned short biClrUsed = Data;
  delete raw; laterDelRaw = NULL;

   //�õ���ɫ��ռ��С
   unsigned char Mask = FunMask & 0x60;
   unsigned short PaletteSize = 0;//��ɫ���ұ��С
   if(biBitCount <= 8){//8λ�����в��ұ�
     PaletteSize = (unsigned short)1 << biBitCount;
     if(biClrUsed != 0){//0ʹ��ȫ��
       //if(biClrUsed > (PaletteSize + 1)) //ʵ��4λͼ�񱣴�Ϊ8λʱ��Ҳ��0xff 256ɫ��ɫ��,��ע���
       //  return  QString(tr("��ɫ�������쳣"));//����1��͸��ɫ
       //else 
         PaletteSize = biClrUsed;//ʹ�þֲ�
     }
     PaletteSize *= 4;//���ұ�ת��Ϊ�ֽ�
   }

  //������ݳ����Ƿ���ȷ
  if(picSize <= (PaletteSize + 54))  return  QString(tr("�ļ���С�쳣����ͼ��������"));

  //�������ͷ
  if(FunMask & 0x01){//��Ҫ����ͷʱ
    unsigned char vLenMask = FunMask & 0x80; //�ɱ䳤��λ
    if(HeaderMask & 0x01) {
      char Flag = 'b';//ǰ׺
      if(vLenMask) Flag |= 0x80; //ѹ������ͷ��־
      dest << (quint8)Flag;
    }
    if(HeaderMask & 0x02) dest << (quint8)biBitCount; //ɫ��
    PushWH(dest, biWidth, (HeaderMask &0x0C) | vLenMask);//���
    PushWH(dest, biHeight, (HeaderMask &0x30) | vLenMask);//�߶�
    if(HeaderMask & 0x40) dest << (quint8)biCompression;//ѹ����ʽ
    if(HeaderMask & 0x80) dest << (quint8)biClrUsed;//��ɫ�����
  }

   //��ʱ��������ȡ��ɫ������
   char *color = NULL;
   if(PaletteSize){//
     color = new char[PaletteSize];
     pic.readRawData(color, PaletteSize); //��ȡԭ���ݵ�ɫ��
     laterDelRaw = color;
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

   }//end if
   if(color != NULL) {delete color;   laterDelRaw = NULL;}

  //�������
  if(FunMask & 0x08){//��Ҫ����ʱ
    picSize -= (PaletteSize + 54);
    if(picSize != biSizeImages)  return  QString(tr("ͼ�����ݴ�С���쳣"));//��Ϊ4�ı���

    char *rawData = new char[picSize];
    pic.readRawData(rawData, picSize);
    dest.writeRawData(rawData ,picSize);//�ϲ�
    delete rawData;
  }
  
  return QString();//��
}