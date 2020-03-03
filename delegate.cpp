/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

/*
    delegate.cpp

    A delegate that allows the user to change integer values from the model
    using a spin box widget.
*/

#include "delegate.h"

ControlSchemeDelegate::ControlSchemeDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}



QWidget *ControlSchemeDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem & option ,
    const QModelIndex & index ) const
{
    /*根据列数，临时创造一个widget用来编辑数据
    if(index.column() == 1)
    {
        QTimeEdit *timeEdit = new QTimeEdit(parent);
        timeEdit->setDisplayFormat("mm:ss");
//        connect(timeEdit, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()));
        return timeEdit;
    }
    else if(index.column() == 2)
    {
        QSpinBox *editor = new QSpinBox(parent);
        editor->setFrame(false);
        editor->setMinimum(0);
        editor->setMaximum(100);
        return editor;
    }
    else if(index.column() == 3)
    {
        QComboBox *editor = new QComboBox(parent);
        editor->addItem(tr("关灯"));
        editor->addItem(tr("协调控制"));
        editor->addItem(tr("感应控制"));
        editor->addItem(tr("无电缆协调控制"));
        editor->addItem(tr("多时段定时控制"));
        editor->addItem(tr("黄闪控制"));
        return editor;
    }
    else*/

    return QItemDelegate::createEditor(parent,option,index);
}



void ControlSchemeDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    /*
    if(index.column() == 2)
    {
        int value = index.model()->data(index, Qt::EditRole).toInt();
        QTimeEdit * timeEdit = static_cast<QTimeEdit*>(editor);
        timeEdit->setTime(QTime(0, value / 60, value % 60));
    }
    else if(index.column() == 4)
    {
        int value = index.model()->data(index, Qt::EditRole).toInt();
        //data()应该是display role
        QComboBox * comboBox= static_cast<QComboBox*>(editor);
      //  spinBox->setValue(value);
    }
    else*/

    QItemDelegate::setEditorData(editor,index);
    return;
}



void ControlSchemeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    /*
    if(index.column() == 2)
    {
        QTimeEdit *timeEdit = static_cast<QTimeEdit*>(editor);
        QTime time = timeEdit->time();
        int secs = (time.minute() * 60) + time.second();
        model->setData(index, secs, Qt::EditRole);
    }
    */

    /*先写入model，再发送信号，submitall*/
    QItemDelegate::setModelData(editor,model,index);
    //emit ModelDataChanged();
    return;
}



void ControlSchemeDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

