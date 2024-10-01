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
    QmlGeneratorVisitor();

    QByteArray result() const;

    bool isTable() const;
    void setIsTable(bool isTable);


    int indentation() const;
    void setIndentation(int indentation);

    QString generateSheet(const QString& imports,
                          const QString& headCode,
                          const QString& bottomCode,
                          int pageMax,
                          const QString& mainItem,
                          const QString& source,
                          int width,
                          int height,
                          const QString& baseWidth,
                          bool pageAdapt,
                          TreeSheetItem* m_root);

protected:
    QString generateTreeSheetItem();
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


    QStringList generateTableDelegate(TableFieldController* item, QStringList &columnWidths);
    QString getId(FieldController* item);

private:    
    QByteArray m_result;
    TreeSheetItem* m_root= nullptr;
    bool m_isTable= false;
    int m_indentation= 1;
    QString m_indenSpace;
};

#endif // QMLGENERATORVISITOR_H
