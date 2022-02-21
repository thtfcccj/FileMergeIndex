/************************************************************************************************

                   ePicת��-תpng��ģ��

************************************************************************************************/


#include "dialog.h"

//---------------------------------git��ʽתePic----------------------------------------------

//���ؿո��ת����ȷ������������ʶλ��
QString  Dialog::Png2epic(QDataStream &pic,
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
