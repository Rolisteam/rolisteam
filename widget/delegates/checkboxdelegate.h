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

#include <QStyledItemDelegate>
#include <memory>

class CenteredCheckBox;

namespace rwidgets
{
/**
 * @brief The CheckBoxDelegate class
 */
class CheckBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    /**
     * @brief CheckBoxDelegate
     * @param aRedCheckBox
     * @param parent
     */
    CheckBoxDelegate(bool aRedCheckBox= false, QObject* parent= nullptr);
    ~CheckBoxDelegate();
    /**
     * @brief createEditor
     * @param parent
     * @param option
     * @param index
     * @return
     */
    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    /**
     * @brief setEditorData
     * @param editor
     * @param index
     */
    virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;
    /**
     * @brief setModelData
     * @param editor
     * @param model
     * @param index
     */
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
    /**
     * @brief sizeHint
     * @param option
     * @param index
     * @return
     */
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
    /**
     * @brief paint
     * @param painter
     * @param option
     * @param index
     */
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

public slots:
    void commitEditor();

private:
    std::unique_ptr<CenteredCheckBox> m_editor;
};

} // namespace rwidgets
