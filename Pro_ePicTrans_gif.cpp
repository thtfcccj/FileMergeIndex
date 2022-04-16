/************************************************************************************************

                   ePicת��-תgif��ģ��
��Ҫ�ο���https://blog.csdn.net/Swallow_he/article/details/76165202
************************************************************************************************/


#include "dialog.h"


//---------------------------------�õ�������ɫ��----------------------------------------------
//��������ɫ��ռ���ֽ�������ֵ��ɫ���쳣
int Dialog::FullGifPalette(QDataStream &pic,
                           QDataStream &dest,
                           unsigned char mcrspixel, //b8:����ɫ�壬b0~2��ɫ���С-1;
                           unsigned char EnFullDesc,//�Ƿ������Ŀ��
                           unsigned char FunMask,
                           unsigned char toColorType)
{
  if(!(mcrspixel & 0x80)) return 0;//�޵�ɫ��
  quint16 PaletteSize = (unsigned short)1 << (((mcrspixel >> 0) & 0x07) + 1); //��ȡ��ɫ�����
  //��ȡ��ɫ��
  char *color = NULL;
  PaletteSize *= 3;//RGB
  color = new char[PaletteSize];
  pic.readRawData(color, PaletteSize); //��ȡԭ���ݵ�ɫ��

  if(color == NULL){
    return -1;//�쳣
  }
  //�������
  if(EnFullDesc == 0){
    delete color;
    return PaletteSize;
  }
  //���������Ŀ��ʱ
  if((toColorType == 0) || (!(FunMask & 0x04))){//ʹ�����ݵ�ɫ��
    dest.writeRawData(color ,PaletteSize);
    delete color;
    return PaletteSize;
  }
  //��ת��ΪĿ���ɫ��ʱ
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
      else{
        delete color;
        return -2;//��֧�ֵ�Ŀ��ɫϵ
      }
    }
  }//end for
  
  delete color;
  return PaletteSize;
}


//---------------------------------�����ӿ鴦��----------------------------------------------
//���ؽ���λ��
static qint64  _DataSubBlocksPro(QDataStream &pic,
                                 QDataStream &dest,
                                 qint64 Pos,
                                 qint64 picSize,
                                 unsigned char EnMask)//0bѹ�����ݣ�2bͬʱѹ�볤����,3bѹ������� 
{
  char *block = new char[256];
  quint8 Len = 0;
  while(Pos < picSize){
    pic.readRawData((char*)&Len, 1); Pos += 1; //�����еĳ�����
    if(Len == 0) break;//������
    
    if((EnMask & 0x05) == 0x05){//����ͼ�������еĿ鳤��λʱ
      dest << (quint8)Len;
    }
    pic.readRawData(block, Len); Pos += Len; //�����е�����
    if((EnMask & 0x01) == 0x01){//��ͼ������ʱ
      dest.writeRawData(block ,Len);
    }
  };
  if((EnMask & 0x09) == 0x09){//����ͼ�������еĿ鳤��λʱ
    dest << (quint8)Len;
  }
  return Pos;
}

//------------------------------------------------git��ʽתePic-----------------------------------------------------
//���ؿո��ת����ȷ������������ʶλ��
QString  Dialog::Gif2epic(QDataStream &pic,
                                      QDataStream &dest,
                                      qint64 picSize,
                                      unsigned char FunMask,
                                      unsigned char HeaderMask,
                                      unsigned char toColorType)
{
  //================================����bmp�ļ�ͷ,һ��13�ֽ�===================================
  char *raw = new char[6+7];
  laterDelRaw = raw;
  pic.readRawData(raw, 6+7);
  //��6���̶��ֽ���ɣ�  ��6���̶��ֽ����
  if((raw[0] != 'G') || (raw[1] != 'I') || (raw[2] != 'F') || (raw[3] != '8') /*|| (raw[4] != '9')*/ || (raw[5] != 'a'))
    return  QString(tr("ͼ���ʶ�����, ��֧��GIF87a �� GIF89a ��ʽ"));


  //�߼���Ļ��ʶ��(Logical Screen Descriptor)��  ��7���ֽ����
  quint16 biWidth = Lsb2Us(&raw[6 + 0]);
  quint16 biHeight = Lsb2Us(&raw[6 + 2]);
  quint8 mcrspixel = Lsb2Us(&raw[6 + 4]);//��װ�� m(b7��ȫ�ֵ�ɫ��)cr(b6~b4,��ɫ���-1)s(b3) pixel(b3~0ȫ�ֵ�ɫ�����-1)��
  quint8 bgColor = Lsb2Us(&raw[6 + 5]);//������ɫ(��ȫ����ɫ�б��е����������û��ȫ����ɫ�б���ֵû������)

  if(GifDataMask & 0x4000){//��Gifͼ����ʱ
    dest.writeRawData(raw ,6+7); //�����Ŀ��
  }
  delete raw;
  laterDelRaw = NULL; //�ͷ�

  //ȫ����ɫ�б�ռ��С
  quint16 colorDeep =   (unsigned short)1 << (((mcrspixel >> 4) & 0x07) + 1);//ɫ��
  quint16 PaletteSize = (unsigned short)1 << (((mcrspixel >> 0) & 0x07) + 1); //��ɫ�����
  if(PaletteSize > colorDeep) return  QString(tr("mcrspixel�ڵ�ɫ�������쳣"));//����1��͸��ɫ


  //�������ͷ
  if(!(GifDataMask & 0x4000) && (FunMask & 0x01)){//��Ҫ����ͷʱ
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

  //====================================���������ȫ�ֲ�ɫ��================================
   // ȫ�ֲ�ɫ���־(mcrspixel & 0x80)G����˵���Ƿ���ȫ�ֲ�ɫ����ڡ�
  //���G��1����ʾ��һ��ȫ�ֲ�ɫ��(Global Color Table)������������߼���Ļ������(LogicalScreen Descriptor)֮��
  if(GifDataMask & 0x4000) toColorType = 0; //��Gifͼ����ʱ��ת��ȫ��ɫ

  qint64 Pos = FullGifPalette(pic, dest, mcrspixel, FunMask & 0x02, FunMask, toColorType);
  if(Pos < 0) return  QString(tr("ȫ�ֵ�ɫ�壺��֧�ֵ�Ŀ��ɫϵ"));
   
  //====================================�������������ݿ�================================
  Pos += (6 + 7); //�Ѷ�ȡλ��
  char *block = new char[16];
  laterDelRaw = block;
  bool isFinal = false;
  int FrameCount = 0;//ͼ��֡����
  int CurPicFramePos;
  if(GifDataMask & (1 << 15)) CurPicFramePos = PicFramePos;//ָ��λ��
  else CurPicFramePos = 0;//����Ϊ��һ֡ͼ��
  while(Pos < picSize){
    pic.readRawData(block, 1); Pos += 1;//�ȶ�ȡ��ͷ��־
    //+++++++++0x2c: ͼ��������ͷ(Image Descriptor),0x2c��ͷ����+++++++++++++++++++++++++++++
    //�����п��޵ľֲ���ɫ�����Ǵ��б��ͼ������, ��ͷռ10�ֽ�
    if((unsigned char)block[0] == 0x2c){
      bool EnFullPalette = true;
      pic.readRawData(&block[1], 9); Pos += 9;//��ȡ��
      if(GifDataMask & (1 << 0)){//ͼ��������ͷ�����ʱ
        if(!(GifDataMask & (1 << 15)) || (FrameCount == CurPicFramePos)){//���ж�֡���ʱ
          if((CurPicFramePos != 0) && (GifDataMask & (1 << 13))) {//�����0֡ʱ��ǿ��ѡȥ���ֲ���ɫ���ұ�
            unsigned char Org = block[9];
            block[9] &= ~0x87;//ȥ�ֲ���־�����ұ�λ
            EnFullPalette = false;//��ʱ��������
            dest.writeRawData(block ,10); //�����Ŀ��
            block[9] = Org;//�ָ��Թ�FullGifPaletteȥ��
          }
          else dest.writeRawData(block ,10); //�����Ŀ��
        }
      }
      //�����ܵľֲ����ұ�
      unsigned char Mask;
      if(EnFullPalette == false) Mask = 0;//ǿ�Ʋ�������ұ�
      else{
        Mask = GifDataMask & (1 << 1);
        if((GifDataMask & (1 << 15)) && (FrameCount != CurPicFramePos)) Mask = 0; //�����ж�֡���ʱ
      }
      int cCount = FullGifPalette(pic, dest, block[9], Mask, FunMask, toColorType);
      if(cCount < 0) return  QString(tr("�ֲ���ɫ�壺��֧�ֵ�Ŀ��ɫϵ"));
      Pos += cCount;
      //���ͼ�����ݴ���->���ͼ�������е�LZW��С���볤��λ
      pic.readRawData(block, 1); Pos += 1; //��LZW��С���볤��λ
      if((GifDataMask & 0x0c) == 0x0c){//�����ͼ�����ݼ����е�LZW��С���볤��λʱ���
        if(!(GifDataMask & (1 << 15)) || (FrameCount == CurPicFramePos)){//���ж�֡���ʱ
          dest << (quint8)block[0];
        }
      }
      //���ͼ�����ݴ���->���ݿ�
      Mask = GifDataMask >> 2;
      if((GifDataMask & (1 << 15)) && (FrameCount != CurPicFramePos)) Mask = 0; //�����ж�֡���ʱ
      Pos = _DataSubBlocksPro(pic,dest, Pos, picSize, Mask);//ѹ���
      FrameCount++;//��һ֡������
      continue; //����
    }//block[0] == 0x2c

    //+++++++++++++++++++++++++++++++++++0x21��ͷ����++++++++++++++++++++++++++++++++++++++++
    if((unsigned char)block[0] == 0x21){
      pic.readRawData(&block[1], 1); Pos++;//��ȡ�ڶ���־λ
      //ͼ�ο�����չ��,�������С1Byte+��־1Byte+��ʱʱ��2Byte+͸ʱ��ɫ����1Byte+0����1Byte����8Byte��� 
      if((unsigned char)block[1] == 0xf9){
        pic.readRawData(&block[2], 6); Pos += 6;//��ȡ��
        if(GifDataMask & (1 << 6)){//����ʱ
          dest.writeRawData(block ,8); //�����Ŀ��
        }
        continue; //����
      }
      //�޸�ʽ�ı���չ��: 0x21,0x01��ͷ���������С1Byte�̶�Ϊ13..." ��15�ֽ����,����޸�ʽ�ı�����
      unsigned char EnMask = 0;
      if((unsigned char)block[1] == 0x01){
        pic.readRawData(&block[2], 13); Pos += 13;//��ȡ��
        if(GifDataMask & (1 << 7)){//����ʱ
          dest.writeRawData(block ,15); //�����Ŀ��
          EnMask = 0xff;//ȫ��
        }
        //����޸�ʽ�ı�����
        Pos = _DataSubBlocksPro(pic,dest, Pos, picSize, EnMask);//ѹ���
        continue; //����
      }
      //ע����չ��:  0x21,0xfe��ͷ������ע�����ݿ�0...n" ���, ��0��β
      if((unsigned char)block[1] == 0xfe){
        if(GifDataMask & (1 << 8)){//����ʱ
          dest.writeRawData(block ,2); //ͷ�����Ŀ��
          EnMask = 0xff;//ȫ��
        }
        //ע����չ��
        Pos = _DataSubBlocksPro(pic,dest, Pos, picSize, EnMask);//ѹ���
        continue; //����
      }
      //Ӧ����չ��:  0x21,0xff��ͷ���������С1Byte�̶�Ϊ12..." ��14�ֽ����,���Ӧ������
      if((unsigned char)block[1] == 0xff){
        pic.readRawData(&block[2], 12); Pos += 12;//��ȡ��
        if(GifDataMask & (1 << 9)){//����ʱ
          dest.writeRawData(block ,14); //�����Ŀ��
          EnMask = 0xff;//ȫ��
        }
        //���Ӧ������
        Pos = _DataSubBlocksPro(pic,dest, Pos, picSize, EnMask);//ѹ���
        continue; //����
      }
      //�����ˣ�
      return  QString(tr("����λ��0x") + QString::number(Pos,16)  + tr("  :����ʶ���GIF 0x21���ӱ�ʶͷ!"));
    }
    //++++++++++++++++++++++++++++++++++����������++++++++++++++++++++++++++++++++++++++++
    //GIF�ļ������飺 0X3b��β
    if((unsigned char)block[0] == 0x3b){
      if(Pos != picSize){
        //�����ˣ�
        return  QString(tr("����λ��0x") + QString::number(Pos,16)  + tr("  :��GIF�ļ�ĩβ�������ļ�������!"));
      }
      isFinal = true;
      break; //��ȷʱ�˳�
    }
    //�����ˣ�
    return  QString(tr("����λ��0x") + QString::number(Pos,16)  + tr("  :����ʶ���GIF����ʶͷ!"));
  }//end while

  if(isFinal == false){
    return  QString("GIF�ļ�ĩβ����������0x3b, �쳣����!");
  }

  //��ȷ����ʱ���Ͻ�����
  if(GifDataMask & (1 << 10)){//����ʱ
    dest << (quint8)block[0];
  }
  //��ȷ������,��֡ͼ��ʱ��ʾ
  if(FrameCount > 1){
	    QMessageBox msgBox;
      QString note = tr("��GIF��ͼ������") + QString::number(FrameCount)  + tr("  ֡ͼ�����ݣ�");
      msgBox.setText(note);
	    msgBox.exec();
  }

  //��ȷ������
  return QString();//��
}

