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


#define _BAT_NEST_DEEP    3  //批处理时，嵌套深度

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
  bool IsDisFinal; //禁止提示完成

    //处理文件
    bool Dialog::Pro(bool isIdent); //是否为识别,返回false表示异常

    //色系转换
    unsigned short toRGB565(unsigned long u32);//标准色转RGB565,四舍五入
    unsigned char Scolor256To6(unsigned char sColor);//单种色转换为6色
    unsigned char toRGBM666(unsigned long u32);//标准色转RGBM666,四舍五入

    //其它
    //向数据流填充指长度字符,超限时返回1，否则返回0
     int Pro_fullLenData(QDataStream &dest,
                              quint32 Data,
                              int Len);

    //资源文件合并:
    bool Pro_ResourceMerge(QTextStream &t);  

    //字符串编译与合并:
    bool Pro_StringCompile(QTextStream &t);      

    //bin文件合并与拆分
    bool Pro_BinMerge(QTextStream &t);    
    bool Pro_BinSplit(QTextStream &t);  

    //配置文件编译
    bool Pro_CfgCompile(QTextStream &t); 
    int Pro_CfgCompileData(QDataStream &dest, QString &Desc, QString &Data); //编译一个变量数据


    //图像转换：
    bool Pro_ePicTrans(QTextStream &t);  
   //数据转换：
   unsigned long Lsb2Ul(const char *raw);
   unsigned short Lsb2Us(const char *raw);
   unsigned long Msb2Ul(const char *raw);
   unsigned short Msb2Us(const char *raw);
   unsigned long RGB24toUl(const char *raw);//RGB数组转换

  //压入宽度与高度入图像数据头
   void PushWH(QDataStream &dest,
                    unsigned short wh,
                    unsigned char Mask);


   //wbmp格式转ePic, 返回空格符转换正确，否则描述错识位置
   QString  Wbmp2epic(QDataStream &pic,
                                      QDataStream &dest,
                                      qint64 picSize,
                                      unsigned char FunMask,
                                      unsigned char HeaderMask);
   //bmp格式转ePic, 返回空格符转换正确，否则描述错识位置
    QString  Bmp2epic(QDataStream &pic,
                                      QDataStream &dest,
                                      qint64 picSize,
                                      unsigned char FunMask,
                                      unsigned char HeaderMask,
                                      unsigned char toColorType);
   //Gif格式转ePic, 返回空格符转换正确，否则描述错识位置
    QString  Gif2epic(QDataStream &pic,
                                      QDataStream &dest,
                                      qint64 picSize,
                                      unsigned char FunMask,
                                      unsigned char HeaderMask,
                                      unsigned char toColorType);
   //Png格式转ePic, 返回空格符转换正确，否则描述错识位置
    QString  Png2epic(QDataStream &pic,
                                      QDataStream &dest,
                                      qint64 picSize,
                                      unsigned char FunMask,
                                      unsigned char HeaderMask,
                                      unsigned char toColorType);

    char *laterDelRaw;//ePic图像处理中过中图像数据错误时，等删除的指针
    unsigned long PngDataMask; //PNG时数据块掩码

    //批处理
    bool Pro_BatPro(QTextStream &t); 

    int BatNestDeep; //批处理嵌套深度
    QList<QString> BatNestOutFile; //批处理时的输出文件
    QString WorkDir;//批文件所有文件的总目录
};

#endif
