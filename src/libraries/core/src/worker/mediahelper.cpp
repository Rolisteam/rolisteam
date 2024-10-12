#include "worker/mediahelper.h"

#include "controller/view_controller/webpagecontroller.h"
#include "media/mediatype.h"
#include <QPointF>
#include <QUrl>

namespace MediaHelper
{
std::map<QString, QVariant> prepareWebView(const QString& urlText, bool localIsGM)
{
    QUrl url= QUrl::fromUserInput(urlText);
    std::map<QString, QVariant> params;
    if(url.isValid())
    {
        params.insert({Core::keys::KEY_TYPE, QVariant::fromValue(Core::ContentType::WEBVIEW)});
        params.insert({Core::keys::KEY_NAME, url.host()});
        params.insert({Core::keys::KEY_PATH, urlText});
        params.insert(
            {Core::keys::KEY_STATE, localIsGM ? WebpageController::localIsGM : WebpageController::LocalIsPlayer});
    }
    return params;
}

std::map<QString, QVariant> addImageIntoController(const QPointF& pos, const QString& path, const QByteArray& data)
{
    std::map<QString, QVariant> params;
    params.insert({Core::vmapkeys::KEY_POS, pos});
    params.insert({Core::vmapkeys::KEY_TOOL, Core::SelectableTool::IMAGE});
    if(!path.isEmpty())
        params.insert({Core::vmapkeys::KEY_PATH, path});

    if(!data.isEmpty())
        params.insert({Core::vmapkeys::KEY_DATA, data});
    // ctrl->insertItemAt(params);

    return params;
}
} // namespace MediaHelper
