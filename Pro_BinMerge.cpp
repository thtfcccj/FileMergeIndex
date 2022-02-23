/************************************************************************************************

                   ccj bin�ļ��ϲ��ű�����ģ��

************************************************************************************************/


#include "dialog.h"

bool  Dialog::Pro_BinMerge(QTextStream &t) //����true�������
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

  //������ָ�������ļ��Ĵ�С��ʮ�����Ʊ�ʾ
  Line = t.readLine();
  Para = Line.split(';'); //;��Ϊע��
  unsigned long fileSize = Para[0].toLongLong(&OK,16);
  if(OK == false){
    QMessageBox msgBox;
    msgBox.setText(tr("�ļ���Сָ����Ч��Ӧ�ԡ�0xnnnnnnnn����ʽ���!"));
    msgBox.exec();
    return false;
  }

  //=======================================��ȡ��������λ����·��λ��========================================
  //��������Ϊ����ϲ��ļ����λ�ã� ����·��;��,��;���н�β,�Ȼ��·��λ��
  QVector<unsigned long> baseAry;  
  QStringList listPath;
  int ValidCount = 0;
  int binFileCount = 0;
  do{
	  Line = t.readLine();
	  Para = Line.split(';'); //;��Ϊע��
    if(Para[0].isEmpty()) break; //������

	  unsigned long base, prvBase = 0;
    QString Pos;
	  if(Para[0][0] == ' ') {
		  Pos = ' '; //�ո��ʾ�м�Ԥ��
      base = 0;
	  }
	  else{//��������
      QStringList subPara = Para[0].split(','); //,�ֿ�
		  base = subPara[0].toLongLong(&OK,16);//��ȡ��ַ
		  if(OK == false){
		   	QMessageBox msgBox;
			  msgBox.setText(subPara[0] + tr("��ַָ����Ч��Ӧ�ԡ�0xnnnnnnnn����ʽ���!"));
			  msgBox.exec();
			  return false;
		 }
		if(base < prvBase){
			QMessageBox msgBox;
			msgBox.setText(subPara[0] + tr("����һ����Ч��ַС���밴����˳������!"));
			msgBox.exec();
			return false;
		 }
     prvBase = base;//��������

		Pos = directoryLabel->text() + '\\' + subPara[1]; //��ȡλ�ò���ϳɾ���Ŀ¼
		ValidCount++;
	}
    baseAry.append(base);
    listPath << Pos;
    binFileCount++;
  }while(1);

  if(ValidCount < 1){
    QMessageBox msgBox;
    msgBox.setText(tr("��������δ�ҵ���Ҫ�ϲ��ļ�"));
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

  //=======================================���Ŀ������========================================
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
  QString fileName;
  if(BatNestDeep == 0) //��ǰ�ᶨ
    fileName = QFileDialog::getSaveFileName(0, tr("����ɹ����ɵ��ļ�..."),QDir::currentPath(),tr("Bin��ʽ(*.Bin)"));
  else fileName = BatNestOutFile[BatNestDeep - 1]; //������ָ��

  QFile::remove (fileName); //ǿ����ɾ��
  if(!distFile.copy(fileName)){
	  QMessageBox finalmsgBox;
	  QString finalMsg = fileName + tr("\n Ŀ���ļ��������ش����쳣!");
	  finalmsgBox.setText(finalMsg);
	  finalmsgBox.exec();

    distFile.close();
    return false;
  }

  distFile.close();
  return true; //�������
}




