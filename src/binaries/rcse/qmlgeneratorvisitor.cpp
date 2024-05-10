#include "qmlgeneratorvisitor.h"

#include <QFile>

#include "charactersheet/charactersheetitem.h"
#include "charactersheet/controllers/fieldcontroller.h"
#include "charactersheet/controllers/tablefield.h"
#include "include/inja.hpp"
#include "include/json.hpp"

namespace constants
{
namespace json
{
constexpr auto id{"id"};
constexpr auto hrName{"hrName"};
constexpr auto position{"position"};
constexpr auto indent{"indent"};
constexpr auto value{"value"};
constexpr auto command{"command"};
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
constexpr auto aliasEnable{"aliasEnable"};
constexpr auto idChooser{"idChooser"};
constexpr auto columns{"Columns"};
constexpr auto model{"model"};
constexpr auto maxRow{"maxRow"};
constexpr auto availableValues{"availableValues"};
constexpr auto from{"from"};
constexpr auto to{"to"};
constexpr auto next{"next"};
}
namespace templates
{
constexpr auto CheckBoxField{":/template/templates/CheckBoxField.template"};
constexpr auto DiceButton{":/template/templates/DiceButton.template"};
constexpr auto ImageField{":/template/templates/ImageField.template"};
constexpr auto PageButton{":/template/templates/PageButton.template"};
constexpr auto SelectField{":/template/templates/SelectField.template"};
constexpr auto SliderField{":/template/templates/SliderField.template"};
constexpr auto Table{":/template/templates/Table.template"};
constexpr auto TextArea{":/template/templates/TextArea.template"};
constexpr auto TextFieldField{":/template/templates/TextFieldField.template"};
constexpr auto TextInput{":/template/templates/TextInput.template"};
constexpr auto WebView{":/template/templates/WebView.template"};
constexpr auto Label{":/template/templates/Label.template"};
constexpr auto HiddenField{":/template/templates/Hidden.template"};
constexpr auto FuncButton{":/template/templates/FuncButton.template"};
}

}

namespace ct = constants::templates;
namespace cj = constants::json;

QString evaluateTemplate(const QString& templateName, inja::json data)
{
    QFile file(templateName);
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "no template to open: " << templateName;
        return {};
    }
    auto all = file.readAll();
    try
    {
        auto text = QString::fromStdString(inja::render(all.toStdString(), data));
        return text;
    }
    catch(const inja::RenderError& e)
    {
        qDebug() << e.message;
        return {};
    }
}

void pageMangament(inja::json& data, int page, CSItem::TypeField type)
{
    QString res;
    if(page >= 0)
        res =  QStringLiteral("sheetCtrl.currentPage == %1? true : false").arg(page);
    else if(type == CSItem::NEXTPAGE)
        res = QStringLiteral("sheetCtrl.currentPage != root.maxPage");
    else if(type == CSItem::PREVIOUSPAGE)
        res =  QStringLiteral("sheetCtrl.currentPage != 0");
    else
        res =  QStringLiteral("true");

    data[cj::pageManagement] = QStringLiteral("visible: %1").arg(res).toStdString();
}

void toolTip(inja::json& data, const QString& toolTip)
{
    data[cj::toolTip] = toolTip.toStdString();
}

void alignment(inja::json& data, const QPair<QString, QString>& align)
{
    data[cj::hoAlign] = align.first.toStdString();
    data[cj::veAlign] = align.second.toStdString();
}

void position(inja::json& data, qreal x, qreal y, qreal width, qreal height)
{
   /* if(m_isTable)
    {
        QString op("%2    Layout.fillHeight: true\n"
                   "%2    Layout.preferredWidth: %1*root.realscale\n");

        return op.arg(item->width()).arg(m_indenSpace);
    }*/

    data[cj::position]= QStringLiteral("x:%1*imagebg.realscale\n"
                          "y:%2*imagebg.realscale\n"
                          "width:%3*imagebg.realscale\n"
                                      "height:%4*imagebg.realscale\n").arg(x).arg(y).arg(width).arg(height).toStdString();
}

void font(inja::json& data, const QFont& font)
{

    auto func = [](bool b){
        return b ? "true" : "false";
    };
    data[cj::fontFamily]=font.family().toStdString();
    data[cj::fontBold]=func(font.bold());
    data[cj::fontItalic]=func(font.italic());
    data[cj::fontUnderline]=func(font.underline());
    data[cj::fontPointSize]=QString::number(font.pointSize()).toStdString();
    data[cj::fontOverLine]=func(font.overline());
    data[cj::fontStrikeOut]=func(font.strikeOut());

    /*QString fontStr("%8    font.family:  \"%1\"\n"
                    "%8    font.bold:    %2\n"
                    "%8    font.italic:  %3\n"
                    "%8    font.underline: %4\n"
                    "%8    font.pointSize: %5\n"
                    "%8    font.overline: %6\n"
                    "%8    font.strikeout: %7\n");

    if(fitfont)
    {
        fontStr.append("%8    fontSizeMode: Text.Fit\n");
        fontStr.replace("font.pointSize", "minimumPixelSize");
    }
    return fontStr.arg(font.family())
        .arg(font.bold() ? QStringLiteral("true") : QStringLiteral("false"))
        .arg(font.italic() ? QStringLiteral("true") : QStringLiteral("false"))
        .arg(font.underline() ? QStringLiteral("true") : QStringLiteral("false"))
        .arg(font.pointSize())
        .arg(font.overline() ? QStringLiteral("true") : QStringLiteral("false"))
        .arg(font.strikeOut() ? QStringLiteral("true") : QStringLiteral("false"))
        .arg(m_indenSpace);*/
}

QString getPageManagement(FieldController* item, QString indent)
{
    return {};
}

QmlGeneratorVisitor::QmlGeneratorVisitor(TreeSheetItem* rootItem) : m_root(rootItem)
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
bool QmlGeneratorVisitor::generateTreeSheetItem()
{
    QTextStream out(&m_result);
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
                    generateTextField(out,field, false);
                    break;
                case CSItem::TEXTAREA:
                    generateTextArea(out,field, false);
                    break;
                case CSItem::SELECT:
                    generateSelect(out,field, false);
                    break;
                case CSItem::CHECKBOX:
                    generateCheckBox(out,field, false);
                    break;
                case CSItem::IMAGE:
                    generateImage(out,field, false);
                    break;
                case CSItem::DICEBUTTON:
                    generateDiceButton(out,field, false);
                    break;
                case CSItem::FUNCBUTTON:
                    generateFuncButton(out,field, false);
                    break;
                case CSItem::RLABEL:
                    generateLabelField(out,field, false);
                    break;
                case CSItem::TABLE:
                    generateTable(out,field);
                    break;
                case CSItem::WEBPAGE:
                    generateWebPage(out,field, false);
                    break;
                case CSItem::NEXTPAGE:
                case CSItem::PREVIOUSPAGE:
                    generateChangePageBtn(out,field, CSItem::NEXTPAGE == field->fieldType());
                    break;
                case CSItem::SLIDER:
                    generateSlider(out,field,false);
                    break;
                case CSItem::HIDDEN:
                    generateHidden(out,field,false);
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
                    generateTable(out,field);
                }
                else
                    out << field->generatedCode();
            }
        }
    }
    return true;
}

/*bool QmlGeneratorVisitor::generateQmlCodeForRoot()
{
    if(nullptr == m_root)
        return false;

    if(m_root->itemType() == TreeSheetItem::FieldItem)
    {
        auto field= dynamic_cast<FieldController*>(m_root);
        switch(field->fieldType())
        {
        case CSItem::TEXTINPUT:
            generateTextInput(field, false);
            break;
        case CSItem::TEXTFIELD:
            generateTextField(field, false);
            break;
        case CSItem::TEXTAREA:
            generateTextArea(field, false);
            break;
        case CSItem::SELECT:
            generateSelect(field, false);
            break;
        case CSItem::CHECKBOX:
            generateCheckBox(field, false);
            break;
        case CSItem::IMAGE:
            generateImage(field, false);
            break;
        case CSItem::DICEBUTTON:
            generateDiceButton(field, false);
            break;
        case CSItem::FUNCBUTTON:
            generateFuncButton(field, false);
            break;
        case CSItem::RLABEL:
            generateLabelField(field, false);
            break;
        case CSItem::TABLE:
            generateTable(field);
            break;
        case CSItem::WEBPAGE:
            generateWebPage(field, false);
            break;
        case CSItem::NEXTPAGE:
        case CSItem::PREVIOUSPAGE:
            generateChangePageBtn(field, field->fieldType() == CSItem::NEXTPAGE);
            break;
        case CSItem::SLIDER:
            generateSlider(field, false);
            break;
        case CSItem::HIDDEN:
            generateHidden(field, false);
            break;
        }
    }
    else if(m_root->itemType() == TreeSheetItem::TableItem)
    {
        auto field= dynamic_cast<TableFieldController*>(m_root);
        if(field)
            generateTable(field);
    }
    return true;
}*/



bool QmlGeneratorVisitor::generateTextInput(QTextStream& out,FieldController* item, bool isInsideTable)
{
    if(!item)
        return false;

    inja::json data;
    data[cj::hrName] = item->label().toStdString();
    data[cj::id] = QString("_%1").arg(getId(item)).toStdString();
    data[cj::color] = item->textColor().name(QColor::HexArgb).toStdString();
    data[cj::bgColor] = item->bgColor().name(QColor::HexArgb).toStdString();
    data[cj::readOnly] = (isInsideTable ? QStringLiteral("readOnly"): QStringLiteral("%1.readOnly").arg(getId(item))).toStdString();
    data[cj::indent] = m_indenSpace.toStdString();
    data[cj::value] = (isInsideTable ? QStringLiteral("display"): QStringLiteral("%1.value").arg(getId(item))).toStdString();
    pageMangament(data, item->page(), item->fieldType());
    toolTip(data, item->getTooltip());
    alignment(data, item->getTextAlign());
    position(data, item->x(), item->y(), item->width(), item->height());
    font(data, item->font());

    out << evaluateTemplate(ct::TextInput, data);

    /*QString text("%5TextInputField {//%1\n"
                 "%6"
                 "%5    text: "+(isInsideTable? QString("display") : QString("%2.value")) +"\n"
                 "%5    color:\"%3\"\n"
                 "%5    backgroundColor: \"%4\"\n"
                 + getPageManagement(item, m_indenSpace) + "%5    readOnly: %2.readOnly\n" + getToolTip(item)
                 + generatePosition(item) + generateAlignment(item) + generateFont(item->font(), false)
                 + "%5    onTextChanged: {\n"
                   "%5        %2.value = text\n"
                   "%5    }\n"
                   "%5}\n");

    m_out << text.arg(item->label()) //%1
                 .arg(getId(item))
                 .arg(item->textColor().name(QColor::HexArgb))
                 .arg(item->bgColor().name(QColor::HexArgb))
                 .arg(m_indenSpace) //%5
                 .arg(m_isTable ? QStringLiteral("") :
                                  QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)));*/

    return true;
}

bool QmlGeneratorVisitor::generateTextArea(QTextStream& out,FieldController* item, bool insideTable)
{
    if(!item)
        return false;

    inja::json data;
    data[cj::hrName] = item->label().toStdString();
    data[cj::id] = QString("_%1").arg(getId(item)).toStdString();
    data[cj::color] = item->textColor().name(QColor::HexArgb).toStdString();
    data[cj::bgColor] = item->bgColor().name(QColor::HexArgb).toStdString();
    data[cj::indent] = m_indenSpace.toStdString();
    data[cj::readOnly] = (insideTable ? QStringLiteral("readOnly"): QStringLiteral("%1.readOnly").arg(getId(item))).toStdString();
    data[cj::fitFont] = std::string("");
    data[cj::value] = (insideTable ? QStringLiteral("display"): QStringLiteral("%1.value").arg(getId(item))).toStdString();
    pageMangament(data, item->page(), item->fieldType());
    toolTip(data, item->getTooltip());
    alignment(data, item->getTextAlign());
    position(data, item->x(), item->y(), item->width(), item->height());
    font(data, item->font());

    out << evaluateTemplate(ct::TextArea, data);

    /*QString text("%5TextAreaField {//%1\n"
                 "%6"
                 "%5    text: %2.value\n"
                 "%5    color:\"%3\"\n"
                 "%5    backgroundColor: \"%4\"\n"
                 + getPageManagement(item, m_indenSpace) + "%5    readOnly: %2.readOnly\n" + getToolTip(item)
                 + generatePosition(item) + generateAlignment(item) + generateFont(item->font(), false) + "%5}\n"
                 + "%5    onEditingFinished: {\n"
                 + (insideTable ? "%5      display = text" : "%5        %2.value = text\n")
                 + "%5    }\n");

    m_out << text.arg(item->label()) //%1
                 .arg(getId(item))
                 .arg(item->textColor().name(QColor::HexArgb))
                 .arg(item->bgColor().name(QColor::HexArgb))
                 .arg(m_indenSpace) //%5
                 .arg(m_isTable ? QStringLiteral("") :
                                  QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)));=*/

    return true;
}

bool QmlGeneratorVisitor::generateTextField(QTextStream& out,FieldController* item, bool isInsideTable)
{
    if(!item)
        return false;

    inja::json data;
    data[cj::hrName] = item->label().toStdString();
    data[cj::id] = QString("_%1").arg(getId(item)).toStdString();
    data[cj::color] = item->textColor().name(QColor::HexArgb).toStdString();
    data[cj::bgColor] = item->bgColor().name(QColor::HexArgb).toStdString();
    data[cj::indent] = m_indenSpace.toStdString();
    data[cj::fitFont] = std::string("");
    data[cj::readOnly] = (isInsideTable ? QStringLiteral("readOnly"): QStringLiteral("%1.readOnly").arg(getId(item))).toStdString();
    data[cj::value] = (isInsideTable ? QStringLiteral("display"): QStringLiteral("%1.value").arg(getId(item))).toStdString();
    pageMangament(data, item->page(), item->fieldType());
    toolTip(data, item->getTooltip());
    alignment(data, item->getTextAlign());
    position(data, item->x(), item->y(), item->width(), item->height());
    font(data, item->font());

    out << evaluateTemplate(ct::TextFieldField, data);


    /*QString text("%5TextFieldField {//%1\n"
                 "%6"
                 + (isInsideTable ? QStringLiteral("%5    text: display\n") :QStringLiteral("%5    text: %2.value\n")) +
                 "%5    color:\"%3\"\n"
                 "%5    backgroundColor: \"%4\"\n"
                 + getPageManagement(item, m_indenSpace) + "%5    readOnly: %2.readOnly\n" + getToolTip(item)
                 + generatePosition(item) + generateAlignment(item) + generateFont(item->font(), false)
                 + "%5    onTextEdited: {\n"
                     + (isInsideTable ? "%5        display = text\n" : "%5        %2.value = text\n")+
                   "%5    }\n"
                   "%5}\n");

    m_out << text.arg(item->label()) //%1
                 .arg(getId(item))
                 .arg(item->textColor().name(QColor::HexArgb))
                 .arg(item->bgColor().name(QColor::HexArgb))
                 .arg(m_indenSpace) //%5
                 .arg(m_isTable ? QStringLiteral("") :
                                  QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)));*/

    return true;
}
bool QmlGeneratorVisitor::generateLabelField(QTextStream& out,FieldController* item, bool isInsideTable)
{
    if(!item)
        return false;

    inja::json data;
    data[cj::hrName] = item->label().toStdString();
    data[cj::id] = QString("_%1").arg(getId(item)).toStdString();
    data[cj::color] = item->textColor().name(QColor::HexArgb).toStdString();
    data[cj::bgColor] = item->bgColor().name(QColor::HexArgb).toStdString();
    data[cj::indent] = m_indenSpace.toStdString();
    data[cj::fitFont] = std::string("");
    data[cj::readOnly] = (isInsideTable ? QStringLiteral("readOnly"): QStringLiteral("%1.readOnly").arg(getId(item))).toStdString();
    data[cj::value] = (isInsideTable ? QStringLiteral("display"): QStringLiteral("%1.value").arg(getId(item))).toStdString();
    pageMangament(data, item->page(), item->fieldType());
    toolTip(data, item->getTooltip());
    alignment(data, item->getTextAlign());
    position(data, item->x(), item->y(), item->width(), item->height());
    font(data, item->font());

    out << evaluateTemplate(ct::Label, data);

    /*QString text("%5RLabel {//%1\n"
                 "%6"
                 + (isInsideTable ? QStringLiteral("%5    text: display\n") : QStringLiteral("%5    text: %2.value\n"))
                 + "%5    color:\"%3\"\n"
                 "%5    backgroundColor: \"%4\"\n"
                 + getPageManagement(item, m_indenSpace) + "%5    readOnly: %2.readOnly\n" + getToolTip(item)
                 + generatePosition(item) + generateAlignment(item) + generateFont(item->font(), true) + "%5}\n");

    m_out << text.arg(item->label()) //%1
                 .arg(getId(item))
                 .arg(item->textColor().name(QColor::HexArgb))
                 .arg(item->bgColor().name(QColor::HexArgb))
                 .arg(m_indenSpace) //%5
                 .arg(m_isTable ? QStringLiteral("") :
                                  QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)));*/

    return true;
}

bool QmlGeneratorVisitor::generateSlider(QTextStream& out,FieldController* item, bool isInsideTable)
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
    data[cj::hrName] = item->label().toStdString();
    data[cj::id] = QString("_%1").arg(getId(item)).toStdString();
    position(data, item->x(), item->y(), item->width(), item->height());
    data[cj::valueColor] = item->textColor().name(QColor::HexArgb).toStdString();
    data[cj::bgColor] = item->bgColor().name(QColor::HexArgb).toStdString();
    data[cj::indent] = m_indenSpace.toStdString();
    data[cj::from] = QString::number(start).toStdString();
    data[cj::to] = QString::number(end).toStdString();
    data[cj::readOnly] = (isInsideTable ? QStringLiteral("readOnly"): QStringLiteral("%1.readOnly").arg(getId(item))).toStdString();
    data[cj::value] = (isInsideTable ? QStringLiteral("display"): QStringLiteral("%1.value").arg(getId(item))).toStdString();
    pageMangament(data, item->page(), item->fieldType());
    toolTip(data, item->getTooltip());

    out << evaluateTemplate(ct::SliderField, data);

    /*QString text("%5SliderField {//%1\n"
                 "%6"
                 "%5    value: parseFloat("+ (isInsideTable? QStringLiteral("display"): QStringLiteral("%2.value")) +")\n"
                 "%5    field: %2\n"
                 "%5    from: %7\n"
                 "%5    to: %8\n"
                 "%5    backgroundColor:\"%4\"\n"
                 "%5    valueColor: \"%3\"\n"
                 + getPageManagement(item, m_indenSpace) + "%5    readOnly: %2.readOnly\n" + getToolTip(item)
                 + generatePosition(item) + "%5}\n");



    m_out << text.arg(item->label()) //%1
                 .arg(getId(item))
                 .arg(item->textColor().name(QColor::HexArgb))
                 .arg(item->bgColor().name(QColor::HexArgb))
                 .arg(m_indenSpace) //%5
                 .arg(m_isTable ? QStringLiteral("") :
                                  QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)))
                 .arg(start)
                 .arg(end);*/

    return true;
}

bool QmlGeneratorVisitor::generateHidden(QTextStream &out, FieldController* item, bool isInsideTable)
{
    if(!item)
        return false;

    inja::json data;
    data[cj::hrName] = item->label().toStdString();
    data[cj::id] = QString("_%1").arg(getId(item)).toStdString();
    data[cj::indent] = m_indenSpace.toStdString();
    data[cj::value] = (isInsideTable ? QStringLiteral("display"): QStringLiteral("%1.value").arg(getId(item))).toStdString();



    out << evaluateTemplate(ct::HiddenField, data);


    // WARNING no aligment and font for selectfield.
    /*QString text("%3Item {//%1\n"
                 "%4"
                 + (isInsideTable ? QStringLiteral("%3    property string value: display\n") : QStringLiteral("%3    property string value: %2.value\n"))
                 + generatePosition(item) + "%3}\n");

    m_out << text.arg(item->label()) //%1
                 .arg(getId(item))
                 .arg(m_indenSpace) //%3
                 .arg(m_isTable ? QStringLiteral("") :
                                  QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)));*/
    return true;
}
bool QmlGeneratorVisitor::generateSelect(QTextStream& out,FieldController* item, bool isInsideTable)
{
    if(!item)
        return false;

    inja::json data;
    data[cj::hrName] = item->label().toStdString();
    data[cj::id] = QString("_%1").arg(getId(item)).toStdString();
    position(data, item->x(), item->y(), item->width(), item->height());
    data[cj::textColor] = item->textColor().name(QColor::HexArgb).toStdString();
    data[cj::bgColor] = item->bgColor().name(QColor::HexArgb).toStdString();
    data[cj::indent] = m_indenSpace.toStdString();
    data[cj::availableValues] = QStringLiteral("[\"%1\"]").arg(item->availableValues().join("\",\"")).toStdString();
    data[cj::readOnly] = (isInsideTable ? QStringLiteral("readOnly"): QStringLiteral("%1.readOnly").arg(getId(item))).toStdString();
    data[cj::value] = (isInsideTable ? QStringLiteral("display"): QStringLiteral("%1.value").arg(getId(item))).toStdString();
    pageMangament(data, item->page(), item->fieldType());
    toolTip(data, item->getTooltip());

    out << evaluateTemplate(ct::SelectField, data);

    // WARNING no aligment and font for selectfield.
    /*QString text("%6SelectField {//%1\n"
                 "%7"
                 "%6    selected: %2\n"
                 "%6    availableValues:%5\n"
                 "%6    currentIndex: combo.find(text)\n"
                 "%6    textColor:\"%3\"\n"
                 "%6    color: \"%4\"\n"
                 "%6    onCurrentIndexChanged:{\n"
                 "%6        if(%2 !== currentIndex)\n"
                 "%6        {\n"
                 "%6            %2 = currentIndex\n"
                 "%6        }\n"
                 "%6    }\n"
                 + getPageManagement(item, m_indenSpace) + "%6    enabled: !%2.readOnly\n" + getToolTip(item)
                 + generatePosition(item) + "%6}\n");

    m_out << text.arg(item->label()) //%1
                 .arg(isInsideTable ? QStringLiteral("display") : QStringLiteral("%1.value").arg(getId(item)))
                 .arg(item->textColor().name(QColor::HexArgb))
                 .arg(item->bgColor().name(QColor::HexArgb))
                 .arg() //%5
                 .arg(m_indenSpace)
                 .arg(m_isTable ? QStringLiteral("") :
                                  QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)));*/
    return true;
}

bool QmlGeneratorVisitor::generateCheckBox(QTextStream& out,FieldController* item, bool isInsideTable )
{
    if(!item)
        return false;

    inja::json data;
    data[cj::hrName] = item->label().toStdString();
    data[cj::id] = QString("_%1").arg(getId(item)).toStdString();
    position(data, item->x(), item->y(), item->width(), item->height());
    data[cj::color] = item->bgColor().name(QColor::HexArgb).toStdString();
    data[cj::borderColor] = item->textColor().name(QColor::HexArgb).toStdString();
    data[cj::indent] = m_indenSpace.toStdString();
    data[cj::availableValues] = QStringLiteral("[\"%1\"]").arg(item->availableValues().join("\",\"")).toStdString();
    data[cj::readOnly] = (isInsideTable ? QStringLiteral("readOnly"): QStringLiteral("%1.readOnly").arg(getId(item))).toStdString();
    data[cj::value] = (isInsideTable ? QStringLiteral("display"): QStringLiteral("%1.value").arg(getId(item))).toStdString();
    pageMangament(data, item->page(), item->fieldType());
    toolTip(data, item->getTooltip());

    out << evaluateTemplate(ct::CheckBoxField, data);

    /*QString text("%5CheckBoxField {//%1\n"
                "%6"
                 "%5    text: %2\n"
                "%5    color:\"%3\"\n"
                "%5    borderColor: \"%4\"\n"
                + getPageManagement(item, m_indenSpace) + "%5    readOnly: %2.readOnly\n" + getToolTip(item)
                + generatePosition(item) +
                "%5    onClicked:{\n"
                "%5      if(root.tristate)\n"
                "%5        %2 = checkState == Qt.Unchecked ? \"0\" : checkState == Qt.PartiallyChecked ? \"1\" : \"2\"\n"
                "%5      else\n"
                "%5         %2 = checked ? \"1\" : \"0\"\n"
                "%5     }\n"
                "%5}\n");

    m_out << text.arg(item->label()) //%1
                 .arg(isInsideTable ? QStringLiteral("display") : QStringLiteral("%1.value").arg(getId(item)))
                 .arg(item->bgColor().name(QColor::HexArgb))
                 .arg(item->textColor().name(QColor::HexArgb))
                 .arg(m_indenSpace) //%5
                 .arg(m_isTable ? QStringLiteral("") :
                                  QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)));*/

    return true;
}

bool QmlGeneratorVisitor::generateFuncButton(QTextStream& out,FieldController* item, bool isInsideTable)
{
    if(!item)
        return false;

    inja::json data;
    data[cj::hrName] = item->label().toStdString();
    data[cj::id] = QString("_%1").arg(getId(item)).toStdString();
    position(data, item->x(), item->y(), item->width(), item->height());
    data[cj::pressedColor] = item->textColor().name(QColor::HexArgb).toStdString();
    data[cj::color] = item->bgColor().name(QColor::HexArgb).toStdString();
    data[cj::bgColor] = item->bgColor().name(QColor::HexArgb).toStdString();
    data[cj::textColor] = item->textColor().name(QColor::HexArgb).toStdString();
    data[cj::indent] = m_indenSpace.toStdString();
    data[cj::availableValues] = QStringLiteral("[\"%1\"]").arg(item->availableValues().join("\",\"")).toStdString();
    data[cj::readOnly] = (isInsideTable ? QStringLiteral("readOnly"): QStringLiteral("%1.readOnly").arg(getId(item))).toStdString();
    data[cj::command] = (isInsideTable ? QStringLiteral("formula"): QStringLiteral("%1.value").arg(getId(item))).toStdString();
    data[cj::value] = (isInsideTable ? QStringLiteral("display"): QStringLiteral("%1.label").arg(getId(item))).toStdString();
    pageMangament(data, item->page(), item->fieldType());
    font(data, item->font());
    alignment(data, item->getTextAlign());
    data[cj::fitFont] = std::string();
    toolTip(data, item->getTooltip());

    out << evaluateTemplate(ct::FuncButton, data);

    /*QString text("%6DiceButton {//%1\n"
                 "%7"
                 "%6    command: %2.value\n"
                 "%6    text: %2.label\n"
                 "%6    pressedColor: \"%3\"\n"
                 "%6    color: \"%4\"\n"
                 "%6    backgroundColor: \"%8\"\n"
                 "%6    textColor: \"%9\"\n"
                 + getPageManagement(item, m_indenSpace) + "%6    readOnly: %2.readOnly\n" + getToolTip(item)
                 + generatePosition(item) + generateAlignment(item) + generateFont(item->font(), false)
                 + "%6    onClicked: {\n"
                   "%6        %5\n"
                   "%6    }\n"
                   "%6}\n");

    m_out << text.arg(item->label()) //%1
                 .arg(getId(item))
                 .arg(item->textColor().name(QColor::HexArgb))
                 .arg(item->bgColor().name(QColor::HexArgb))
                 .arg(item->value()) //%5
                 .arg(m_indenSpace)
                 .arg(m_isTable ? QStringLiteral("") :
                                  QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)))
                 .arg(item->bgColor().name(QColor::HexArgb))
                 .arg(item->textColor().name(QColor::HexArgb));*/

    return true;
}

bool QmlGeneratorVisitor::generateDiceButton(QTextStream& out,FieldController* item, bool isInsideTable)
{
    if(!item)
        return false;


    inja::json data;
    data[cj::indent] = m_indenSpace.toStdString();
    data[cj::hrName] = item->label().toStdString();
    data[cj::id] = QString("_%1").arg(getId(item)).toStdString();
    position(data, item->x(), item->y(), item->width(), item->height());
    data[cj::pressedColor] = item->textColor().name(QColor::HexArgb).toStdString();
    data[cj::color] = item->bgColor().name(QColor::HexArgb).toStdString();
    data[cj::bgColor] = item->bgColor().name(QColor::HexArgb).toStdString();
    data[cj::textColor] = item->textColor().name(QColor::HexArgb).toStdString();
    pageMangament(data, item->page(), item->fieldType());
    data[cj::readOnly] = (isInsideTable ? QStringLiteral("readOnly"): QStringLiteral("%1.readOnly").arg(getId(item))).toStdString();
    data[cj::command] = (isInsideTable ? QStringLiteral("formula"): QStringLiteral("%1.value").arg(getId(item))).toStdString();
    data[cj::value] = (isInsideTable ? QStringLiteral("display"): QStringLiteral("%1.label").arg(getId(item))).toStdString();
    font(data, item->font());
    data[cj::fitFont] = std::string();
    alignment(data, item->getTextAlign());
    data[cj::aliasEnable] = item->aliasEnabled() ? "true" : "false";
    toolTip(data, item->getTooltip());

    out << evaluateTemplate(ct::DiceButton, data);

    /*QString text("%6DiceButton {//%1\n"
                 "%7"
                 "%6    command: %2.value\n"
                 "%6    text: %2.label ? %2.label: \"Dice\"\n"
                 "%6    pressedColor: \"%3\"\n"
                 "%6    color: \"%4\"\n"
                 "%6    backgroundColor: \"%8\"\n"
                 "%6    textColor: \"%9\"\n"
                 + getPageManagement(item, m_indenSpace) + "%6    readOnly: %2.readOnly\n" + getToolTip(item)
                 + generatePosition(item) + generateAlignment(item) + generateFont(item->font(), false)
                 + "%6    onClicked:rollDiceCmd(%2.value,%5)\n"
                   "%6}\n");

    m_out << text.arg(item->label()) //%1
                 .arg(getId(item))
                 .arg(item->textColor().lighter().name(QColor::HexArgb))
                 .arg(item->bgColor().lighter().name(QColor::HexArgb))
                 .arg(item->aliasEnabled() ? "true" : "false") //%5
                 .arg(m_indenSpace)
                 .arg(m_isTable ? QStringLiteral("") :
                                  QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)))
                 .arg(item->bgColor().name(QColor::HexArgb))
                 .arg(item->textColor().name(QColor::HexArgb));*/

    return true;
}

bool QmlGeneratorVisitor::generateImage(QTextStream& out,FieldController* item, bool isInsideTable)
{
    if(!item)
        return false;

    inja::json data;
    data[cj::indent] = m_indenSpace.toStdString();
    data[cj::hrName] = item->label().toStdString();
    data[cj::id] = QString("_%1").arg(getId(item)).toStdString();
    position(data, item->x(), item->y(), item->width(), item->height());
    data[cj::color] = item->bgColor().name(QColor::HexArgb).toStdString();
    data[cj::bgColor] = item->bgColor().name(QColor::HexArgb).toStdString();
    pageMangament(data, item->page(), item->fieldType());
    data[cj::readOnly] = (isInsideTable ? QStringLiteral("readOnly"): QStringLiteral("%1.readOnly").arg(getId(item))).toStdString();
    data[cj::value] = (isInsideTable ? QStringLiteral("display"): QStringLiteral("%1.value").arg(getId(item))).toStdString();

    out << evaluateTemplate(ct::ImageField, data);

    /*QString text("%4ImageField {//%1\n"
                 "%4    %5"
                 "%4    source: %2\n"
                 "%4    color: \"%3\"\n"
                 + getPageManagement(item, m_indenSpace) + "%4    readOnly: %2.readOnly\n" + generatePosition(item)
                 + "%4}\n");

    m_out << text.arg(item->label()) //%1
                 .arg( isInsideTable ? QStringLiteral("display") : QStringLiteral("%1.value").arg(getId(item)))
                 .arg(item->bgColor().name(QColor::HexArgb))
                 .arg(m_indenSpace) //%5
                 .arg(m_isTable ? QStringLiteral("") :
                                  QStringLiteral("id: _%1\n").arg(getId(item)));*/

    return true;
}

QStringList QmlGeneratorVisitor::generateTableDelegate(TableFieldController* item)
{
    auto const model = item->model();
    auto const& cols = model->columns();

    using cs = CSItem::TypeField;
    QStringList res;
    //int i = 0;

    for(auto const& col : cols)
    {
        QString line;
        QTextStream lineOut(&line);
        switch(col->fieldType())
        {
        case cs::TEXTINPUT:
            generateTextInput(lineOut, col, true);
            break;
        case cs::TEXTFIELD:
            generateTextField(lineOut,col, true);
            break;
        case cs::TEXTAREA :
                generateTextArea(lineOut,col, true);
              break;
       case cs::SELECT:
            generateSelect(lineOut,col, true);
              break;
          case  cs::CHECKBOX:
           generateCheckBox(lineOut,col,true);
              break;
          case cs::IMAGE:
              generateImage(lineOut,col, true);
              break;
          case cs::RLABEL:
              generateLabelField(lineOut,col, true);
              break;
         case cs::DICEBUTTON:
              generateDiceButton(lineOut,col, true);
              break;
         case   cs::FUNCBUTTON:
             generateFuncButton(lineOut,col, true);
              break;

         case   cs::SLIDER:
              generateSlider(lineOut,col, true);
              break;
         case   cs::WEBPAGE:
         case   cs::HIDDEN:
         case   cs::NEXTPAGE:
         case   cs::PREVIOUSPAGE:
         case cs::TABLE:
             qDebug() << "Invalid column type for Table column" << col->fieldType();
            break;
        }
         lineOut.flush();
        res << line;
    }

    return res;
}


bool QmlGeneratorVisitor::generateTable(QTextStream& out,FieldController* item)
{
    auto tablefield= dynamic_cast<TableFieldController*>(item);
    if(!tablefield)
        return false;

    tablefield->fillModel();

    inja::json data;
    data[cj::indent] = m_indenSpace.toStdString();
    data[cj::hrName] = item->label().toStdString();
    data[cj::id] = QString("_%1").arg(getId(item)).toStdString();
    position(data, item->x(), item->y(), item->width(), item->height());
    pageMangament(data, item->page(), item->fieldType());
    data[cj::model] = QStringLiteral("%1.model").arg(tablefield->id()).toStdString();
    data[cj::maxRow] = QString::number(tablefield->displayedRow()).toStdString();
    data[cj::idChooser] = QStringLiteral("_%1chooser").arg(tablefield->id()).toStdString();


    auto list = generateTableDelegate(tablefield);
    std::vector<std::string> vector;
    vector.reserve(list.size());
    std::transform(std::begin(list), std::end(list), std::back_inserter(vector), [](const QString& line){
        return line.toStdString();
    });

    data[cj::columns] = vector;

    out << evaluateTemplate(ct::Table, data);

    /*QString node("%1Table{//%2\n"
                 "%1    id: _%3list\n"
                 + generatePosition(item) + getPageManagement(item, m_indenSpace)
                 + "%1    maxRow:%4\n"
                   "%1    model: %3.model\n"
                   "%1    DelegateChooser {\n"
                   "%1        id: \"_%3chooser\"\n"
                       "%1        role: \"column\"\n");

    m_out << node.arg(m_indenSpace).arg(item->label()).arg(tablefield->id()).arg(tablefield->displayedRow());
    generateTableDelegate(tablefield);

    QString node2("%1    \n"
                   "%1    }\n"
                   "%1    onAddLine:{ %2.addLine()}\n"
                   "%1    onRemoveLine:(index)=>{ %2.removeLine(index)}\n"
                   "%1    delegate: _%2chooser \n"
                    "%1    }\n");

    m_out << node2.arg(m_indenSpace).arg(getId(item));*/


    /*QmlGeneratorVisitor visitor(m_out, tablefield->getRoot());
    visitor.setIndentation(m_indentation + 2);
    visitor.setIsTable(true);
    visitor.generateTreeSheetItem();*/

    //m_out << end.arg(m_indenSpace);

    return true;
}

bool QmlGeneratorVisitor::generateChangePageBtn(QTextStream& out,FieldController* item, bool next)
{
    if(!item)
        return false;

    inja::json data;
    data[cj::indent] = m_indenSpace.toStdString();
    data[cj::hrName] = item->label().toStdString();
    data[cj::value] = QStringLiteral("%1.value").arg(getId(item)).toStdString();
    data[cj::next] = next ? "true" : "false";
    position(data, item->x(), item->y(), item->width(), item->height());
    pageMangament(data, item->page(), item->fieldType());
    toolTip(data, item->getTooltip());

    out << evaluateTemplate(ct::PageButton, data);

    /*QString node("%1PageButton{//%2\n"
                 "%1    next:%3\n"
                 "%1    text: %4.value\n"
                 + getToolTip(item) + generatePosition(item) + getPageManagement(item, m_indenSpace)
                 + "%1    showImage:true\n"
                   "%1    onClicked: {\n"
                   "%1        root.page += next ? 1 : -1\n"
                   "%1    }\n"
                   "%1}\n");

    out << node.arg(m_indenSpace).arg(item->label()).arg(next ? "true" : "false").arg(getId(item));*/

    return true;
}

bool QmlGeneratorVisitor::generateWebPage(QTextStream& out,FieldController* item, bool isInsideTable)
{
    if(!item)
        return false;



    inja::json data;
    data[cj::id] = QString("_%1").arg(getId(item)).toStdString();
    data[cj::indent] = m_indenSpace.toStdString();
    data[cj::hrName] = item->label().toStdString();
    data[cj::readOnly] = (isInsideTable ? QStringLiteral("readOnly"): QStringLiteral("%1.readOnly").arg(getId(item))).toStdString();
    data[cj::value] = QStringLiteral("%1.value").arg(getId(item)).toStdString();
    position(data, item->x(), item->y(), item->width(), item->height());
    pageMangament(data, item->page(), item->fieldType());
    toolTip(data, item->getTooltip());

    out << evaluateTemplate(ct::WebView, data);

    /*QString node("%1WebView{//%2\n"
                 "%4"
                 "%1    url:%3\n"
                 + getToolTip(item) + generatePosition(item) + getPageManagement(item, m_indenSpace)
                 + "%1    readOnly: %3.readOnly\n"
                   "%1}\n");

    out << node.arg(m_indenSpace)
                 .arg(item->label())
                 .arg(isInsideTable ? QStringLiteral("display") : QStringLiteral("%1.value").arg(getId(item)))
                 .arg(m_isTable ? QStringLiteral("") :
                                  QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)));*/

    return true;
}

/*QString QmlGeneratorVisitor::generatePosition(FieldController* item)
{
    if(m_isTable)
    {
        QString op("%2    Layout.fillHeight: true\n"
                   "%2    Layout.preferredWidth: %1*root.realscale\n");

        return op.arg(item->width()).arg(m_indenSpace);
    }
    QString positionLines("%5    x:%1*imagebg.realscale\n"
                          "%5    y:%2*imagebg.realscale\n"
                          "%5    width:%3*imagebg.realscale\n"
                          "%5    height:%4*imagebg.realscale\n");

    return positionLines.arg(item->x()).arg(item->y()).arg(item->width()).arg(item->height()).arg(m_indenSpace);
}

QString QmlGeneratorVisitor::generateAlignment(FieldController* item)
{
    QPair<QString, QString> pair= item->getTextAlign();
    QString alignments("%3    hAlign: %1\n"
                       "%3    vAlign: %2\n");

    return alignments.arg(pair.first).arg(pair.second).arg(m_indenSpace);
}

QString QmlGeneratorVisitor::generateFont(const QFont& font, bool fitfont)
{
    QString fontStr("%8    font.family:  \"%1\"\n"
                    "%8    font.bold:    %2\n"
                    "%8    font.italic:  %3\n"
                    "%8    font.underline: %4\n"
                    "%8    font.pointSize: %5\n"
                    "%8    font.overline: %6\n"
                    "%8    font.strikeout: %7\n");

    if(fitfont)
    {
        fontStr.append("%8    fontSizeMode: Text.Fit\n");
        fontStr.replace("font.pointSize", "minimumPixelSize");
    }
    return fontStr.arg(font.family())
        .arg(font.bold() ? QStringLiteral("true") : QStringLiteral("false"))
        .arg(font.italic() ? QStringLiteral("true") : QStringLiteral("false"))
        .arg(font.underline() ? QStringLiteral("true") : QStringLiteral("false"))
        .arg(font.pointSize())
        .arg(font.overline() ? QStringLiteral("true") : QStringLiteral("false"))
        .arg(font.strikeOut() ? QStringLiteral("true") : QStringLiteral("false"))
        .arg(m_indenSpace);
}*/

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

/*QString QmlGeneratorVisitor::getToolTip(FieldController* item)
{
    QString tooltip= item->getTooltip();
    return QStringLiteral("%1    tooltip:\"%2\"\n").arg(m_indenSpace).arg(tooltip);
}*/
