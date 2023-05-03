/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
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
#include "controller/view_controller/notecontroller.h"

#include <QFile>
#include <QFileInfo>
#include <QStringDecoder>
#include <QTextDocument>

NoteController::NoteController(const QString& id, QObject* parent)
    : MediaControllerBase(id, Core::ContentType::NOTES, parent)
{

    connect(this, &NoteController::urlChanged, this, &NoteController::loadText);

    /*auto future= QtConcurrent::run(IOHelper::loadFile, m_path);
    auto watcher= new QFutureWatcher<QByteArray>();
    connect(watcher, &QFutureWatcher<QByteArray>::finished, this, [this, watcher]() {
        auto result= watcher->result();

        if(!result.isEmpty())
            setText(QString(result));

        watcher->deleteLater();
    });*/
}

QString NoteController::text() const
{
    return m_text;
}

void NoteController::setText(const QString& text)
{
    if(text == m_text)
        return;
    m_text= text;
    emit textChanged(m_text);
}

bool NoteController::isHtml() const
{
    return m_html;
}

void NoteController::loadText()
{
    QFileInfo fi(url().toLocalFile());
    const QString ext= fi.completeSuffix().toLower();
    if(!fi.exists())
    {
        return;
    }

    if(ext == "odt" || ext == "ott")
    {
        emit loadOdt(url().toLocalFile());
    }
    else
    {
        // TODO use helper/worker
        QFile file(url().toLocalFile());
        if(!file.open(QFile::ReadOnly))
            return;

        QByteArray data= file.readAll();
        auto encoding= QStringConverter::encodingForHtml(data);
        auto toHtml= QStringDecoder(encoding.value());
        QString str= toHtml.decode(data);

        setHtml(Qt::mightBeRichText(str));
        setText(isHtml() ? str : QString::fromLocal8Bit(QByteArrayView{data}));
    }
}

void NoteController::setHtml(bool b)
{
    if(m_html == b)
        return;
    m_html= b;
    emit htmlChanged();
}
