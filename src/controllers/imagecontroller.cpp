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

#include "imagemodel.h"
#include "rolisteamimageprovider.h"

ImageController::ImageController(QTableView* view, QObject* parent)
    : QObject(parent), m_model(new ImageModel()), m_view(view)
{
    m_view->setModel(m_model.get());
#ifndef Q_OS_OSX
    m_view->setAlternatingRowColors(true);
#endif

    connect(m_model.get(), &ImageModel::internalDataChanged, this, &ImageController::dataChanged);

    m_copyPath= new QAction(tr("Copy Path"), this);
    m_copyPath->setShortcut(QKeySequence("CTRL+c"));

    m_copyUrl= new QAction(tr("Copy Url"), this);
    m_copyUrl->setShortcut(QKeySequence("CTRL+u"));

    m_replaceImage= new QAction(tr("Replace Image"), this);
    m_removeImage= new QAction(tr("Remove Image"), this);
    m_reloadImageFromFile= new QAction(tr("Reload image from file"), this);

    connect(m_copyPath, &QAction::triggered, this, &ImageController::copyPath);
    connect(m_copyUrl, &QAction::triggered, this, &ImageController::copyPath);
    connect(m_replaceImage, &QAction::triggered, this, &ImageController::copyPath);
    connect(m_removeImage, &QAction::triggered, this, &ImageController::copyPath);
    connect(m_reloadImageFromFile, &QAction::triggered, this, &ImageController::copyPath);

    m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_view, &QTableView::customContextMenuRequested, this, &ImageController::contextualMenu);

    auto headerView= m_view->horizontalHeader();
    headerView->setStretchLastSection(true);

    connect(m_model.get(), &ImageModel::rowsInserted, this, [this]() {
        auto view= m_view->horizontalHeader();
        view->resizeSections(QHeaderView::ResizeToContents);
    });
}

void ImageController::clearData()
{
    m_model->clear();
    setUuid(QUuid::createUuid().toString(QUuid::WithoutBraces));
}

void ImageController::save(QJsonObject& obj) const
{
    QJsonArray images;
    m_model->save(images);
    obj["background"]= images;
}

void ImageController::load(const QJsonObject& obj) {}

QString ImageController::uuid() const
{
    return m_uuid;
}

ImageModel* ImageController::model() const
{
    return m_model.get();
}

void ImageController::addBackgroundImage(int idx, const QPixmap& pix, const QString& filename)
{
    if(!m_model->insertImage(pix, QStringLiteral("%2_background_%1.jpg").arg(idx).arg(m_uuid), filename, true))
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

void ImageController::contextualMenu(const QPoint& pos)
{
    QMenu menu;

    menu.addAction(m_copyPath);
    menu.addAction(m_copyUrl);
    menu.addSeparator();
    menu.addAction(m_replaceImage);
    menu.addAction(m_removeImage);
    menu.addAction(m_reloadImageFromFile);

    menu.exec(QCursor::pos());
}
void ImageController::copyPath()
{
    auto act= dynamic_cast<QAction*>(sender());
    if(nullptr == act)
        return;

    QModelIndex index= m_view->currentIndex();
    if(index.column() == act->data().toInt())
    {
        QString path= index.data().toString();
        QClipboard* clipboard= QGuiApplication::clipboard();
        clipboard->setText(path);
    }
}

void ImageController::copyUrl() {}

void ImageController::replaceImage()
{
    auto index= m_view->currentIndex();
    auto filepath= QFileDialog::getOpenFileName(m_view, tr("Load Image"), QDir::homePath(),
                                                tr("Supported Image Format (*.jpg *.png *.svg *.gif)"));
    if(filepath.isEmpty())
        return;
    m_model->setPathFor(index, filepath);
}

void ImageController::reloadImageFromFile() {}

void ImageController::removeImage(int idx)
{
    m_model->removeImage(idx);
}

QSize ImageController::backgroundSize() const
{
    return m_model->backgroundSize();
}

RolisteamImageProvider* ImageController::getNewProvider() const
{
    return new RolisteamImageProvider(m_model.get());
}
