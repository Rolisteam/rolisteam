#include <QGraphicsObject>
#include <QStyledItemDelegate>
#include <QTest>

#include "XML_Editor.h"
#include "aboutrcse.h"
#include "alignmentdelegate.h"
#include "borderlisteditor.h"
#include "charactercontroller.h"
#include "characterlist.h"
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
#include "charactersheetitem.h"
#include "clientmanager.h"
#include "codeedit.h"
#include "codeeditor.h"
#include "codeeditordialog.h"
#include "common/common_types.h"
#include "common/logcontroller.h"
#include "common/remotelogcontroller.h"
#include "common/uploadlogtoserver.h"
#include "common_qml/theme.h"
#include "common_widgets/busyindicatordialog.h"
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
#include "controller/networkcontroller.h"
#include "controller/playercontroller.h"
#include "controller/preferencescontroller.h"
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
#include "controllers/maincontroller.h"
#include "csitem.h"
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
#include "editorcontroller.h"
#include "fieldview.h"
#include "instantmessaging/commandmessage.h"
#include "instantmessaging/dicemessage.h"
#include "instantmessaging/errormessage.h"
#include "instantmessaging/im_global.h"
#include "instantmessaging/messagefactory.h"
#include "instantmessaging/messageinterface.h"
#include "instantmessaging/textmessage.h"
#include "instantmessaging/textwritercontroller.h"
#include "itemeditor.h"
#include "maincontroller.h"
#include "mappinghelper.h"
#include "media/mediafactory.h"
#include "media/mediatype.h"
#include "mediamodel.h"
#include "minditem.h"
#include "minditemmodel.h"
#include "mindmap/controller/selectioncontroller.h"
#include "mindmap/controller/spacingcontroller.h"
#include "mindmap/data/mindmaptypes.h"
#include "mindmap/data/mindnode.h"
#include "mindmap/data/nodestyle.h"
#include "mindmap/geometry/linknode.h"
#include "mindmap/model/imagemodel.h"
#include "mindmap/model/minditemmodel.h"
#include "mindmap/model/nodeimageprovider.h"
#include "mindmap/model/nodestylemodel.h"
#include "mindmap/qmlItems/linkitem.h"
#include "mindmap/worker/fileserializer.h"
#include "mindmapcontrollerbase.h"
// #include "model/actiononlistmodel.h"
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
#include "mrichtextedit.h"
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
#include "notecontainer.h"
#include "pdfmanager.h"
#include "positioneditem.h"
#include "preferences/preferenceslistener.h"
#include "preferences/rcsepreferencesmanager.h"
#include "proxystatemodel.h"
#include "qml_components/avatarprovider.h"
#include "qml_views/include/qml_views/image_selector.h"
#include "rcse/mainwindow.h"
#include "rcseapplicationcontroller.h"
#include "rolisteamapplication.h"
#include "rolisteamdaemon.h"
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
#include "sheetcontroller.h"
#include "sheetproperties.h"
#include "sidemenucontroller.h"
#include "slidercontroller.h"
#include "statedelegate.h"
#include "tablecanvasfield.h"
#include "textedit.h"
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
#include "upnp/upnpnat.h"
#include "utils/countdownobject.h"
#include "utils/iohelper.h"
#include "utils/networkdownloader.h"
#include "visualitemcontroller.h"
#include "vtoolbar.h"
#include "widget/mainwindow.h"
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
#include "workspace.h"

#include <QStringList>
#include <QWidget>
#include <helper.h>
#include <memory>

struct IgnoredProperty
{
    QObject* object;
    QStringList names;
};

class QObjectsTest : public QObject
{
    Q_OBJECT
public:
    QObjectsTest();

private slots:
    void init();

    void propertiesTest();
    void propertiesTest_data();

private:
    QList<IgnoredProperty> m_ignored;
};

QObjectsTest::QObjectsTest() {}
void QObjectsTest::init() {}

/*void QObjectsTest::profilePropertiesTest()
{
    ProfileModel model;
    model.appendProfile();

    SelectConnProfileController ctrl(&model);
}*/

void QObjectsTest::propertiesTest()
{
    QFETCH(QObject*, object);
    QFETCH(bool, setAgain);

    auto meta= object->metaObject();
    qDebug() << "class:" << meta->className();
    auto it= std::find_if(std::begin(m_ignored), std::end(m_ignored),
                          [object](const IgnoredProperty& pop) { return pop.object == object; });
    QStringList ignoredPop;
    if(it != std::end(m_ignored))
        ignoredPop= it->names;

    auto res= Helper::testAllProperties(object, ignoredPop, setAgain);
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
    QTest::addColumn<bool>("setAgain");

    auto model= new ProfileModel();
    auto profileCtrl= new SelectConnProfileController(model);
    profileCtrl->addProfile();
    profileCtrl->setCurrentProfileIndex(0);
    QMap<QString, QVariant> params;
    auto ctrl= new mindmap::MindItemModel(nullptr);
    std::vector<DiceShortCut> shorCuts;

    // clang-format off
    // QTest::addRow("obj", i++) << new MRichTextEdit()<< true ;
    QTest::addRow("AboutRcse")  << static_cast<QObject*>(new AboutRcse(Helper::randomString()))<< true ;
    QTest::addRow("AboutRolisteam") << static_cast<QObject*>(new AboutRolisteam({})) << true;
    QTest::addRow("ActionDelegate") << static_cast<QObject*>(new ActionDelegate()) << true;
    //QTest::addRow("ActionOnListModel") << static_cast<QObject*>(new ActionOnListModel({}, {}, {})) << true;
    QTest::addRow("AlignmentDelegate")  << static_cast<QObject*>(new AlignmentDelegate())<< true ;
    QTest::addRow("AnchorItem obj") << static_cast<QObject*>(new AnchorItem()) << true;
    QTest::addRow("AntagonistBoard") << static_cast<QObject*>(new campaign::AntagonistBoard(nullptr)) << true; // 200
    QTest::addRow("AntagonistBoardController")        << static_cast<QObject*>(new campaign::AntagonistBoardController(new campaign::CampaignEditor())) << true;
    QTest::addRow("AudioController") << static_cast<QObject*>(new AudioController(nullptr, nullptr)) << true;
    QTest::addRow("AudioPlayer") << static_cast<QObject*>(new AudioPlayer(nullptr)) << true;
    QTest::addRow("AudioPlayerController") << static_cast<QObject*>(new AudioPlayerController(0, "k", nullptr)) << true;
    QTest::addRow("AudioPlayerUpdater") << static_cast<QObject*>(new AudioPlayerUpdater(nullptr, nullptr)) << true;
    QTest::addRow("AutoSaveController") << static_cast<QObject*>(new AutoSaveController(nullptr)) << true;
    QTest::addRow("AvatarDelegate") << static_cast<QObject*>(new AvatarDelegate()) << true;
    QTest::addRow("AvatarProvider") << static_cast<QObject*>(new AvatarProvider(nullptr)) << true;
    QTest::addRow("BackgroundButton") << static_cast<QObject*>(new BackgroundButton(nullptr)) << true;
    QTest::addRow("BorderListEditor")  << static_cast<QObject*>(new BorderListEditor())<< true ;
    QTest::addRow("BusyIndicatorDialog") << static_cast<QObject*>(new BusyIndicatorDialog(Helper::randomString(), Helper::randomString(), Helper::randomString())) << true;
    QTest::addRow("ButtonCanvas")  << static_cast<QObject*>(new ButtonCanvas())<< true ;
    //QTest::addRow("CSItem")  << static_cast<QObject*>(new CSItem(TreeSheetItem::FieldItem))<< true ;
    QTest::addRow("Campaign") << static_cast<QObject*>(new campaign::Campaign()) << true;
    QTest::addRow("CampaignDock") << static_cast<QObject*>(new campaign::CampaignDock(new campaign::CampaignEditor()))                  << true;
    QTest::addRow("CampaignEditor") << static_cast<QObject*>(new campaign::CampaignEditor()) << true;
    QTest::addRow("CampaignIntegrityController") << static_cast<QObject*>(new campaign::CampaignIntegrityController({}, {}, {})) << true;
    QTest::addRow("CampaignIntegrityDialog") << static_cast<QObject*>(new campaign::CampaignIntegrityDialog(new campaign::CampaignIntegrityController({}, {}, {}))) << true;
    QTest::addRow("CampaignManager") << static_cast<QObject*>(new campaign::CampaignManager(nullptr)) << true;
    QTest::addRow("CampaignProperties") << static_cast<QObject*>(new CampaignProperties(new campaign::Campaign(), nullptr))                                        << true;
    QTest::addRow("CampaignUpdater") << static_cast<QObject*>(new campaign::CampaignUpdater(nullptr, new campaign::Campaign()))                                     << true; // 100
    QTest::addRow("CampaignView") << static_cast<QObject*>(new campaign::CampaignView()) << true;
    QTest::addRow("Canvas")  << static_cast<QObject*>(new Canvas(new EditorController(new ImageController())))<< true ;
    QTest::addRow("CanvasField")  << static_cast<QObject*>(new CanvasField(new FieldController(TreeSheetItem::FieldItem, true)))<< true ;
    QTest::addRow("CategoryModel obj") << static_cast<QObject*>(new GMTOOL::CategoryModel()) << true;
    QTest::addRow("CenteredCheckBox") << static_cast<QObject*>(new CenteredCheckBox()) << true;
    QTest::addRow("Channel") << static_cast<QObject*>(new Channel()) << true;
    QTest::addRow("ChannelListPanel") << static_cast<QObject*>(new ChannelListPanel(nullptr, new NetworkController())) << true;
    QTest::addRow("ChannelModel") << static_cast<QObject*>(new ChannelModel()) << true;
    auto character = new Character();
    character->setHealthPointsMax(std::numeric_limits<int>::max());
    character->setHealthPointsMin(std::numeric_limits<int>::min());
    QTest::addRow("Character") << static_cast<QObject*>(character) << true;
    QTest::addRow("CharacterController")  << static_cast<QObject*>(new CharacterController())<< true ;
    QTest::addRow("CharacterDataModel") << static_cast<QObject*>(new CharacterDataModel()) << true;
    QTest::addRow("CharacterItem") << static_cast<QObject*>( new CharacterItem(new vmap::CharacterItemController({}, new VectorialMapController())))                                   << true;
    QTest::addRow("CharacterItemController") << static_cast<QObject*>(        new vmap::CharacterItemController({{Core::vmapkeys::KEY_CHARAC_NPC, false}}, new VectorialMapController()))                                             << true;
    QTest::addRow("CharacterItemUpdater") << static_cast<QObject*>(new CharacterItemUpdater()) << true;
    QTest::addRow("CharacterList")  << static_cast<QObject*>(new CharacterList(new CharacterSheetModel()))<< true ;
    QTest::addRow("CharacterModel") << static_cast<QObject*>(new CharacterModel()) << true;
    QTest::addRow("CharacterSheet")             << static_cast<QObject*>(new CharacterSheet()) << true;
    QTest::addRow("CharacterSheetController") << static_cast<QObject*>(new CharacterSheetController()) << true;
    QTest::addRow("CharacterSheetModel")        << static_cast<QObject*>(new CharacterSheetModel()) << true;
    QTest::addRow("CharacterSheetUpdater") << static_cast<QObject*>(new CharacterSheetUpdater(nullptr,{})) << true;
    QTest::addRow("CharacterSheetWindow") << static_cast<QObject*>(new CharacterSheetWindow(nullptr)) << true;
    QTest::addRow("CharacterStateModel") << static_cast<QObject*>(new CharacterStateModel()) << true;
    QTest::addRow("CharacterVision") << static_cast<QObject*>(new CharacterVision()) << true;
    QTest::addRow("ChatRoom") << static_cast<QObject*>(        new InstantMessaging::ChatRoom(nullptr, InstantMessaging::ChatRoom::GLOBAL))                              << true;
    QTest::addRow("ChatroomSplitterModel")        << static_cast<QObject*>(new InstantMessaging::ChatroomSplitterModel()) << true;
    QTest::addRow("CheckBoxDelegate") << static_cast<QObject*>(new rwidgets::CheckBoxDelegate()) << true;
    QTest::addRow("CheckedLabel") << static_cast<QObject*>(new CheckedLabel()) << true;
    QTest::addRow("ChildPointItem obj") << static_cast<QObject*>(new ChildPointItem(nullptr, 0, nullptr)) << true;
    QTest::addRow("CircleDisplayer") << static_cast<QObject*>(new CircleDisplayer()) << true;
    QTest::addRow("CleverURI") << static_cast<QObject*>(new CleverURI(Core::ContentType::UNKNOWN)) << true;
    QTest::addRow("CleverUriMimeData") << static_cast<QObject*>(new CleverUriMimeData()) << true;
    QTest::addRow("ClientConnection") << static_cast<QObject*>(new ClientConnection()) << true;
    QTest::addRow("ClientManager") << static_cast<QObject*>(new ClientManager()) << true;
    QTest::addRow("ClientMimeData") << static_cast<QObject*>(new ClientMimeData()) << true;
    QTest::addRow("CodeEdit")  << static_cast<QObject*>(new CodeEdit(new FieldController(TreeSheetItem::FieldItem, true)))<< true ;
    QTest::addRow("CodeEditor") << static_cast<QObject*>(new CodeEditor(nullptr)) << true;
    QTest::addRow("CodeEditorDialog")  << static_cast<QObject*>(new CodeEditorDialog())<< true ;
    QTest::addRow("ColorButton") << static_cast<QObject*>(new ColorButton()) << true;
    QTest::addRow("ColorDelegate") << static_cast<QObject*>(new rwidgets::ColorDelegate()) << true;
    QTest::addRow("ColorListEditor") << static_cast<QObject*>(new rwidgets::ColorListEditor()) << true;
    QTest::addRow("ColorModel")  << static_cast<QObject*>(new ColorModel())<< true ;
    QTest::addRow("ColorTable") << static_cast<QObject*>(new ColorTable()) << true;
    QTest::addRow("ColorTableChooser") << static_cast<QObject*>(new ColorTableChooser()) << true;
    QTest::addRow("ColumnDefinitionDialog")  << static_cast<QObject*>(new ColumnDefinitionDialog())<< true ;
    QTest::addRow("CommandMessage") << static_cast<QObject*>(new InstantMessaging::CommandMessage({}, {}, {})) << true;
    QTest::addRow("ConnectionProfile") << static_cast<QObject*>(new ConnectionProfile()) << true;
    QTest::addRow("ConnectionRetryDialog") << static_cast<QObject*>(new ConnectionRetryDialog()) << true;

    auto contentCtrl = static_cast<QObject*>(new ContentController(nullptr, nullptr, nullptr, nullptr));
    m_ignored.append({contentCtrl, {"mediaRoot"}});
    QTest::addRow("ContentController") <<  contentCtrl  << true;
    QTest::addRow("ContentModel") << static_cast<QObject*>(new ContentModel()) << true;
    QTest::addRow("Convertor obj") << static_cast<QObject*>(new GMTOOL::Convertor()) << true;
    QTest::addRow("CountDownObject") << static_cast<QObject*>(new CountDownObject(20, 1000)) << true;
    QTest::addRow("CustomRuleModel obj") << static_cast<QObject*>(new GMTOOL::CustomRuleModel()) << true;
    QTest::addRow("Delegate")  << static_cast<QObject*>(new Delegate())<< true ;
    QTest::addRow("Delegate") << static_cast<QObject*>(new Delegate()) << true;
    QTest::addRow("DiameterSelector") << static_cast<QObject*>(new DiameterSelector()) << true;
    QTest::addRow("DiceAliasModel") << static_cast<QObject*>(new DiceAliasModel()) << true;
    QTest::addRow("DiceBookMarkModel") << static_cast<QObject*>(new DiceBookMarkModel(shorCuts)) << true;
    QTest::addRow("DiceBookMarkWidget obj") << static_cast<QObject*>(new DiceBookMarkWidget(shorCuts)) << true;
    QTest::addRow("DiceMessage") << static_cast<QObject*>(new InstantMessaging::DiceMessage({}, {}, {})) << true;
    QTest::addRow("DiceRoller") << static_cast<QObject*>(new DiceRoller()) << true;
    QTest::addRow("Document") << static_cast<QObject*>(new Document(nullptr)) << true;
    QTest::addRow("DragableTableView") << static_cast<QObject*>(new DragableTableView()) << true;
    QTest::addRow("EditorController")  << static_cast<QObject*>(new EditorController(new ImageController))<< true ;
    QTest::addRow("EllipsItem obj") << static_cast<QObject*>(new EllipsItem(nullptr)) << true;
    QTest::addRow("EllipseController") << static_cast<QObject*>(        new vmap::EllipseController({}, new VectorialMapController()))                                       << true;
    QTest::addRow("EllipseControllerUpdater") << static_cast<QObject*>(new EllipseControllerUpdater()) << true;
    QTest::addRow("ErrorMessage") << static_cast<QObject*>(new InstantMessaging::ErrorMessage({}, {}, {})) << true;
    QTest::addRow("FieldController")            << static_cast<QObject*>(new FieldController(TreeSheetItem::FieldItem, true))    << true;
    QTest::addRow("FieldModel")  << static_cast<QObject*>(new FieldModel())<< true ;
    QTest::addRow("FieldView")  << static_cast<QObject*>(new FieldView())<< true ;
    QTest::addRow("FileDirChooser") << static_cast<QObject*>(new FileDirChooser()) << true;
    QTest::addRow("FileSerializer") << static_cast<QObject*>(new mindmap::FileSerializer()) << true;
    QTest::addRow("FilterInstantMessagingModel")  << static_cast<QObject*>(new InstantMessaging::FilterInstantMessagingModel()) << true;
    QTest::addRow("FilteredCharacterModel") << static_cast<QObject*>(new campaign::FilteredCharacterModel()) << true;
    QTest::addRow("FilteredContentModel")        << static_cast<QObject*>(new FilteredContentModel(Core::ContentType::UNKNOWN)) << true;
    QTest::addRow("FilteredModel")  << static_cast<QObject*>(new mindmap::FilteredModel())<< true ;
    QTest::addRow("FilteredPlayerModel") << static_cast<QObject*>(new InstantMessaging::FilteredPlayerModel({}))   << true;
    QTest::addRow("FindDialog") << static_cast<QObject*>(new FindDialog()) << true;
    QTest::addRow("FindToolBar") << static_cast<QObject*>(new FindToolBar()) << true;
    QTest::addRow("FlowLayout") << static_cast<QObject*>(new FlowLayout(nullptr)) << true;
    QTest::addRow("GameController") << static_cast<QObject*>(new GameController("test", "1.2", nullptr)) << true;
    QTest::addRow("GenericModel")  << static_cast<QObject*>(new GenericModel({Helper::randomString(),Helper::randomString(),Helper::randomString()}))<< true ;
    QTest::addRow("GenericModel") << static_cast<QObject*>(new GenericModel({})) << true;
    QTest::addRow("GenericUpdater") << static_cast<QObject*>(new GenericUpdater(nullptr)) << true;
    QTest::addRow("Gloader")  << static_cast<QObject*>(new Gloader())<< true ;
    QTest::addRow("GridController") << static_cast<QObject*>(new vmap::GridController(new VectorialMapController()))  << true;
    QTest::addRow("GridItem obj") << static_cast<QObject*>(new GridItem(nullptr)) << true;
    QTest::addRow("HeartBeatSender")  << static_cast<QObject*>(new HeartBeatSender())<< true ;
    QTest::addRow("HiddingButton")  << static_cast<QObject*>(new HiddingButton(nullptr))<< true ;
    QTest::addRow("HighlighterItem obj") << static_cast<QObject*>(new HighlighterItem(nullptr, {}, 3, Qt::red, nullptr, false))                                         << true;
    QTest::addRow("HistoryModel") << static_cast<QObject*>(new history::HistoryModel()) << true;
    QTest::addRow("HistoryViewerDialog") << static_cast<QObject*>(new HistoryViewerDialog(nullptr)) << true;
    QTest::addRow("Image") << static_cast<QObject*>(new Image(new ImageController())) << true;
    QTest::addRow("ImageController") << static_cast<QObject*>(new ImageController()) << true;
    QTest::addRow("ImageControllerUpdater") << static_cast<QObject*>(new ImageControllerUpdater()) << true;
    QTest::addRow("ImageItem obj") << static_cast<QObject*>(new ImageItem(nullptr)) << true;
    QTest::addRow("ImageItemController") << static_cast<QObject*>(        new vmap::ImageItemController({}, new VectorialMapController()))                                         << true;
    QTest::addRow("ImageModel") << static_cast<QObject*>(new charactersheet::ImageModel()) << true;
    QTest::addRow("ImagePathDelegateItem") << static_cast<QObject*>(new rwidgets::ImagePathDelegateItem({})) << true;
    QTest::addRow("ImagePathEditor") << static_cast<QObject*>(new rwidgets::ImagePathEditor({})) << true;
    QTest::addRow("ImageSelector") << static_cast<QObject*>(new ImageSelector()) << true;
    QTest::addRow("ImageSelectorController") << static_cast<QObject*>(new ImageSelectorController()) << true;
    QTest::addRow("ImageSelectorDialog") << static_cast<QObject*>(new ImageSelectorDialog(new ImageSelectorController())) << true;
    QTest::addRow("ImportDataFromCampaignDialog")    << static_cast<QObject*>(new ImportDataFromCampaignDialog({})) << true;
    QTest::addRow("InstantMessagerManager") << static_cast<QObject*>(new InstantMessagerManager()) << true;
    QTest::addRow("InstantMessaging") << static_cast<QObject*>(new InstantMessaging::InstantMessagingModel(nullptr, nullptr)) << true;
    QTest::addRow("InstantMessagingController")        << static_cast<QObject*>(new InstantMessagingController(nullptr, nullptr)) << true;
    QTest::addRow("InstantMessagingModel")  << static_cast<QObject*>(new InstantMessaging::InstantMessagingModel(new DiceRoller,new PlayerModel()))<< true ;
    QTest::addRow("InstantMessagingUpdater")        << static_cast<QObject*>(new InstantMessaging::InstantMessagingUpdater()) << true;
    QTest::addRow("InstantMessagingView")  << static_cast<QObject*>(new InstantMessagingView(new InstantMessagingController(new DiceRoller, new PlayerModel()))) << true ;
    QTest::addRow("IpChecker") << static_cast<QObject*>(new IpChecker()) << true;
    QTest::addRow("ItemEditor")  << static_cast<QObject*>(new ItemEditor())<< true ;
    QTest::addRow("KeyGeneratorDialog") << static_cast<QObject*>(new KeyGeneratorDialog()) << true;
    QTest::addRow("LabelWithOptions") << static_cast<QObject*>(new LabelWithOptions()) << true;
    QTest::addRow("LanguageModel") << static_cast<QObject*>(new LanguageModel()) << true;
    QTest::addRow("LineController") << static_cast<QObject*>(new vmap::LineController({}, new VectorialMapController())) << true;
    QTest::addRow("LineControllerUpdater") << static_cast<QObject*>(new LineControllerUpdater()) << true;
    //QTest::addRow("LineFieldItem")              << static_cast<QObject*>(new LineFieldItem()) << true;
    QTest::addRow("LineItem obj") << static_cast<QObject*>(new LineItem(nullptr)) << true;
    QTest::addRow("TableModel")                  << static_cast<QObject*>(new TableModel(nullptr)) << true;
    QTest::addRow("LinkController") << static_cast<QObject*>(new mindmap::LinkController()) << true;
    QTest::addRow("LinkItem") << static_cast<QObject*>(new mindmap::LinkItem()) << true;
    QTest::addRow("LocalPersonModel")  << static_cast<QObject*>(new LocalPersonModel())<< true ;
    QTest::addRow("LocalPersonModel") << static_cast<QObject*>(new LocalPersonModel()) << true;
    QTest::addRow("LogController")              << static_cast<QObject*>(new LogController(true)) << true;
    QTest::addRow("LogPanel")                   << static_cast<QObject*>(new LogPanel()) << true;
    QTest::addRow("LogUploader")                << static_cast<QObject*>(new LogUploader()) << true;
    QTest::addRow("MRichTextEdit")  << static_cast<QObject*>(new MRichTextEdit())<< true ;
    QTest::addRow("MTextEdit") << static_cast<QObject*>(new MTextEdit()) << true;
    QTest::addRow("MainController")  << static_cast<QObject*>(new MainController())<< true ;
    QTest::addRow("RCSE::MainController")  << static_cast<QObject*>(new rcse::MainController())<< true ;
    QTest::addRow("rcse::MainWindow")  << static_cast<QObject*>(new rcse::MainWindow())<< true ;
    //QTest::addRow("MainWindow")  << static_cast<QObject*>(new MainWindow(new GameController(Helper::randomString(),Helper::randomString(), nullptr), {}))<< true ;
    QTest::addRow("MapWizzardDialog") << static_cast<QObject*>(new MapWizzardDialog(nullptr)) << true;
    QTest::addRow("MappingHelper")  << static_cast<QObject*>(new utils::MappingHelper())<< true ;
    QTest::addRow("MarkDownHighlighter") << static_cast<QObject*>(new MarkDownHighlighter()) << true;
    QTest::addRow("Media") << static_cast<QObject*>(new campaign::Media({}, {}, {}, Core::MediaType::AudioFile)) << true;
    QTest::addRow("MediaContainer") << static_cast<QObject*>(new MediaContainer(new MediaControllerBase("uuid", Core::ContentType::UNKNOWN), MediaContainer::ContainerType::NoteContainer))                << true;
    QTest::addRow("MediaControllerBase")  << static_cast<QObject*>(new MediaControllerBase(Helper::randomString(), Core::ContentType::NOTES))<< true ;
    QTest::addRow("MediaFilteredModel") << static_cast<QObject*>(new MediaFilteredModel()) << true;
    QTest::addRow("MediaModel")  << static_cast<QObject*>(new campaign::MediaModel(new campaign::Campaign))<< true ;
    QTest::addRow("MediaNode") << static_cast<QObject*>(new campaign::MediaNode(campaign::MediaNode::File, {})) << true;
    QTest::addRow("MessageDispatcher") << static_cast<QObject*>(new MessageDispatcher()) << true;
    QTest::addRow("MessageModel") << static_cast<QObject*>(new InstantMessaging::MessageModel(nullptr)) << true;
    QTest::addRow("MindItem")  << static_cast<QObject*>(new mindmap::MindItem(mindmap::MindItem::NodeType))<< true ;
    QTest::addRow("MindItemModel")  << static_cast<QObject*>(new mindmap::MindItemModel(new mindmap::ImageModel))<< true ;
    QTest::addRow("MindMapController") << static_cast<QObject*>(new MindMapController("idxxxxx")) << true;
    QTest::addRow("MindMapControllerBase")  << static_cast<QObject*>(new mindmap::MindMapControllerBase(false, Helper::randomString()))<< true ;
    QTest::addRow("MindMapUpdater") << static_cast<QObject*>(new MindMapUpdater(nullptr, nullptr)) << true;
    QTest::addRow("MindMapView") << static_cast<QObject*>(new MindMapView(nullptr)) << true;
    QTest::addRow("MindNode") << static_cast<QObject*>(new mindmap::MindNode()) << true;
    QTest::addRow("MindmapManager") << static_cast<QObject*>(new MindmapManager()) << true;
    QTest::addRow("MockCharacter")  << static_cast<QObject*>(new MockCharacter())<< true ;
    QTest::addRow("MusicModel") << static_cast<QObject*>(new MusicModel(new PreferencesManager("aad","blbl"))) << true;
    QTest::addRow("NameGeneratorWidget obj") << static_cast<QObject*>(new NameGeneratorWidget()) << true;
    QTest::addRow("NetworkController") << static_cast<QObject*>(new NetworkController()) << true;
    QTest::addRow("NetworkDownloader") << static_cast<QObject*>(new NetworkDownloader({})) << true;
    QTest::addRow("NewFileDialog") << static_cast<QObject*>(new NewFileDialog(Core::ContentType::VECTORIALMAP)) << true;
    QTest::addRow("NodeImageProvider") << static_cast<QObject*>(new mindmap::NodeImageProvider(nullptr)) << true;
    QTest::addRow("NodeStyle") << static_cast<QObject*>(new mindmap::NodeStyle()) << true;
    QTest::addRow("NodeStyleModel") << static_cast<QObject*>(new mindmap::NodeStyleModel()) << true;
    QTest::addRow("NonPlayableCharacter") << static_cast<QObject*>(new campaign::NonPlayableCharacter()) << true;
    QTest::addRow("NonPlayableCharacterModel")<< static_cast<QObject*>(new campaign::NonPlayableCharacterModel(nullptr)) << true;
    QTest::addRow("NoteContainer")  << static_cast<QObject*>(new NoteContainer(new NoteController()))<< true ;
    QTest::addRow("NoteController") << static_cast<QObject*>(new NoteController()) << true;
    QTest::addRow("NotificationZone") << static_cast<QObject*>(new NotificationZone(new LogController(true))) << true;
    QTest::addRow("OOFormat")  << static_cast<QObject*>(new OOFormat())<< true ;
    QTest::addRow("OOReader")  << static_cast<QObject*>(new OOReader())<< true ;
    QTest::addRow("Overlay") << static_cast<QObject*>(new Overlay()) << true;
    QTest::addRow("PaletteModel") << static_cast<QObject*>(new PaletteModel()) << true;
    QTest::addRow("ParticipantModel") << static_cast<QObject*>(new ParticipantModel({}, nullptr)) << true;
    QTest::addRow("ParticipantsModel") << static_cast<QObject*>(new ParticipantsModel()) << true;
    QTest::addRow("ParticipantsPane") << static_cast<QObject*>(new ParticipantsPane(nullptr)) << true;
    QTest::addRow("PathController") << static_cast<QObject*>(new vmap::PathController({}, new VectorialMapController()))                                    << true;
    QTest::addRow("PathControllerUpdater") << static_cast<QObject*>(new PathControllerUpdater()) << true;
    QTest::addRow("PathItem") << static_cast<QObject*>(new PathItem(nullptr)) << true;
    QTest::addRow("PatternModel") << static_cast<QObject*>(new PatternModel()) << true;
    QTest::addRow("PdfController") << static_cast<QObject*>(new PdfController()) << true;
    QTest::addRow("PdfManager")  << static_cast<QObject*>(new PdfManager())<< true ;
    QTest::addRow("PdfViewer") << static_cast<QObject*>(new PdfViewer(nullptr)) << true;
    QTest::addRow("PersonDialog") << static_cast<QObject*>(new PersonDialog()) << true;
    QTest::addRow("Player") << static_cast<QObject*>(new Player()) << true;
    QTest::addRow("PlayerController") << static_cast<QObject*>(new PlayerController(nullptr)) << true;
    QTest::addRow("PlayerController2") << static_cast<QObject*>(new PlayerController(new NetworkController)) << true;
    QTest::addRow("PlayerModel") << static_cast<QObject*>(new PlayerModel()) << true;
    QTest::addRow("PlayerProxyModel") << static_cast<QObject*>(new PlayerProxyModel()) << true;
    QTest::addRow("PlayerWidget") << static_cast<QObject*>(new PlayerWidget(nullptr)) << true;
    QTest::addRow("PlayersPanel") << static_cast<QObject*>(new PlayersPanel(nullptr, nullptr)) << true;
    QTest::addRow("PositionedItem")  << static_cast<QObject*>(new mindmap::PositionedItem(mindmap::MindItem::LinkType))<< true ;
    QTest::addRow("PreferencesController") << static_cast<QObject*>(new PreferencesController()) << false;
    QTest::addRow("PreferencesDialog") << static_cast<QObject*>(new PreferencesDialog(new PreferencesController()))                                       << true;
    QTest::addRow("PreferencesManager") << static_cast<QObject*>(new PreferencesManager("test_rolisteam", "test"))                                        << true;
    QTest::addRow("PreventClosing")  << static_cast<QObject*>(new PreventClosing(nullptr))<< true ;
    QTest::addRow("ProfileModel") << static_cast<QObject*>(new ProfileModel()) << true;
    QTest::addRow("ProxyStateModel")  << static_cast<QObject*>(new ProxyStateModel(new CharacterStateModel()))<< true ;
    QTest::addRow("QmlGeneratorController")  << static_cast<QObject*>(new QmlGeneratorController())<< true ;
    QTest::addRow("QmlHighlighter")  << static_cast<QObject*>(new QmlHighlighter())<< true ;
    QTest::addRow("RGraphicsView") << static_cast<QObject*>(new RGraphicsView(nullptr)) << true;

    auto server = static_cast<QObject*>(new RServer(params, true));
    m_ignored.append({server, {"port"}});
    QTest::addRow("RServer") << server << true;
    QTest::addRow("RcseApplicationController")  << static_cast<QObject*>(new RcseApplicationController())<< true ;
    QTest::addRow("RealSlider") << static_cast<QObject*>(new RealSlider()) << true;
    QTest::addRow("RectController") << static_cast<QObject*>(new vmap::RectController({}, new VectorialMapController()))                                    << true;
    QTest::addRow("RectControllerUpdater") << static_cast<QObject*>(new RectControllerUpdater()) << true;
    QTest::addRow("RectItem") << static_cast<QObject*>(new RectItem(nullptr)) << true;
    QTest::addRow("RemoteLogController")        << static_cast<QObject*>(new RemoteLogController()) << true;
    QTest::addRow("RemotePlayerModel") << static_cast<QObject*>(new RemotePlayerModel(nullptr)) << true;
    QTest::addRow("RichTextEditDialog") << static_cast<QObject*>(new RichTextEditDialog()) << true;
    QTest::addRow("RichTextEditDialog") << static_cast<QObject*>(new RichTextEditDialog()) << true;
    char param[12]={"tst_qobject"};
    char* argv[1]={param};
    int count = 1;
    QTest::addRow("RolisteamDaemon")  << static_cast<QObject*>(new RolisteamDaemon())<< true ;
    QTest::addRow("RolisteamImageProvider")     << static_cast<QObject*>(new RolisteamImageProvider(new charactersheet::ImageModel())) << true;
    QTest::addRow("RolisteamMimeData") << static_cast<QObject*>(new RolisteamMimeData()) << true;
    QTest::addRow("RuleItem") << static_cast<QObject*>(new RuleItem(nullptr)) << true;
    QTest::addRow("SaturationChooser") << static_cast<QObject*>(new SaturationChooser()) << true;
    QTest::addRow("Section")                    << static_cast<QObject*>(new Section()) << true;
    QTest::addRow("SelectConnProfileController") << static_cast<QObject*>(profileCtrl) << true;
    QTest::addRow("SelectConnectionProfileDialog")        << static_cast<QObject*>(new SelectConnectionProfileDialog(nullptr)) << true;
    QTest::addRow("SelectionController") << static_cast<QObject*>(new mindmap::SelectionController()) << true;
    QTest::addRow("ServerConnection") << static_cast<QObject*>(new ServerConnection(nullptr)) << true;
    QTest::addRow("ServerConnectionManager") << static_cast<QObject*>(new ServerConnectionManager(params)) << true;
    QTest::addRow("ServerManagerUpdater")        << static_cast<QObject*>(new ServerManagerUpdater(new ServerConnectionManager(params), true)) << true;
    QTest::addRow("ServerManagerUpdater") << static_cast<QObject*>(new ServerManagerUpdater(nullptr, true)) << true;
    QTest::addRow("SessionItemModel") << static_cast<QObject*>(new session::SessionItemModel()) << true;
    QTest::addRow("SharedNote") << static_cast<QObject*>(new SharedNote(nullptr)) << true;
    QTest::addRow("SharedNoteContainer") << static_cast<QObject*>(new SharedNoteContainer(nullptr)) << true;
    QTest::addRow("SharedNoteController") << static_cast<QObject*>(new SharedNoteController()) << true;
    QTest::addRow("SharedNoteControllerUpdater")        << static_cast<QObject*>(new SharedNoteControllerUpdater(nullptr, nullptr)) << true;
    auto sheetCtrl = new SheetController();
    sheetCtrl->setAppCtrl(new RcseApplicationController());
    m_ignored.append({sheetCtrl, {"currentPage"}});
    QTest::addRow("SheetController")  << static_cast<QObject*>(sheetCtrl)<< true ;
    QTest::addRow("SheetProperties")  << static_cast<QObject*>(new SheetProperties(new QmlGeneratorController()))<< true ;
    QTest::addRow("SheetWidget")                << static_cast<QObject*>(new SheetWidget(nullptr, nullptr)) << true;
    QTest::addRow("ShortCutEditorDialog") << static_cast<QObject*>(new ShortCutEditorDialog()) << true;
    QTest::addRow("ShortCutModel") << static_cast<QObject*>(new ShortCutModel()) << true;
    QTest::addRow("ShortcutVisitor") << static_cast<QObject*>(new ShortcutVisitor()) << true;
    auto sideMenuController = new mindmap::SideMenuController();
    sideMenuController->setController(new MindMapController(Helper::randomString()));
    QTest::addRow("SideMenuController")  << static_cast<QObject*>(sideMenuController)<< true ;
    QTest::addRow("SightController") << static_cast<QObject*>(new vmap::SightController(new VectorialMapController()))                                     << true;
    QTest::addRow("SightItem") << static_cast<QObject*>(new SightItem(nullptr)) << true;
    QTest::addRow("SingleContentTypeModel")        << static_cast<QObject*>(new SingleContentTypeModel(Core::ContentType::VECTORIALMAP)) << true;
    QTest::addRow("SliderController")  << static_cast<QObject*>(new SliderController())<< true ;
    QTest::addRow("SpacingController") << static_cast<QObject*>(new mindmap::SpacingController(ctrl, nullptr)) << true;
    QTest::addRow("StateDelegate")  << static_cast<QObject*>(new StateDelegate(new CharacterStateModel()))<< true ;
    QTest::addRow("StateModel")  << static_cast<QObject*>(new StateModel())<< true ;
    QTest::addRow("StyleSheet")                 << static_cast<QObject*>(new customization::StyleSheet(nullptr)) << true;
    QTest::addRow("TableCanvasField")  << static_cast<QObject*>(new TableCanvasField(new TableFieldController()))<< true ;
    QTest::addRow("TableField")                 << static_cast<QObject*>(new TableFieldController()) << true;
    QTest::addRow("TagListDelegate") << static_cast<QObject*>(new TagListDelegate()) << true;
    QTest::addRow("TextController") << static_cast<QObject*>(new vmap::TextController({}, new VectorialMapController()))                                    << true;
    QTest::addRow("TextControllerUpdater") << static_cast<QObject*>(new TextControllerUpdater()) << true;
    QTest::addRow("TextEdit")  << static_cast<QObject*>(new TextEdit(new NoteController()))<< true ;
    QTest::addRow("TextItem") << static_cast<QObject*>(new TextItem(nullptr)) << true;
    QTest::addRow("TextLabel") << static_cast<QObject*>(new TextLabel()) << true;
    QTest::addRow("TextLabel") << static_cast<QObject*>(new TextLabel(nullptr)) << true;
    QTest::addRow("TextMessage") << static_cast<QObject*>(new InstantMessaging::TextMessage({}, {}, {})) << true;
    QTest::addRow("TextWriterController")  << static_cast<QObject*>(new InstantMessaging::TextWriterController()) << true;
    QTest::addRow("Theme")                      << static_cast<QObject*>(new customization::Theme()) << true;
    QTest::addRow("ThemeModel") << static_cast<QObject*>(new ThemeModel()) << true;
    QTest::addRow("TipChecker") << static_cast<QObject*>(new TipChecker()) << true;
    QTest::addRow("TipOfDayViewer") << static_cast<QObject*>(new TipOfDayViewer({}, {}, {})) << true;
    QTest::addRow("ToolBox") << static_cast<QObject*>(new ToolBox(nullptr)) << true;
    QTest::addRow("UnitModel obj") << static_cast<QObject*>(new GMTOOL::UnitModel()) << true;
    QTest::addRow("UpdateChecker") << static_cast<QObject*>(new UpdateChecker({})) << true;
    QTest::addRow("UpdaterWindow") << static_cast<QObject*>(new UpdaterWindow()) << true;
    QTest::addRow("UpnpNat")  << static_cast<QObject*>(new UpnpNat())<< true ;
    QTest::addRow("UserListDelegate") << static_cast<QObject*>(new UserListDelegate()) << true;
    QTest::addRow("UserListView") << static_cast<QObject*>(new UserListView()) << true;
    QTest::addRow("VColorLabel") << static_cast<QObject*>(new VColorLabel()) << true;
    QTest::addRow("VColorSelector") << static_cast<QObject*>(new VColorSelector()) << true;
    QTest::addRow("VMap") << static_cast<QObject*>(new VMap(new VectorialMapController())) << true;
    QTest::addRow("VMapFrame") << static_cast<QObject*>(new VMapFrame(nullptr)) << true;
    QTest::addRow("VMapItemControllerUpdater") << static_cast<QObject*>(new VMapItemControllerUpdater()) << true;
    QTest::addRow("VMapUpdater") << static_cast<QObject*>(new VMapUpdater(nullptr, nullptr)) << true;
    QTest::addRow("VectorialMapController") << static_cast<QObject*>(new VectorialMapController()) << true;
    QTest::addRow("VmapItemModel") << static_cast<QObject*>(new vmap::VmapItemModel()) << true;
    QTest::addRow("VmapTopBar") << static_cast<QObject*>(new VmapTopBar(nullptr)) << true;
    QTest::addRow("WebView") << static_cast<QObject*>(new WebView(nullptr)) << true;
    QTest::addRow("WebViewUpdater") << static_cast<QObject*>(new WebViewUpdater(nullptr)) << true;
    QTest::addRow("WebpageController") << static_cast<QObject*>(new WebpageController()) << true;
    QTest::addRow("Workspace") << static_cast<QObject*>(new Workspace(nullptr, nullptr, nullptr, nullptr)) << true;
    QTest::addRow("XMLTextEdit")  << static_cast<QObject*>(new XMLTextEdit())<< true ;
    //QTest::addRow("RolisteamApplication")  << static_cast<QObject*>(new RolisteamApplication(Helper::randomString(),Helper::randomString(), count, argv))<< true ;
    // clang-format off
}

QTEST_MAIN(QObjectsTest)

#include "tst_qobjects.moc"
