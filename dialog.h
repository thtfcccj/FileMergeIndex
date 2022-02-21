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
    QLabel *openFileNameLabel;
	QLabel *directoryLabel;
 
  unsigned char Fun;	
  bool IsDisFinal; //禁止提示完成

    //处理文件
    void Dialog::Pro(bool isIdent); //是否为识别
    //色系转换

    unsigned short toRGB565(unsigned long u32);//标准色转RGB565,四舍五入
    unsigned char Scolor256To6(unsigned char sColor);//单种色转换为6色
    unsigned char toRGBM666(unsigned long u32);//标准色转RGBM666,四舍五入

	//处理程序:
    bool Pro_ResourceMerge(QTextStream &t);  //资源文件合并
    bool Pro_BinMerge(QTextStream &t);       //bin文件合并
    bool Pro_CfgCompile(QTextStream &t);     //配置文件编译


    //图像转换：
    bool Pro_ePicTrans(QTextStream &t);  
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

};

#endif
