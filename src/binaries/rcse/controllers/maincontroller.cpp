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
#include "maincontroller.h"

MainController::MainController(QObject* parent)
    : QObject{parent}
    , m_imageCtrl{new ImageController}
    , m_characterCtrl{new CharacterController}
    , m_editCtrl{new EditorController(m_imageCtrl.get())}
    , m_generatorCtrl{new QmlGeneratorController()}
    , m_logCtrl{new LogController(true)}
{
    connect(m_generatorCtrl.get(), &QmlGeneratorController::reportLog, m_logCtrl.get(), &LogController::manageMessage);
    m_logCtrl->setCurrentModes(LogController::Gui);

    connect(m_characterCtrl.get(), &CharacterController::performCommand, this, &MainController::processCommand);
    connect(m_editCtrl.get(), &EditorController::performCommand, this, &MainController::processCommand);

    m_editCtrl->setCurrentTool(Canvas::MOVE);

    auto f= [this]() { setModified(true); };
    connect(m_editCtrl.get(), &EditorController::dataChanged, this, f);
    connect(m_characterCtrl.get(), &CharacterController::dataChanged, this, f);
    connect(m_generatorCtrl.get(), &QmlGeneratorController::dataChanged, this, f);
    connect(m_imageCtrl.get(), &ImageController::dataChanged, this, f);

    connect(m_editCtrl.get(), &EditorController::canvasBackgroundChanged, m_imageCtrl.get(),
            &ImageController::addBackgroundImage);

    connect(m_imageCtrl.get(), &ImageController::errorOccurs, this,
            [this](const QString& msg) { m_logCtrl->manageMessage(msg, LogController::Error); });

    connect(m_generatorCtrl.get(), &QmlGeneratorController::errors, this,
            &MainController::displayQmlError); //[this](const QList<QQmlError>& errors) {

    connect(m_generatorCtrl.get(), &QmlGeneratorController::sectionChanged, m_characterCtrl.get(),
            &CharacterController::setRootSection);

    connect(m_generatorCtrl->fieldModel(), &FieldModel::fieldAdded, m_editCtrl.get(), &EditorController::addFieldItem);

    m_characterCtrl->setRootSection(m_generatorCtrl->fieldModel()->getRootSection());

    connect(m_editCtrl.get(), &EditorController::pageCountChanged, this,
            [this]() { m_generatorCtrl->setLastPageId(static_cast<unsigned int>(m_editCtrl->pageCount() - 1)); });
}

CharacterController* MainController::characterCtrl() const
{
    return m_characterCtrl.get();
}
EditorController* MainController::editCtrl() const
{
    return m_editCtrl.get();
}
ImageController* MainController::imageCtrl() const
{
    return m_imageCtrl.get();
}
QmlGeneratorController* MainController::generatorCtrl() const
{
    return m_generatorCtrl.get();
}
LogController* MainController::logCtrl() const
{
    return m_logCtrl.get();
}

const QUndoStack& MainController::undoStack() const
{
    return m_undoStack;
}

void MainController::processCommand(QUndoCommand* cmd)
{
    m_undoStack.push(cmd);
}

bool MainController::modified() const
{
    return m_modified;
}

void MainController::setModified(bool newModified)
{
    if(m_modified == newModified)
        return;
    m_modified= newModified;
    emit modifiedChanged();
}

void MainController::cleanUpData(bool addPage)
{
    m_generatorCtrl->clearData();
    CSItem::resetCount();
    m_editCtrl->clearData();
    m_imageCtrl->clearData();
    m_characterCtrl->clear();
    m_undoStack.clear();
    setCurrentFile(QString());
    setModified(false);

    if(addPage)
        m_editCtrl->addPage();
}

void MainController::displayQmlError(const QList<QQmlError>& errors)
{
    for(const auto& er : errors)
    {
        LogController::LogLevel lvl= LogController::Info;
        switch(er.messageType())
        {
        case QtDebugMsg:
            lvl= LogController::Debug;
            break;
        case QtWarningMsg:
            lvl= LogController::Warning;
            break;
        case QtCriticalMsg:
            lvl= LogController::Error;
            break;
        case QtFatalMsg:
            lvl= LogController::Error;
            break;
        case QtInfoMsg:
            lvl= LogController::Info;
            break;
        }
        m_logCtrl->manageMessage(er.toString(), lvl);
    }
}

const QString& MainController::currentFile() const
{
    return m_currentFile;
}

void MainController::setCurrentFile(const QString& newCurrentFile)
{
    if(m_currentFile == newCurrentFile)
        return;
    m_currentFile= newCurrentFile;
    emit currentFileChanged();
}
