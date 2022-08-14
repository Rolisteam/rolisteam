#ifndef QMLGENERATORVISITOR_H
#define QMLGENERATORVISITOR_H

#include <QFont>
#include <QTextStream>

class TreeSheetItem;
class FieldController;
class QmlGeneratorVisitor
{
public:
    QmlGeneratorVisitor(QTextStream& out, TreeSheetItem* rootItem);

    bool isTable() const;
    void setIsTable(bool isTable);

    bool generateTreeSheetItem();
    bool generateQmlCodeForRoot();

    int indentation() const;
    void setIndentation(int indentation);

protected:
    bool generateTextInput(FieldController* item);
    bool generateTextArea(FieldController* item);
    bool generateTextField(FieldController* item);
    bool generateSelect(FieldController* item);
    bool generateCheckBox(FieldController* item);
    bool generateFuncButton(FieldController* item);
    bool generateDiceButton(FieldController* item);
    bool generateImage(FieldController* item);
    bool generateWebPage(FieldController* item);
    bool generateTable(FieldController* item);
    bool generateChangePageBtn(FieldController* item, bool next);
    bool generateLabelField(FieldController* item);
    bool generateSlider(FieldController* item);
    bool generateHidden(FieldController* item);

    QString generatePosition(FieldController* item);
    QString generateAlignment(FieldController* item);
    QString generateFont(const QFont& font, bool fitfont);

    QString getId(FieldController* item);

    QString getToolTip(FieldController* item);

private:
    QTextStream& m_out;
    TreeSheetItem* m_root= nullptr;
    bool m_isTable= false;
    int m_indentation= 1;
    QString m_indenSpace;
};

#endif // QMLGENERATORVISITOR_H
