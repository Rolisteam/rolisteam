/*************************************************************************
 *    Copyright (C) 2011 by Renaud Guezennec                             *
 *                                                                       *
 *      http://www.rolisteam.org/                                        *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
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

#ifndef TIPCHECKER_H
#define TIPCHECKER_H

#include <QString>
#include <QNetworkAccessManager>
/**
 * @brief The TipChecker class displays link to article on the rolisteam website.
 */
class TipChecker : public QObject
{
    Q_OBJECT
public:
    TipChecker(QObject* obj = nullptr);

    bool hasArticle();
    void startChecking();

    QString getArticleTitle();
    QString getArticleContent();
    QString getUrl();
    int getId() const;
    void setId(int value);

signals:
    void checkFinished();

private slots:
    void readJSon(QNetworkReply* p);

private:
    QString m_title;
    QString m_msg;
    QString m_url;
    int m_id;
    bool m_state = false;
    QNetworkAccessManager* m_manager = nullptr;
    bool m_noErrror;
};

#endif // UPDATECHECKER_H
