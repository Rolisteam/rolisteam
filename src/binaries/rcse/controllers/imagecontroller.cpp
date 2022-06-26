#include "imagecontroller.h"

#include <QAction>
#include <QClipboard>
#include <QFileDialog>
#include <QFileInfo>
#include <QGuiApplication>
#include <QHeaderView>
#include <QMenu>
#include <QTableView>
#include <QUuid>

#include "charactersheet/imagemodel.h"
#include "charactersheet/rolisteamimageprovider.h"

ImageController::ImageController(QObject* parent) : QObject(parent), m_model(new charactersheet::ImageModel())
{

    connect(m_model.get(), &charactersheet::ImageModel::internalDataChanged, this, &ImageController::dataChanged);
}

void ImageController::clearData()
{
    m_model->clear();
    setUuid(QUuid::createUuid().toString(QUuid::WithoutBraces));
}

QString ImageController::uuid() const
{
    return m_uuid;
}

charactersheet::ImageModel* ImageController::model() const
{
    return m_model.get();
}

void ImageController::addBackgroundImage(int idx, const QPixmap& pix, const QString& filename, const QString& uuid)
{
    auto id= uuid;
    if(id.isEmpty())
    {
        id= QStringLiteral("%2_background_%1.jpg").arg(idx).arg(m_uuid);
    }
    if(!m_model->insertImage(pix, id, filename, true))
    {
        emit errorOccurs(tr("Image %1 has not the same size than the others").arg(filename));
    }
}
void ImageController::addImage(const QPixmap& pix, const QString& filename)
{
    QFileInfo info(filename);
    m_model->insertImage(pix, info.fileName(), filename, false);
}

void ImageController::setUuid(const QString& uuid)
{
    if(uuid == m_uuid)
        return;
    m_uuid= uuid;
    emit uuidChanged();
}

void ImageController::copyPath(const QModelIndex& index)
{
    auto path= index.data(charactersheet::ImageModel::FilenameRole).toString();
    QClipboard* clipboard= QGuiApplication::clipboard();
    clipboard->setText(path);
}

void ImageController::copyUrl(const QModelIndex& index)
{
    QString path= index.data(charactersheet::ImageModel::UrlRole).toString();
    QClipboard* clipboard= QGuiApplication::clipboard();
    clipboard->setText(path);
}

void ImageController::replaceImage(const QModelIndex& index, const QString& filepath)
{
    m_model->setPathFor(index, filepath);
}

void ImageController::reloadImage(const QModelIndex& index)
{
    m_model->reloadImage(index);
}

void ImageController::removeImage(int idx)
{
    m_model->removeImage(idx);
}

QSize ImageController::backgroundSize() const
{
    return m_model->backgroundSize();
}

RolisteamImageProvider* ImageController::provider() const
{
    return new RolisteamImageProvider(m_model.get());
}
