/************************************************************************************************

                   ePicת��-תpng��ģ��

************************************************************************************************/


#include "dialog.h"

//---------------------------------png��ʽתePic----------------------------------------------
//���ؿո��ת����ȷ������������ʶλ��
QString  Dialog::Png2epic(QDataStream &pic,
                                      QDataStream &dest,
                                      qint64 picSize,
                                      unsigned char FunMask,
                                      unsigned char HeaderMask,
                                      unsigned char toColorType)
{
  //PNG Signature��PNGǩ���飬����PNG���͵ı�ʶ�� + �������ļ�ͷ���ݿ�IHDR(header chunk��13���ֽ�)
  //���ݿ���4Byte����������n+4Byte����õ����ݿ��ʶ+������n��+4ByteCRCУ�����
  
  //��������ͼ���Է���ɨ����ҵ�ɫ��
  char *raw = new char[picSize]; 
  laterDelRaw = raw;
  pic.readRawData(raw, picSize);

  //ͼ���ʶ����λ�жϳ���ע����ˣ�
  if(/*(raw[0] != 0x89) || */(raw[1] != 'P') || (raw[2] != 'N') || (raw[3] != 'G') || (raw[4] !=  0x0d) || (raw[5] != 0x0a) || (raw[6] != 0x1a) || (raw[7] != 0x0a))
    return  QString(tr("ͼ���ʶ�����"));

  //==========================���ݿ�IHDR(header chunk��13���ֽ�)�����ֱ��ʡ�������ȡ�ɫ��ģʽ��ѹ������
  if((raw[12] != 'I') || (raw[13] != 'H') || (raw[14] != 'D') || (raw[15] !=  'R'))
    return  QString(tr("δ�ҵ�IHDR��ʶ"));
  unsigned long Data = Msb2Ul(&raw[8]);
  if(Data != 13) return  QString(tr("IHDR���������"));
  Data = Msb2Ul(&raw[16]);
  if(Data >= 16384) return  QString(tr("��֧�ֿ�16384���ص�ͼ��"));
  quint16 biWidth = Data;
  Data = Msb2Ul(&raw[20]);
  if(Data >= 16384) return  QString(tr("��֧�ָ߳�16384���ص�ͼ��"));
  quint16 biHeight = Data;
  quint8 colorDeep = raw[24];
  if((colorDeep != 1) && (colorDeep != 2) && (colorDeep != 4) && (colorDeep != 8) && (colorDeep != 16))
    return  QString(tr("��ɫ��������쳣"));
  quint8 colorType = raw[25];
  if(colorType > 6) return  QString(tr("��ɫ���������쳣"));
  quint8 Compression = raw[26]; //ѹ��������0ΪĬ��
  quint8 Filter = raw[27];//�˲���������0Ϊ5���˲�����Ĭ������Ӧ
  quint8 Scan = raw[28];//ɨ�跽����0Ϊ5���˲�����Ĭ������Ӧ

 //=======================ɨ���������ݣ��Ի�ȡ�����Ϣ
 //׼��MAP
  QMap<QString, int> mapHeader; //������ͷ�ҵ�����,b7:��PLTE��IDAT֮ǰ,b6��IDAT֮ǰ,b5:����ȥ�����ݿ飬 ��5bit��Ӧʹ������
  //�Ѵ���mapHeader[" PNG"] = 0xc8 |0;//PNGͷ(ePicĬ��ȥ��)
  //�Ѵ���mapHeader["IHDR"] = 0xc8 |1;//�ļ�ͷ���ݿ�(ePicĬ��ȥ��)
  mapHeader["cHRM"] = 0xe0 |2;//��ɫ�Ͱ�ɫ�����ݿ�	
  mapHeader["gAMA"] = 0xc0 |3;//ͼ������ݿ�
  mapHeader["sBIT"] = 0xc0 |4;//������Чλ���ݿ�
  mapHeader["PLTE"] = 0x60 |5;//��ɫ�����ݿ�(ePicĬ��ȥ��)
  mapHeader["bKGD"] = 0xc0 |6;//������ɫ���ݿ�
  mapHeader["hIST"] = 0xc0 |7;//ͼ��ֱ��ͼ���ݿ�
  mapHeader["tRNS"] = 0xc0 |8;//ͼ��͸�����ݿ�
  mapHeader["oFFs"] = 0x40 |9;//(ר�ù������ݿ�)
  mapHeader["pHYs"] = 0x40 |10;//�������سߴ����ݿ�
  mapHeader["sCAL"] = 0x40 |11;//(ר�ù������ݿ�)
  mapHeader["IDAT"] = 0x20 |12;//ͼ�����ݿ�
  mapHeader["tIME"] = 0x00 |13;//ͼ������޸�ʱ�����ݿ�(ePicĬ��ȥ��)
  mapHeader["tEXt"] = 0x00 |14;//�ı���Ϣ���ݿ�(ePicĬ��ȥ��)
  mapHeader["zTXt"] = 0x00 |15;//ѹ���ı����ݿ�(ePicĬ��ȥ��)
  mapHeader["fRAc"] = 0x00 |16;//(ר�ù������ݿ�)
  mapHeader["gIFg"] = 0x00 |17;//(ר�ù������ݿ�)
  mapHeader["gIFt"] = 0x00 |18;//(ר�ù������ݿ�)
  mapHeader["gIFx"] = 0x00 |19;//(ר�ù������ݿ�)
  mapHeader["IEND"] = 0x00 |20;//���һ�����ݿ�(ePicĬ��ȥ��)

  //��ʼ�������ͼ�����ݿ�λ�ø��������ұ�λ��
  qint64 RdPos = 33;//�ӳ�����ʼ
  unsigned long RdSize = 0; 
  unsigned char headerType = 0;
  unsigned long FulledMask = 0;
  qint64 pltePos = 0;
  qint64 idatPos = 0;
  int idatCount = 0;//����IDAT����
  do{
    RdSize = Msb2Ul(&raw[RdPos]) + 4 * 3; //������4����ʶ��4+ ��������+У����4
    QString Header(raw[RdPos + 4]); Header.append(raw[RdPos + 5]); Header.append(raw[RdPos + 6]); Header.append(raw[RdPos + 7]);
    headerType = mapHeader[Header] & 0x1f;
    unsigned long Mask = (unsigned long)1 << headerType;
    if(headerType > 0){//��ֵʱ
      if(headerType == 12){//ͼ�����ݿ�
        idatCount++; 
        idatPos = RdPos;
      }
      else{
        if(headerType == 5) pltePos = RdPos; //�е�ɫ����
        if(Mask & PngDataMask) FulledMask |= Mask;//�������ʱ
      }
    }
    RdPos += RdSize; //�´�λ��
    if(headerType == 20) break;//���һ�����ݿ��ˡ�
  }while(RdPos < picSize);
  if(headerType != 20) return  QString(tr("��Ч���ļ�������"));

  //��ȡ��־
  bool isOnlyData;
  if((FunMask & 0x20) && (FulledMask == 0) && (idatCount == 1))//���н�����1��IDATʱ�����ұ��Ϊ��IDAT����
    isOnlyData = true;
  else isOnlyData = false;

  //=======================��Ҫʱ���ҵ�ɫ��
  quint16 PaletteCount; //��ɫ����
  if((FunMask & 0x02) && (colorType == 3)){
    if(pltePos == 0) return  QString(tr("δ�ҵ���ɫ��"));
    PaletteCount = Msb2Ul(&raw[pltePos]) / 3;//RGB24
  }
  else PaletteCount = 0; //�޵�ɫ��

  //====================�������ͷ
  if(FunMask & 0x01){//��Ҫ����ͷʱ
    unsigned char vLenMask = FunMask & 0x80; //�ɱ䳤��λ
    if(HeaderMask & 0x01) {
      char Flag = 'n';//nǰ׺
      if(vLenMask) Flag |= 0x80; //ѹ������ͷ��־
      dest << (quint8)Flag;
    }
    if(HeaderMask & 0x02) dest << (quint8)(((colorType & 0x07) << 5) | colorDeep); //��ɫ����b5~7��ɫ����Ϣb4~0
    PushWH(dest, biWidth, (HeaderMask &0x0C) | vLenMask);//���
    PushWH(dest, biHeight, (HeaderMask &0x30) | vLenMask);//�߶�
    quint8 Data = (Scan & 0x03) << 5;//ɨ�跽��(b6~5)
    Data |= (Filter & 0x07) << 2;//�˲�������(b4~2)
    if(isOnlyData) Data |= 0x80; //������ʱ�����λb7
    if(HeaderMask & 0x40) dest << (quint8)(Data | Compression & 0x03); //ѹ������(b1~0), 
    if(HeaderMask & 0x80) dest << (quint8)PaletteCount;//256ʱΪ0
  }

   //=====================����ɫ��
  if(PaletteCount){
    unsigned short PaletteSize = PaletteCount * 3; ////��ɫ�����ֽ���
    pltePos += 8;//������ɫ��λ����
    if(!(FunMask & 0x04) || (toColorType <= 1)){//ԭ��ɫ���RGB24ʱ��ֱ�Ӷ�Ӧ
      dest.writeRawData(&raw[pltePos] ,PaletteSize);//����
    }
    else{//������Ҫһ����ת��
      for(; PaletteSize > 0; PaletteSize-= 3, pltePos+= 3){
        unsigned long ARGB = RGB24toUl(&raw[pltePos]);
        if(toColorType == 2) dest << (quint16)toRGB565(ARGB);
        else if(toColorType == 3)  dest << (quint8)toRGBM666(ARGB);
        else return  QString(tr("��֧�ֵ�Ŀ��ɫϵ"));
     }//end for
   }//end if
  }

  //======================�������================================
  if(!(FunMask & 0x08)) return QString(); //�����������
  if(PngDataMask & 0x01){//ǿ�����ͷ���˹��ܿ����ڶ�����ȡPNG��Ϣ
    dest.writeRawData(&raw[0] ,8);//����
  }
  if(PngDataMask & 0x02){//���IHDR���˹��ܿ����ڶ�����ȡPNG��Ϣ
    dest.writeRawData(&raw[8] ,13 + 12);//����
  }
  if(isOnlyData && (PngDataMask & (1 << 12))){//������ʱ����������
    dest.writeRawData(&raw[idatPos + 8] ,Msb2Ul(&raw[idatPos]));//����
    return QString();//��
  }

  RdPos = 33;//�ӳ�����ʼ�������Ҫ������
  do{
    RdSize = Msb2Ul(&raw[RdPos]) + 4 * 3; //������4����ʶ��4+ ��������+У����4
    QString Header(raw[RdPos + 4]); Header.append(raw[RdPos + 5]); Header.append(raw[RdPos + 6]); Header.append(raw[RdPos + 7]);
    headerType = mapHeader[Header] & 0x1f;
    unsigned long Mask = (unsigned long)1 << headerType;
    if((headerType > 0) && (Mask & PngDataMask)){//��ֵ������ʱ���
      if(FunMask & 0x40) dest.writeRawData(&raw[RdPos + 8] ,RdSize - 12);//ȥͷȥβ����
      else dest.writeRawData(&raw[RdPos] ,RdSize);//ȫ������
    }
    RdPos += RdSize; //�´�λ��
    if(headerType == 20) break;//���һ�����ݿ��ˡ�
  }while(RdPos < picSize);

  return QString();//��
}
