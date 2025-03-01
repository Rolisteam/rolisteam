#include "worker/campaignfinder.h"

#include "data/campaignmanager.h"

QPointer<campaign::CampaignManager> CampaignFinder::m_manager= QPointer<campaign::CampaignManager>(nullptr);

CampaignFinder::CampaignFinder() {}

bool CampaignFinder::pathIsInCampaign(const QString& path)
{
    if(!m_manager)
        return false;

    auto root= m_manager->campaignDir();

    return path.contains(root);
}

void CampaignFinder::setManager(campaign::CampaignManager* manager)
{
    m_manager= manager;
}

QString CampaignFinder::campaignRoot()
{
    return m_manager ? m_manager->campaignDir() : QString();
}

QString CampaignFinder::staticContentRoot()
{
    if(!m_manager)
        return {};
    return m_manager->placeDirectory(campaign::Campaign::Place::STATIC_ROOT);
}
