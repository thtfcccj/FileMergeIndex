/************************************************************************************************

                   ePicת��-תwbmp��ģ��

************************************************************************************************/


#include "dialog.h"

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
    delete rawData;
  }
  delete raw; laterDelRaw = NULL;
  return QString();//��
}
