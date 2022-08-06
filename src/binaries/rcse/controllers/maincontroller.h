/***************************************************************************
 *	Copyright (C) 2022 by Renaud Guezennec                               *
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
#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>
#include <QUndoStack>

#include "common/logcontroller.h"
#include "controllers/charactercontroller.h"
#include "controllers/editorcontroller.h"
#include "controllers/imagecontroller.h"
#include "controllers/qmlgeneratorcontroller.h"

class MainController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(CharacterController* characterCtrl READ characterCtrl CONSTANT)
    Q_PROPERTY(EditorController* editCtrl READ editCtrl CONSTANT)
    Q_PROPERTY(ImageController* imageCtrl READ imageCtrl CONSTANT)
    Q_PROPERTY(QmlGeneratorController* generatorCtrl READ generatorCtrl CONSTANT)
    Q_PROPERTY(LogController* logCtrl READ logCtrl CONSTANT)
    Q_PROPERTY(bool modified READ modified WRITE setModified NOTIFY modifiedChanged)
    Q_PROPERTY(QString currentFile READ currentFile WRITE setCurrentFile NOTIFY currentFileChanged)

public:
    explicit MainController(QObject* parent= nullptr);

    CharacterController* characterCtrl() const;
    EditorController* editCtrl() const;
    ImageController* imageCtrl() const;
    QmlGeneratorController* generatorCtrl() const;
    LogController* logCtrl() const;
    const QUndoStack& undoStack() const;
    bool modified() const;

    const QString& currentFile() const;
    void setCurrentFile(const QString& newCurrentFile);

public slots:
    void processCommand(QUndoCommand* cmd);
    void setModified(bool newModified);

    void cleanUpData(bool addPage= true);

    void displayQmlError(const QList<QQmlError>& errors);

signals:
    void modifiedChanged(bool modified);
    void currentFileChanged();

private:
    std::unique_ptr<ImageController> m_imageCtrl;
    std::unique_ptr<CharacterController> m_characterCtrl;
    std::unique_ptr<EditorController> m_editCtrl;
    std::unique_ptr<QmlGeneratorController> m_generatorCtrl;
    std::unique_ptr<LogController> m_logCtrl;

    QUndoStack m_undoStack;
    bool m_modified;
    QString m_currentFile;
};

#endif // MAINCONTROLLER_H
