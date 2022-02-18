/************************************************************************************************

                   ccj �����ļ�����ģ��

************************************************************************************************/


#include "dialog.h"

bool  Dialog::Pro_CfgCompile(QTextStream &t) //����true�������
{
  //=======================================��ȡ������Ϣ========================================
  //�ڶ���ָ��δ���������(�ֽ�Ϊ��λ)������ַ�(һ��Ϊ0xff)������ʮ�����Ʊ�ʾ
  QString Line = t.readLine();
  QStringList Para = Line.split(';'); //;��Ϊע��
  bool OK;
  unsigned char nullData = Para[0].toInt(&OK,16);
  if((OK == false) || (nullData > 255)){
    QMessageBox msgBox;
    msgBox.setText(tr("����ַ�ָ����Ч��ֵ��255��Ӧ�ԡ�0xnn����ʽ���!"));
    msgBox.exec();
    return false;
  }

  //������ָ�����ֽ�ʱ��Ϸ�ʽ
  Line = t.readLine();
  Para = Line.split(';'); //;��Ϊע��
  bool isMsb = Para[0].toInt(&OK);
  if(OK == false){
    QMessageBox msgBox;
    msgBox.setText(tr("���뷽ʽָ����Ч��ӦΪ0 �� 1!"));
    msgBox.exec();
    return false;
  }

  //============================================�õ���ʱĿ���ļ�===================================
  QTemporaryFile distFile;
  if(!distFile.open()) {//��ʱ�ļ�����ʧ��
	QMessageBox finalmsgBox;
	QString finalMsg = tr("�ļ������������ʱ�ļ�����ռ�ʧ�ܣ�����ϵͳ�ռ����!");
	finalmsgBox.setText(finalMsg);
	finalmsgBox.exec();

    return false;
  }

  /*/======================================����Ŀ������========================================
  QDataStream dest(&distFile);  //���Ϊ��������������ƴ���
  unsigned long curPos = 0;
  for(int Pos = 0; Pos < binFileCount; Pos++){
    //���ļ�����
	if(listPath[Pos][0] == ' '){
	  continue;
	}
    //��ǰλ������Ч������û�����λ�������ַ�
    unsigned long curBase = baseAry[Pos]; //���δ����ļ���ַ
	if(curPos < curBase){
		for( ;curPos < curBase; curPos++){
		  dest << (quint8)nullData;
		}
	 }
	else if(curBase < curPos){
	  QMessageBox finalmsgBox;
	  QString finalMsg = listPath[Pos] + tr(" ���¸���Ч�ļ�Ŀ��ռ��н���,\n���ܱ��ļ����󣬻������ļ�ĳ����ַ�������󣬺ϲ�����ֹ��");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

      distFile.close();
      return false;
	}
    //�����ļ�
    QFile *curFile = new QFile(listPath[Pos]);
    if(curFile->open(QIODevice::ReadOnly) == false){//�ļ���ʧ��
	  QMessageBox finalmsgBox;
	  QString finalMsg = listPath[Pos] + tr(" δ�ҵ����ʧ��,�ϲ�����ֹ��");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

      delete curFile;
      distFile.close();
      return false;
    };
	qint64 curSize = curFile->size();
    if(curSize > (qint64)(fileSize - curPos)){
	  QMessageBox finalmsgBox;
	  QString finalMsg = listPath[Pos] + tr(" �ϲ�����ļ�����ָ����С,ע����Ŀ���ļ���С�Ƿ�������ȷ���ϲ�����ֹ��");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

      curFile->close();
      delete curFile;
      distFile.close();
      return false;
    }
	//����������
    QDataStream source(curFile);  //��������
    char *raw = new char[curSize];
	source.readRawData(raw, curSize);
	//�ϲ���������
    dest.writeRawData(raw ,curSize);//�ϲ�
	//�����¸�������ʼλ��
    curPos += curSize; 
    delete raw;

    curFile->close();
    delete curFile;
  }

  //��������ָ���ļ���С
  for( ;curPos < fileSize; curPos++){
    dest << (quint8)nullData;
  }

  //======================================��ȷʱ��󱣴�����========================================
  distFile.flush();//����
  QString fileName = QFileDialog::getSaveFileName(0, tr("����ɹ����ɵ��ļ�..."),QDir::currentPath(),tr("Bin��ʽ(*.Bin)"));
  QFile::remove (fileName); //ǿ����ɾ��
  if(!distFile.copy(fileName)){
	QMessageBox finalmsgBox;
	QString finalMsg = tr("δָ�������ļ�����ش����쳣!");
	finalmsgBox.setText(finalMsg);
	finalmsgBox.exec();

    distFile.close();
    return false;
  }*/

  distFile.close();
  return true; //�������
}




