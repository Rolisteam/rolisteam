#include <QTest>
#include <QObject>
#include <QVariant>
#include <QMetaType>
#include "testhelper.h"

#include "campaignintegritydialog.h"
#include "persondialog.h"
#include "vmapwizzarddialog.h"
#include "instantmessagingview.h"
#include "linecontroller.h"
#include "networkdownloader.h"
#include "notificationzone.h"
#include "textitem.h"
#include "chatroom.h"
#include "sessionitemmodel.h"
#include "gridcontroller.h"
#include "audiocontroller.h"
#include "campaigneditor.h"
#include "mediacontainer.h"
#include "maincontroller.h"
#include "antagonistboardcontroller.h"
#include "image_selector.h"
#include "logcontroller.h"
#include "rcseapplicationcontroller.h"
#include "characteritemupdater.h"
#include "finddialog.h"
#include "selectioncontroller.h"
#include "shortcutvisitor.h"
#include "webpagemediacontroller.h"
#include "localpersonmodel.h"
#include "campaigndock.h"
#include "resourcescontroller.h"
#include "section.h"
#include "rserver.h"
#include "charactersheetitem.h"
#include "characteritem.h"
#include "sidemenucontroller.h"
#include "dragabletableview.h"
#include "newfiledialog.h"
#include "character.h"
#include "unitmodel.h"
#include "borderlisteditor.h"
#include "charactercontroller.h"
#include "dbusadaptor.h"
#include "characterdatamodel.h"
#include "qmlhighlighter.h"
#include "characterlist.h"
#include "controllerinterface.h"
#include "mindnode.h"
#include "contentcontroller.h"
#include "playerwidget.h"
#include "itemeditor.h"
#include "pdfmanager.h"
#include "commandmessage.h"
#include "imagemediacontroller.h"
#include "realslider.h"
#include "charactersheet.h"
#include "canvas.h"
#include "sightitem.h"
#include "charactersheetupdater.h"
#include "clientmanager.h"
#include "abstractapplicationcontroller.h"
#include "genericmodel.h"
#include "colorlisteditor.h"
#include "sharednotecontroller.h"
#include "sharednotecontainer.h"
#include "diceserver.h"
#include "upnpnat.h"
#include "codeeditordialog.h"
#include "playerspanel.h"
#include "campaignview.h"
#include "audioplayercontroller.h"
#include "ruleitem.h"
#include "linkcontroller.h"
#include "nodestyle.h"
#include "fileserializer.h"
#include "instantmessagingcontroller.h"
#include "clientconnection.h"
#include "instantmessagingmodel.h"
#include "diceroller.h"
#include "nodestylemodel.h"
#include "mediaupdaterinterface.h"
#include "campaignmanager.h"
#include "visualitemcontroller.h"
#include "webviewupdater.h"
#include "columndefinitiondialog.h"
#include "importdatafromcampaigndialog.h"
#include "mainwindow.h"
#include "csitem.h"
#include "charactersheetcontroller.h"
#include "campaignproperties.h"
#include "antagonistboard.h"
#include "qmlgeneratorcontroller.h"
#include "convertor.h"
#include "textwritercontroller.h"
#include "charactervision.h"
#include "sharednotemediacontroller.h"
#include "preferencescontroller.h"
#include "charactermodel.h"
#include "channellistpanel.h"
#include "statemodel.h"
#include "charactersheetmediacontroller.h"
#include "chatroomsplittermodel.h"
#include "vmap.h"
#include "updatechecker.h"
#include "userlistview.h"
#include "mrichtextedit.h"
#include "characterstatemodel.h"
#include "fieldview.h"
#include "rolisteammimedata.h"
#include "document.h"
#include "slidercontroller.h"
#include "linkitem.h"
#include "nodeitem.h"
#include "vmaptoolbar.h"
#include "campaignupdater.h"
#include "codeeditor.h"
#include "filedirchooser.h"
#include "griditem.h"
#include "vcolorselector.h"
#include "selectconnectionprofiledialog.h"
#include "campaign.h"
#include "participantsmodel.h"
#include "media.h"
#include "webview.h"
#include "historymodel.h"
#include "filterinstantmessagingmodel.h"
#include "OOReader.h"
#include "characteritemcontroller.h"
#include "playermodel.h"
#include "sightcontroller.h"
#include "shortcuteditordialog.h"
#include "imageselectordialog.h"
#include "participantmodel.h"
#include "serverconnectionmanager.h"
#include "remotelogcontroller.h"
#include "ellipsecontroller.h"
#include "sheetcontroller.h"
#include "editorcontroller.h"
#include "pathcontroller.h"
#include "centeredcheckbox.h"
#include "imagepathdelegateitem.h"
#include "pathcontrollerupdater.h"
#include "imageitem.h"
#include "networkcontroller.h"
#include "childpointitem.h"
#include "heartbeatsender.h"
#include "notecontainer.h"
#include "canvasfield.h"
#include "mediamanagerbase.h"
#include "sharednote.h"
#include "rolisteamapplication.h"
#include "tipchecker.h"
#include "languagemodel.h"
#include "channel.h"
#include "positioneditem.h"
#include "mediafilteredmodel.h"
#include "playerproxymodel.h"
#include "theme.h"
#include "addnodecommand.h"
#include "audioPlayer.h"
#include "mindmapupdater.h"
#include "nonplayablecharactermodel.h"
#include "pdfviewer.h"
#include "filteredplayermodel.h"
#include "textcontrollerupdater.h"
#include "countdownobject.h"
#include "messageinterface.h"
#include "diameterselector.h"
#include "gamecontroller.h"
#include "vectorialmapcontroller.h"
#include "rgraphicsview.h"
#include "notemediacontroller.h"
#include "fieldmodel.h"
#include "vmapframe.h"
#include "mtextedit.h"
#include "logpanel.h"
#include "selectconnprofilecontroller.h"
#include "audioplayerupdater.h"
#include "imagemodel.h"
#include "player.h"
#include "overlay.h"
#include "highlighteritem.h"
#include "preferencesmanager.h"
#include "notecontroller.h"
#include "musicmodel.h"
#include "shortcutmodel.h"
#include "servermanagerupdater.h"
#include "dicealiasmodel.h"
#include "image.h"
#include "sheetproperties.h"
#include "treeitem.h"
#include "mindmapcontroller.h"
#include "mediamodel.h"
#include "minditemmodel.h"
#include "serverconnection.h"
#include "messagemodel.h"
#include "webpagecontroller.h"
#include "imagecontroller.h"
#include "namegeneratorwidget.h"
#include "mappinghelper.h"
#include "genericupdater.h"
#include "spacingcontroller.h"
#include "aboutrcse.h"
#include "participantspane.h"
#include "resourcesnode.h"
#include "palettemodel.h"
#include "tablefield.h"
#include "filteredcharactermodel.h"
#include "sharednotecontrollerupdater.h"
#include "connectionretrydialog.h"
#include "preferencesdialog.h"
#include "colormodel.h"
#include "dicebookmarkmodel.h"
#include "markdownhighlighter.h"
#include "instantmessagingupdater.h"
#include "checkedlabel.h"
#include "textmessage.h"
#include "messagedispatcher.h"
#include "vcolortablechooser.h"
#include "playercontroller.h"
#include "tipofdayviewer.h"
#include "boxmodel.h"
#include "delegate.h"
#include "mockcharacter.h"
#include "receiveevent.h"
#include "busyindicatordialog.h"
#include "linkmodel.h"
#include "errormessage.h"
#include "visualitem.h"
#include "actiononlistmodel.h"
#include "aboutrolisteam.h"
#include "textedit.h"
#include "remoteplayermodel.h"
#include "dicemessage.h"
#include "pdfcontroller.h"
#include "colordelegate.h"
#include "vmapitemcontrollerupdater.h"
#include "ellipsecontrollerupdater.h"
#include "cleverurimimedata.h"
#include "contentmodel.h"
#include "actiondelegate.h"
#include "linecontrollerupdater.h"
#include "circledisplayer.h"
#include "rectitem.h"
#include "tablecanvasfield.h"
#include "anchoritem.h"
#include "historyviewerdialog.h"
#include "cleveruri.h"
#include "charactersheetmodel.h"
#include "mindmapcontrollerbase.h"
#include "uploadlogtoserver.h"
#include "imageselectorcontroller.h"
#include "proxystatemodel.h"
#include "updaterwindow.h"
#include "ipchecker.h"
#include "OOFormat.h"
#include "XML_Editor.h"
#include "workspace.h"
#include "vmapitemmodel.h"
#include "mediacontrollerbase.h"
#include "commandmodel.h"
#include "imagepatheditor.h"
#include "channelmodel.h"
#include "autosavecontroller.h"
#include "sheetwidget.h"
#include "codeedit.h"
#include "textcontroller.h"
#include "charactersheetwindow.h"
#include "alignmentdelegate.h"
#include "person.h"
#include "vmapupdater.h"
#include "pathitem.h"
#include "thememodel.h"
#include "fieldcontroller.h"
#include "packagenode.h"
#include "customrulemodel.h"
#include "keygeneratordialog.h"
#include "statedelegate.h"
#include "findtoolbar.h"
#include "patternmodel.h"
#include "minditem.h"
#include "colorbutton.h"
#include "taglistdelegate.h"
#include "rectcontrollerupdater.h"
#include "pdfmediacontroller.h"
#include "profilemodel.h"
#include "rectcontroller.h"
#include "vtoolbar.h"
#include "dicebookmarkwidget.h"
#include "checkboxdelegate.h"
#include "imageitemcontroller.h"
#include "rolisteamdaemon.h"
#include "vectorialmapmediacontroller.h"
#include "link.h"
#include "connectionprofile.h"
#include "mindmapview.h"

std::function<QVariantList(QMetaType type, QVariant currentValue)> testhelper::extraConvertorFunction;

class QObjectsTest : public QObject
{
    Q_OBJECT
public:
   QObjectsTest();
private slots:
    void metaobjectTest();
    void metaobjectTest_data();
};

QObjectsTest::QObjectsTest()
{
    testhelper::extraConvertorFunction = std::function<QVariantList(QMetaType type, QVariant currentValue)>([](QMetaType type, QVariant currentValue) -> QVariantList{
        qDebug() << "Please provide values for type" << type;
        return {}
;    });
}
void QObjectsTest::metaobjectTest()
{
     QFETCH(QObject*, object);
     auto res= testhelper::inspectQObjectRecursively(object);

    Q_ASSERT(!res.isEmpty())
;    for(auto& pair : res)
    {
        if(!pair.second.isEmpty())
            QVERIFY2(pair.first, pair.second.join(";").toStdString().c_str());
        for(const auto& f : pair.second)
        {
            qDebug() << "Property named" << f << "cannot be tested automatically:" << f;
        }
    }
    object->deleteLater();
}
void QObjectsTest::metaobjectTest_data(){
    QTest::addColumn<QObject*>("object");

    QTest::addRow("CampaignIntegrityDialog")  << static_cast<QObject*>(new CampaignIntegrityDialog());
    QTest::addRow("PersonDialog")  << static_cast<QObject*>(new PersonDialog());
    QTest::addRow("MapWizzardDialog")  << static_cast<QObject*>(new MapWizzardDialog());
    QTest::addRow("InstantMessagerManager")  << static_cast<QObject*>(new InstantMessagerManager());
    QTest::addRow("InstantMessagingView")  << static_cast<QObject*>(new InstantMessagingView());
    QTest::addRow("LineController")  << static_cast<QObject*>(new LineController());
    QTest::addRow("NetworkDownloader")  << static_cast<QObject*>(new NetworkDownloader());
    QTest::addRow("NotificationZone")  << static_cast<QObject*>(new NotificationZone());
    QTest::addRow("TextLabel")  << static_cast<QObject*>(new TextLabel());
    QTest::addRow("TextItem")  << static_cast<QObject*>(new TextItem());
    QTest::addRow("ChatRoom")  << static_cast<QObject*>(new ChatRoom());
    QTest::addRow("SessionItemModel")  << static_cast<QObject*>(new SessionItemModel());
    QTest::addRow("GridController")  << static_cast<QObject*>(new GridController());
    QTest::addRow("AudioController")  << static_cast<QObject*>(new AudioController());
    QTest::addRow("CampaignEditor")  << static_cast<QObject*>(new CampaignEditor());
    QTest::addRow("MediaContainer")  << static_cast<QObject*>(new MediaContainer());
    QTest::addRow("MainController")  << static_cast<QObject*>(new MainController());
    QTest::addRow("AntagonistBoardController")  << static_cast<QObject*>(new AntagonistBoardController());
    QTest::addRow("ImageSelector")  << static_cast<QObject*>(new ImageSelector());
    QTest::addRow("LogController")  << static_cast<QObject*>(new LogController());
    QTest::addRow("RcseApplicationController")  << static_cast<QObject*>(new RcseApplicationController());
    QTest::addRow("CharacterItemUpdater")  << static_cast<QObject*>(new CharacterItemUpdater());
    QTest::addRow("FindDialog")  << static_cast<QObject*>(new FindDialog());
    QTest::addRow("SelectionController")  << static_cast<QObject*>(new SelectionController());
    QTest::addRow("ShortcutVisitor")  << static_cast<QObject*>(new ShortcutVisitor());
    QTest::addRow("WebpageMediaController")  << static_cast<QObject*>(new WebpageMediaController());
    QTest::addRow("LocalPersonModel")  << static_cast<QObject*>(new LocalPersonModel());
    QTest::addRow("CampaignDock")  << static_cast<QObject*>(new CampaignDock());
    QTest::addRow("ResourcesController")  << static_cast<QObject*>(new ResourcesController());
    QTest::addRow("Section")  << static_cast<QObject*>(new Section());
    QTest::addRow("RServer")  << static_cast<QObject*>(new RServer());
    QTest::addRow("TreeSheetItem")  << static_cast<QObject*>(new TreeSheetItem());
    QTest::addRow("CharacterItem")  << static_cast<QObject*>(new CharacterItem());
    QTest::addRow("FilteredModel")  << static_cast<QObject*>(new FilteredModel());
    QTest::addRow("SideMenuController")  << static_cast<QObject*>(new SideMenuController());
    QTest::addRow("DragableTableView")  << static_cast<QObject*>(new DragableTableView());
    QTest::addRow("NewFileDialog")  << static_cast<QObject*>(new NewFileDialog());
    QTest::addRow("Character")  << static_cast<QObject*>(new Character());
    QTest::addRow("CategoryModel")  << static_cast<QObject*>(new CategoryModel());
    QTest::addRow("UnitModel")  << static_cast<QObject*>(new UnitModel());
    QTest::addRow("BorderListEditor")  << static_cast<QObject*>(new BorderListEditor());
    QTest::addRow("CharacterController")  << static_cast<QObject*>(new CharacterController());
    QTest::addRow("DbusAdaptor")  << static_cast<QObject*>(new DbusAdaptor());
    QTest::addRow("CharacterDataModel")  << static_cast<QObject*>(new CharacterDataModel());
    QTest::addRow("QmlHighlighter")  << static_cast<QObject*>(new QmlHighlighter());
    QTest::addRow("CharacterList")  << static_cast<QObject*>(new CharacterList());
    QTest::addRow("AbstractControllerInterface")  << static_cast<QObject*>(new AbstractControllerInterface());
    QTest::addRow("MindNode")  << static_cast<QObject*>(new MindNode());
    QTest::addRow("ContentController")  << static_cast<QObject*>(new ContentController());
    QTest::addRow("PlayerWidget")  << static_cast<QObject*>(new PlayerWidget());
    QTest::addRow("ItemEditor")  << static_cast<QObject*>(new ItemEditor());
    QTest::addRow("PdfManager")  << static_cast<QObject*>(new PdfManager());
    QTest::addRow("CommandMessage")  << static_cast<QObject*>(new CommandMessage());
    QTest::addRow("ImageMediaController")  << static_cast<QObject*>(new ImageMediaController());
    QTest::addRow("RealSlider")  << static_cast<QObject*>(new RealSlider());
    QTest::addRow("CharacterSheet")  << static_cast<QObject*>(new CharacterSheet());
    QTest::addRow("Canvas")  << static_cast<QObject*>(new Canvas());
    QTest::addRow("SightItem")  << static_cast<QObject*>(new SightItem());
    QTest::addRow("CharacterSheetUpdater")  << static_cast<QObject*>(new CharacterSheetUpdater());
    QTest::addRow("ClientManager")  << static_cast<QObject*>(new ClientManager());
    QTest::addRow("AbstractApplicationController")  << static_cast<QObject*>(new AbstractApplicationController());
    QTest::addRow("GenericModel")  << static_cast<QObject*>(new GenericModel());
    QTest::addRow("ColorListEditor")  << static_cast<QObject*>(new ColorListEditor());
    QTest::addRow("SharedNoteController")  << static_cast<QObject*>(new SharedNoteController());
    QTest::addRow("SharedNoteContainer")  << static_cast<QObject*>(new SharedNoteContainer());
    QTest::addRow("DiceServer")  << static_cast<QObject*>(new DiceServer());
    QTest::addRow("UpnpNat")  << static_cast<QObject*>(new UpnpNat());
    QTest::addRow("CodeEditorDialog")  << static_cast<QObject*>(new CodeEditorDialog());
    QTest::addRow("PlayersPanel")  << static_cast<QObject*>(new PlayersPanel());
    QTest::addRow("CampaignView")  << static_cast<QObject*>(new CampaignView());
    QTest::addRow("AudioPlayerController")  << static_cast<QObject*>(new AudioPlayerController());
    QTest::addRow("RuleItem")  << static_cast<QObject*>(new RuleItem());
    QTest::addRow("LinkController")  << static_cast<QObject*>(new LinkController());
    QTest::addRow("NodeStyle")  << static_cast<QObject*>(new NodeStyle());
    QTest::addRow("FileSerializer")  << static_cast<QObject*>(new FileSerializer());
    QTest::addRow("InstantMessagingController")  << static_cast<QObject*>(new InstantMessagingController());
    QTest::addRow("ClientConnection")  << static_cast<QObject*>(new ClientConnection());
    QTest::addRow("InstantMessagingModel")  << static_cast<QObject*>(new InstantMessagingModel());
    QTest::addRow("DiceRoller")  << static_cast<QObject*>(new DiceRoller());
    QTest::addRow("NodeStyleModel")  << static_cast<QObject*>(new NodeStyleModel());
    QTest::addRow("MediaUpdaterInterface")  << static_cast<QObject*>(new MediaUpdaterInterface());
    QTest::addRow("CampaignManager")  << static_cast<QObject*>(new CampaignManager());
    QTest::addRow("VisualItemController")  << static_cast<QObject*>(new VisualItemController());
    QTest::addRow("WebViewUpdater")  << static_cast<QObject*>(new WebViewUpdater());
    QTest::addRow("ColumnDefinitionDialog")  << static_cast<QObject*>(new ColumnDefinitionDialog());
    QTest::addRow("ImportDataFromCampaignDialog")  << static_cast<QObject*>(new ImportDataFromCampaignDialog());
    QTest::addRow("MainWindow")  << static_cast<QObject*>(new MainWindow());
    QTest::addRow("CSItem")  << static_cast<QObject*>(new CSItem());
    QTest::addRow("CharacterSheetController")  << static_cast<QObject*>(new CharacterSheetController());
    QTest::addRow("CampaignProperties")  << static_cast<QObject*>(new CampaignProperties());
    QTest::addRow("AntagonistBoard")  << static_cast<QObject*>(new AntagonistBoard());
    QTest::addRow("QmlGeneratorController")  << static_cast<QObject*>(new QmlGeneratorController());
    QTest::addRow("Convertor")  << static_cast<QObject*>(new Convertor());
    QTest::addRow("TextWriterController")  << static_cast<QObject*>(new TextWriterController());
    QTest::addRow("CharacterVision")  << static_cast<QObject*>(new CharacterVision());
    QTest::addRow("SharedNoteMediaController")  << static_cast<QObject*>(new SharedNoteMediaController());
    QTest::addRow("PreferencesController")  << static_cast<QObject*>(new PreferencesController());
    QTest::addRow("CharacterModel")  << static_cast<QObject*>(new CharacterModel());
    QTest::addRow("ChannelListPanel")  << static_cast<QObject*>(new ChannelListPanel());
    QTest::addRow("StateModel")  << static_cast<QObject*>(new StateModel());
    QTest::addRow("CharacterSheetMediaController")  << static_cast<QObject*>(new CharacterSheetMediaController());
    QTest::addRow("ChatroomSplitterModel")  << static_cast<QObject*>(new ChatroomSplitterModel());
    QTest::addRow("VMap")  << static_cast<QObject*>(new VMap());
    QTest::addRow("UpdateChecker")  << static_cast<QObject*>(new UpdateChecker());
    QTest::addRow("UserListView")  << static_cast<QObject*>(new UserListView());
    QTest::addRow("MRichTextEdit")  << static_cast<QObject*>(new MRichTextEdit());
    QTest::addRow("CharacterStateModel")  << static_cast<QObject*>(new CharacterStateModel());
    QTest::addRow("FieldView")  << static_cast<QObject*>(new FieldView());
    QTest::addRow("RolisteamMimeData")  << static_cast<QObject*>(new RolisteamMimeData());
    QTest::addRow("Document")  << static_cast<QObject*>(new Document());
    QTest::addRow("SliderController")  << static_cast<QObject*>(new SliderController());
    QTest::addRow("LinkItem")  << static_cast<QObject*>(new LinkItem());
    QTest::addRow("NodeItem")  << static_cast<QObject*>(new NodeItem());
    QTest::addRow("VmapTopBar")  << static_cast<QObject*>(new VmapTopBar());
    QTest::addRow("CampaignUpdater")  << static_cast<QObject*>(new CampaignUpdater());
    QTest::addRow("CodeEditor")  << static_cast<QObject*>(new CodeEditor());
    QTest::addRow("FileDirChooser")  << static_cast<QObject*>(new FileDirChooser());
    QTest::addRow("GridItem")  << static_cast<QObject*>(new GridItem());
    QTest::addRow("VColorLabel")  << static_cast<QObject*>(new VColorLabel());
    QTest::addRow("BackgroundButton")  << static_cast<QObject*>(new BackgroundButton());
    QTest::addRow("VColorSelector")  << static_cast<QObject*>(new VColorSelector());
    QTest::addRow("SelectConnectionProfileDialog")  << static_cast<QObject*>(new SelectConnectionProfileDialog());
    QTest::addRow("Campaign")  << static_cast<QObject*>(new Campaign());
    QTest::addRow("ParticipantsModel")  << static_cast<QObject*>(new ParticipantsModel());
    QTest::addRow("Media")  << static_cast<QObject*>(new Media());
    QTest::addRow("WebView")  << static_cast<QObject*>(new WebView());
    QTest::addRow("HistoryModel")  << static_cast<QObject*>(new HistoryModel());
    QTest::addRow("FilterInstantMessagingModel")  << static_cast<QObject*>(new FilterInstantMessagingModel());
    QTest::addRow("LoadGetImage")  << static_cast<QObject*>(new LoadGetImage());
    QTest::addRow("Gloader")  << static_cast<QObject*>(new Gloader());
    QTest::addRow("OOReader")  << static_cast<QObject*>(new OOReader());
    QTest::addRow("CharacterItemController")  << static_cast<QObject*>(new CharacterItemController());
    QTest::addRow("PlayerModel")  << static_cast<QObject*>(new PlayerModel());
    QTest::addRow("SightController")  << static_cast<QObject*>(new SightController());
    QTest::addRow("ShortCutEditorDialog")  << static_cast<QObject*>(new ShortCutEditorDialog());
    QTest::addRow("ImageSelectorDialog")  << static_cast<QObject*>(new ImageSelectorDialog());
    QTest::addRow("ParticipantModel")  << static_cast<QObject*>(new ParticipantModel());
    QTest::addRow("ServerConnectionManager")  << static_cast<QObject*>(new ServerConnectionManager());
    QTest::addRow("RemoteLogController")  << static_cast<QObject*>(new RemoteLogController());
    QTest::addRow("EllipseController")  << static_cast<QObject*>(new EllipseController());
    QTest::addRow("SheetController")  << static_cast<QObject*>(new SheetController());
    QTest::addRow("EditorController")  << static_cast<QObject*>(new EditorController());
    QTest::addRow("PathController")  << static_cast<QObject*>(new PathController());
    QTest::addRow("CenteredCheckBox")  << static_cast<QObject*>(new CenteredCheckBox());
    QTest::addRow("ImagePathDelegateItem")  << static_cast<QObject*>(new ImagePathDelegateItem());
    QTest::addRow("PathControllerUpdater")  << static_cast<QObject*>(new PathControllerUpdater());
    QTest::addRow("ImageItem")  << static_cast<QObject*>(new ImageItem());
    QTest::addRow("NetworkController")  << static_cast<QObject*>(new NetworkController());
    QTest::addRow("ChildPointItem")  << static_cast<QObject*>(new ChildPointItem());
    QTest::addRow("HeartBeatSender")  << static_cast<QObject*>(new HeartBeatSender());
    QTest::addRow("NoteContainer")  << static_cast<QObject*>(new NoteContainer());
    QTest::addRow("CanvasField")  << static_cast<QObject*>(new CanvasField());
    QTest::addRow("MediaManagerBase")  << static_cast<QObject*>(new MediaManagerBase());
    QTest::addRow("SharedNote")  << static_cast<QObject*>(new SharedNote());
    QTest::addRow("RolisteamApplication")  << static_cast<QObject*>(new RolisteamApplication());
    QTest::addRow("TipChecker")  << static_cast<QObject*>(new TipChecker());
    QTest::addRow("LanguageModel")  << static_cast<QObject*>(new LanguageModel());
    QTest::addRow("Channel")  << static_cast<QObject*>(new Channel());
    QTest::addRow("PositionedItem")  << static_cast<QObject*>(new PositionedItem());
    QTest::addRow("MediaFilteredModel")  << static_cast<QObject*>(new MediaFilteredModel());
    QTest::addRow("PlayerProxyModel")  << static_cast<QObject*>(new PlayerProxyModel());
    QTest::addRow("StyleSheet")  << static_cast<QObject*>(new StyleSheet());
    QTest::addRow("Theme")  << static_cast<QObject*>(new Theme());
    QTest::addRow("AddNodeCommand")  << static_cast<QObject*>(new AddNodeCommand());
    QTest::addRow("AudioPlayer")  << static_cast<QObject*>(new AudioPlayer());
    QTest::addRow("MindMapUpdater")  << static_cast<QObject*>(new MindMapUpdater());
    QTest::addRow("NonPlayableCharacter")  << static_cast<QObject*>(new NonPlayableCharacter());
    QTest::addRow("NonPlayableCharacterModel")  << static_cast<QObject*>(new NonPlayableCharacterModel());
    QTest::addRow("PdfViewer")  << static_cast<QObject*>(new PdfViewer());
    QTest::addRow("FilteredPlayerModel")  << static_cast<QObject*>(new FilteredPlayerModel());
    QTest::addRow("TextControllerUpdater")  << static_cast<QObject*>(new TextControllerUpdater());
    QTest::addRow("CountDownObject")  << static_cast<QObject*>(new CountDownObject());
    QTest::addRow("MessageInterface")  << static_cast<QObject*>(new MessageInterface());
    QTest::addRow("MessageBase")  << static_cast<QObject*>(new MessageBase());
    QTest::addRow("DiameterSelector")  << static_cast<QObject*>(new DiameterSelector());
    QTest::addRow("GameController")  << static_cast<QObject*>(new GameController());
    QTest::addRow("VectorialMapController")  << static_cast<QObject*>(new VectorialMapController());
    QTest::addRow("RGraphicsView")  << static_cast<QObject*>(new RGraphicsView());
    QTest::addRow("NoteMediaController")  << static_cast<QObject*>(new NoteMediaController());
    QTest::addRow("FieldModel")  << static_cast<QObject*>(new FieldModel());
    QTest::addRow("VMapFrame")  << static_cast<QObject*>(new VMapFrame());
    QTest::addRow("MTextEdit")  << static_cast<QObject*>(new MTextEdit());
    QTest::addRow("LogPanel")  << static_cast<QObject*>(new LogPanel());
    QTest::addRow("SelectConnProfileController")  << static_cast<QObject*>(new SelectConnProfileController());
    QTest::addRow("AudioPlayerUpdater")  << static_cast<QObject*>(new AudioPlayerUpdater());
    QTest::addRow("ImageModel")  << static_cast<QObject*>(new ImageModel());
    QTest::addRow("Player")  << static_cast<QObject*>(new Player());
    QTest::addRow("Overlay")  << static_cast<QObject*>(new Overlay());
    QTest::addRow("HighlighterItem")  << static_cast<QObject*>(new HighlighterItem());
    QTest::addRow("PreferencesManager")  << static_cast<QObject*>(new PreferencesManager());
    QTest::addRow("NoteController")  << static_cast<QObject*>(new NoteController());
    QTest::addRow("MusicModel")  << static_cast<QObject*>(new MusicModel());
    QTest::addRow("ShortCutModel")  << static_cast<QObject*>(new ShortCutModel());
    QTest::addRow("ServerManagerUpdater")  << static_cast<QObject*>(new ServerManagerUpdater());
    QTest::addRow("DiceAliasModel")  << static_cast<QObject*>(new DiceAliasModel());
    QTest::addRow("Image")  << static_cast<QObject*>(new Image());
    QTest::addRow("SheetProperties")  << static_cast<QObject*>(new SheetProperties());
    QTest::addRow("TreeItem")  << static_cast<QObject*>(new TreeItem());
    QTest::addRow("MindMapController")  << static_cast<QObject*>(new MindMapController());
    QTest::addRow("MediaNode")  << static_cast<QObject*>(new MediaNode());
    QTest::addRow("MediaModel")  << static_cast<QObject*>(new MediaModel());
    QTest::addRow("MindItemModel")  << static_cast<QObject*>(new MindItemModel());
    QTest::addRow("ServerConnection")  << static_cast<QObject*>(new ServerConnection());
    QTest::addRow("MessageModel")  << static_cast<QObject*>(new MessageModel());
    QTest::addRow("WebpageController")  << static_cast<QObject*>(new WebpageController());
    QTest::addRow("ImageController")  << static_cast<QObject*>(new ImageController());
    QTest::addRow("NameGeneratorWidget")  << static_cast<QObject*>(new NameGeneratorWidget());
    QTest::addRow("MappingHelper")  << static_cast<QObject*>(new MappingHelper());
    QTest::addRow("GenericUpdater")  << static_cast<QObject*>(new GenericUpdater());
    QTest::addRow("SpacingController")  << static_cast<QObject*>(new SpacingController());
    QTest::addRow("AboutRcse")  << static_cast<QObject*>(new AboutRcse());
    QTest::addRow("ParticipantsPane")  << static_cast<QObject*>(new ParticipantsPane());
    QTest::addRow("ResourcesNode")  << static_cast<QObject*>(new ResourcesNode());
    QTest::addRow("PaletteModel")  << static_cast<QObject*>(new PaletteModel());
    QTest::addRow("LineFieldItem")  << static_cast<QObject*>(new LineFieldItem());
    QTest::addRow("LineModel")  << static_cast<QObject*>(new LineModel());
    QTest::addRow("TableField")  << static_cast<QObject*>(new TableField());
    QTest::addRow("FilteredCharacterModel")  << static_cast<QObject*>(new FilteredCharacterModel());
    QTest::addRow("SharedNoteControllerUpdater")  << static_cast<QObject*>(new SharedNoteControllerUpdater());
    QTest::addRow("ConnectionRetryDialog")  << static_cast<QObject*>(new ConnectionRetryDialog());
    QTest::addRow("PreferencesDialog")  << static_cast<QObject*>(new PreferencesDialog());
    QTest::addRow("ColorModel")  << static_cast<QObject*>(new ColorModel());
    QTest::addRow("DiceBookMarkModel")  << static_cast<QObject*>(new DiceBookMarkModel());
    QTest::addRow("MarkDownHighlighter")  << static_cast<QObject*>(new MarkDownHighlighter());
    QTest::addRow("InstantMessagingUpdater")  << static_cast<QObject*>(new InstantMessagingUpdater());
    QTest::addRow("CheckedLabel")  << static_cast<QObject*>(new CheckedLabel());
    QTest::addRow("TextMessage")  << static_cast<QObject*>(new TextMessage());
    QTest::addRow("MessageDispatcher")  << static_cast<QObject*>(new MessageDispatcher());
    QTest::addRow("SaturationChooser")  << static_cast<QObject*>(new SaturationChooser());
    QTest::addRow("ColorTable")  << static_cast<QObject*>(new ColorTable());
    QTest::addRow("ColorTableChooser")  << static_cast<QObject*>(new ColorTableChooser());
    QTest::addRow("PlayerController")  << static_cast<QObject*>(new PlayerController());
    QTest::addRow("TipOfDayViewer")  << static_cast<QObject*>(new TipOfDayViewer());
    QTest::addRow("BoxModel")  << static_cast<QObject*>(new BoxModel());
    QTest::addRow("Delegate")  << static_cast<QObject*>(new Delegate());
    QTest::addRow("MockCharacter")  << static_cast<QObject*>(new MockCharacter());
    QTest::addRow("DelayReceiveEvent")  << static_cast<QObject*>(new DelayReceiveEvent());
    QTest::addRow("BusyIndicatorDialog")  << static_cast<QObject*>(new BusyIndicatorDialog());
    QTest::addRow("LinkModel")  << static_cast<QObject*>(new LinkModel());
    QTest::addRow("ErrorMessage")  << static_cast<QObject*>(new ErrorMessage());
    QTest::addRow("VisualItem")  << static_cast<QObject*>(new VisualItem());
    QTest::addRow("ActionOnListModel")  << static_cast<QObject*>(new ActionOnListModel());
    QTest::addRow("AboutRolisteam")  << static_cast<QObject*>(new AboutRolisteam());
    QTest::addRow("TextEdit")  << static_cast<QObject*>(new TextEdit());
    QTest::addRow("RemotePlayerModel")  << static_cast<QObject*>(new RemotePlayerModel());
    QTest::addRow("DiceMessage")  << static_cast<QObject*>(new DiceMessage());
    QTest::addRow("PdfController")  << static_cast<QObject*>(new PdfController());
    QTest::addRow("ColorDelegate")  << static_cast<QObject*>(new ColorDelegate());
    QTest::addRow("VMapItemControllerUpdater")  << static_cast<QObject*>(new VMapItemControllerUpdater());
    QTest::addRow("EllipseControllerUpdater")  << static_cast<QObject*>(new EllipseControllerUpdater());
    QTest::addRow("CleverUriMimeData")  << static_cast<QObject*>(new CleverUriMimeData());
    QTest::addRow("ContentModel")  << static_cast<QObject*>(new ContentModel());
    QTest::addRow("LabelWithOptions")  << static_cast<QObject*>(new LabelWithOptions());
    QTest::addRow("ActionDelegate")  << static_cast<QObject*>(new ActionDelegate());
    QTest::addRow("LineControllerUpdater")  << static_cast<QObject*>(new LineControllerUpdater());
    QTest::addRow("CircleDisplayer")  << static_cast<QObject*>(new CircleDisplayer());
    QTest::addRow("RectItem")  << static_cast<QObject*>(new RectItem());
    QTest::addRow("ButtonCanvas")  << static_cast<QObject*>(new ButtonCanvas());
    QTest::addRow("TableCanvasField")  << static_cast<QObject*>(new TableCanvasField());
    QTest::addRow("AnchorItem")  << static_cast<QObject*>(new AnchorItem());
    QTest::addRow("HistoryViewerDialog")  << static_cast<QObject*>(new HistoryViewerDialog());
    QTest::addRow("CleverURI")  << static_cast<QObject*>(new CleverURI());
    QTest::addRow("CharacterSheetModel")  << static_cast<QObject*>(new CharacterSheetModel());
    QTest::addRow("MainControler")  << static_cast<QObject*>(new MainControler());
    QTest::addRow("MindMapControllerBase")  << static_cast<QObject*>(new MindMapControllerBase());
    QTest::addRow("LogUploader")  << static_cast<QObject*>(new LogUploader());
    QTest::addRow("ImageSelectorController")  << static_cast<QObject*>(new ImageSelectorController());
    QTest::addRow("ProxyStateModel")  << static_cast<QObject*>(new ProxyStateModel());
    QTest::addRow("UpdaterWindow")  << static_cast<QObject*>(new UpdaterWindow());
    QTest::addRow("IpChecker")  << static_cast<QObject*>(new IpChecker());
    QTest::addRow("OOFormat")  << static_cast<QObject*>(new OOFormat());
    QTest::addRow("XMLTextEdit")  << static_cast<QObject*>(new XMLTextEdit());
    QTest::addRow("PreventClosing")  << static_cast<QObject*>(new PreventClosing());
    QTest::addRow("Workspace")  << static_cast<QObject*>(new Workspace());
    QTest::addRow("VmapItemModel")  << static_cast<QObject*>(new VmapItemModel());
    QTest::addRow("MediaControllerBase")  << static_cast<QObject*>(new MediaControllerBase());
    QTest::addRow("CommandModel")  << static_cast<QObject*>(new CommandModel());
    QTest::addRow("ImagePathEditor")  << static_cast<QObject*>(new ImagePathEditor());
    QTest::addRow("ClientMimeData")  << static_cast<QObject*>(new ClientMimeData());
    QTest::addRow("ChannelModel")  << static_cast<QObject*>(new ChannelModel());
    QTest::addRow("AutoSaveController")  << static_cast<QObject*>(new AutoSaveController());
    QTest::addRow("SheetWidget")  << static_cast<QObject*>(new SheetWidget());
    QTest::addRow("CodeEdit")  << static_cast<QObject*>(new CodeEdit());
    QTest::addRow("TextController")  << static_cast<QObject*>(new TextController());
    QTest::addRow("CharacterSheetWindow")  << static_cast<QObject*>(new CharacterSheetWindow());
    QTest::addRow("AlignmentDelegate")  << static_cast<QObject*>(new AlignmentDelegate());
    QTest::addRow("Person")  << static_cast<QObject*>(new Person());
    QTest::addRow("VMapUpdater")  << static_cast<QObject*>(new VMapUpdater());
    QTest::addRow("PathItem")  << static_cast<QObject*>(new PathItem());
    QTest::addRow("ThemeModel")  << static_cast<QObject*>(new ThemeModel());
    QTest::addRow("FieldController")  << static_cast<QObject*>(new FieldController());
    QTest::addRow("PackageNode")  << static_cast<QObject*>(new PackageNode());
    QTest::addRow("CustomRuleModel")  << static_cast<QObject*>(new CustomRuleModel());
    QTest::addRow("KeyGeneratorDialog")  << static_cast<QObject*>(new KeyGeneratorDialog());
    QTest::addRow("StateDelegate")  << static_cast<QObject*>(new StateDelegate());
    QTest::addRow("FindToolBar")  << static_cast<QObject*>(new FindToolBar());
    QTest::addRow("PatternModel")  << static_cast<QObject*>(new PatternModel());
    QTest::addRow("MindItem")  << static_cast<QObject*>(new MindItem());
    QTest::addRow("ColorButton")  << static_cast<QObject*>(new ColorButton());
    QTest::addRow("TagListDelegate")  << static_cast<QObject*>(new TagListDelegate());
    QTest::addRow("RectControllerUpdater")  << static_cast<QObject*>(new RectControllerUpdater());
    QTest::addRow("PdfMediaController")  << static_cast<QObject*>(new PdfMediaController());
    QTest::addRow("ProfileModel")  << static_cast<QObject*>(new ProfileModel());
    QTest::addRow("RectController")  << static_cast<QObject*>(new RectController());
    QTest::addRow("HiddingButton")  << static_cast<QObject*>(new HiddingButton());
    QTest::addRow("ToolBox")  << static_cast<QObject*>(new ToolBox());
    QTest::addRow("DiceBookMarkWidget")  << static_cast<QObject*>(new DiceBookMarkWidget());
    QTest::addRow("CheckBoxDelegate")  << static_cast<QObject*>(new CheckBoxDelegate());
    QTest::addRow("ImageItemController")  << static_cast<QObject*>(new ImageItemController());
    QTest::addRow("RolisteamDaemon")  << static_cast<QObject*>(new RolisteamDaemon());
    QTest::addRow("VectorialMapMediaController")  << static_cast<QObject*>(new VectorialMapMediaController());
    QTest::addRow("Link")  << static_cast<QObject*>(new Link());
    QTest::addRow("ConnectionProfile")  << static_cast<QObject*>(new ConnectionProfile());
    QTest::addRow("MindmapManager")  << static_cast<QObject*>(new MindmapManager());
    QTest::addRow("MindMapView")  << static_cast<QObject*>(new MindMapView());
}
QTEST_MAIN(QObjectsTest)
#include "tst_qobjects.moc"
