#ifndef QMLGENERATORVISITOR_H
#define QMLGENERATORVISITOR_H

#include <QTextStream>

class CharacterSheetItem;
class Field;
class QmlGeneratorVisitor
{
public:
    QmlGeneratorVisitor(QTextStream& out, CharacterSheetItem* rootItem);

    bool isTable() const;
    void setIsTable(bool isTable);

    bool generateCharacterSheetItem();
    bool generateQmlCodeForRoot();

    int indentation() const;
    void setIndentation(int indentation);

protected:
    bool generateTextInput(Field* item);
    bool generateTextArea(Field* item);
    bool generateTextField(Field* item);
    bool generateSelect(Field* item);
    bool generateCheckBox(Field* item);
    bool generateFuncButton(Field* item);
    bool generateDiceButton(Field* item);
    bool generateImage(Field* item);
    bool generateWebPage(Field* item);
    bool generateTable(Field* item);
    bool generateChangePageBtn(Field* item, bool next);
    bool generateLabelField(Field* item);

    QString generatePosition(Field* item);
    QString generateAlignment(Field* item);
    QString generateFont(QFont font);

    QString getId(Field* item);

    QString getToolTip(Field* item);

private:
    QTextStream& m_out;
    CharacterSheetItem* m_root= nullptr;
    bool m_isTable= false;
    int m_indentation= 1;
    QString m_indenSpace;
};

#endif // QMLGENERATORVISITOR_H
