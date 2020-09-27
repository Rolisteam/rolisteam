/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                                 *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef MINDMAPCONTROLLER_H
#define MINDMAPCONTROLLER_H

#include <QObject>
#include <QRectF>
#include <QUndoStack>
#include <memory>

class QAbstractItemModel;
class BoxModel;
class LinkModel;
class MindNode;
class SpacingController;
class SelectionController;
class NodeStyleModel;
class NodeStyle;

class MindMapController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* nodeModel READ nodeModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel* linkModel READ linkModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel* styleModel READ styleModel CONSTANT)
    Q_PROPERTY(int defaultStyleIndex READ defaultStyleIndex WRITE setDefaultStyleIndex NOTIFY defaultStyleIndexChanged)
    Q_PROPERTY(QString filename READ filename WRITE setFilename NOTIFY filenameChanged)
    Q_PROPERTY(bool spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
    Q_PROPERTY(SelectionController* selectionCtrl READ selectionController CONSTANT)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY canRedoChanged)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged)
    Q_PROPERTY(QString errorMsg READ errorMsg WRITE setErrorMsg NOTIFY errorMsgChanged)
    Q_PROPERTY(QRectF contentRect READ contentRect NOTIFY contentRectChanged)
public:
    explicit MindMapController(QObject* parent= nullptr);
    ~MindMapController();

    QAbstractItemModel* nodeModel() const;
    QAbstractItemModel* linkModel() const;
    QAbstractItemModel* styleModel() const;

    SelectionController* selectionController() const;
    const QString& filename() const;
    const QString& errorMsg() const;
    QRectF contentRect() const;

    bool spacing() const;
    bool canRedo() const;
    bool canUndo() const;
    int defaultStyleIndex() const;

signals:
    void filenameChanged();
    void spacingChanged();
    void canRedoChanged();
    void canUndoChanged();
    void errorMsgChanged();
    void defaultStyleIndexChanged();
    void contentRectChanged();

public slots:
    void saveFile();
    void loadFile();
    void setFilename(const QString& path);
    void resetData();
    void setSpacing(bool b);
    void redo();
    void undo();
    void setErrorMsg(const QString& msg);
    void importFile(const QString& path);
    void setDefaultStyleIndex(int indx);

    void addBox(const QString& idparent);
    void reparenting(MindNode* parent, const QString& id);
    void removeSelection();
    NodeStyle* getStyle(int index) const;

protected:
    void clearData();

private:
    QString m_filename;
    QString m_errorMsg;
    std::unique_ptr<SpacingController> m_spacingController;
    std::unique_ptr<SelectionController> m_selectionController;
    std::unique_ptr<LinkModel> m_linkModel;
    std::unique_ptr<BoxModel> m_nodeModel;
    std::unique_ptr<NodeStyleModel> m_styleModel;
    QThread* m_spacing= nullptr;
    QUndoStack m_stack;
};

#endif // MINDMAPCONTROLLER_H
