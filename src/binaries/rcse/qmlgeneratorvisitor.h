#ifndef QMLGENERATORVISITOR_H
#define QMLGENERATORVISITOR_H

#include <QFont>
#include <QTextStream>

class TreeSheetItem;
class FieldController;
class TableFieldController;
class QmlGeneratorVisitor
{
public:
    QmlGeneratorVisitor(TreeSheetItem* rootItem);

    QByteArray result() const;

    bool isTable() const;
    void setIsTable(bool isTable);

    bool generateTreeSheetItem();
    //bool generateQmlCodeForRoot();

    int indentation() const;
    void setIndentation(int indentation);

protected:
    bool generateTextInput(QTextStream& out, FieldController* item, bool isInsideTable);
    bool generateTextArea(QTextStream& out,FieldController* item, bool insideTable);
    bool generateTextField(QTextStream& out,FieldController* item, bool isInsideTable);
    bool generateSelect(QTextStream& out,FieldController* item, bool isInsideTable);
    bool generateCheckBox(QTextStream& out,FieldController* item, bool isInsideTable);
    bool generateFuncButton(QTextStream& out,FieldController* item, bool isInsideTable);
    bool generateDiceButton(QTextStream& out,FieldController* item, bool isInsideTable);
    bool generateImage(QTextStream& out,FieldController* item, bool isInsideTable);
    bool generateWebPage(QTextStream& out,FieldController* item, bool isInsideTable);
    bool generateTable(QTextStream& out,FieldController* item);
    bool generateChangePageBtn(QTextStream& out,FieldController* item, bool next);
    bool generateLabelField(QTextStream& out,FieldController* item, bool isInsideTable);
    bool generateSlider(QTextStream& out,FieldController* item, bool isInsideTable);
    bool generateHidden(QTextStream& out,FieldController* item, bool isInsideTable);


    QStringList generateTableDelegate(TableFieldController* item);
    //QString generatePosition(FieldController* item);
    //QString generateAlignment(FieldController* item);
    //QString generateFont(const QFont& font, bool fitfont);
    QString getId(FieldController* item);
    //QString getToolTip(FieldController* item);

private:    
    QByteArray m_result;
    TreeSheetItem* m_root= nullptr;
    bool m_isTable= false;
    int m_indentation= 1;
    QString m_indenSpace;
};

#endif // QMLGENERATORVISITOR_H
