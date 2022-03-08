/************************************************************************************************

                                  Png2RGBAת��ģ��
��Ҫ���ڲ����lodepng��Դ�������
    https://github.com/lvandeve/lodepng.git
************************************************************************************************/


#include "dialog.h"
#include "lodepng.h"


bool  Dialog::Pro_Png2RGBA(QTextStream &t) //����true�������
{
  (void)t;
  //======================================ͼ����========================================
  std::vector<unsigned char> image; //the raw pixels
  unsigned width, height;

  //std::string str = directoryLabel->text().toStdString();
  //onst char* filename = str.c_str();
  QByteArray ba = directoryLabel->text().toLocal8Bit();  // toLocal8Bit ֧������
  const char* filename = ba.data();

  //decode inflateHuffmanBlock �ǹؼ���ѹ���� ZIPѹ���㷨�ο�ͷ�����ߣ�yuanwufan�� ZIPѹ���㷨��ϸ��������ѹʵ������
  //https://www.toutiao.com/i6675160779332780557
  unsigned error = lodepng::decode(image, width, height, filename);

  //if there's an error, display it
  if(error){
	    QMessageBox msgBox;
      QString note = tr("PNGͼ��������\n ������");
      note += QString::number(error) + tr("��") + lodepng_error_text(error);
      note +=  tr("\nת����ֹ!");
      msgBox.setText(note);
	    msgBox.exec();

      return false;
  }

  //the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
  //======================================��ȷʱ��󱣴�����========================================
   QString fileName;
  if(BatNestDeep == 0) //��ǰ�ᶨ
    fileName = QFileDialog::getSaveFileName(0, tr("����ɹ����ɵ��ļ�..."),QDir::currentPath(),tr("Ƕ��ʽͼ���ʽ(*.ePic);;������ʽ (*.*)"));
  else fileName = BatNestOutFile[BatNestDeep - 1]; //������ָ��

  //��Ҫ������ļ�
  QFile *curFile = new QFile(fileName);
  if(curFile->open(QIODevice::WriteOnly | QIODevice::Truncate	) == false){//�ļ���ʧ��
    QMessageBox finalmsgBox;
    finalmsgBox.setText( tr("δ�ҵ����ʧ��,��������ֹ!"));
    finalmsgBox.exec();

    delete curFile;
    return false;
  };

  QDataStream dest(curFile);  //��������

  //�����ļ�(��ɽ�RGBA����ת��Ϊ������ʽ)
  qint64 count = image.size();
  for(qint64 pos = 0; pos < count; pos++){
    dest << (qint8)image[pos];
  }
  curFile->close();
  delete curFile;

  return true; //�������
}




