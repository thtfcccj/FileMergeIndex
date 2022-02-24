/************************************************************************************************

                   ccj ������ģ��

************************************************************************************************/


#include "dialog.h"

bool  Dialog::Pro_BatPro(QTextStream &t) //����true�������
{
  //ɨ��ǰ����Ƕ��
  if(BatNestDeep > _BAT_NEST_DEEP){
		QMessageBox msgBox;
		msgBox.setText( tr("������Ƕ����Ȳ�Ӧ����3��������ֹ��"));
		msgBox.exec();
		return false;
  }

  QString prvScpriptName(openFileNameLabel->text());
  QString prvDirName(directoryLabel->text());
  QString curUnitName;

  int UnitState = 0;//һ������Ԫ��״̬
  int LinePos = 1;//����ɨ�����λ��,��1��ʼ
  int curFunType = -1;//��ǰ��������
  do{
    if(t.atEnd()) break; //������
    LinePos++; //��1��ʼ
    QString Line = t.readLine();
    if(Line.isEmpty()) continue; //����������
    Line = Line.simplified(); //ȥ��ǰ��ո�
    if((Line[0] == '/') && ((Line[1] == '/'))){//ע�⴦��
      continue; //������һ��
    }

    QStringList linePara = Line.split(';'); //;��Ϊע��
    if(linePara[0].isEmpty()) continue; //��������Ч���ݣ�������һ��

    //�õ����ԡ�:��������׺��ַ�
    QStringList subPara = linePara[0].split(':'); //:��Ϊ����
    if(subPara.size() < 2){
	    QMessageBox msgBox;
      msgBox.setText(QString::number(LinePos)  + tr("�У�δ�ҵ��ԡ�:�����ţ�������ֹ"));
	    msgBox.exec();
      openFileNameLabel->setText(prvScpriptName);
      directoryLabel->setText(prvDirName);
      noteLabel->setText(tr("�ڵ� ") + QString::number(LinePos)  + tr("  �г��ֱ����������ֹ��"));
      return false;
    }
    QString para0 = subPara[0].simplified(); //ȥ��ǰ��ո�
    QString para1 = subPara[1].simplified(); //ȥ��ǰ��ո�
    //================================��Ԫͷ����======================================
    //��ʽʾ����>>cfgCompile������ȫ�������ļ�;
    if((para0[0] == '>') && ((para0[1] == '>'))){
      UnitState = 1; //��Ԫ״̬�б�������
      curUnitName = QString(para1);//curUnitName.clear(); curUnitName.append(para1);//�õ�����
      //�õ�����ID
      if(para0 == ">>resourceMerge")curFunType = 1;
      else if(para0 == ">>bineMerge")curFunType = 2;
      else if(para0 == ">>cfgCompile")curFunType = 3;
      else if(para0 == ">>ePicTrans")curFunType = 4;
      else if(para0 == ">>stringCompile")curFunType = 6;
      else if(para0 == ">>binSplit")curFunType = 7;
      else{
	      QMessageBox msgBox;
        msgBox.setText(QString::number(LinePos)  + tr("�У�����ʶ��ı��������ƣ�ע�����ִ�Сд��������ֹ"));
	      msgBox.exec();
        openFileNameLabel->setText(prvScpriptName);
        directoryLabel->setText(prvDirName);
        noteLabel->setText(tr("�ڵ� ") + QString::number(LinePos)  + tr("  �г��ֱ����������ֹ��"));
        return false;
      }
      continue; //������һ��
    }
    //================================�ű����ƴ���===================================
    //ָ�������滻�ű�����
    if(para0 == "scriptFile"){
      if(UnitState < 1){
	      QMessageBox msgBox;
        msgBox.setText(QString::number(LinePos)  + tr("�У��ű��ļ�ǰδָ����������������ֹ"));
	      msgBox.exec();
        openFileNameLabel->setText(prvScpriptName);
        directoryLabel->setText(prvDirName);
        noteLabel->setText(tr("�ڵ� ") + QString::number(LinePos)  + tr("  �г��ֱ����������ֹ��"));
        return false;
      }
      UnitState = 2; //��Ԫ״̬�б������ͽű��ļ�����
      if(para1[1] != ':')//��ǰ����·��
        openFileNameLabel->setText(WorkDir + para1);
      else //��··��
        openFileNameLabel->setText(para1);

      if(Pro(true) == false){//����Ԥ����,���°������������,�����ڲ��Ի�����ʾ��
        openFileNameLabel->setText(prvScpriptName);
        directoryLabel->setText(prvDirName);
        noteLabel->setText(tr("�ڵ� ") + QString::number(LinePos)  + tr("  �г��ֱ����������ֹ��"));
        return false;
      }
      if(Fun != curFunType){
	      QMessageBox msgBox;
        msgBox.setText(QString::number(LinePos)  + tr("�У��ű�������ʶ�����뵱ǰ��Ԫ����������Ӧ��������ֹ"));
	      msgBox.exec();
        openFileNameLabel->setText(prvScpriptName);
        directoryLabel->setText(prvDirName);
        noteLabel->setText(tr("�ڵ� ") + QString::number(LinePos)  + tr("  �г��ֱ����������ֹ��"));
        return false;
      }
      continue; //������һ��
    }
    //================================·�����ļ����ƴ���===================================
    //ָ�������滻�ű�����
    if((curFunType <= 2) && (para0 == "InDir") ||//�ϲ��ļ�ʱΪĿ¼
     (curFunType == 3) && (para0 == "csvFile") ||
     (curFunType == 4) && (para0 == "picFile") ||
     (curFunType == 6) && (para0 == "txtFile") ||
     (curFunType == 7) && (para0 == "splitFile"))
    {
      if(UnitState < 2){
	      QMessageBox msgBox;
        msgBox.setText(QString::number(LinePos)  + tr("�У�δָ�����������ű���������ֹ"));
	      msgBox.exec();
        openFileNameLabel->setText(prvScpriptName);
        directoryLabel->setText(prvDirName);
        noteLabel->setText(tr("�ڵ� ") + QString::number(LinePos)  + tr("  �г��ֱ����������ֹ��"));
        return false;
      }
      UnitState = 3; //��Ԫ״̬�б������ͽű��ļ������ļ���
      if((para1.size() >  1) || subPara.size() <= 2) //��ǰ����·��
        directoryLabel->setText(WorkDir + para1);
      else //��··������ΪA~F, �м�:���ֳ�������
        directoryLabel->setText(para1 + subPara[2]);
      continue; //������һ��
    }
    //================================���ִ�д���===================================
    //ָ�������滻�ű�����
    if(para0 == "outFile"){
      if(UnitState < 3){
	      QMessageBox msgBox;
        msgBox.setText(QString::number(LinePos)  + tr("�У����ǰδָ�����������ű���������ֹ"));
	      msgBox.exec();
        openFileNameLabel->setText(prvScpriptName);
        directoryLabel->setText(prvDirName);
        noteLabel->setText(tr("�ڵ� ") + QString::number(LinePos)  + tr("  �г��ֱ����������ֹ��"));
        return false;
      }

      QString OutFile;
      if((para1.size() >  1) || subPara.size() <= 2) //��ǰ����·��
        OutFile = WorkDir + para1;
      else //��··������ΪA~F, �м�:���ֳ�������
        OutFile = para1 + subPara[2];

      BatNestOutFile.insert(BatNestDeep - 1,OutFile);
      //������ʾ
      noteLabel->setText(tr("���ڱ���: ") + curUnitName + tr("  ��Ԫ,���:") + OutFile);

      if(Pro(false) == false){//��ʽ�����쳣�����˳�
        openFileNameLabel->setText(prvScpriptName);
        directoryLabel->setText(prvDirName);
        noteLabel->setText(tr("�ڵ� ") + QString::number(LinePos) + tr("  �г��ֱ����������ֹ��"));
        return false;
      }
      continue; //������һ��
    }

	  QMessageBox msgBox;
    msgBox.setText(QString::number(LinePos)  + tr("�У��ű��������󣬱�����ֹ"));
	  msgBox.exec();
    openFileNameLabel->setText(prvScpriptName);
    directoryLabel->setText(prvDirName);
    noteLabel->setText(tr("�ڵ� ") + QString::number(LinePos) + tr("  �г��ֱ����������ֹ��"));
    return false;
  }
  while(1);
   
  openFileNameLabel->setText(prvScpriptName);
  directoryLabel->setText(prvDirName);
  noteLabel->setText(tr("����ɹ���ɣ�"));
  return true; //�������
}




