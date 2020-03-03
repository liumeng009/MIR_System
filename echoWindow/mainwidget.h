#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QStandardItemModel>
#include <QListView>
#include "chartitemdelegte.h"
#include "chartmodel.h"
#include "chartform.h"

namespace Ui {
class ChartWidget;
}

class ChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChartWidget(QWidget *parent = 0);
    ~ChartWidget();

    //单例依赖
    ChartForm * chartForm = nullptr;

private:
    Ui::ChartWidget *ui;

signals:
    void DataChanged();

public slots:
    void CancelFunc();
    void SubmitFunc();
    void SubmitFuncReply();

    void UpdateTheView();

};

class ChartWidgetFactory
{
public:
    static ChartWidget * instance(QWidget *parent = nullptr)
    {
        if (m_pInstance == NULL)
        {
            m_pInstance = new ChartWidget(parent);
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
    static ChartWidget * m_pInstance;
};

#endif // CHARTWIDGET_H
