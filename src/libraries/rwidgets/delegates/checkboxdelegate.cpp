/*************************************************************************
 *     Copyright (C) 2021 by Renaud Guezennec                            *
 *                                                                       *
 *     https://rolisteam.org/                                         *
 *                                                                       *
 *   rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/
#include "checkboxdelegate.h"

#include <QPainter>

#include "customs/centeredcheckbox.h"

namespace rwidgets
{
CheckBoxDelegate::CheckBoxDelegate(bool aRedCheckBox, QObject* parent) : m_editor(new CenteredCheckBox)
{
    Q_UNUSED(aRedCheckBox)
    Q_UNUSED(parent)
    // m_editor= new CenteredCheckBox();
    // m_editor->setParent(parent);
    connect(m_editor.get(), &CenteredCheckBox::commitEditor, this, &CheckBoxDelegate::commitEditor);
}
CheckBoxDelegate::~CheckBoxDelegate()= default;
QWidget* CheckBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                                        const QModelIndex& index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    CenteredCheckBox* cb= new CenteredCheckBox(parent);
    return cb;
}
void CheckBoxDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    CenteredCheckBox* cb= qobject_cast<CenteredCheckBox*>(editor);
    bool checked= index.data().toBool();
    cb->setCheckedDelegate(!checked);
}
void CheckBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    CenteredCheckBox* cb= qobject_cast<CenteredCheckBox*>(editor);
    model->setData(index, cb->isCheckedDelegate());
    QStyledItemDelegate::setEditorData(editor, index);
}
QSize CheckBoxDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize a= QStyledItemDelegate::sizeHint(option, index);
    a.setHeight(30);
    a.setWidth(150);
    return a;
}

void CheckBoxDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    painter->save();

    if(!index.isValid())
    {
        return;
    }

    // QStyleOptionViewItemV4 opt = option;
    // QStyledItemDelegate::initStyleOption(&option, index);

    QVariant var= index.data();

    bool checked= var.toBool();
    // QVariant color= index.data(Qt::BackgroundRole);

    if(option.state & QStyle::State_Selected)
    {
        painter->fillRect(option.rect, option.palette.highlight());
    }

    m_editor->setCheckedDelegate(checked);

    m_editor->resize(option.rect.size());
    m_editor->setAutoFillBackground(false);
    painter->translate(option.rect.topLeft());
    m_editor->render(painter, QPoint(), QRegion(), QWidget::DrawChildren);

    painter->restore();
}
void CheckBoxDelegate::commitEditor()
{
    CenteredCheckBox* editor= qobject_cast<CenteredCheckBox*>(sender());
    //	std::cout<<"commitEditor "<<(editor==m_editor)<<"  "<<editor->isCheckedDelegate()<<std::endl;
    emit commitData(editor);
}
} // namespace rwidgets
