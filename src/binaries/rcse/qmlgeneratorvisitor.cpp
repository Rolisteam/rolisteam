#include "qmlgeneratorvisitor.h"

#include "charactersheet/charactersheetitem.h"
#include "charactersheet/controllers/fieldcontroller.h"
#include "charactersheet/controllers/tablefield.h"

QString getPageManagement(FieldController* item, QString indent)
{
    int page= item->page();
    if(page >= 0)
        return QStringLiteral("%1    visible: sheetCtrl.currentPage == %5? true : false\n").arg(indent).arg(page);
    else if(item->fieldType() == CSItem::NEXTPAGE)
        return QStringLiteral("%1    visible: sheetCtrl.currentPage != root.maxPage\n").arg(indent);
    else if(item->fieldType() == CSItem::PREVIOUSPAGE)
        return QStringLiteral("%1    visible: sheetCtrl.currentPage != 0\n").arg(indent);
    else
        return QStringLiteral("%1    visible: true\n").arg(indent);
}

QmlGeneratorVisitor::QmlGeneratorVisitor(QTextStream& out, TreeSheetItem* rootItem) : m_out(out), m_root(rootItem)
{
    setIndentation(m_indentation);
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
                    generateTextInput(field);
                    break;
                case CSItem::TEXTFIELD:
                    generateTextField(field);
                    break;
                case CSItem::TEXTAREA:
                    generateTextArea(field);
                    break;
                case CSItem::SELECT:
                    generateSelect(field);
                    break;
                case CSItem::CHECKBOX:
                    generateCheckBox(field);
                    break;
                case CSItem::IMAGE:
                    generateImage(field);
                    break;
                case CSItem::DICEBUTTON:
                    generateDiceButton(field);
                    break;
                case CSItem::FUNCBUTTON:
                    generateFuncButton(field);
                    break;
                case CSItem::RLABEL:
                    generateLabelField(field);
                    break;
                case CSItem::TABLE:
                    generateTable(field);
                    break;
                case CSItem::WEBPAGE:
                    generateWebPage(field);
                    break;
                case CSItem::NEXTPAGE:
                    generateChangePageBtn(field, true);
                    break;
                case CSItem::PREVIOUSPAGE:
                    generateChangePageBtn(field, false);
                    break;
                case CSItem::SLIDER:
                    generateSlider(field);
                    break;
                case CSItem::HIDDEN:
                    generateHidden(field);
                    break;
                }
            }
            else
            {
                m_out << field->generatedCode();
            }
        }
        else if(child->itemType() == TreeSheetItem::TableItem)
        {
            auto field= dynamic_cast<TableField*>(child);
            if(nullptr != field)
            {
                if(field->generatedCode().isEmpty())
                {
                    generateTable(field);
                }
                else
                    m_out << field->generatedCode();
            }
        }
    }
    return true;
}

bool QmlGeneratorVisitor::generateQmlCodeForRoot()
{
    if(nullptr == m_root)
        return false;

    if(m_root->itemType() == TreeSheetItem::FieldItem)
    {
        auto field= dynamic_cast<FieldController*>(m_root);
        switch(field->fieldType())
        {
        case CSItem::TEXTINPUT:
            generateTextInput(field);
            break;
        case CSItem::TEXTFIELD:
            generateTextField(field);
            break;
        case CSItem::TEXTAREA:
            generateTextArea(field);
            break;
        case CSItem::SELECT:
            generateSelect(field);
            break;
        case CSItem::CHECKBOX:
            generateCheckBox(field);
            break;
        case CSItem::IMAGE:
            generateImage(field);
            break;
        case CSItem::DICEBUTTON:
            generateDiceButton(field);
            break;
        case CSItem::FUNCBUTTON:
            generateFuncButton(field);
            break;
        case CSItem::RLABEL:
            generateLabelField(field);
            break;
        case CSItem::TABLE:
            generateTable(field);
            break;
        case CSItem::WEBPAGE:
            generateWebPage(field);
            break;
        case CSItem::NEXTPAGE:
            generateChangePageBtn(field, true);
            break;
        case CSItem::PREVIOUSPAGE:
            generateChangePageBtn(field, false);
            break;
        case CSItem::SLIDER:
            generateSlider(field);
            break;
        case CSItem::HIDDEN:
            generateHidden(field);
            break;
        }
    }
    else if(m_root->itemType() == TreeSheetItem::TableItem)
    {
        auto field= dynamic_cast<TableField*>(m_root);
        if(field)
            generateTable(field);
    }
    return true;
}

bool QmlGeneratorVisitor::generateTextInput(FieldController* item)
{
    if(!item)
        return false;

    QString text("%5TextInputField {//%1\n"
                 "%6"
                 "%5    text: %2.value\n"
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
                                  QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)));

    return true;
}

bool QmlGeneratorVisitor::generateTextArea(FieldController* item)
{
    if(!item)
        return false;

    QString text("%5TextAreaField {//%1\n"
                 "%6"
                 "%5    text: %2.value\n"
                 "%5    color:\"%3\"\n"
                 "%5    backgroundColor: \"%4\"\n"
                 "%5    field: %7\n"
                 + getPageManagement(item, m_indenSpace) + "%5    readOnly: %2.readOnly\n" + getToolTip(item)
                 + generatePosition(item) + generateAlignment(item) + generateFont(item->font(), false) + "%5}\n");
    /*                 + "%5    onEditingFinished: {\n"
                   "%5        %2.value = text\n"
                   "%5    }\n"*/

    m_out << text.arg(item->label()) //%1
                 .arg(getId(item))
                 .arg(item->textColor().name(QColor::HexArgb))
                 .arg(item->bgColor().name(QColor::HexArgb))
                 .arg(m_indenSpace) //%5
                 .arg(m_isTable ? QStringLiteral("") :
                                  QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)))
                 .arg(getId(item));

    return true;
}

bool QmlGeneratorVisitor::generateTextField(FieldController* item)
{
    if(!item)
        return false;

    QString text("%5TextFieldField {//%1\n"
                 "%6"
                 "%5    text: %2.value\n"
                 "%5    color:\"%3\"\n"
                 "%5    backgroundColor: \"%4\"\n"
                 + getPageManagement(item, m_indenSpace) + "%5    readOnly: %2.readOnly\n" + getToolTip(item)
                 + generatePosition(item) + generateAlignment(item) + generateFont(item->font(), false)
                 + "%5    onTextEdited: {\n"
                   "%5        %2.value = text\n"
                   "%5    }\n"
                   "%5}\n");

    m_out << text.arg(item->label()) //%1
                 .arg(getId(item))
                 .arg(item->textColor().name(QColor::HexArgb))
                 .arg(item->bgColor().name(QColor::HexArgb))
                 .arg(m_indenSpace) //%5
                 .arg(m_isTable ? QStringLiteral("") :
                                  QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)));

    return true;
}
bool QmlGeneratorVisitor::generateLabelField(FieldController* item)
{
    if(!item)
        return false;

    QString text("%5RLabel {//%1\n"
                 "%6"
                 "%5    text: %2.value\n"
                 "%5    color:\"%3\"\n"
                 "%5    backgroundColor: \"%4\"\n"
                 + getPageManagement(item, m_indenSpace) + "%5    readOnly: %2.readOnly\n" + getToolTip(item)
                 + generatePosition(item) + generateAlignment(item) + generateFont(item->font(), true) + "%5}\n");

    m_out << text.arg(item->label()) //%1
                 .arg(getId(item))
                 .arg(item->textColor().name(QColor::HexArgb))
                 .arg(item->bgColor().name(QColor::HexArgb))
                 .arg(m_indenSpace) //%5
                 .arg(m_isTable ? QStringLiteral("") :
                                  QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)));

    return true;
}

bool QmlGeneratorVisitor::generateSlider(FieldController* item)
{
    if(!item)
        return false;

    QString text("%5SliderField {//%1\n"
                 "%6"
                 "%5    value: parseFloat(%2.value)\n"
                 "%5    field: %2\n"
                 "%5    from: %7\n"
                 "%5    to: %8\n"
                 "%5    backgroundColor:\"%4\"\n"
                 "%5    valueColor: \"%3\"\n"
                 + getPageManagement(item, m_indenSpace) + "%5    readOnly: %2.readOnly\n" + getToolTip(item)
                 + generatePosition(item) + "%5}\n");

    auto vals= item->availableValues();
    qreal start= 0.;
    qreal end= 100.;
    if(vals.size() == 2)
    {
        start= vals[0].toDouble();
        end= vals[1].toDouble();
    }

    m_out << text.arg(item->label()) //%1
                 .arg(getId(item))
                 .arg(item->textColor().name(QColor::HexArgb))
                 .arg(item->bgColor().name(QColor::HexArgb))
                 .arg(m_indenSpace) //%5
                 .arg(m_isTable ? QStringLiteral("") :
                                  QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)))
                 .arg(start)
                 .arg(end);

    return true;
}

bool QmlGeneratorVisitor::generateHidden(FieldController* item)
{
    if(!item)
        return false;

    // WARNING no aligment and font for selectfield.
    QString text("%3Item {//%1\n"
                 "%4"
                 "%3    property string value: %2.value\n"
                 + generatePosition(item) + "%3}\n");

    m_out << text.arg(item->label()) //%1
                 .arg(getId(item))
                 .arg(m_indenSpace) //%3
                 .arg(m_isTable ? QStringLiteral("") :
                                  QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)));
    ;

    return true;
}
bool QmlGeneratorVisitor::generateSelect(FieldController* item)
{
    if(!item)
        return false;

    // WARNING no aligment and font for selectfield.
    QString text("%6SelectField {//%1\n"
                 "%7"
                 "%6    selected: %2.value\n"
                 "%6    availableValues:%5\n"
                 "%6    currentIndex: combo.find(text)\n"
                 "%6    textColor:\"%3\"\n"
                 "%6    color: \"%4\"\n"
                 "%6    onCurrentIndexChanged:{\n"
                 "%6        if(%2.value !== currentIndex)\n"
                 "%6        {\n"
                 "%6            %2.value = currentIndex\n"
                 "%6        }\n"
                 "%6    }\n"
                 + getPageManagement(item, m_indenSpace) + "%6    enabled: !%2.readOnly\n" + getToolTip(item)
                 + generatePosition(item) + "%6}\n");

    m_out << text.arg(item->label()) //%1
                 .arg(getId(item))
                 .arg(item->textColor().name(QColor::HexArgb))
                 .arg(item->bgColor().name(QColor::HexArgb))
                 .arg(QStringLiteral("[\"%1\"]").arg(item->availableValues().join("\",\""))) //%5
                 .arg(m_indenSpace)
                 .arg(m_isTable ? QStringLiteral("") :
                                  QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)));
    return true;
}

bool QmlGeneratorVisitor::generateCheckBox(FieldController* item)
{
    if(!item)
        return false;

    QString text("%5CheckBoxField {//%1\n"
                 "%6"
                 "%5    field: %2\n"
                 "%5    text: %2.value\n"
                 "%5    color:\"%3\"\n"
                 "%5    borderColor: \"%4\"\n"
                 + getPageManagement(item, m_indenSpace) + "%5    readOnly: %2.readOnly\n" + getToolTip(item)
                 + generatePosition(item) + "%5}\n");

    m_out << text.arg(item->label()) //%1
                 .arg(getId(item))
                 .arg(item->bgColor().name(QColor::HexArgb))
                 .arg(item->textColor().name(QColor::HexArgb))
                 .arg(m_indenSpace) //%5
                 .arg(m_isTable ? QStringLiteral("") :
                                  QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)));

    return true;
}

bool QmlGeneratorVisitor::generateFuncButton(FieldController* item)
{
    if(!item)
        return false;

    QString text("%6DiceButton {//%1\n"
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
                 .arg(item->textColor().name(QColor::HexArgb));

    return true;
}

bool QmlGeneratorVisitor::generateDiceButton(FieldController* item)
{
    if(!item)
        return false;

    QString text("%6DiceButton {//%1\n"
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
                 .arg(item->textColor().name(QColor::HexArgb));

    return true;
}

bool QmlGeneratorVisitor::generateImage(FieldController* item)
{
    if(!item)
        return false;

    QString text("%4ImageField {//%1\n"
                 "%5"
                 "%4    source: %2.value\n"
                 "%4    color: \"%3\"\n"
                 + getPageManagement(item, m_indenSpace) + "%4    readOnly: %2.readOnly\n" + generatePosition(item)
                 + "%4}\n");

    m_out << text.arg(item->label()) //%1
                 .arg(getId(item))
                 .arg(item->bgColor().name(QColor::HexArgb))
                 .arg(m_indenSpace) //%5
                 .arg(m_isTable ? QStringLiteral("") :
                                  QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)));

    return true;
}

bool QmlGeneratorVisitor::generateTable(FieldController* item)
{
    auto tablefield= dynamic_cast<TableField*>(item);
    if(!tablefield)
        return false;

    tablefield->fillModel();

    QString node("%1Table{//%2\n"
                 "%1    id: _%3list\n"
                 "%1    field: %3\n"
                 + generatePosition(item) + getPageManagement(item, m_indenSpace)
                 + "%1    maxRow:%4\n"
                   "%1    model: %3.model\n"
                   "%1    delegate: RowLayout {\n"
                   "%1        height: _%3list.height/_%3list.maxRow\n"
                   "%1        width:  _%3list.width\n"
                   "%1        spacing:0\n");

    QString end("%1    }\n"
                "%1}\n");

    m_out << node.arg(m_indenSpace).arg(item->label()).arg(tablefield->id()).arg(tablefield->getMaxVisibleRowCount());

    QmlGeneratorVisitor visitor(m_out, tablefield->getRoot());
    visitor.setIndentation(m_indentation + 2);
    visitor.setIsTable(true);
    visitor.generateTreeSheetItem();

    m_out << end.arg(m_indenSpace);

    return true;
}

bool QmlGeneratorVisitor::generateChangePageBtn(FieldController* item, bool next)
{
    if(!item)
        return false;

    QString node("%1PageButton{//%2\n"
                 "%1    next:%3\n"
                 "%1    text: %4.value\n"
                 + getToolTip(item) + generatePosition(item) + getPageManagement(item, m_indenSpace)
                 + "%1    showImage:true\n"
                   "%1    onClicked: {\n"
                   "%1        root.page += next ? 1 : -1\n"
                   "%1    }\n"
                   "%1}\n");

    m_out << node.arg(m_indenSpace).arg(item->label()).arg(next ? "true" : "false").arg(getId(item));

    return true;
}

bool QmlGeneratorVisitor::generateWebPage(FieldController* item)
{
    if(!item)
        return false;

    QString node("%1WebView{//%2\n"
                 "%4"
                 "%1    url:%3.value\n"
                 + getToolTip(item) + generatePosition(item) + getPageManagement(item, m_indenSpace)
                 + "%1    readOnly: %3.readOnly\n"
                   "%1}\n");

    m_out << node.arg(m_indenSpace)
                 .arg(item->label())
                 .arg(getId(item))
                 .arg(m_isTable ? QStringLiteral("") :
                                  QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)));

    return true;
}

QString QmlGeneratorVisitor::generatePosition(FieldController* item)
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

QString QmlGeneratorVisitor::getToolTip(FieldController* item)
{
    QString tooltip= item->getTooltip();
    return QStringLiteral("%1    tooltip:\"%2\"\n").arg(m_indenSpace).arg(tooltip);
}
