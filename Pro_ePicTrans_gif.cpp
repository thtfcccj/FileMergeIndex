/************************************************************************************************

                   ePicת��-תgif��ģ��

************************************************************************************************/


#include "dialog.h"

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
  laterDelRaw = raw;
  pic.readRawData(raw, 6+7);
  if((raw[0] != 'G') || (raw[1] != 'I') || (raw[2] != 'F') || (raw[3] != '8') /*|| (raw[4] != '9')*/ || (raw[5] != 'a'))
    return  QString(tr("ͼ���ʶ�����, ��֧��GIF87a �� GIF89a ��ʽ"));


  //�����߼���Ļ��ʶ��(Logical Screen Descriptor) ��7���ֽ����
  quint16 biWidth = Lsb2Us(&raw[6 + 0]);
  quint16 biHeight = Lsb2Us(&raw[6 + 2]);
  quint8 mcrspixel = Lsb2Us(&raw[6 + 4]);//m(b7��ȫ�ֵ�ɫ��)cr(b6~b4,��ɫ���-1)s(b3) pixel(b3~0ȫ�ֵ�ɫ�����-1)��
  quint8 bgColor = Lsb2Us(&raw[6 + 5]);//������ɫ(��ȫ����ɫ�б��е����������û��ȫ����ɫ�б���ֵû������)

  //ȫ����ɫ�б�ռ��С
  quint16 colorDeep =   (unsigned short)1 << (((mcrspixel >> 4) & 0x07) + 1);//ɫ��
  quint16 PaletteSize = (unsigned short)1 << (((mcrspixel >> 0) & 0x07) + 1); //��ɫ�����
  if(PaletteSize > colorDeep) return  QString(tr("mcrspixel�ڵ�ɫ�������쳣"));//����1��͸��ɫ


  //�������ͷ
  if(FunMask & 0x01){//��Ҫ����ͷʱ
    unsigned char vLenMask = FunMask & 0x80; //�ɱ䳤��λ
    if(HeaderMask & 0x01) {
      char Flag = 'g';//ǰ׺
      if(vLenMask) Flag |= 0x80; //ѹ������ͷ��־
      dest << (quint8)Flag;
    }
    if(HeaderMask & 0x02) dest << (quint8)mcrspixel; //ɫ����ұ����Ϣ
    PushWH(dest, biWidth, (HeaderMask &0x0C) | vLenMask);//���
    PushWH(dest, biHeight, (HeaderMask &0x30) | vLenMask);//�߶�
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
           unsigned long ARGB = RGB24toUl(&color[pos]);
           if(toColorType == 2){ //RGB565
             dest << (quint16)toRGB565(ARGB);
           }
           else if(toColorType == 3){ //RGBM666
             dest << (quint8)toRGBM666(ARGB);
           }
           else{delete color; return  QString(tr("��֧�ֵ�Ŀ��ɫϵ"));};
         }
       }//end for
     }//endif
     else dest.writeRawData(color ,PaletteSize);//ʹ�����ݵ�ɫ��
     ;
   }//end if
   if(color != NULL) {delete color;}


  //������Ӧ����ͼ�ο�����չ(Graphic Control Extension)��0x21,0xf9,0x04��ͷ��0x04��ʾ��4�����ݣ�0��β����8����:
  //0x21: ��չ���־
  //0xf9��ǩ���ͣ� ͼ�ο�����չ��ǩ; ��0xfeע�Ϳ��ǩ; ��0x01ͼ���ı���չ��; ��0xffӦ�ó�����չ��
  //0x04: �˱�ǩ���ȣ��������
  //������ݶ�Ӧ4�� (��ͼʱΪ0x00 0x00 0x00 0x00)
  //0X00��β
 //->ͼ�ο�����չ�����LZWѹ����ʽͼƬ��
  //gif�ļ��Թ̶�ֵ0x3B����
  if((FunMask & 0x18) == 0x18){//��Ҫȥ��������ͼ�ο�����չʱ
    char *gce = new char[8];
    pic.readRawData(gce, 8);
    //����Ĭ��ȫ��ͼ�ο�����չʱ����ȥ��
    if((gce[0] != 0x21) ||  (gce[1] != 0xf9) ||  (gce[2] != 0x04) ||  (gce[3] != 0x00) ||  (gce[4] != 0x00) ||  (gce[5] != 0x00) ||  (gce[6] != 0x00) || (gce[7] != 0x00))
      dest.writeRawData(gce ,8);
    delete gce;
  }

  //�������
  if(FunMask & 0x08){//��Ҫ����ʱ
    PaletteSize += (6+7); //��ͷ��
    if(picSize <= PaletteSize)  return  QString(tr("ͼ�����ݴ�С���쳣"));
    picSize -= PaletteSize;
    char *rawData = new char[picSize];
    pic.readRawData(rawData, picSize);
    dest.writeRawData(rawData ,picSize);//�ϲ�
    delete rawData;
  }
  
  return QString();//��
}
