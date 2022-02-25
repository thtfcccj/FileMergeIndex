/************************************************************************************************

                   ccj bin�ļ��߼�����ű�����ģ��

************************************************************************************************/


#include "dialog.h"

bool  Dialog::Pro_BinLogic(QTextStream &t) //����true�������
{
  //==========================================��2�д�A�ļ�=================================
	QString Line = t.readLine();
	QStringList Para = Line.split(';'); //;��Ϊע��
  QString curPara = Para[0].simplified(); //ȥ��ǰ��ո�
  QString Pos;
  if(curPara[1] != ':')//��ǰ����·��
    Pos = directoryLabel->text() + '\\' + curPara; //��ȡλ�ò���ϳɾ���Ŀ¼
  else //��··��
    Pos = curPara;
  //�����ļ�
  QFile *aFile = new QFile(Pos);
  if(aFile->open(QIODevice::ReadOnly) == false){//�ļ���ʧ��
	  QMessageBox finalmsgBox;
	  QString finalMsg = Pos + tr(" ���߼�A�ļ�δ�ҵ����ʧ��,�ϲ�����ֹ��");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

    delete aFile;
    return false;
  }

  //==========================================��3�д�B�ļ�=================================
	Line = t.readLine();
	Para = Line.split(';'); //;��Ϊע��
  curPara = Para[0].simplified(); //ȥ��ǰ��ո�
  if(curPara[1] != ':')//��ǰ����·��
    Pos = directoryLabel->text() + '\\' + curPara; //��ȡλ�ò���ϳɾ���Ŀ¼
  else //��··��
    Pos = curPara;
  //�����ļ�
  QFile *bFile = new QFile(Pos);
  if(bFile->open(QIODevice::ReadOnly) == false){//�ļ���ʧ��
	  QMessageBox finalmsgBox;
	  QString finalMsg = Pos + tr(" ���߼�B�ļ�δ�ҵ����ʧ��,�ϲ�����ֹ��");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

    aFile->close();
    delete aFile;
    delete bFile;
    return false;
  }
  //======================================��4�ж�ȡ�߼�����========================================
  //�ڶ���ָ���߼�����ID
  Line = t.readLine();
  Para = Line.split(';'); //;��Ϊע��
  bool OK;
  unsigned char type = Para[0].toInt(&OK,10);
  if(OK == false){
    QMessageBox msgBox;
    msgBox.setText(tr("�߼������޷�ʶ��ָ����Ч!"));
    msgBox.exec();
    return false;
  }

  //============================================�õ���ʱĿ���ļ�===================================
  QTemporaryFile cFile;
  if(!cFile.open()) {//��ʱ�ļ�����ʧ��
	  QMessageBox finalmsgBox;
	  QString finalMsg = tr("�ļ������������ʱ�ļ�����ռ�ʧ�ܣ�����ϵͳ�ռ����!");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

    aFile->close();
    delete aFile;
    bFile->close();
    delete bFile;
    return false;
  }

  //=======================================�߼�����========================================
  QDataStream A(aFile);  //���Ϊ��������������ƴ���
  QDataStream B(bFile);  //���Ϊ��������������ƴ���
  QDataStream C(&cFile);  //���Ϊ��������������ƴ���

  qint64 aSize = aFile->size();
  qint64 bSize = bFile->size();
  qint64 size = (aSize > bSize) ? bSize : aSize; //ȡСֵ

  quint8 a,b,c;
  for(; size > 0; size--){//���ļ��ϴ󣬿��Ż���ΪСѭ����
     A >> a; B >> b;//������ֵ
     switch(type){
       case 0: c = a ^ b; break;
       case 1: c = a | b; break;
       case 2: c = a & b; break;
       case 3: c = a + b; break;
       case 4: c = a + b; if(c < a){c = 0xff;} break;
       case 5: c = a - b; break;
       case 6: c = b - a; break;
       case 10: if(a != 0) c = b; else c = 0; break;
       case 11: if(a != 0) c = 0; else c = b; break;
       case 12: if(a != 0) c = b; else c = 0xff; break;
       case 13: if(a != 0) c = 0xff; else c = b; break;
       default: c = b; break;
     }
     C << c; //д���ļ�
  }//end for
  //C=B����
  if(bSize > aSize){
    qint64 curSize = bSize - aSize;
    char *raw = new char[curSize];
	  B.readRawData(raw, curSize);
    C.writeRawData(raw ,curSize);//�ϲ�
  }

  //======================================��ȷʱ��󱣴�����========================================
  cFile.flush();//����
  QString fileName;
  if(BatNestDeep == 0) //��ǰ�ᶨ
    fileName = QFileDialog::getSaveFileName(0, tr("����ɹ����ɵ��ļ�..."),QDir::currentPath(),tr("Bin��ʽ(*.Bin)"));
  else fileName = BatNestOutFile[BatNestDeep - 1]; //������ָ��

  QFile::remove (fileName); //ǿ����ɾ��
  if(!cFile.copy(fileName)){
	  QMessageBox finalmsgBox;
	  QString finalMsg = fileName + tr("\n Ŀ���ļ��������ش����쳣!");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();


    aFile->close();
    delete aFile;
    bFile->close();
    delete bFile;
    cFile.close();
    return false;
  }

  aFile->close();
  delete aFile;
  bFile->close();
  delete bFile;
  cFile.close();
  return true; //�������
}




