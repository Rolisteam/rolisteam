#include "qmlgeneratorvisitor.h"

#include <QFile>

#include "charactersheet/charactersheetitem.h"
#include "charactersheet/controllers/fieldcontroller.h"
#include "charactersheet/controllers/tablefield.h"
#include "common/logcategory.h"
#include "include/inja.hpp"
#include "include/json.hpp"

namespace constants
{
namespace json
{
constexpr auto id{"id"};
constexpr auto hrName{"hrName"};
constexpr auto label{"label"};
constexpr auto position{"position"};
constexpr auto indent{"indent"};
constexpr auto value{"value"};
constexpr auto color{"color"};
constexpr auto bgColor{"bgColor"};
constexpr auto pressedColor{"pressedColor"};
constexpr auto textColor{"textColor"};
constexpr auto valueColor{"valueColor"};
constexpr auto borderColor{"borderColor"};
constexpr auto pageManagement{"pageManagement"};
constexpr auto readOnly{"readOnly"};
constexpr auto toolTip{"toolTip"};
constexpr auto fontFamily{"fontFamily"};
constexpr auto fontBold{"fontBold"};
constexpr auto fontItalic{"fontItalic"};
constexpr auto fontUnderline{"fontUnderline"};
constexpr auto fontPointSize{"fontPointSize"};
constexpr auto fontOverLine{"fontOverLine"};
constexpr auto fontStrikeOut{"fontStrikeOut"};
constexpr auto fitFont{"fitFont"};
constexpr auto hoAlign{"hoAlign"};
constexpr auto veAlign{"veAlign"};
constexpr auto idChooser{"idChooser"};
constexpr auto columns{"Columns"};
constexpr auto columnWidths{"columnWidths"};
constexpr auto model{"model"};
constexpr auto maxRow{"maxRow"};
constexpr auto availableValues{"availableValues"};
constexpr auto from{"from"};
constexpr auto to{"to"};
constexpr auto next{"next"};
constexpr auto item{"item"};

constexpr auto imports{"imports"};
constexpr auto headCode{"headCode"};
constexpr auto bottomCode{"bottomCode"};
constexpr auto fields{"fields"};

constexpr auto mainSource{"mainSource"};
constexpr auto mainWidth{"mainWidth"};
constexpr auto mainHeight{"mainHeight"};
constexpr auto pageAdapt{"pageAdapt"};
constexpr auto baseWidth{"baseWidth"};
constexpr auto pageMax{"pageMax"};
constexpr auto mainItem{"mainItem"};

constexpr auto x{"x"};
constexpr auto y{"y"};
constexpr auto width{"width"};
constexpr auto height{"height"};
} // namespace json
namespace templates
{
constexpr auto CheckBoxField{":/templates/CheckBoxField.template"};
constexpr auto DiceButton{":/templates/DiceButton.template"};
constexpr auto ImageField{":/templates/ImageField.template"};
constexpr auto PageButton{":/templates/PageButton.template"};
constexpr auto SelectField{":/templates/SelectField.template"};
constexpr auto SliderField{":/templates/SliderField.template"};
constexpr auto Table{":/templates/Table.template"};
constexpr auto TextArea{":/templates/TextArea.template"};
constexpr auto TextFieldField{":/templates/TextFieldField.template"};
constexpr auto TextInput{":/templates/TextInput.template"};
constexpr auto WebView{":/templates/WebView.template"};
constexpr auto Label{":/templates/Label.template"};
constexpr auto HiddenField{":/templates/Hidden.template"};
constexpr auto FuncButton{":/templates/FuncButton.template"};
constexpr auto position{":/templates/position.template"};
constexpr auto delegateChoice{":/templates/DelegateChoice.template"};
constexpr auto Sheet{":/templates/Sheet.template"};
} // namespace templates

} // namespace constants

namespace ct= constants::templates;
namespace cj= constants::json;

QString evaluateTemplate(const QString& templateName, inja::json data)
{
    QFile file(templateName);
    if(!file.open(QIODevice::ReadOnly))
    {
        qCWarning(CharacterSheetCat) << "no template to open: " << templateName;
        return {};
    }
    auto all= file.readAll();
    try
    {
        auto text= QString::fromStdString(inja::render(all.toStdString(), data));
        return text;
    }
    catch(const inja::RenderError& e)
    {
        qCWarning(CharacterSheetCat) << e.message << templateName;
        return {};
    }
}

void pageMangament(inja::json& data, int page, CSItem::TypeField type)
{
    QString res;
    if(page >= 0)
        res= QStringLiteral("sheetCtrl.currentPage == %1? true : false").arg(page);
    else if(type == CSItem::NEXTPAGE)
        res= QStringLiteral("sheetCtrl.currentPage != root.maxPage");
    else if(type == CSItem::PREVIOUSPAGE)
        res= QStringLiteral("sheetCtrl.currentPage != 0");
    else
        res= QStringLiteral("true");

    data[cj::pageManagement]= QStringLiteral("visible: %1").arg(res).toStdString();
}

void toolTip(inja::json& data, const QString& toolTip)
{
    data[cj::toolTip]= toolTip.toStdString();
}

void alignment(inja::json& data, const QPair<QString, QString>& align)
{
    data[cj::hoAlign]= align.first.toStdString();
    data[cj::veAlign]= align.second.toStdString();
}

void position(inja::json& data, qreal x, qreal y, qreal width, qreal height, bool insideTable)
{
    if(insideTable)
    {
        data[cj::position]= std::string("");
        return;
    }

    inja::json subdata;
    subdata[cj::x]= x;
    subdata[cj::y]= y;
    subdata[cj::width]= width;
    subdata[cj::height]= height;
    subdata[cj::indent]= data[cj::indent];

    data[cj::position]= evaluateTemplate(ct::position, subdata).chopped(1).toStdString();
}

void font(inja::json& data, const QFont& font)
{

    auto func= [](bool b) { return b ? "true" : "false"; };
    data[cj::fontFamily]= font.family().toStdString();
    data[cj::fontBold]= func(font.bold());
    data[cj::fontItalic]= func(font.italic());
    data[cj::fontUnderline]= func(font.underline());
    data[cj::fontPointSize]= QString::number(font.pointSize()).toStdString();
    data[cj::fontOverLine]= func(font.overline());
    data[cj::fontStrikeOut]= func(font.strikeOut());
}

QmlGeneratorVisitor::QmlGeneratorVisitor()
{
    setIndentation(m_indentation);
}

QByteArray QmlGeneratorVisitor::result() const
{
    return m_result;
}

bool QmlGeneratorVisitor::isTable() const
{
    return m_isTable;
}

void QmlGeneratorVisitor::setIsTable(bool isTable)
{
    m_isTable= isTable;
}

QString QmlGeneratorVisitor::generateSheet(const QString& imports, const QString& headCode, const QString& bottomCode,
                                           int maxPage, const QString& mainItem, const QString& source, int width,
                                           int height, const QString& baseWidth, bool pageAdapt, TreeSheetItem* root)
{
    m_root= root;
    inja::json data;
    data[cj::imports]= imports.toStdString();
    data[cj::headCode]= headCode.toStdString();
    data[cj::bottomCode]= bottomCode.toStdString();
    setIndentation(m_indentation + 1);
    data[cj::fields]= generateTreeSheetItem().toStdString();
    setIndentation(m_indentation - 1);
    data[cj::mainSource]= source.toStdString();
    data[cj::mainWidth]= width;
    data[cj::mainHeight]= height;
    data[cj::pageAdapt]= pageAdapt;
    data[cj::pageMax]= maxPage;
    data[cj::baseWidth]= baseWidth.toStdString();
    data[cj::width]= width;
    data[cj::height]= height;
    data[cj::mainItem]= mainItem.toStdString();

    return evaluateTemplate(ct::Sheet, data);
}

QString QmlGeneratorVisitor::generateTreeSheetItem()
{
    QString res;
    QTextStream out(&res);
    for(int i= 0; i < m_root->childrenCount(); ++i)
    {
        auto child= m_root->childAt(i);
        if(child->itemType() == TreeSheetItem::FieldItem)
        {
            auto field= dynamic_cast<FieldController*>(child);
            if(field->generatedCode().isEmpty())
            {
                switch(field->fieldType())
                {
                case CSItem::TEXTINPUT:
                    generateTextInput(out, field, false);
                    break;
                case CSItem::TEXTFIELD:
                    generateTextField(out, field, false);
                    break;
                case CSItem::TEXTAREA:
                    generateTextArea(out, field, false);
                    break;
                case CSItem::SELECT:
                    generateSelect(out, field, false);
                    break;
                case CSItem::CHECKBOX:
                    generateCheckBox(out, field, false);
                    break;
                case CSItem::IMAGE:
                    generateImage(out, field, false);
                    break;
                case CSItem::DICEBUTTON:
                    generateDiceButton(out, field, false);
                    break;
                case CSItem::FUNCBUTTON:
                    generateFuncButton(out, field, false);
                    break;
                case CSItem::RLABEL:
                    generateLabelField(out, field, false);
                    break;
                case CSItem::TABLE:
                    generateTable(out, field);
                    break;
                case CSItem::WEBPAGE:
                    generateWebPage(out, field, false);
                    break;
                case CSItem::NEXTPAGE:
                case CSItem::PREVIOUSPAGE:
                    generateChangePageBtn(out, field, CSItem::NEXTPAGE == field->fieldType());
                    break;
                case CSItem::SLIDER:
                    generateSlider(out, field, false);
                    break;
                case CSItem::HIDDEN:
                    generateHidden(out, field, false);
                    break;
                }
            }
            else
            {
                out << field->generatedCode();
            }
        }
        else if(child->itemType() == TreeSheetItem::TableItem)
        {
            auto field= dynamic_cast<TableFieldController*>(child);
            if(nullptr != field)
            {
                if(field->generatedCode().isEmpty())
                {
                    generateTable(out, field);
                }
                else
                    out << field->generatedCode();
            }
        }
    }
    return res;
}

bool QmlGeneratorVisitor::generateTextInput(QTextStream& out, FieldController* item, bool isInsideTable)
{
    if(!item)
        return false;

    inja::json data;
    data[cj::hrName]= item->label().toStdString();
    data[cj::id]= QString("_%1").arg(getId(item)).toStdString();
    data[cj::color]= item->textColor().name(QColor::HexArgb).toStdString();
    data[cj::bgColor]= item->bgColor().name(QColor::HexArgb).toStdString();
    data[cj::readOnly]
        = (isInsideTable ? QStringLiteral("readOnly") : QStringLiteral("%1.readOnly").arg(getId(item))).toStdString();
    data[cj::indent]= m_indenSpace.toStdString();
    data[cj::value]
        = (isInsideTable ? QStringLiteral("display") : QStringLiteral("%1.value").arg(getId(item))).toStdString();
    pageMangament(data, item->page(), item->fieldType());
    toolTip(data, item->getTooltip());
    alignment(data, item->getTextAlign());
    position(data, item->x(), item->y(), item->width(), item->height(), isInsideTable);
    font(data, item->font());

    out << evaluateTemplate(ct::TextInput, data);

    return true;
}

bool QmlGeneratorVisitor::generateTextArea(QTextStream& out, FieldController* item, bool insideTable)
{
    if(!item)
        return false;

    inja::json data;
    data[cj::hrName]= item->label().toStdString();
    data[cj::id]= QString("_%1").arg(getId(item)).toStdString();
    data[cj::color]= item->textColor().name(QColor::HexArgb).toStdString();
    data[cj::bgColor]= item->bgColor().name(QColor::HexArgb).toStdString();
    data[cj::indent]= m_indenSpace.toStdString();
    data[cj::readOnly]
        = (insideTable ? QStringLiteral("readOnly") : QStringLiteral("%1.readOnly").arg(getId(item))).toStdString();
    data[cj::fitFont]= std::string("");
    data[cj::value]
        = (insideTable ? QStringLiteral("display") : QStringLiteral("%1.value").arg(getId(item))).toStdString();
    pageMangament(data, item->page(), item->fieldType());
    toolTip(data, item->getTooltip());
    alignment(data, item->getTextAlign());
    position(data, item->x(), item->y(), item->width(), item->height(), insideTable);
    font(data, item->font());

    out << evaluateTemplate(ct::TextArea, data);

    return true;
}

bool QmlGeneratorVisitor::generateTextField(QTextStream& out, FieldController* item, bool isInsideTable)
{
    if(!item)
        return false;

    inja::json data;
    data[cj::hrName]= item->label().toStdString();
    data[cj::id]= QString("_%1").arg(getId(item)).toStdString();
    data[cj::color]= item->textColor().name(QColor::HexArgb).toStdString();
    data[cj::bgColor]= item->bgColor().name(QColor::HexArgb).toStdString();
    data[cj::indent]= m_indenSpace.toStdString();
    data[cj::fitFont]= std::string("");
    data[cj::readOnly]
        = (isInsideTable ? QStringLiteral("readOnly") : QStringLiteral("%1.readOnly").arg(getId(item))).toStdString();
    data[cj::value]
        = (isInsideTable ? QStringLiteral("display") : QStringLiteral("%1.value").arg(getId(item))).toStdString();
    pageMangament(data, item->page(), item->fieldType());
    toolTip(data, item->getTooltip());
    alignment(data, item->getTextAlign());
    position(data, item->x(), item->y(), item->width(), item->height(), isInsideTable);
    font(data, item->font());
    out << evaluateTemplate(ct::TextFieldField, data);
    return true;
}
bool QmlGeneratorVisitor::generateLabelField(QTextStream& out, FieldController* item, bool isInsideTable)
{
    if(!item)
        return false;

    inja::json data;
    data[cj::hrName]= item->label().toStdString();
    data[cj::id]= QString("_%1").arg(getId(item)).toStdString();
    data[cj::color]= item->textColor().name(QColor::HexArgb).toStdString();
    data[cj::bgColor]= item->bgColor().name(QColor::HexArgb).toStdString();
    data[cj::indent]= m_indenSpace.toStdString();
    data[cj::fitFont]= std::string("");
    data[cj::readOnly]
        = (isInsideTable ? QStringLiteral("readOnly") : QStringLiteral("%1.readOnly").arg(getId(item))).toStdString();
    data[cj::value]
        = (isInsideTable ? QStringLiteral("display") : QStringLiteral("%1.value").arg(getId(item))).toStdString();
    pageMangament(data, item->page(), item->fieldType());
    toolTip(data, item->getTooltip());
    alignment(data, item->getTextAlign());
    position(data, item->x(), item->y(), item->width(), item->height(), isInsideTable);
    font(data, item->font());

    out << evaluateTemplate(ct::Label, data);
    return true;
}

bool QmlGeneratorVisitor::generateSlider(QTextStream& out, FieldController* item, bool isInsideTable)
{
    if(!item)
        return false;

    auto vals= item->availableValues();
    qreal start= 0.;
    qreal end= 100.;
    if(vals.size() == 2)
    {
        start= vals[0].toDouble();
        end= vals[1].toDouble();
    }

    inja::json data;
    data[cj::indent]= m_indenSpace.toStdString();
    data[cj::hrName]= item->label().toStdString();
    data[cj::id]= QString("_%1").arg(getId(item)).toStdString();
    position(data, item->x(), item->y(), item->width(), item->height(), isInsideTable);
    data[cj::valueColor]= item->textColor().name(QColor::HexArgb).toStdString();
    data[cj::bgColor]= item->bgColor().name(QColor::HexArgb).toStdString();
    data[cj::from]= QString::number(start).toStdString();
    data[cj::to]= QString::number(end).toStdString();
    data[cj::readOnly]
        = (isInsideTable ? QStringLiteral("readOnly") : QStringLiteral("%1.readOnly").arg(getId(item))).toStdString();
    data[cj::value]
        = (isInsideTable ? QStringLiteral("display") : QStringLiteral("%1.value").arg(getId(item))).toStdString();
    pageMangament(data, item->page(), item->fieldType());
    toolTip(data, item->getTooltip());

    out << evaluateTemplate(ct::SliderField, data);
    return true;
}

bool QmlGeneratorVisitor::generateHidden(QTextStream& out, FieldController* item, bool isInsideTable)
{
    if(!item)
        return false;

    inja::json data;
    data[cj::hrName]= item->label().toStdString();
    data[cj::id]= QString("_%1").arg(getId(item)).toStdString();
    data[cj::indent]= m_indenSpace.toStdString();
    data[cj::value]
        = (isInsideTable ? QStringLiteral("display") : QStringLiteral("%1.value").arg(getId(item))).toStdString();
    out << evaluateTemplate(ct::HiddenField, data);
    return true;
}
bool QmlGeneratorVisitor::generateSelect(QTextStream& out, FieldController* item, bool isInsideTable)
{
    if(!item)
        return false;

    inja::json data;
    data[cj::hrName]= item->label().toStdString();
    data[cj::id]= QString("_%1").arg(getId(item)).toStdString();
    position(data, item->x(), item->y(), item->width(), item->height(), isInsideTable);
    data[cj::textColor]= item->textColor().name(QColor::HexArgb).toStdString();
    data[cj::bgColor]= item->bgColor().name(QColor::HexArgb).toStdString();
    data[cj::indent]= m_indenSpace.toStdString();
    data[cj::availableValues]= QStringLiteral("[\"%1\"]").arg(item->availableValues().join("\",\"")).toStdString();
    data[cj::readOnly]
        = (isInsideTable ? QStringLiteral("readOnly") : QStringLiteral("%1.readOnly").arg(getId(item))).toStdString();
    data[cj::value]
        = (isInsideTable ? QStringLiteral("display") : QStringLiteral("%1.value").arg(getId(item))).toStdString();
    pageMangament(data, item->page(), item->fieldType());
    toolTip(data, item->getTooltip());

    out << evaluateTemplate(ct::SelectField, data);
    return true;
}

bool QmlGeneratorVisitor::generateCheckBox(QTextStream& out, FieldController* item, bool isInsideTable)
{
    if(!item)
        return false;

    inja::json data;
    data[cj::hrName]= item->label().toStdString();
    data[cj::id]= QString("_%1").arg(getId(item)).toStdString();
    data[cj::indent]= m_indenSpace.toStdString();
    position(data, item->x(), item->y(), item->width(), item->height(), isInsideTable);
    data[cj::color]= item->bgColor().name(QColor::HexArgb).toStdString();
    data[cj::borderColor]= item->textColor().name(QColor::HexArgb).toStdString();
    data[cj::availableValues]= QStringLiteral("[\"%1\"]").arg(item->availableValues().join("\",\"")).toStdString();
    data[cj::readOnly]
        = (isInsideTable ? QStringLiteral("readOnly") : QStringLiteral("%1.readOnly").arg(getId(item))).toStdString();
    data[cj::label]
        = (isInsideTable ? QStringLiteral("model.display") : QStringLiteral("%1.label").arg(getId(item))).toStdString();
    data[cj::value]
        = (isInsideTable ? QStringLiteral("model.display") : QStringLiteral("%1.value").arg(getId(item))).toStdString();
    pageMangament(data, item->page(), item->fieldType());
    toolTip(data, item->getTooltip());

    out << evaluateTemplate(ct::CheckBoxField, data);

    return true;
}

bool QmlGeneratorVisitor::generateFuncButton(QTextStream& out, FieldController* item, bool isInsideTable)
{
    if(!item)
        return false;

    inja::json data;
    data[cj::indent]= m_indenSpace.toStdString();
    data[cj::hrName]= item->label().toStdString();
    data[cj::label]
        = (isInsideTable ? QStringLiteral("display") : QStringLiteral("%1.label").arg(getId(item))).toStdString();
    data[cj::id]= QString("_%1").arg(getId(item)).toStdString();
    position(data, item->x(), item->y(), item->width(), item->height(), isInsideTable);
    data[cj::pressedColor]= item->textColor().name(QColor::HexArgb).toStdString();
    data[cj::color]= item->bgColor().name(QColor::HexArgb).toStdString();
    data[cj::bgColor]= item->bgColor().name(QColor::HexArgb).toStdString();
    data[cj::textColor]= item->textColor().name(QColor::HexArgb).toStdString();
    data[cj::availableValues]= QStringLiteral("[\"%1\"]").arg(item->availableValues().join("\",\"")).toStdString();
    data[cj::readOnly]
        = (isInsideTable ? QStringLiteral("readOnly") : QStringLiteral("%1.readOnly").arg(getId(item))).toStdString();
    data[cj::value]
        = (isInsideTable ? QStringLiteral("formula") : QStringLiteral("%1.value").arg(getId(item))).toStdString();
    pageMangament(data, item->page(), item->fieldType());
    font(data, item->font());
    alignment(data, item->getTextAlign());
    data[cj::fitFont]= std::string();
    toolTip(data, item->getTooltip());

    out << evaluateTemplate(ct::FuncButton, data);
    return true;
}

bool QmlGeneratorVisitor::generateDiceButton(QTextStream& out, FieldController* item, bool isInsideTable)
{
    if(!item)
        return false;

    inja::json data;
    data[cj::indent]= m_indenSpace.toStdString();
    data[cj::hrName]= item->label().toStdString();
    data[cj::label]
        = (isInsideTable ? QStringLiteral("display") : QStringLiteral("%1.label").arg(getId(item))).toStdString();
    data[cj::hrName]= item->label().toStdString();
    data[cj::id]= QString("_%1").arg(getId(item)).toStdString();
    position(data, item->x(), item->y(), item->width(), item->height(), isInsideTable);
    data[cj::pressedColor]= item->textColor().name(QColor::HexArgb).toStdString();
    data[cj::color]= item->bgColor().name(QColor::HexArgb).toStdString();
    data[cj::bgColor]= item->bgColor().name(QColor::HexArgb).toStdString();
    data[cj::textColor]= item->textColor().name(QColor::HexArgb).toStdString();
    pageMangament(data, item->page(), item->fieldType());
    data[cj::readOnly]
        = (isInsideTable ? QStringLiteral("readOnly") : QStringLiteral("%1.readOnly").arg(getId(item))).toStdString();
    data[cj::value]
        = (isInsideTable ? QStringLiteral("formula") : QStringLiteral("%1.value").arg(getId(item))).toStdString();

    font(data, item->font());
    data[cj::fitFont]= std::string();
    alignment(data, item->getTextAlign());
    toolTip(data, item->getTooltip());

    out << evaluateTemplate(ct::DiceButton, data);

    return true;
}

bool QmlGeneratorVisitor::generateImage(QTextStream& out, FieldController* item, bool isInsideTable)
{
    if(!item)
        return false;

    inja::json data;
    data[cj::indent]= m_indenSpace.toStdString();
    data[cj::hrName]= item->label().toStdString();
    data[cj::id]= QString("_%1").arg(getId(item)).toStdString();
    position(data, item->x(), item->y(), item->width(), item->height(), isInsideTable);
    data[cj::color]= item->bgColor().name(QColor::HexArgb).toStdString();
    data[cj::bgColor]= item->bgColor().name(QColor::HexArgb).toStdString();
    pageMangament(data, item->page(), item->fieldType());
    data[cj::readOnly]
        = (isInsideTable ? QStringLiteral("readOnly") : QStringLiteral("%1.readOnly").arg(getId(item))).toStdString();
    data[cj::value]
        = (isInsideTable ? QStringLiteral("display") : QStringLiteral("%1.value").arg(getId(item))).toStdString();

    out << evaluateTemplate(ct::ImageField, data);

    return true;
}

QStringList QmlGeneratorVisitor::generateTableDelegate(TableFieldController* item, QStringList& columnWidths)
{
    auto const model= item->model();
    auto const& cols= model->columns();

    using cs= CSItem::TypeField;
    QStringList res;

    int i= 0;
    for(auto const& col : cols)
    {
        QString line;
        QTextStream lineOut(&line);
        auto indentationOld= m_indentation;
        setIndentation(m_indentation + 3);
        col->setPage(item->page());
        columnWidths.append(QString::number(col->width()));
        switch(col->fieldType())
        {
        case cs::TEXTINPUT:
            generateTextInput(lineOut, col, true);
            break;
        case cs::TEXTFIELD:
            generateTextField(lineOut, col, true);
            break;
        case cs::TEXTAREA:
            generateTextArea(lineOut, col, true);
            break;
        case cs::SELECT:
            generateSelect(lineOut, col, true);
            break;
        case cs::CHECKBOX:
            generateCheckBox(lineOut, col, true);
            break;
        case cs::IMAGE:
            generateImage(lineOut, col, true);
            break;
        case cs::RLABEL:
            generateLabelField(lineOut, col, true);
            break;
        case cs::DICEBUTTON:
            generateDiceButton(lineOut, col, true);
            break;
        case cs::FUNCBUTTON:
            generateFuncButton(lineOut, col, true);
            break;

        case cs::SLIDER:
            generateSlider(lineOut, col, true);
            break;
        case cs::WEBPAGE:
        case cs::HIDDEN:
        case cs::NEXTPAGE:
        case cs::PREVIOUSPAGE:
        case cs::TABLE:
            qDebug() << "Invalid column type for Table column" << col->fieldType();
            break;
        }
        lineOut.flush();
        setIndentation(indentationOld);

        inja::json data;
        data[cj::value]= QString::number(i).toStdString();
        data[cj::item]= line.toStdString();
        setIndentation(m_indentation + 2);
        data[cj::indent]= m_indenSpace.toStdString();
        setIndentation(indentationOld);

        res << evaluateTemplate(ct::delegateChoice, data);
        ;
        ++i;
    }

    return res;
}

bool QmlGeneratorVisitor::generateTable(QTextStream& out, FieldController* item)
{
    auto tablefield= dynamic_cast<TableFieldController*>(item);
    if(!tablefield)
        return false;

    inja::json data;
    data[cj::indent]= m_indenSpace.toStdString();
    data[cj::hrName]= item->label().toStdString();
    data[cj::id]= QString("_%1").arg(getId(item)).toStdString();
    data[cj::value]= QString("%1").arg(getId(item)).toStdString();
    position(data, item->x(), item->y(), item->width(), item->height(), false);
    pageMangament(data, item->page(), item->fieldType());
    data[cj::model]= QStringLiteral("%1.model").arg(tablefield->id()).toStdString();
    data[cj::maxRow]= QString::number(tablefield->displayedRow()).toStdString();
    data[cj::idChooser]= QStringLiteral("_%1chooser").arg(tablefield->id()).toStdString();

    QStringList columnWidths;
    auto list= generateTableDelegate(tablefield, columnWidths);
    std::vector<std::string> vector;
    vector.reserve(list.size());
    std::transform(std::begin(list), std::end(list), std::back_inserter(vector),
                   [](const QString& line) { return line.toStdString(); });
    data[cj::columns]= vector;
    data[cj::columnWidths]= columnWidths.join(',').toStdString();

    out << evaluateTemplate(ct::Table, data);

    return true;
}

bool QmlGeneratorVisitor::generateChangePageBtn(QTextStream& out, FieldController* item, bool next)
{
    if(!item)
        return false;

    inja::json data;

    data[cj::indent]= m_indenSpace.toStdString();
    data[cj::hrName]= item->label().toStdString();
    data[cj::value]= QStringLiteral("%1.value").arg(getId(item)).toStdString();
    data[cj::next]= next ? "true" : "false";
    position(data, item->x(), item->y(), item->width(), item->height(), false);
    pageMangament(data, item->page(), item->fieldType());
    toolTip(data, item->getTooltip());

    out << evaluateTemplate(ct::PageButton, data);

    return true;
}

bool QmlGeneratorVisitor::generateWebPage(QTextStream& out, FieldController* item, bool isInsideTable)
{
    if(!item)
        return false;

    inja::json data;
    data[cj::id]= QString("_%1").arg(getId(item)).toStdString();
    data[cj::indent]= m_indenSpace.toStdString();
    data[cj::hrName]= item->label().toStdString();
    data[cj::readOnly]
        = (isInsideTable ? QStringLiteral("readOnly") : QStringLiteral("%1.readOnly").arg(getId(item))).toStdString();
    data[cj::value]= QStringLiteral("%1.value").arg(getId(item)).toStdString();
    position(data, item->x(), item->y(), item->width(), item->height(), isInsideTable);
    pageMangament(data, item->page(), item->fieldType());
    toolTip(data, item->getTooltip());

    out << evaluateTemplate(ct::WebView, data);

    return true;
}

int QmlGeneratorVisitor::indentation() const
{
    return m_indentation;
}

void QmlGeneratorVisitor::setIndentation(int indentation)
{
    m_indentation= indentation;
    m_indenSpace.clear();
    for(int i= 0; i < indentation * 4; ++i)
    {
        m_indenSpace+= " ";
    }
}

QString QmlGeneratorVisitor::getId(FieldController* item)
{
    QString result= item->id();
    if(m_isTable)
    {
        result= item->label();
    }
    return result;
}
