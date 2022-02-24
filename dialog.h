/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the example classes of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef DIALOG_H
#define DIALOG_H

#include <QtGui>
#include <QDialog>

QT_BEGIN_NAMESPACE
class QCheckBox;
class QLabel;
class QErrorMessage;
QT_END_NAMESPACE


#define _BAT_NEST_DEEP    3  //������ʱ��Ƕ�����

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = 0);

private slots:
    void setOpenFileName();

	void setExistingDirectory();

    void setSaveFileName();

private:
	  QPushButton *directoryButton;
    QPushButton *openFileNameButton;
    QPushButton *saveFileNameButton;

    QLabel *openFileNameLabel;
	  QLabel *directoryLabel;
	  QLabel *noteLabel;
 
  unsigned char Fun;	
  bool IsDisFinal; //��ֹ��ʾ���

    //�����ļ�
    bool Dialog::Pro(bool isIdent); //�Ƿ�Ϊʶ��,����false��ʾ�쳣

    //ɫϵת��
    unsigned short toRGB565(unsigned long u32);//��׼ɫתRGB565,��������
    unsigned char Scolor256To6(unsigned char sColor);//����ɫת��Ϊ6ɫ
    unsigned char toRGBM666(unsigned long u32);//��׼ɫתRGBM666,��������

    //����
    //�����������ָ�����ַ�,����ʱ����1�����򷵻�0
     int Pro_fullLenData(QDataStream &dest,
                              quint32 Data,
                              int Len);

    //��Դ�ļ��ϲ�:
    bool Pro_ResourceMerge(QTextStream &t);  

    //�ַ���������ϲ�:
    bool Pro_StringCompile(QTextStream &t);      

    //bin�ļ��ϲ�����
    bool Pro_BinMerge(QTextStream &t);    
    bool Pro_BinSplit(QTextStream &t);  

    //�����ļ�����
    bool Pro_CfgCompile(QTextStream &t); 
    int Pro_CfgCompileData(QDataStream &dest, QString &Desc, QString &Data); //����һ����������


    //ͼ��ת����
    bool Pro_ePicTrans(QTextStream &t);  
   //����ת����
   unsigned long Lsb2Ul(const char *raw);
   unsigned short Lsb2Us(const char *raw);
   unsigned long Msb2Ul(const char *raw);
   unsigned short Msb2Us(const char *raw);
   unsigned long RGB24toUl(const char *raw);//RGB����ת��

  //ѹ������߶���ͼ������ͷ
   void PushWH(QDataStream &dest,
                    unsigned short wh,
                    unsigned char Mask);


   //wbmp��ʽתePic, ���ؿո��ת����ȷ������������ʶλ��
   QString  Wbmp2epic(QDataStream &pic,
                                      QDataStream &dest,
                                      qint64 picSize,
                                      unsigned char FunMask,
                                      unsigned char HeaderMask);
   //bmp��ʽתePic, ���ؿո��ת����ȷ������������ʶλ��
    QString  Bmp2epic(QDataStream &pic,
                                      QDataStream &dest,
                                      qint64 picSize,
                                      unsigned char FunMask,
                                      unsigned char HeaderMask,
                                      unsigned char toColorType);
   //Gif��ʽתePic, ���ؿո��ת����ȷ������������ʶλ��
    QString  Gif2epic(QDataStream &pic,
                                      QDataStream &dest,
                                      qint64 picSize,
                                      unsigned char FunMask,
                                      unsigned char HeaderMask,
                                      unsigned char toColorType);
   //Png��ʽתePic, ���ؿո��ת����ȷ������������ʶλ��
    QString  Png2epic(QDataStream &pic,
                                      QDataStream &dest,
                                      qint64 picSize,
                                      unsigned char FunMask,
                                      unsigned char HeaderMask,
                                      unsigned char toColorType);

    char *laterDelRaw;//ePicͼ�����й���ͼ�����ݴ���ʱ����ɾ����ָ��
    unsigned long PngDataMask; //PNGʱ���ݿ�����

    //������
    bool Pro_BatPro(QTextStream &t); 

    int BatNestDeep; //������Ƕ�����
    QList<QString> BatNestOutFile; //������ʱ������ļ�
    QString WorkDir;//���ļ������ļ�����Ŀ¼
};

#endif
