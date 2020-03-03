#ifndef CHARTFORM_H
#define CHARTFORM_H

#include <QObject>
#include <QList>
#include <QString>

typedef struct
{
    int boxNum;
    int drawerNum;
    int cellNum;
    QString categoryName;
    QString specsName;
    int stockAmount;
    int increaseAmount;
}ChartItemData;

class ChartItem;
class ChartForm : public QObject
{
    Q_OBJECT
public:
    explicit ChartForm(QObject *parent = nullptr);

    //单例依赖
    //BoxUnit * boxUnit = nullptr;

    //成员
    QVector<ChartItemData> chartDataList;
signals:
    void DataChanged();

public slots:
    void ClearData();
    void PrintData();
    bool RemoveAll();
    int GetTotalCellNum();
    bool Rulecheck();
    QString GetItemName(int row);
    int GetItemNum(int row);

    int AddItemNum(int row);
    int RemoveItemNum(int row);
    void UpdateFromApplyForm();
    void UpdateToApplyForm();
};

class ChartFormFactory
{
public:
    static ChartForm * instance(QObject *parent = nullptr)
    {
        if (m_pInstance == NULL)
        {
            m_pInstance = new ChartForm(parent);
        }
        return m_pInstance;
    }

    static void Release()
    {
        if (m_pInstance != NULL)
        {
            delete m_pInstance;
            m_pInstance = NULL;
        }
    }
public:
    static ChartForm * m_pInstance;
};

#endif // CHARTFORM_H
