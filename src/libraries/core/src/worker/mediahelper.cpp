#include "worker/mediahelper.h"

#include "media/mediatype.h"
#include <QUrl>

namespace MediaHelper
{
std::map<QString, QVariant> prepareWebView(const QString& urlText)
{
    QUrl url= QUrl::fromUserInput(urlText);
    std::map<QString, QVariant> params;
    if(url.isValid())
    {
        params.insert({Core::keys::KEY_TYPE, QVariant::fromValue(Core::ContentType::WEBVIEW)});
        params.insert({Core::keys::KEY_NAME, url.fileName()});
        params.insert({Core::keys::KEY_PATH, urlText});
    }
    return params;
}
} // namespace MediaHelper
