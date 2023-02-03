#include <QtTest/QtTest>

#include <QGraphicsObject>
#include <charactersheet/charactersheet.h>
#include <charactersheet/charactersheetitem.h>
#include <charactersheet/charactersheetmodel.h>
#include <charactersheet/imagemodel.h>
#include <charactersheet/rolisteamimageprovider.h>

#include <charactersheet/controllers/fieldcontroller.h>
#include <charactersheet/controllers/section.h>
#include <charactersheet/controllers/tablefield.h>
#include <charactersheet/csitem.h>

#include <charactersheet_widgets/sheetwidget.h>

#include "controller/item_controllers/characteritemcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"

#include "charactersheet/charactersheet.h"
#include "charactersheet/charactersheet_global.h"
#include "charactersheet/charactersheetbutton.h"
#include "charactersheet/charactersheetitem.h"
#include "charactersheet/charactersheetmodel.h"
#include "charactersheet/controllers/fieldcontroller.h"
#include "charactersheet/controllers/section.h"
#include "charactersheet/controllers/tablefield.h"
#include "charactersheet/csitem.h"
#include "charactersheet/imagemodel.h"
#include "charactersheet/rolisteamimageprovider.h"
#include "charactersheet_formula/formula_global.h"
#include "charactersheet_formula/formulamanager.h"
#include "charactersheet_formula/nodes/formulanode.h"
#include "charactersheet_formula/nodes/operator.h"
#include "charactersheet_formula/nodes/parenthesesfnode.h"
#include "charactersheet_formula/nodes/scalaroperatorfnode.h"
#include "charactersheet_formula/nodes/startnode.h"
#include "charactersheet_formula/nodes/valuefnode.h"
#include "charactersheet_formula/parsingtoolformula.h"
#include "charactersheet_widgets/charactersheet_widget_global.h"
#include "charactersheet_widgets/sheetwidget.h"
#include "common/common_types.h"
#include "common/logcontroller.h"
#include "common/remotelogcontroller.h"
#include "common/task/uploadlogtoserver.h"
#include "common_qml/theme.h"
#include "common_widgets/colorbutton.h"
#include "common_widgets/common_widgets_global.h"
#include "common_widgets/logpanel.h"
#include "controller/antagonistboardcontroller.h"
#include "controller/audiocontroller.h"
#include "controller/audioplayercontroller.h"
#include "controller/contentcontroller.h"
#include "controller/controllerinterface.h"
#include "controller/gamecontroller.h"
#include "controller/instantmessagingcontroller.h"
#include "controller/item_controllers/characteritemcontroller.h"
#include "controller/item_controllers/ellipsecontroller.h"
#include "controller/item_controllers/gridcontroller.h"
#include "controller/item_controllers/imageitemcontroller.h"
#include "controller/item_controllers/linecontroller.h"
#include "controller/item_controllers/pathcontroller.h"
#include "controller/item_controllers/rectcontroller.h"
#include "controller/item_controllers/sightcontroller.h"
#include "controller/item_controllers/textcontroller.h"
#include "controller/item_controllers/visualitemcontroller.h"
#include "controller/item_controllers/vmapitemfactory.h"
#include "controller/media_controller/charactersheetmediacontroller.h"
#include "controller/media_controller/imagemediacontroller.h"
#include "controller/media_controller/mediamanagerbase.h"
#include "controller/media_controller/notemediacontroller.h"
#include "controller/media_controller/pdfmediacontroller.h"
#include "controller/media_controller/sharednotemediacontroller.h"
#include "controller/media_controller/vectorialmapmediacontroller.h"
#include "controller/media_controller/webpagemediacontroller.h"
#include "controller/networkcontroller.h"
#include "controller/playercontroller.h"
#include "controller/preferencescontroller.h"
#include "controller/resourcescontroller.h"
#include "controller/view_controller/charactersheetcontroller.h"
#include "controller/view_controller/imagecontroller.h"
#include "controller/view_controller/imageselectorcontroller.h"
#include "controller/view_controller/mediacontrollerbase.h"
#include "controller/view_controller/mindmapcontroller.h"
#include "controller/view_controller/notecontroller.h"
#include "controller/view_controller/pdfcontroller.h"
#include "controller/view_controller/selectconnprofilecontroller.h"
#include "controller/view_controller/sharednotecontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "controller/view_controller/webpagecontroller.h"
#include "data/campaign.h"
#include "data/campaigneditor.h"
#include "data/campaignmanager.h"
#include "data/character.h"
#include "data/characterstate.h"
#include "data/charactervision.h"
#include "data/chatroom.h"
#include "data/chatroomfactory.h"
#include "data/cleveruri.h"
#include "data/cleverurimimedata.h"
#include "data/data.h"
#include "data/diceshortcut.h"
#include "data/localpersonmodel.h"
#include "data/media.h"
#include "data/person.h"
#include "data/player.h"
#include "data/resourcesnode.h"
#include "data/rolisteammimedata.h"
#include "data/rolisteamtheme.h"
#include "data/shortcutmodel.h"
#include "diceparser_qobject/diceroller.h"
#include "instantmessaging/commandmessage.h"
#include "instantmessaging/dicemessage.h"
#include "instantmessaging/errormessage.h"
#include "instantmessaging/im_global.h"
#include "instantmessaging/messagefactory.h"
#include "instantmessaging/messageinterface.h"
#include "instantmessaging/textmessage.h"
#include "instantmessaging/textwritercontroller.h"
#include "media/mediafactory.h"
#include "media/mediatype.h"
#include "mindmap/controller/selectioncontroller.h"
#include "mindmap/controller/spacingcontroller.h"
#include "mindmap/data/link.h"
#include "mindmap/data/mindmaptypes.h"
#include "mindmap/data/mindnode.h"
#include "mindmap/data/nodestyle.h"
#include "mindmap/geometry/linknode.h"
#include "mindmap/model/imagemodel.h"
#include "mindmap/model/minditemmodel.h"
#include "mindmap/model/nodeimageprovider.h"
#include "mindmap/model/nodestylemodel.h"
#include "mindmap/qmlItems/linkitem.h"
#include "mindmap/qmlItems/nodeitem.h"
#include "mindmap/worker/fileserializer.h"
#include "model/actiononlistmodel.h"
#include "model/charactermodel.h"
#include "model/characterstatemodel.h"
#include "model/chatroomsplittermodel.h"
#include "model/contentmodel.h"
#include "model/dicealiasmodel.h"
#include "model/filteredplayermodel.h"
#include "model/filterinstantmessagingmodel.h"
#include "model/genericmodel.h"
#include "model/historymodel.h"
#include "model/instantmessagingmodel.h"
#include "model/languagemodel.h"
#include "model/mediafilteredmodel.h"
#include "model/mediamodel.h"
#include "model/messagemodel.h"
#include "model/musicmodel.h"
#include "model/nonplayablecharactermodel.h"
#include "model/palettemodel.h"
#include "model/participantmodel.h"
#include "model/participantsmodel.h"
#include "model/patternmodel.h"
#include "model/playermodel.h"
#include "model/playerproxymodel.h"
#include "model/profilemodel.h"
#include "model/remoteplayermodel.h"
#include "model/singlecontenttypemodel.h"
#include "model/thememodel.h"
#include "model/vmapitemmodel.h"
#include "network/channel.h"
#include "network/channelmodel.h"
#include "network/characterdatamodel.h"
#include "network/clientconnection.h"
#include "network/clientmanager.h"
#include "network/connectionaccepter.h"
#include "network/connectionprofile.h"
#include "network/heartbeatsender.h"
#include "network/ipbanaccepter.h"
#include "network/iprangeaccepter.h"
#include "network/messagedispatcher.h"
#include "network/network_type.h"
#include "network/networkmessage.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "network/networkreceiver.h"
#include "network/passwordaccepter.h"
#include "network/receiveevent.h"
#include "network/rserver.h"
#include "network/serverconnection.h"
#include "network/timeaccepter.h"
#include "network/treeitem.h"
#include "preferences/preferenceslistener.h"
#include "preferences/preferencesmanager.h"
#include "qml_components/avatarprovider.h"
#include "qml_views/include/qml_views/image_selector.h"
#include "rwidgets/commands/movevmapitem.h"
#include "rwidgets/customs/centeredcheckbox.h"
#include "rwidgets/customs/checkedlabel.h"
#include "rwidgets/customs/circledisplayer.h"
#include "rwidgets/customs/colorlisteditor.h"
#include "rwidgets/customs/diameterselector.h"
#include "rwidgets/customs/filedirchooser.h"
#include "rwidgets/customs/imagepatheditor.h"
#include "rwidgets/customs/overlay.h"
#include "rwidgets/customs/playerwidget.h"
#include "rwidgets/customs/realslider.h"
#include "rwidgets/customs/rgraphicsview.h"
#include "rwidgets/customs/shortcutvisitor.h"
#include "rwidgets/customs/updaterwindow.h"
#include "rwidgets/customs/vcolorselector.h"
#include "rwidgets/customs/vcolortablechooser.h"
#include "rwidgets/customs/vmap.h"
#include "rwidgets/customs/workspace.h"
#include "rwidgets/delegates/actiondelegate.h"
#include "rwidgets/delegates/avatardelegate.h"
#include "rwidgets/delegates/checkboxdelegate.h"
#include "rwidgets/delegates/colordelegate.h"
#include "rwidgets/delegates/delegate.h"
#include "rwidgets/delegates/imagepathdelegateitem.h"
#include "rwidgets/delegates/taglistdelegate.h"
#include "rwidgets/delegates/userlistdelegate.h"
#include "rwidgets/dialogs/aboutrolisteam.h"
#include "rwidgets/dialogs/campaignintegritydialog.h"
#include "rwidgets/dialogs/campaignproperties.h"
#include "rwidgets/dialogs/connectionretrydialog.h"
#include "rwidgets/dialogs/historyviewerdialog.h"
#include "rwidgets/dialogs/imageselectordialog.h"
#include "rwidgets/dialogs/importdatafromcampaigndialog.h"
#include "rwidgets/dialogs/keygeneratordialog.h"
#include "rwidgets/dialogs/newfiledialog.h"
#include "rwidgets/dialogs/persondialog.h"
#include "rwidgets/dialogs/preferencesdialog.h"
#include "rwidgets/dialogs/selectconnectionprofiledialog.h"
#include "rwidgets/dialogs/shortcuteditordialog.h"
#include "rwidgets/dialogs/tipofdayviewer.h"
#include "rwidgets/dialogs/vmapwizzarddialog.h"
#include "rwidgets/docks/antagonistboard.h"
#include "rwidgets/docks/audioPlayer.h"
#include "rwidgets/docks/campaigndock.h"
#include "rwidgets/docks/channellistpanel.h"
#include "rwidgets/docks/notificationzone.h"
#include "rwidgets/docks/playerspanel.h"
#include "rwidgets/docks/vmaptoolbar.h"
#include "rwidgets/editors/mtextedit.h"
#include "rwidgets/editors/noteeditor/src/notecontainer.h"
#include "rwidgets/editors/noteeditor/src/textedit.h"
#include "rwidgets/editors/sharededitor/codeeditor.h"
#include "rwidgets/editors/sharededitor/connecttodocument.h"
#include "rwidgets/editors/sharededitor/document.h"
#include "rwidgets/editors/sharededitor/enu.h"
#include "rwidgets/editors/sharededitor/finddialog.h"
#include "rwidgets/editors/sharededitor/findtoolbar.h"
#include "rwidgets/editors/sharededitor/firstrundialog.h"
#include "rwidgets/editors/sharededitor/markdownhighlighter.h"
#include "rwidgets/editors/sharededitor/participantspane.h"
#include "rwidgets/editors/sharededitor/sharednote.h"
#include "rwidgets/editors/sharededitor/sharednotecontainer.h"
#include "rwidgets/editors/sharededitor/utilities.h"
#include "rwidgets/gmtoolbox/DiceBookMark/dicebookmarkmodel.h"
#include "rwidgets/gmtoolbox/DiceBookMark/dicebookmarkwidget.h"
#include "rwidgets/gmtoolbox/NameGenerator/namegeneratorwidget.h"
#include "rwidgets/gmtoolbox/UnitConvertor/convertor.h"
#include "rwidgets/gmtoolbox/UnitConvertor/convertoroperator.h"
#include "rwidgets/gmtoolbox/UnitConvertor/customrulemodel.h"
#include "rwidgets/gmtoolbox/UnitConvertor/unit.h"
#include "rwidgets/gmtoolbox/UnitConvertor/unitmodel.h"
#include "rwidgets/gmtoolbox/gamemastertool.h"
#include "rwidgets/graphicsItems/anchoritem.h"
#include "rwidgets/graphicsItems/characteritem.h"
#include "rwidgets/graphicsItems/childpointitem.h"
#include "rwidgets/graphicsItems/ellipsitem.h"
#include "rwidgets/graphicsItems/griditem.h"
#include "rwidgets/graphicsItems/highlighteritem.h"
#include "rwidgets/graphicsItems/imageitem.h"
#include "rwidgets/graphicsItems/lineitem.h"
#include "rwidgets/graphicsItems/pathitem.h"
#include "rwidgets/graphicsItems/rectitem.h"
#include "rwidgets/graphicsItems/ruleitem.h"
#include "rwidgets/graphicsItems/sightitem.h"
#include "rwidgets/graphicsItems/textitem.h"
#include "rwidgets/graphicsItems/visualitem.h"
#include "rwidgets/layouts/flowlayout.h"
#include "rwidgets/mediacontainers/charactersheetwindow.h"
#include "rwidgets/mediacontainers/image.h"
#include "rwidgets/mediacontainers/instantmessagingview.h"
#include "rwidgets/mediacontainers/mediacontainer.h"
#include "rwidgets/mediacontainers/mindmapview.h"
#include "rwidgets/mediacontainers/pdfviewer.h"
#include "rwidgets/mediacontainers/vmapframe.h"
#include "rwidgets/mediacontainers/webview.h"
#include "rwidgets/modelviews/campaignview.h"
#include "rwidgets/modelviews/dragabletableview.h"
#include "rwidgets/modelviews/userlistview.h"
#include "rwidgets/rwidgets_global.h"
#include "rwidgets/toolbars/vtoolbar.h"
#include "services/ipchecker.h"
#include "services/tipchecker.h"
#include "services/updatechecker.h"
#include "session/sessionitemmodel.h"
#include "updater/controller/audioplayerupdater.h"
#include "updater/controller/servermanagerupdater.h"
#include "updater/media/campaignupdater.h"
#include "updater/media/charactersheetupdater.h"
#include "updater/media/genericupdater.h"
#include "updater/media/instantmessagingupdater.h"
#include "updater/media/mediaupdaterinterface.h"
#include "updater/media/mindmapupdater.h"
#include "updater/media/sharednotecontrollerupdater.h"
#include "updater/media/vmapupdater.h"
#include "updater/media/webviewupdater.h"
#include "updater/vmapitem/characteritemupdater.h"
#include "updater/vmapitem/ellipsecontrollerupdater.h"
#include "updater/vmapitem/imagecontrollerupdater.h"
#include "updater/vmapitem/linecontrollerupdater.h"
#include "updater/vmapitem/pathcontrollerupdater.h"
#include "updater/vmapitem/rectcontrollerupdater.h"
#include "updater/vmapitem/textcontrollerupdater.h"
#include "updater/vmapitem/vmapitemcontrollerupdater.h"
#include "utils/countdownobject.h"
#include "utils/insertionsortedmap.h"
#include "utils/iohelper.h"
#include "utils/networkdownloader.h"
#include "worker/autosavecontroller.h"
#include "worker/convertionhelper.h"
#include "worker/fileserializer.h"
#include "worker/iohelper.h"
#include "worker/messagehelper.h"
#include "worker/modelhelper.h"
#include "worker/networkhelper.h"
#include "worker/playermessagehelper.h"
#include "worker/utilshelper.h"
#include "worker/vectorialmapmessagehelper.h"

#include <helper.h>
#include <memory>

class QObjectsTest : public QObject
{
    Q_OBJECT
public:
    QObjectsTest();

private slots:
    void init();

    void propertiesTest();
    void propertiesTest_data();
};

QObjectsTest::QObjectsTest() {}
void QObjectsTest::init() {}
void QObjectsTest::propertiesTest()
{
    QFETCH(QObject*, object);
    auto res= Helper::testAllProperties(object);
    if(!res.second.isEmpty())
        QVERIFY(res.first);
    for(const auto& f : res.second)
    {
        qDebug() << "Unmanaged:" << f;
    }

    object->deleteLater();
}

void QObjectsTest::propertiesTest_data()
{
    QTest::addColumn<QObject*>("object");

    QTest::addRow("CharacterSheet") << static_cast<QObject*>(new CharacterSheet());
    QTest::addRow("CharacterSheetModel") << static_cast<QObject*>(new CharacterSheetModel());
    QTest::addRow("charactersheet::ImageModel") << static_cast<QObject*>(new charactersheet::ImageModel());
    QTest::addRow("RolisteamImageProvider")
        << static_cast<QObject*>(new RolisteamImageProvider(new charactersheet::ImageModel()));
    QTest::addRow("FieldController") << static_cast<QObject*>(new FieldController(TreeSheetItem::FieldItem, true));
    QTest::addRow("Section") << static_cast<QObject*>(new Section());
    QTest::addRow("TableField") << static_cast<QObject*>(new TableField());
    QTest::addRow("LineModel") << static_cast<QObject*>(new LineModel());
    QTest::addRow("LineFieldItem") << static_cast<QObject*>(new LineFieldItem());
    QTest::addRow("SheetWidget") << static_cast<QObject*>(new SheetWidget());
    QTest::addRow("LogController") << static_cast<QObject*>(new LogController(true));
    QTest::addRow("RemoteLogController") << static_cast<QObject*>(new RemoteLogController());
    QTest::addRow("LogUploader") << static_cast<QObject*>(new LogUploader());
    QTest::addRow("StyleSheet") << static_cast<QObject*>(new customization::StyleSheet(nullptr));
    QTest::addRow("Theme") << static_cast<QObject*>(new customization::Theme());
    QTest::addRow("ColorButton") << static_cast<QObject*>(new ColorButton());
    QTest::addRow("LogPanel") << static_cast<QObject*>(new LogPanel());
    QTest::addRow("CharacterItemController")
        << static_cast<QObject*>(new vmap::CharacterItemController({}, new VectorialMapController()));
    QTest::addRow("EllipseController") << static_cast<QObject*>(
        new vmap::EllipseController({}, new VectorialMapController()));
    QTest::addRow("GridController") << static_cast<QObject*>(new vmap::GridController(new VectorialMapController()));
    QTest::addRow("ImageItemController") << static_cast<QObject*>(
        new vmap::ImageItemController({}, new VectorialMapController()));
    QTest::addRow("LineController") << static_cast<QObject*>(
        new vmap::LineController({}, new VectorialMapController()));
    QTest::addRow("PathController") << static_cast<QObject*>(
        new vmap::PathController({}, new VectorialMapController()));
    QTest::addRow("RectController") << static_cast<QObject*>(
        new vmap::RectController({}, new VectorialMapController()));
    QTest::addRow("SightController") << static_cast<QObject*>(new vmap::SightController(new VectorialMapController()));
    QTest::addRow("TextController") << static_cast<QObject*>(
        new vmap::TextController({}, new VectorialMapController()));
    QTest::addRow("CharacterSheetController") << static_cast<QObject*>(new CharacterSheetController());
    QTest::addRow("ImageController") << static_cast<QObject*>(new ImageController());
    QTest::addRow("ImageSelectorController") << static_cast<QObject*>(new ImageSelectorController());
    QTest::addRow("MindMapController") << static_cast<QObject*>(new MindMapController("idxxxxx"));
    QTest::addRow("NoteController") << static_cast<QObject*>(new NoteController());
    QTest::addRow("PdfController") << static_cast<QObject*>(new PdfController());
    QTest::addRow("SelectConnProfileController")
        << static_cast<QObject*>(new SelectConnProfileController(new ProfileModel()));
    QTest::addRow("SharedNoteController") << static_cast<QObject*>(new SharedNoteController());
    QTest::addRow("VectorialMapController") << static_cast<QObject*>(new VectorialMapController());
    QTest::addRow("WebpageController") << static_cast<QObject*>(new WebpageController());
    QTest::addRow("FilteredCharacterModel") << static_cast<QObject*>(new campaign::FilteredCharacterModel());
    QTest::addRow("AntagonistBoardController")
        << static_cast<QObject*>(new campaign::AntagonistBoardController(new campaign::CampaignEditor()));
    QTest::addRow("AudioController") << static_cast<QObject*>(new AudioController(nullptr, nullptr));
    QTest::addRow("AudioPlayerController") << static_cast<QObject*>(new AudioPlayerController(0, "k", nullptr));
    QTest::addRow("ContentController") << static_cast<QObject*>(
        new ContentController(nullptr, nullptr, nullptr, nullptr));
    QTest::addRow("GameController") << static_cast<QObject*>(new GameController("test", "1.2", nullptr));
    QTest::addRow("InstantMessagingController") << static_cast<QObject*>(new InstantMessagingController(nullptr));
    QTest::addRow("NetworkController") << static_cast<QObject*>(new NetworkController());
    QTest::addRow("PlayerController") << static_cast<QObject*>(new PlayerController());
    QTest::addRow("PreferencesController") << static_cast<QObject*>(new PreferencesController());
    QTest::addRow("Campaign") << static_cast<QObject*>(new campaign::Campaign());
    QTest::addRow("CampaignEditor") << static_cast<QObject*>(new campaign::CampaignEditor());
    QTest::addRow("CampaignManager") << static_cast<QObject*>(new campaign::CampaignManager(nullptr));
    QTest::addRow("CharacterVision") << static_cast<QObject*>(new CharacterVision());
    QTest::addRow("ChatRoom") << static_cast<QObject*>(
        new InstantMessaging::ChatRoom(nullptr, InstantMessaging::ChatRoom::GLOBAL));
    QTest::addRow("CleverURI") << static_cast<QObject*>(new CleverURI(Core::ContentType::UNKNOWN));
    QTest::addRow("CleverUriMimeData") << static_cast<QObject*>(new CleverUriMimeData());
    QTest::addRow("obj") << static_cast<QObject*>(new LocalPersonModel());
    QTest::addRow("Media") << static_cast<QObject*>(new campaign::Media({}, {}, {}, Core::MediaType::AudioFile));
    QTest::addRow("RolisteamMimeData") << static_cast<QObject*>(new RolisteamMimeData());
    QTest::addRow("ShortCutModel") << static_cast<QObject*>(new ShortCutModel());
    QTest::addRow("ActionOnListModel") << static_cast<QObject*>(new ActionOnListModel({}, {}, {}));
    QTest::addRow("CharacterModel") << static_cast<QObject*>(new CharacterModel());
    QTest::addRow("CharacterStateModel") << static_cast<QObject*>(new CharacterStateModel());
    QTest::addRow("ChatroomSplitterModel") << static_cast<QObject*>(new InstantMessaging::ChatroomSplitterModel());
    QTest::addRow("FilteredContentModel")
        << static_cast<QObject*>(new FilteredContentModel(Core::ContentType::UNKNOWN));
    QTest::addRow("ContentModel") << static_cast<QObject*>(new ContentModel());
    QTest::addRow("DiceAliasModel") << static_cast<QObject*>(new DiceAliasModel());
    QTest::addRow("FilteredPlayerModel") << static_cast<QObject*>(new InstantMessaging::FilteredPlayerModel({}));
    QTest::addRow("FilterInstantMessagingModel")
        << static_cast<QObject*>(new InstantMessaging::FilterInstantMessagingModel());
    QTest::addRow("GenericModel") << static_cast<QObject*>(new GenericModel({}));
    QTest::addRow("HistoryModel") << static_cast<QObject*>(new history::HistoryModel());
    QTest::addRow("InstantMessaging") << static_cast<QObject*>(new InstantMessaging::InstantMessagingModel(nullptr));
    QTest::addRow("LanguageModel") << static_cast<QObject*>(new LanguageModel());
    QTest::addRow("MediaFilteredModel") << static_cast<QObject*>(new MediaFilteredModel());
    QTest::addRow("MediaNode") << static_cast<QObject*>(new campaign::MediaNode(campaign::MediaNode::File, {}));
    QTest::addRow("MessageModel") << static_cast<QObject*>(new InstantMessaging::MessageModel(nullptr));
    QTest::addRow("MusicModel") << static_cast<QObject*>(new MusicModel());
    QTest::addRow("NonPlayableCharacter") << static_cast<QObject*>(new campaign::NonPlayableCharacter());
    QTest::addRow("PaletteModel") << static_cast<QObject*>(new PaletteModel());
    QTest::addRow("ParticipantModel") << static_cast<QObject*>(new ParticipantModel({}, nullptr));
    QTest::addRow("ParticipantsModel") << static_cast<QObject*>(new ParticipantsModel());
    QTest::addRow("PatternModel") << static_cast<QObject*>(new PatternModel());
    QTest::addRow("PlayerModel") << static_cast<QObject*>(new PlayerModel());
    QTest::addRow("PlayerProxyModel") << static_cast<QObject*>(new PlayerProxyModel());
    QTest::addRow("ProfileModel") << static_cast<QObject*>(new ProfileModel());
    QTest::addRow("RemotePlayerModel") << static_cast<QObject*>(new RemotePlayerModel(nullptr));
    QTest::addRow("SingleContentTypeModel")
        << static_cast<QObject*>(new SingleContentTypeModel(Core::ContentType::VECTORIALMAP));
    QTest::addRow("ThemeModel") << static_cast<QObject*>(new ThemeModel());
    QTest::addRow("VmapItemModel") << static_cast<QObject*>(new vmap::VmapItemModel());
    QTest::addRow("PreferencesManager") << static_cast<QObject*>(new PreferencesManager("test_rolisteam", "test"));
    QTest::addRow("IpChecker") << static_cast<QObject*>(new IpChecker());
    QTest::addRow("TipChecker") << static_cast<QObject*>(new TipChecker());
    QTest::addRow("UpdateChecker") << static_cast<QObject*>(new UpdateChecker({}));
    QTest::addRow("SessionItemModel") << static_cast<QObject*>(new session::SessionItemModel());
    QTest::addRow("AudioPlayerUpdater") << static_cast<QObject*>(new AudioPlayerUpdater(nullptr, nullptr));
    QTest::addRow("ServerManagerUpdater") << static_cast<QObject*>(new ServerManagerUpdater(nullptr, true));
    QMap<QString, QVariant> params;
    QTest::addRow("ServerManagerUpdater")
        << static_cast<QObject*>(new ServerManagerUpdater(new ServerConnectionManager(params), true));
    QTest::addRow("CampaignUpdater") << static_cast<QObject*>(
        new campaign::CampaignUpdater(nullptr, new campaign::Campaign())); // 100
    QTest::addRow("CharacterSheetUpdater") << static_cast<QObject*>(new CharacterSheetUpdater({}));
    QTest::addRow("GenericUpdater") << static_cast<QObject*>(new GenericUpdater(nullptr));
    QTest::addRow("InstantMessagingUpdater") << static_cast<QObject*>(new InstantMessaging::InstantMessagingUpdater());
    QTest::addRow("MindMapUpdater") << static_cast<QObject*>(new MindMapUpdater(nullptr, nullptr));
    QTest::addRow("SharedNoteControllerUpdater")
        << static_cast<QObject*>(new SharedNoteControllerUpdater(nullptr, nullptr));
    QTest::addRow("VMapUpdater") << static_cast<QObject*>(new VMapUpdater(nullptr, nullptr));
    QTest::addRow("WebViewUpdater") << static_cast<QObject*>(new WebViewUpdater(nullptr));
    QTest::addRow("CharacterItemUpdater") << static_cast<QObject*>(new CharacterItemUpdater());
    QTest::addRow("EllipseControllerUpdater") << static_cast<QObject*>(new EllipseControllerUpdater());
    QTest::addRow("ImageControllerUpdater") << static_cast<QObject*>(new ImageControllerUpdater());
    QTest::addRow("LineControllerUpdater") << static_cast<QObject*>(new LineControllerUpdater());
    QTest::addRow("PathControllerUpdater") << static_cast<QObject*>(new PathControllerUpdater());
    QTest::addRow("RectControllerUpdater") << static_cast<QObject*>(new RectControllerUpdater());
    QTest::addRow("TextControllerUpdater") << static_cast<QObject*>(new TextControllerUpdater());
    QTest::addRow("VMapItemControllerUpdater") << static_cast<QObject*>(new VMapItemControllerUpdater());
    QTest::addRow("CountDownObject") << static_cast<QObject*>(new CountDownObject(20, 1000));
    QTest::addRow("AutoSaveController") << static_cast<QObject*>(new AutoSaveController(nullptr));
    QTest::addRow("NetworkDownloader") << static_cast<QObject*>(new NetworkDownloader({}));
    QTest::addRow("Character") << static_cast<QObject*>(new Character());
    QTest::addRow("Player") << static_cast<QObject*>(new Player());
    QTest::addRow("Channel") << static_cast<QObject*>(new Channel());
    QTest::addRow("ChannelModel") << static_cast<QObject*>(new ChannelModel());
    QTest::addRow("ClientMimeData") << static_cast<QObject*>(new ClientMimeData());
    QTest::addRow("CharacterDataModel") << static_cast<QObject*>(new CharacterDataModel());
    QTest::addRow("ClientManager") << static_cast<QObject*>(new ClientManager());
    QTest::addRow("ConnectionProfile") << static_cast<QObject*>(new ConnectionProfile());
    QTest::addRow("MessageDispatcher") << static_cast<QObject*>(new MessageDispatcher());
    QTest::addRow("ServerConnection") << static_cast<QObject*>(new ServerConnection(nullptr));
    QTest::addRow("RServer") << static_cast<QObject*>(new RServer(params, true));
    QTest::addRow("ServerConnectionManager") << static_cast<QObject*>(new ServerConnectionManager(params));
    QTest::addRow("ClientConnection") << static_cast<QObject*>(new ClientConnection());
    // QTest::addRow("obj", i++) << new TreeItem();
    QTest::addRow("DiceRoller") << static_cast<QObject*>(new DiceRoller());
    QTest::addRow("CommandMessage") << static_cast<QObject*>(new InstantMessaging::CommandMessage({}, {}, {}));
    QTest::addRow("DiceMessage") << static_cast<QObject*>(new InstantMessaging::DiceMessage({}, {}, {}));
    QTest::addRow("ErrorMessage") << static_cast<QObject*>(new InstantMessaging::ErrorMessage({}, {}, {}));
    QTest::addRow("TextMessage") << static_cast<QObject*>(new InstantMessaging::TextMessage({}, {}, {}));
    QTest::addRow("TextWriterController") << static_cast<QObject*>(new InstantMessaging::TextWriterController());
    QTest::addRow("SelectionController") << static_cast<QObject*>(new mindmap::SelectionController());
    auto ctrl= new mindmap::MindItemModel(nullptr);
    QTest::addRow("SpacingController") << static_cast<QObject*>(new mindmap::SpacingController(ctrl, nullptr));
    QTest::addRow("LinkController") << static_cast<QObject*>(new mindmap::LinkController());
    QTest::addRow("NodeStyle") << static_cast<QObject*>(new mindmap::NodeStyle());
    QTest::addRow("MindNode") << static_cast<QObject*>(new mindmap::MindNode());
    // QTest::addRow("obj", i++) << new mindmap::LinkNode();
    QTest::addRow("ImageModel") << static_cast<QObject*>(new charactersheet::ImageModel());
    QTest::addRow("NodeImageProvider") << static_cast<QObject*>(new mindmap::NodeImageProvider(nullptr));
    QTest::addRow("NodeStyleModel") << static_cast<QObject*>(new mindmap::NodeStyleModel());
    QTest::addRow("LinkItem") << static_cast<QObject*>(new mindmap::LinkItem());
    QTest::addRow("FileSerializer") << static_cast<QObject*>(new mindmap::FileSerializer());
    QTest::addRow("AvatarProvider") << static_cast<QObject*>(new AvatarProvider(nullptr));
    QTest::addRow("ImageSelector") << static_cast<QObject*>(new ImageSelector());
    QTest::addRow("CenteredCheckBox") << static_cast<QObject*>(new CenteredCheckBox());
    QTest::addRow("CheckedLabel") << static_cast<QObject*>(new CheckedLabel());
    QTest::addRow("CircleDisplayer") << static_cast<QObject*>(new CircleDisplayer());
    QTest::addRow("ColorListEditor") << static_cast<QObject*>(new rwidgets::ColorListEditor());
    QTest::addRow("DiameterSelector") << static_cast<QObject*>(new DiameterSelector());
    QTest::addRow("FileDirChooser") << static_cast<QObject*>(new FileDirChooser());
    QTest::addRow("ImagePathEditor") << static_cast<QObject*>(new rwidgets::ImagePathEditor({}));
    QTest::addRow("Overlay") << static_cast<QObject*>(new Overlay());
    QTest::addRow("PlayerWidget") << static_cast<QObject*>(new PlayerWidget(nullptr));
    QTest::addRow("RealSlider") << static_cast<QObject*>(new RealSlider());
    QTest::addRow("RGraphicsView") << static_cast<QObject*>(new RGraphicsView(nullptr));
    QTest::addRow("ShortcutVisitor") << static_cast<QObject*>(new ShortcutVisitor());
    QTest::addRow("UpdaterWindow") << static_cast<QObject*>(new UpdaterWindow());
    QTest::addRow("VColorLabel") << static_cast<QObject*>(new VColorLabel());
    QTest::addRow("BackgroundButton") << static_cast<QObject*>(new BackgroundButton(nullptr));
    QTest::addRow("VColorSelector") << static_cast<QObject*>(new VColorSelector());
    QTest::addRow("SaturationChooser") << static_cast<QObject*>(new SaturationChooser());
    QTest::addRow("ColorTable") << static_cast<QObject*>(new ColorTable());
    QTest::addRow("ColorTableChooser") << static_cast<QObject*>(new ColorTableChooser());
    QTest::addRow("VMap") << static_cast<QObject*>(new VMap(new VectorialMapController()));
    QTest::addRow("Workspace") << static_cast<QObject*>(new Workspace(nullptr, nullptr, nullptr));
    QTest::addRow("LabelWithOptions") << static_cast<QObject*>(new LabelWithOptions());
    QTest::addRow("ActionDelegate") << static_cast<QObject*>(new ActionDelegate());
    QTest::addRow("AvatarDelegate") << static_cast<QObject*>(new AvatarDelegate());
    QTest::addRow("CheckBoxDelegate") << static_cast<QObject*>(new rwidgets::CheckBoxDelegate());
    QTest::addRow("ColorDelegate") << static_cast<QObject*>(new rwidgets::ColorDelegate());
    QTest::addRow("Delegate") << static_cast<QObject*>(new Delegate());
    QTest::addRow("ImagePathDelegateItem") << static_cast<QObject*>(new rwidgets::ImagePathDelegateItem({}));
    QTest::addRow("TagListDelegate") << static_cast<QObject*>(new TagListDelegate());
    QTest::addRow("UserListDelegate") << static_cast<QObject*>(new UserListDelegate());
    QTest::addRow("AboutRolisteam") << static_cast<QObject*>(new AboutRolisteam({}));
    QTest::addRow("CampaignIntegrityDialog")
        << static_cast<QObject*>(new campaign::CampaignIntegrityDialog({}, {}, {}));
    QTest::addRow("CampaignProperties") << static_cast<QObject*>(
        new CampaignProperties(new campaign::Campaign(), nullptr));
    QTest::addRow("ConnectionRetryDialog") << static_cast<QObject*>(new ConnectionRetryDialog());
    QTest::addRow("HistoryViewerDialog") << static_cast<QObject*>(new HistoryViewerDialog(nullptr));
    QTest::addRow("ImageSelectorDialog") << static_cast<QObject*>(
        new ImageSelectorDialog(new ImageSelectorController()));
    QTest::addRow("ImportDataFromCampaignDialog") << static_cast<QObject*>(new ImportDataFromCampaignDialog({}));
    QTest::addRow("KeyGeneratorDialog") << static_cast<QObject*>(new KeyGeneratorDialog());
    QTest::addRow("NewFileDialog") << static_cast<QObject*>(new NewFileDialog(Core::ContentType::VECTORIALMAP));
    QTest::addRow("PersonDialog") << static_cast<QObject*>(new PersonDialog());
    QTest::addRow("PreferencesDialog") << static_cast<QObject*>(new PreferencesDialog(new PreferencesController()));
    QTest::addRow("SelectConnectionProfileDialog") << static_cast<QObject*>(new SelectConnectionProfileDialog(nullptr));
    QTest::addRow("ShortCutEditorDialog") << static_cast<QObject*>(new ShortCutEditorDialog());
    QTest::addRow("TipOfDayViewer") << static_cast<QObject*>(new TipOfDayViewer({}, {}, {}));
    QTest::addRow("MapWizzardDialog") << static_cast<QObject*>(new MapWizzardDialog());
    QTest::addRow("AntagonistBoard") << static_cast<QObject*>(new campaign::AntagonistBoard(nullptr)); // 200
    QTest::addRow("AudioPlayer") << static_cast<QObject*>(new AudioPlayer(nullptr));
    QTest::addRow("CampaignDock") << static_cast<QObject*>(new campaign::CampaignDock(new campaign::CampaignEditor()));
    QTest::addRow("ChannelListPanel") << static_cast<QObject*>(new ChannelListPanel(new NetworkController()));
    QTest::addRow("NotificationZone") << static_cast<QObject*>(new NotificationZone(new LogController(true)));
    QTest::addRow("PlayersPanel") << static_cast<QObject*>(new PlayersPanel(nullptr));
    QTest::addRow("VmapTopBar") << static_cast<QObject*>(new VmapTopBar(nullptr));
    // QTest::addRow("obj", i++) << new MRichTextEdit();
    QTest::addRow("MTextEdit") << static_cast<QObject*>(new MTextEdit());
    QTest::addRow("CodeEditor") << static_cast<QObject*>(new CodeEditor(nullptr));
    QTest::addRow("Document") << static_cast<QObject*>(new Document(nullptr));
    QTest::addRow("FindDialog") << static_cast<QObject*>(new FindDialog());
    QTest::addRow("FindToolBar") << static_cast<QObject*>(new FindToolBar());
    QTest::addRow("MarkDownHighlighter") << static_cast<QObject*>(new MarkDownHighlighter());
    QTest::addRow("ParticipantsPane") << static_cast<QObject*>(new ParticipantsPane(nullptr));
    QTest::addRow("SharedNote") << static_cast<QObject*>(new SharedNote(nullptr));
    QTest::addRow("SharedNoteContainer") << static_cast<QObject*>(new SharedNoteContainer(nullptr));
    std::vector<DiceShortCut> shorCuts;
    QTest::addRow("DiceBookMarkModel") << static_cast<QObject*>(new DiceBookMarkModel(shorCuts));
    QTest::addRow("DiceBookMarkWidget obj") << static_cast<QObject*>(new DiceBookMarkWidget(shorCuts));
    QTest::addRow("NameGeneratorWidget obj") << static_cast<QObject*>(new NameGeneratorWidget());
    QTest::addRow("Convertor obj") << static_cast<QObject*>(new GMTOOL::Convertor());
    QTest::addRow("CustomRuleModel obj") << static_cast<QObject*>(new GMTOOL::CustomRuleModel());
    QTest::addRow("UnitModel obj") << static_cast<QObject*>(new GMTOOL::UnitModel());
    QTest::addRow("CategoryModel obj") << static_cast<QObject*>(new GMTOOL::CategoryModel());
    QTest::addRow("AnchorItem obj") << static_cast<QObject*>(new AnchorItem());
    QTest::addRow("CharacterItem") << static_cast<QObject*>(
        new CharacterItem(new vmap::CharacterItemController({}, new VectorialMapController())));
    QTest::addRow("ChildPointItem obj") << static_cast<QObject*>(new ChildPointItem(nullptr, 0, nullptr));
    QTest::addRow("EllipsItem obj") << static_cast<QObject*>(new EllipsItem(nullptr));
    QTest::addRow("GridItem obj") << static_cast<QObject*>(new GridItem(nullptr));
    QTest::addRow("HighlighterItem obj") << static_cast<QObject*>(new HighlighterItem({}, 3, Qt::red, nullptr, false));
    QTest::addRow("ImageItem obj") << static_cast<QObject*>(new ImageItem(nullptr));
    QTest::addRow("LineItem obj") << static_cast<QObject*>(new LineItem(nullptr));
    QTest::addRow("PathItem") << static_cast<QObject*>(new PathItem(nullptr));
    QTest::addRow("RectItem") << static_cast<QObject*>(new RectItem(nullptr));
    QTest::addRow("RuleItem") << static_cast<QObject*>(new RuleItem(nullptr));
    QTest::addRow("SightItem") << static_cast<QObject*>(new SightItem(nullptr));
    QTest::addRow("TextItem") << static_cast<QObject*>(new TextItem(nullptr));
    QTest::addRow("RichTextEditDialog") << static_cast<QObject*>(new RichTextEditDialog());
    QTest::addRow("TextLabel") << static_cast<QObject*>(new TextLabel());
    QTest::addRow("FlowLayout") << static_cast<QObject*>(new FlowLayout(nullptr));
    QTest::addRow("CharacterSheetWindow") << static_cast<QObject*>(new CharacterSheetWindow(nullptr));
    QTest::addRow("Image") << static_cast<QObject*>(new Image(nullptr));
    QTest::addRow("InstantMessagerManager") << static_cast<QObject*>(new InstantMessagerManager());
    QTest::addRow("MediaContainer") << static_cast<QObject*>(
        new MediaContainer(nullptr, MediaContainer::ContainerType::NoteContainer));
    QTest::addRow("MindmapManager") << static_cast<QObject*>(new MindmapManager());
    QTest::addRow("PdfViewer") << static_cast<QObject*>(new PdfViewer(nullptr));
    QTest::addRow("MindMapView") << static_cast<QObject*>(new MindMapView(nullptr));
    QTest::addRow("VMapFrame") << static_cast<QObject*>(new VMapFrame(nullptr));
    QTest::addRow("WebView") << static_cast<QObject*>(new WebView(nullptr));
    QTest::addRow("CampaignView") << static_cast<QObject*>(new campaign::CampaignView());
    QTest::addRow("DragableTableView") << static_cast<QObject*>(new DragableTableView());
    QTest::addRow("UserListView") << static_cast<QObject*>(new UserListView());
    QTest::addRow("ToolBox") << static_cast<QObject*>(new ToolBox(nullptr));
    QTest::addRow("TextLabel") << static_cast<QObject*>(new TextLabel(nullptr));
    QTest::addRow("RichTextEditDialog") << static_cast<QObject*>(new RichTextEditDialog());
    QTest::addRow("NonPlayableCharacterModel") << static_cast<QObject*>(new campaign::NonPlayableCharacterModel());
    QTest::addRow("FilteredCharacterModel") << static_cast<QObject*>(new campaign::FilteredCharacterModel());
}

QTEST_MAIN(QObjectsTest)

#include "tst_qobjects.moc"
