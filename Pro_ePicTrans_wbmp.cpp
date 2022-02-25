/************************************************************************************************

                   ePicת��-תwbmp��ģ��

************************************************************************************************/


#include "dialog.h"

//---------------------------------ѹ������߶���ͼ������ͷ----------------------------------------------
void Dialog::PushWH(QDataStream &dest,
                    unsigned short wh,
                    unsigned char Mask)

{
  if(Mask & 0x80){//�ɱ�����ͷʱ
    if(wh < 127) dest << (quint8)wh;      //1λ��ʾ
    else{//��λ��ʾ
      dest << (quint8)((wh & 0x7f) | 0x80); //��λ
      dest << (quint8)(wh >> 7);
    }
  }
  else{//�̶�����
    if(Mask == 0x0c) dest << (quint16)wh;   //˫�ֽڿ��
    else if(Mask) dest << (quint8)wh;      //0x04,0x08Ϊ���ֽڿ��
  }
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
  laterDelRaw = raw;
  pic.readRawData(raw, 6);
  if(raw[0] != 0) return  QString(tr("ͼ���ʶ0����"));
  else if(raw[1] != 0) return  QString(tr("ͼ���ʶ1����"));
  
  unsigned char DataPos = 3;
  quint16 w = raw[2];
  if(w & 0x80){//���λ��λ��ʾ������һλ
    if(raw[3] & 0x80) w = 0xffff;//������
    else{w = (w & 0x7f) << 7; w += raw[3]; DataPos = 4;}
  }
  quint16 h = raw[DataPos++];
  if(h & 0x80){//���λ��λ��ʾ������һλ
    if(raw[DataPos] & 0x80) h = 0xffff;//������
    else {h = (h & 0x7f) << 7; h += raw[DataPos++];}
  }
  if((w >= 16384) || (h >= 16384)) return  QString(tr("��֧�ֿ��߳�16384���ص�ͼ��"));

  //������ݳ����Ƿ���ȷ
  unsigned long size = w >> 3;
  if(w & 0x07) size++; //ÿ������8�ֽڶ���
  size *= h;
  if(picSize != (size + DataPos))  return  QString(tr("�ļ���С�쳣��������wbmp����!"));


  //�������ͷ
  if(FunMask & 0x01){//��Ҫ����ͷʱ
    unsigned char vLenMask = FunMask & 0x80; //�ɱ䳤��λ
    if(HeaderMask & 0x01) {
      char Flag = 'w';//wǰ׺
      if(vLenMask) Flag |= 0x80; //ѹ������ͷ��־
      dest << (quint8)Flag;
    }
    if(!vLenMask && (HeaderMask & 0x02)) dest << (quint8)1; //1bitɫ��
    PushWH(dest, w, (HeaderMask &0x0C) | vLenMask);//���
    PushWH(dest, h, (HeaderMask &0x30) | vLenMask);//�߶�
    if(!vLenMask){//�̶�����ʱ
      if(HeaderMask & 0x40) dest << (quint8)0;//ѹ����ʽ����ѹ��
      if(HeaderMask & 0x80) dest << (quint8)0;//�޵�ɫ��
    }
  }
  //0x02,0x04: �����ұ�(��)

  //�������
  if(FunMask & 0x08){//��Ҫ����ʱ
    for(; DataPos < 6; DataPos++) dest << (quint8)raw[DataPos];//����ͷ���²���
    picSize -= 6; //���ݳ�����
    char *rawData = new char[picSize];
    pic.readRawData(rawData, picSize);
    dest.writeRawData(rawData ,picSize);//�ϲ�
    delete rawData;
  }
  delete raw; laterDelRaw = NULL;
  return QString();//��
}
