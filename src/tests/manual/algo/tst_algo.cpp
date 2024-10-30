#include <QCoreApplication>
#include <QDebug>
#include <QList>
#include <random>

int m_groupValue= 5;

class DieGroup : public QList<qint64>
{
public:
    int getSum() const;
    void removeValue(DieGroup);

    int getLost() const;

    qint64 getExceptedValue() const;
    void setExceptedValue(qint64 exceptedValue);

    void sort();

private:
    qint64 m_exceptedValue= 0;
};

int DieGroup::getSum() const
{
    int sum= 0;
    for(int i : *this)
    {
        sum+= i;
    }
    return sum;
}

void DieGroup::sort()
{
    std::sort(std::begin(*this), std::end(*this), std::greater<qint64>());
}

void DieGroup::removeValue(DieGroup i)
{
    for(auto x : i)
    {
        removeOne(x);
    }
}

int DieGroup::getLost() const
{
    return getSum() - m_exceptedValue;
}

qint64 DieGroup::getExceptedValue() const
{
    return m_exceptedValue;
}

void DieGroup::setExceptedValue(qint64 exceptedValue)
{
    m_exceptedValue= exceptedValue;
}

bool composeWithPrevious(DieGroup previous, qint64 first, qint64 current, DieGroup& addValue)
{
    if(previous.getSum() + first + current == m_groupValue)
    {
        addValue.append(previous);
        addValue.append(first);
        addValue.append(current);
        return true;
    }

    if(previous.isEmpty())
        return false;

    int maxComboLength= previous.size();
    bool hasReachMax= false;

    QList<DieGroup> possibleUnion;
    for(auto va : previous)
    {
        DieGroup dieG;
        dieG.append(va);
        possibleUnion.append(dieG);
    }

    while(!hasReachMax)
    {
        auto tmpValues= previous;
        QList<DieGroup> possibleTmp;
        for(auto& diaG : possibleUnion)
        {
            if(tmpValues.isEmpty())
                break;
            tmpValues.removeValue(diaG);

            for(auto& value : tmpValues)
            {
                DieGroup dia;
                dia.append(diaG);
                dia.append(value);
                if(dia.size() >= maxComboLength - 1)
                    hasReachMax= true;
                else
                    possibleTmp.append(dia);
            }
        }
        if(possibleTmp.isEmpty())
            hasReachMax= true;
        else
        {
            possibleTmp.append(possibleUnion);
            possibleUnion= possibleTmp;
        }
    }
    std::sort(possibleUnion.begin(), possibleUnion.end(),
              [=](const DieGroup& a, const DieGroup& b) { return a.getLost() > b.getLost(); });
    bool found= false;
    for(int i= 0; (!found && i < possibleUnion.size()); ++i)
    {
        auto& value= possibleUnion.at(i);
        if(value.getSum() + current + first >= m_groupValue)
        {
            addValue << value << current << first;
            found= true;
        }
    }
    return found;
}

QList<DieGroup> currentAlgo(DieGroup values)
{
    if(values.isEmpty())
        return {};

    auto first= values.takeFirst();

    QList<DieGroup> result;
    QMap<qint64, DieGroup> loseMap;
    if(first >= m_groupValue)
    {
        DieGroup group;
        group << first;
        loseMap[0]= group;
    }
    else
    {
        auto it= values.rbegin();
        bool foundPerfect= false;
        qint64 cumuledValue= 0;
        DieGroup previousValue;
        while((values.rend() != it) && !foundPerfect)
        {
            if(first + *it == m_groupValue)
            {
                foundPerfect= true;
                DieGroup group;
                group << first << *it;
                loseMap[0]= group;
            }
            else if(first + *it > m_groupValue)
            {
                DieGroup group;
                group << first << *it;
                loseMap[first + *it - m_groupValue]= group;
            }
            else if(first + *it + cumuledValue == m_groupValue)
            {
                DieGroup group;
                group << first << *it << previousValue;
                foundPerfect= true;
                loseMap[0]= group;
            }
            else if(first + *it + cumuledValue > m_groupValue)
            {
                DieGroup group;
                group.setExceptedValue(m_groupValue);
                auto b= composeWithPrevious(previousValue, first, *it, group);
                if(b)
                    loseMap[group.getLost()]= group;
            }
            previousValue << *it;
            cumuledValue+= *it;
            ++it;
        }
    }
    if(!loseMap.isEmpty())
    {
        DieGroup die= loseMap.first();
        result.append(die);
        DieGroup valueToRemove= die;
        if(!valueToRemove.isEmpty())
        {
            valueToRemove.removeFirst();
            values.removeValue(valueToRemove);

            if(values.getSum() >= m_groupValue)
            {
                result.append(currentAlgo(values));
            }
        }
    }
    return result;
}

DieGroup findPerfect(DieGroup values, int count, int currentValue)
{
    for(auto it= values.rbegin(); it != values.rend(); ++it)
    {
        DieGroup g;
        for(int i= 0; i < count && (it + i) != values.rend(); ++i)
            g << *(it + i);

        if(currentValue + g.getSum() == m_groupValue)
            return g;
    }
    return {};
}

DieGroup findCombo(DieGroup values, int count, int currentValue)
{
    for(auto it= values.rbegin(); it != values.rend(); ++it)
    {
        DieGroup g;
        for(int i= 0; i < count && (it + i) != values.rend(); ++i)
            g << *(it + i);

        if(currentValue + g.getSum() > m_groupValue)
            return g;
    }
    return {};
}

QList<DieGroup> newAlgo(DieGroup values)
{
    if(values.isEmpty())
        return {};

    bool bigger= true;
    QMap<qint64, DieGroup> loseMap;
    QList<DieGroup> result;
    do
    {
        auto it= std::begin(values);
        bigger= (*it >= m_groupValue);
        if(!bigger)
            continue;

        DieGroup group;
        group << *it;
        result << group;
        values.remove(0);

    } while(bigger && !values.isEmpty());

    auto remaining= values;
    while(remaining.getSum() >= m_groupValue)
    {
        if(remaining.isEmpty())
            break;

        QList<qint64> useless;
        bool perfect= false;
        // find perfect
        do
        {
            auto base= remaining[0];
            remaining.removeFirst();
            perfect= false;
            for(int i= 1; i <= remaining.size() && !perfect; ++i)
            {
                auto foundValues= findPerfect(remaining, i, base);
                perfect= !foundValues.isEmpty();
                if(perfect)
                {
                    remaining.removeValue(foundValues);
                    foundValues.prepend(base);
                    foundValues.sort();
                    result.append(foundValues);
                }
            }

            if(!perfect)
                useless.append(base);

        } while(!remaining.isEmpty());

        remaining << useless;

        // find group with lost
        bool found= false;
        if(remaining.isEmpty())
            break;
        do
        {
            auto base= remaining[0];
            remaining.removeFirst();
            found= false;
            for(int i= 1; i <= remaining.size() && !found; ++i)
            {

                auto foundValues= findCombo(remaining, i, base);

                found= !foundValues.isEmpty();
                if(found)
                {
                    remaining.removeValue(foundValues);
                    foundValues.append(base);
                    foundValues.sort();
                    foundValues.setExceptedValue(m_groupValue);

                    loseMap[foundValues.getLost()]= foundValues;
                    result.append(foundValues);
                }
            }
        } while(found && !remaining.isEmpty());
    }

    return result;
}

int generateNumber()
{
    static std::mt19937_64 mt(std::chrono::system_clock::now().time_since_epoch().count());
    static std::uniform_int_distribution<int> greenDist(-3, 10);
    return greenDist(mt);
}

bool isEqual(const QList<DieGroup>& a, const QList<DieGroup>& b)
{
    if(a.size() != b.size())
        return false;

    bool res= std::all_of(std::begin(a), std::end(a),
                          [b](const DieGroup& group)
                          {
                              auto it= std::find_if(std::begin(b), std::end(b),
                                                    [group](const DieGroup& bvalue) { return group == bvalue; });

                              return it != std::end(b);
                          });

    if(res)
        return res;

    auto aRes
        = std::accumulate(std::begin(a), std::end(a), 0, [](int a, const DieGroup& val) { return a + val.getLost(); });
    auto bRes
        = std::accumulate(std::begin(b), std::end(b), 0, [](int a, const DieGroup& val) { return a + val.getLost(); });

    return aRes == bRes;
}

int main(int, char*[])
{

    int count= generateNumber() + generateNumber();
    DieGroup data;
    for(int i= 0; i < count; ++i)
    {
        data << generateNumber();
    }
    // data << 4 << 4 << 3 << 2 << 1;
    std::sort(std::begin(data), std::end(data), std::greater<qint64>());
    auto res= currentAlgo(data);
    auto res2= newAlgo(data);

    /*std::sort(std::begin(res), std::end(res),
              [](const DieGroup& a, const DieGroup& b) { return a.getLost() > b.getLost(); });
    std::sort(std::begin(res2), std::end(res2),
              [](const DieGroup& a, const DieGroup& b) { return a.getLost() > b.getLost(); });*/
    qDebug() << "\n";
    qDebug() << "\n";
    qDebug() << "\n";

    if(isEqual(res, res2))
        qDebug() << "Same Result";
    else
        qDebug() << "Results are different \ndata:" << data << "\nres:" << res << "\nres2" << res2;

    // qDebug() << "res:" << res << "res2:" << res2;

    return 0;
}
