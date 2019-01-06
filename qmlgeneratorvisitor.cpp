#include "qmlgeneratorvisitor.h"

#include "charactersheetitem.h"
#include "field.h"
#include "tablefield.h"

QString getPageManagement(Field* item, QString indent)
{
    int page = item->getPage();
    if(page >= 0)
        return QStringLiteral("%1    visible: root.page == %5? true : false\n").arg(indent).arg(page);
    else if(item->getFieldType() == CharacterSheetItem::NEXTPAGE)
        return QStringLiteral("%1    visible: root.page != root.maxPage\n").arg(indent);
    else if(item->getFieldType() == CharacterSheetItem::PREVIOUSPAGE)
        return QStringLiteral("%1    visible: root.page != 0\n").arg(indent);
    else
        return QStringLiteral("%1    visible: true\n").arg(indent);
}


QmlGeneratorVisitor::QmlGeneratorVisitor(QTextStream& out, CharacterSheetItem* rootItem)
    : m_out(out), m_root(rootItem)
{
    setIndentation(m_indentation);
}

bool QmlGeneratorVisitor::isTable() const
{
    return m_isTable;
}

void QmlGeneratorVisitor::setIsTable(bool isTable)
{
    m_isTable = isTable;
}
bool QmlGeneratorVisitor::generateCharacterSheetItem()
{
    for(int i = 0; i < m_root->getChildrenCount(); ++i)
    {
        auto child = m_root->getChildAt(i);
        if(child->getItemType() == CharacterSheetItem::FieldItem)
        {
            auto field = dynamic_cast<Field*>(child);
            if(field->getGeneratedCode().isEmpty())
            {
                switch (field->getFieldType())
                {
                case CharacterSheetItem::TEXTINPUT:
                    generateTextInput(field);
                    break;
                case CharacterSheetItem::TEXTFIELD:
                    generateTextField(field);
                    break;
                case CharacterSheetItem::TEXTAREA:
                    generateTextArea(field);
                    break;
                case CharacterSheetItem::SELECT:
                    generateSelect(field);
                    break;
                case CharacterSheetItem::CHECKBOX:
                    generateCheckBox(field);
                    break;
                case CharacterSheetItem::IMAGE:
                    generateImage(field);
                    break;
                case CharacterSheetItem::DICEBUTTON:
                    generateDiceButton(field);
                    break;
                case CharacterSheetItem::FUNCBUTTON:
                    generateFuncButton(field);
                    break;
                case CharacterSheetItem::TABLE:
                    generateTable(field);
                    break;
                case CharacterSheetItem::WEBPAGE:
                    generateWebPage(field);
                    break;
                case CharacterSheetItem::NEXTPAGE:
                    generateChangePageBtn(field,true);
                    break;
                case CharacterSheetItem::PREVIOUSPAGE:
                    generateChangePageBtn(field,false);
                    break;
                }
            }
            else
            {
                m_out << field->getGeneratedCode();
            }

        }
        else if(child->getItemType() == CharacterSheetItem::TableItem)
        {
            auto field = dynamic_cast<TableField*>(child);
            if(nullptr != field)
            {
                if(field->getGeneratedCode().isEmpty())
                {
                    generateTable(field);
                }
                else
                    m_out << field->getGeneratedCode();
            }
        }

    }
    return true;
}

bool QmlGeneratorVisitor::generateQmlCodeForRoot()
{
    if(nullptr == m_root)
        return false;

    if(m_root->getItemType() == CharacterSheetItem::FieldItem)
    {
        auto field = dynamic_cast<Field*>(m_root);
        switch (field->getFieldType())
        {
        case CharacterSheetItem::TEXTINPUT:
            generateTextInput(field);
            break;
        case CharacterSheetItem::TEXTFIELD:
            generateTextField(field);
            break;
        case CharacterSheetItem::TEXTAREA:
            generateTextArea(field);
            break;
        case CharacterSheetItem::SELECT:
            generateSelect(field);
            break;
        case CharacterSheetItem::CHECKBOX:
            generateCheckBox(field);
            break;
        case CharacterSheetItem::IMAGE:
            generateImage(field);
            break;
        case CharacterSheetItem::DICEBUTTON:
            generateDiceButton(field);
            break;
        case CharacterSheetItem::FUNCBUTTON:
            generateFuncButton(field);
            break;
        case CharacterSheetItem::TABLE:
            generateTable(field);
            break;
        case CharacterSheetItem::WEBPAGE:
            generateWebPage(field);
            break;
        case CharacterSheetItem::NEXTPAGE:
            generateChangePageBtn(field,true);
            break;
        case CharacterSheetItem::PREVIOUSPAGE:
            generateChangePageBtn(field,false);
            break;
        }

    }
    else if(m_root->getItemType() == CharacterSheetItem::TableItem)
    {
        auto field = dynamic_cast<TableField*>(m_root);
        if(field)
            generateTable(field);
    }
    return true;
}

bool QmlGeneratorVisitor::generateTextInput(Field *item)
{
    if(!item)
        return false;

    QString text("%5TextInputField {//%1\n"
        "%6"
        "%5    text: %2.value\n"
        "%5    textColor:\"%3\"\n"
        "%5    color: \"%4\"\n"+
        getPageManagement(item,m_indenSpace)+
        "%5    readOnly: %2.readOnly\n"+
        getToolTip(item)+
        generatePosition(item) +
        generateAlignment(item) +
        generateFont(item->font()) +
        "%5    onTextChanged: {\n"
        "%5        %2.value = text\n"
        "%5    }\n"
     "%5}\n");

     m_out << text.arg(item->getLabel())//%1
                .arg(getId(item))
                .arg(item->textColor().name(QColor::HexArgb))
                .arg(item->bgColor().name(QColor::HexArgb))
                .arg(m_indenSpace)//%5
              .arg(m_isTable?QStringLiteral(""):QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)));

    return true;
}

bool QmlGeneratorVisitor::generateTextArea(Field *item)
{    if(!item)
        return false;

    QString text("%5TextAreaField {//%1\n"
        "%6"
        "%5    text: %2.value\n"
        "%5    textColor:\"%3\"\n"
        "%5    color: \"%4\"\n"+
        getPageManagement(item,m_indenSpace)+
        "%5    readOnly: %2.readOnly\n"+
        getToolTip(item)+
        generatePosition(item) +
        generateAlignment(item) +
        generateFont(item->font()) +
        "%5    onTextChanged: {\n"
        "%5        %2.value = text\n"
        "%5    }\n"
     "%5}\n");

     m_out << text.arg(item->getLabel())//%1
                   .arg(getId(item))
                   .arg(item->textColor().name(QColor::HexArgb))
                   .arg(item->bgColor().name(QColor::HexArgb))
                   .arg(m_indenSpace)//%5
              .arg(m_isTable?QStringLiteral(""):QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)));

    return true;
}

bool QmlGeneratorVisitor::generateTextField(Field *item)
{
    if(!item)
        return false;

    QString text("%5TextFieldField {//%1\n"
        "%6"
        "%5    text: %2.value\n"
        "%5    textColor:\"%3\"\n"
        "%5    color: \"%4\"\n"+
        getPageManagement(item,m_indenSpace)+
        "%5    readOnly: %2.readOnly\n"+
        getToolTip(item)+
        generatePosition(item) +
        generateAlignment(item) +
        generateFont(item->font()) +
        "%5    onTextChanged: {\n"
        "%5        %2.value = text\n"
        "%5    }\n"
     "%5}\n");

     m_out << text.arg(item->getLabel())//%1
                   .arg(getId(item))
                   .arg(item->textColor().name(QColor::HexArgb))
                   .arg(item->bgColor().name(QColor::HexArgb))
                   .arg(m_indenSpace)//%5
                   .arg(m_isTable?QStringLiteral(""):QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)));

    return true;
}

bool QmlGeneratorVisitor::generateSelect(Field *item)
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
        "%6    }\n"+
        getPageManagement(item,m_indenSpace)+
        "%6    readOnly: %2.readOnly\n"+
        getToolTip(item)+
        generatePosition(item) +
     "%6}\n");

     m_out << text.arg(item->getLabel())//%1
                   .arg(getId(item))
                   .arg(item->textColor().name(QColor::HexArgb))
                   .arg(item->bgColor().name(QColor::HexArgb))
                   .arg(QStringLiteral("[\"%1\"]").arg(item->getAvailableValue().join("\",\"")))//%5
                   .arg(m_indenSpace)
                   .arg(m_isTable?QStringLiteral(""):QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)));
    return true;
}

bool QmlGeneratorVisitor::generateCheckBox(Field *item)
{
    if(!item)
        return false;

    QString text("%5CheckBoxField {//%1\n"
        "%6"
        "%5    field: %2\n"
        "%5    text: %2.value\n"
        "%5    textColor:\"%3\"\n"
        "%5    color: \"%4\"\n"+
        getPageManagement(item,m_indenSpace)+
        "%5    readOnly: %2.readOnly\n"+
        getToolTip(item)+
        generatePosition(item) +
        "%5    onTextChanged: {\n"
        "%5        %2.value = text\n"
        "%5    }\n"
     "%5}\n");

     m_out << text.arg(item->getLabel())//%1
                  .arg(getId(item))
                  .arg(item->textColor().name(QColor::HexArgb))
                  .arg(item->bgColor().name(QColor::HexArgb))
                  .arg(m_indenSpace)//%5
              .arg(m_isTable?QStringLiteral(""):QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)));

    return true;
}

bool QmlGeneratorVisitor::generateFuncButton(Field *item)
{
    if(!item)
        return false;

    QString text("%6DiceButton {//%1\n"
        "%7"
        "%6    command: %2.value\n"
        "%6    text: %2.label\n"
        "%6    textColor: \"%3\"\n"
        "%6    color: \"%4\"\n"+
        getPageManagement(item,m_indenSpace)+
        "%6    readOnly: %2.readOnly\n"+
        getToolTip(item)+
        generatePosition(item) +
        generateAlignment(item) +
        generateFont(item->font()) +
        "%6    onClicked: {\n"
        "%6        %5\n"
        "%6    }\n"
     "%6}\n");

     m_out << text.arg(item->getLabel())//%1
                   .arg(getId(item))
                   .arg(item->textColor().name(QColor::HexArgb))
                   .arg(item->bgColor().name(QColor::HexArgb))
                   .arg(item->value())//%5
                   .arg(m_indenSpace)
              .arg(m_isTable?QStringLiteral(""):QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)));

    return true;
}

bool QmlGeneratorVisitor::generateDiceButton(Field *item)
{

    if(!item)
        return false;

    QString text("%6DiceButton {//%1\n"
        "%7"
        "%6    command: %2.value\n"
        "%6    text: %2.label\n"
        "%6    textColor: \"%3\"\n"
        "%6    color: \"%4\"\n"+
        getPageManagement(item,m_indenSpace)+
        "%6    readOnly: %2.readOnly\n"+
        getToolTip(item)+
        generatePosition(item) +
        generateAlignment(item) +
        generateFont(item->font()) +
        "%6    onClicked:rollDiceCmd(%2.value,%5)\n"
     "%6}\n");

     m_out << text.arg(item->getLabel())//%1
                   .arg(getId(item))
                   .arg(item->textColor().name(QColor::HexArgb))
                   .arg(item->bgColor().name(QColor::HexArgb))
                   .arg(item->getAliasEnabled() ? "true" : "false")//%5
                   .arg(m_indenSpace)
                   .arg(m_isTable?QStringLiteral(""):QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)));

    return true;
}

bool QmlGeneratorVisitor::generateImage(Field *item)
{
    if(!item)
        return false;

    QString text("%5ImageField {//%1\n"
        "%6"
        "%5    source: %2.value\n"
        "%5    textColor:\"%3\"\n"
        "%5    color: \"%4\"\n"+
        getPageManagement(item,m_indenSpace)+
        "%5    readOnly: %2.readOnly\n"+
        generatePosition(item) +
     "%5}\n");

     m_out << text.arg(item->getLabel())//%1
                   .arg(getId(item))
                   .arg(item->textColor().name(QColor::HexArgb))
                   .arg(item->bgColor().name(QColor::HexArgb))
                   .arg(m_indenSpace)//%5
              .arg(m_isTable?QStringLiteral(""):QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)));

    return true;
}

bool QmlGeneratorVisitor::generateTable(Field *item)
{
    auto tablefield = dynamic_cast<TableField*>(item);
    if(!tablefield)
        return false;

    tablefield->fillModel();

    QString node(
    "%1Table{//%2\n"
    "%1    id: _%3list\n"
    "%1    field: %3\n"+
    generatePosition(item)+
    getPageManagement(item,m_indenSpace)+
    "%1    maxRow:%4\n"
    "%1    model: %3.model\n"
    "%1    delegate: RowLayout {\n"
    "%1        height: _%3list.height/_%3list.maxRow\n"
    "%1        width:  _%3list.width\n"
    "%1        spacing:0\n");

    QString end("%1    }\n"
    "%1}\n");

    m_out << node.arg(m_indenSpace)
             .arg(item->getLabel())
             .arg(tablefield->getId())
             .arg(tablefield->getMaxVisibleRowCount());

    QmlGeneratorVisitor visitor(m_out,tablefield->getRoot());
    visitor.setIndentation(m_indentation+2);
    visitor.setIsTable(true);
    visitor.generateCharacterSheetItem();

    m_out << end.arg(m_indenSpace);

    return true;

}

bool QmlGeneratorVisitor::generateChangePageBtn(Field *item, bool next)
{
    if(!item)
        return false;


    QString node(
    "%1PageButton{//%2\n"
    "%1    next:%3\n"
    "%1    text: %4.value\n"+
    getToolTip(item)+
    generatePosition(item)+
    getPageManagement(item,m_indenSpace)+
    "%1    showImage:true\n"
    "%1    onClicked: {\n"
    "%1        root.page += next ? 1 : -1\n"
    "%1    }\n"
    "%1}\n");



    m_out << node.arg(m_indenSpace)
             .arg(item->getLabel())
             .arg(next ? "true": "false").arg(getId(item));

    return true;
}

bool QmlGeneratorVisitor::generateWebPage(Field* item)
{
    if(!item)
        return false;

    QString node(
    "%1WebView{//%2\n"
    "%4"
    "%1    url:%3.value\n"+
    getToolTip(item)+
    generatePosition(item)+
    getPageManagement(item,m_indenSpace)+
    "%1    readOnly: %3.readOnly\n"
    "%1}\n");



    m_out << node.arg(m_indenSpace)
             .arg(item->getLabel())
             .arg(getId(item))
             .arg(m_isTable?QStringLiteral(""):QStringLiteral("%1    id: _%2\n").arg(m_indenSpace).arg(getId(item)));

    return true;
}

QString QmlGeneratorVisitor::generatePosition(Field *item)
{
    if(m_isTable)
    {
        QString op("%2    Layout.fillHeight: true\n"
        "%2    Layout.preferredWidth: %1*root.realscale\n");

                return op.arg(item->getWidth()).arg(m_indenSpace);
    }
    QString positionLines(
    "%5    x:%1*root.realscale\n"
    "%5    y:%2*root.realscale\n"
    "%5    width:%3*root.realscale\n"
    "%5    height:%4*root.realscale\n");

    return positionLines
            .arg(item->getX())
            .arg(item->getY())
            .arg(item->getWidth())
            .arg(item->getHeight())
            .arg(m_indenSpace);


}

QString QmlGeneratorVisitor::generateAlignment(Field *item)
{
    QPair<QString,QString> pair = item->getTextAlign();
    QString alignments("%3    hAlign: %1\n"
                       "%3    vAlign: %2\n");

    return alignments.arg(pair.first).arg(pair.second).arg(m_indenSpace);
}

QString QmlGeneratorVisitor::generateFont(QFont font)
{
    QString fontStr("%8    font.family:  \"%1\"\n"
    "%8    font.bold:    %2\n"
    "%8    font.italic:  %3\n"
    "%8    font.underline: %4\n"
    "%8    font.pointSize: %5\n"
    "%8    font.overline: %6\n"
    "%8    font.strikeout: %7\n");

    return fontStr.arg(font.family())
            .arg(font.bold()?QStringLiteral("true"):QStringLiteral("false"))
            .arg(font.italic()?QStringLiteral("true"):QStringLiteral("false"))
            .arg(font.underline()?QStringLiteral("true"):QStringLiteral("false"))
            .arg(font.pointSize())
            .arg(font.overline()?QStringLiteral("true"):QStringLiteral("false"))
            .arg(font.strikeOut()?QStringLiteral("true"):QStringLiteral("false"))
            .arg(m_indenSpace);
}


int QmlGeneratorVisitor::indentation() const
{
    return m_indentation;
}

void QmlGeneratorVisitor::setIndentation(int indentation)
{
    m_indentation = indentation;
    m_indenSpace.clear();
    for(int i = 0; i < indentation*4; ++i)
    {
        m_indenSpace+=" ";
    }
}

QString QmlGeneratorVisitor::getId(Field* item)
{
    QString result=item->getId();
    if(m_isTable)
    {
        result = item->getLabel();
    }
    return result;
}

QString QmlGeneratorVisitor::getToolTip(Field* item)
{
    QString tooltip = item->getTooltip();
    return QStringLiteral("%1    tooltip:\"%2\"\n").arg(m_indenSpace).arg(tooltip);
}
