#include "applyform.h"

ApplyUnit::ApplyUnit(QObject *parent) : QObject(parent)
{
}
ApplyUnit::~ApplyUnit()
{
    //qDebug()<<"applyunit delete"+QString::number((unsigned int)this,16);
    //qDebug()<<"apply unit delete";
}

/*
 * 清除对象需要格外注意，因为删除过程中有可能由于data而异常！以后更改
*/
void ApplyUnit::ClearData()
{
    boxNum = 0;
    bingqu = QStringLiteral("");
    MIRIP = QStringLiteral("");
    MIRPORT = 8888;
    enabled = 1;
    checked = 0;

    foreach (DrawerUnit * drawer, DrawerUnitList)
    {
        //先清除drawer中的cell
        foreach (CellUnit * cell, drawer->CellUnitList)
        {
            cell->deleteLater();
        }
        drawer->CellUnitList.clear();

        //再清除drawer
        drawer->deleteLater();
    }
    DrawerUnitList.clear();
}
void ApplyUnit::PrintData()
{
    qDebug()<<"箱体号"<<boxNum;
    qDebug()<<"病区"<<bingqu;
    qDebug()<<"IP"<<MIRIP;
    qDebug()<<"port"<<MIRPORT;
    qDebug()<<"enabled"<<enabled;
    qDebug()<<"checked"<<checked;

    foreach (DrawerUnit * drawer, DrawerUnitList)
    {
        qDebug()<<"仓格号"<<drawer->DrawerNum;
        qDebug()<<"背景图"<<drawer->layoutFile;
        qDebug()<<"品类名"<<drawer->categoryName;
        qDebug()<<"enabled"<<drawer->enabled;
        qDebug()<<"checked"<<drawer->checked;

        foreach (CellUnit * cell, drawer->CellUnitList)
        {
            qDebug()<<"单元格号"<<cell->CellNum;
            qDebug()<<"坐标X"<<cell->pos_leftup_X;
            qDebug()<<"坐标Y"<<cell->pos_leftup_Y;
            qDebug()<<"宽度"<<cell->widget_width;
            qDebug()<<"高度"<<cell->widget_height;
            qDebug()<<"规格名"<<cell->specsName;
            qDebug()<<"库存"<<cell->stockAmount;
            qDebug()<<"增量"<<cell->increaseAmount;
            qDebug()<<"预估增量"<<cell->estimateStockAmount;
            qDebug()<<"参考"<<cell->referStockAmount;
            qDebug()<<"可补数量"<<cell->supplyAmount;
            qDebug()<<"缺货数量"<<cell->lackAmount;
            qDebug()<<"缺货状态"<<cell->lackState;
            qDebug()<<"enabled"<<cell->enabled;
            qDebug()<<"checked"<<cell->checked;
        }
    }
}
void ApplyUnit::FromJson(QJsonObject json)
{
    bool result;
    QStringList keys;
    QJsonDocument jsonDmt;
    QJsonObject jBoxObj,jDrawerObj,jCellObj;
    QJsonArray subArray,thirdArray;
    DrawerUnit * pDrawer;
    CellUnit * pCell;

    qDebug()<<"from json"<<json;
    //清空applyform 注意！
    ClearData();

    //解析JSON文件 jBoxObj
    jBoxObj = json;

    //开始输出为applyForm
    bingqu = jBoxObj.value("bingqu").toString();
    boxNum = jBoxObj.value("boxNum").toInt();
    MIRIP = jBoxObj.value("MIR_IP").toString();
    MIRPORT = jBoxObj.value("MIR_PORT").toInt();
    enabled = jBoxObj.value("enabled").toInt();
    checked = jBoxObj.value("checked").toInt();

    //如果有drawers
    if(jBoxObj.contains("drawers"))
    {
        //提取第二层 得到数组-》遍历数组-》得到内容-》添加到applyForm
        subArray = jBoxObj.value("drawers").toArray();

        if(subArray.isEmpty())
            return;

        for(int i = 0; i< subArray.size(); i++)
        {
            jDrawerObj = subArray.at(i).toObject();

            //obj数据提取
            pDrawer = new DrawerUnit(this);
            //qDebug()<<"DrawerUnit create"+QString::number((unsigned int)this,16);
            pDrawer->DrawerNum = jDrawerObj.value("drawerNum").toInt();
            pDrawer->layoutFile = jDrawerObj.value("layoutFile").toString();
            pDrawer->categoryName = jDrawerObj.value("categoryName").toString();
            pDrawer->enabled = jDrawerObj.value("enabled").toInt();
            pDrawer->checked = jDrawerObj.value("checked").toInt();


            //子循环提取第三层 得到数组-》遍历数组-》得到内容-》添加到applyForm
            if(jDrawerObj.contains("cells"))
            {
                thirdArray = jDrawerObj.value("cells").toArray();

                if(thirdArray.isEmpty())
                    return;

                for(int i = 0; i< thirdArray.size(); i++)
                {
                    jCellObj = thirdArray.at(i).toObject();

                    //obj数据提取
                    pCell = new CellUnit(pDrawer);
                    //qDebug()<<"CellUnit create"+QString::number((unsigned int)this,16);
                    pCell->CellNum = jCellObj.value("CellNum").toInt();
                    pCell->pos_leftup_X = jCellObj.value("pos_leftup_X").toInt();
                    pCell->pos_leftup_Y = jCellObj.value("pos_leftup_Y").toInt();
                    pCell->widget_width = jCellObj.value("widget_width").toInt();
                    pCell->widget_height = jCellObj.value("widget_height").toInt();
                    pCell->specsName = jCellObj.value("specsName").toString();
                    pCell->stockAmount = jCellObj.value("stockAmount").toInt();
                    pCell->increaseAmount = jCellObj.value("increaseAmount").toInt();
                    pCell->estimateStockAmount = jCellObj.value("estimateStockAmount").toInt();
                    pCell->referStockAmount = jCellObj.value("referStockAmount").toInt();
                    pCell->supplyAmount = jCellObj.value("supplyAmount").toInt();
                    pCell->lackAmount = jCellObj.value("lackAmount").toInt();
                    pCell->lackState = jCellObj.value("lackState").toInt();
                    pCell->enabled = jCellObj.value("enabled").toInt();
                    pCell->checked = jCellObj.value("checked").toInt();

                    pDrawer->CellUnitList.append(pCell);
                }
            }

            DrawerUnitList.append(pDrawer);
        }
    }


    qDebug()<<"end";
    //将解析后的文件生成applyform
}
QJsonObject ApplyUnit::ToJson()
{
    QJsonObject jBoxObj,jDrawerObj,jCellObj;    //箱体，抽屉，单元格
    QJsonArray subArray,thirdArray; //第二层、第三层


    //从applyForm输出Json object
    jBoxObj.insert("bingqu", QJsonValue(bingqu));
    jBoxObj.insert("boxNum", QJsonValue(boxNum));
    jBoxObj.insert("MIR_IP", QJsonValue(MIRIP));
    jBoxObj.insert("MIR_PORT", QJsonValue(MIRPORT));
    jBoxObj.insert("enabled", QJsonValue(enabled));
    jBoxObj.insert("checked", QJsonValue(checked));

    //遍历applyUnit的drawerlist
    if(!DrawerUnitList.isEmpty())
    {
        for(int i = 0; i < DrawerUnitList.size(); i++)
        {
            jDrawerObj = QJsonObject(); //清空

            jDrawerObj.insert("drawerNum",
                        DrawerUnitList.at(i)->DrawerNum);
            jDrawerObj.insert("layoutFile",
                        DrawerUnitList.at(i)->layoutFile);
            jDrawerObj.insert("categoryName",
                        DrawerUnitList.at(i)->categoryName);
            jDrawerObj.insert("enabled",
                        DrawerUnitList.at(i)->enabled);
            jDrawerObj.insert("checked",
                        DrawerUnitList.at(i)->checked);

            //处理cells后 才算处理完
            thirdArray = QJsonArray();  //清空
            if(!DrawerUnitList.at(i)->CellUnitList.isEmpty())
            {
                for(int j = 0; j < DrawerUnitList.at(i)->CellUnitList.size(); j++)
                {
                    jCellObj = QJsonObject();       //清空

                    jCellObj.insert("CellNum",
                                DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->CellNum);
                    jCellObj.insert("pos_leftup_X",
                                DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->pos_leftup_X);
                    jCellObj.insert("pos_leftup_Y",
                                DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->pos_leftup_Y);
                    jCellObj.insert("widget_width",
                                DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->widget_width);
                    jCellObj.insert("widget_height",
                                DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->widget_height);
                    jCellObj.insert("specsName",
                                DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->specsName);
                    jCellObj.insert("stockAmount",
                                DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->stockAmount);
                    jCellObj.insert("increaseAmount",
                                DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->increaseAmount);
                    jCellObj.insert("estimateStockAmount",
                                DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->estimateStockAmount);
                    jCellObj.insert("referStockAmount",
                                DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->referStockAmount);
                    jCellObj.insert("supplyAmount",
                                DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->supplyAmount);
                    jCellObj.insert("lackAmount",
                                DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->lackAmount);
                    jCellObj.insert("lackState",
                                DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->lackState);
                    jCellObj.insert("enabled",
                                DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->enabled);
                    jCellObj.insert("checked",
                                DrawerUnitList.at(i)\
                                ->CellUnitList.at(j)->checked);

                    thirdArray.append(jCellObj);
                }
            }
            else
            {
                //没有就不添加了
            }
            jDrawerObj.insert("cells",thirdArray);

            subArray.append(jDrawerObj);
        }
    }
    else
    {
        //空则不添加
    }
    jBoxObj.insert("drawers", subArray);

    /*更新applyjson*/
    return jBoxObj;
}
int ApplyUnit::GetStockAmount(int boxNum, int drawerNum, int cellNum)
{
    foreach (DrawerUnit * drawerUnit, DrawerUnitList) {
        if(drawerUnit->DrawerNum == drawerNum)
        {
            foreach (CellUnit * cellUnit, drawerUnit->CellUnitList) {
                if(cellUnit->CellNum == cellNum)
                {
                    //找到
                    return cellUnit->stockAmount;
                }
            }
        }
    }
    return -1;
}
int ApplyUnit::GetIncreaseAmount(int boxNum, int drawerNum, int cellNum)
{
    foreach (DrawerUnit * drawerUnit, DrawerUnitList) {
        if(drawerUnit->DrawerNum == drawerNum)
        {
            foreach (CellUnit * cellUnit, drawerUnit->CellUnitList) {
                if(cellUnit->CellNum == cellNum)
                {
                    //找到
                    return cellUnit->increaseAmount;
                }
            }
        }
    }
    return -1;
}
/*
 * applyUnit减去需要扣除的量
 * 输入是其他的applyUnit
 * 输出是true false
*/
bool ApplyUnit::Minus(ApplyUnit * applyUnitS)
{
    foreach (DrawerUnit * drawerUnit, applyUnitS->DrawerUnitList) {
        if(drawerUnit->checked == 1)
        {
            foreach (CellUnit * cellUnit, drawerUnit->CellUnitList) {
                if(cellUnit->checked == 1)
                {
                    if(RemoveN(applyUnitS->boxNum,
                               drawerUnit->DrawerNum,
                               cellUnit->CellNum,
                               cellUnit->increaseAmount))
                    {
                        //正确处理一项 无动作
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}
/*
 * applyUnit＜另一个applyUnit 申请物品时使用
 * 输入是其他的applyUnit
 * 输出是true false
*/
bool ApplyUnit::LessThan(ApplyUnit * applyUnitS)
{
    foreach (DrawerUnit * drawerUnit, DrawerUnitList) {
        if(drawerUnit->checked == 1)
        {
            foreach (CellUnit * cellUnit, drawerUnit->CellUnitList) {
                if(cellUnit->checked == 1)
                {
                    if(cellUnit->increaseAmount > applyUnitS->GetStockAmount(boxNum,drawerUnit->DrawerNum,cellUnit->CellNum))
                    {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}
bool ApplyUnit::AddOne(int indexBoxNum, int indexDrawerNum, int indexCellNum)
{
    if(boxNum == indexBoxNum)
    {
        foreach (DrawerUnit * drawer, DrawerUnitList)
        {
            if(drawer->DrawerNum == indexDrawerNum)
            {
                foreach (CellUnit * cell, drawer->CellUnitList)
                {
                    if(cell->CellNum == indexCellNum)
                    {
                        cell->increaseAmount++;
                        qDebug()<<GetTotalCellNum();

                        //有效性检查
                        Rulecheck();

                        //更新界面
                        PrintData();

                        //按照逻辑调用

                        return true;
                    }
                }
            }
        }
        return false;
    }
    else
        return false;
}
bool ApplyUnit::AddN(int indexBoxNum, int indexDrawerNum, int indexCellNum, int num)
{
    if(boxNum == indexBoxNum)
    {
        foreach (DrawerUnit * drawer, DrawerUnitList)
        {
            if(drawer->DrawerNum == indexDrawerNum)
            {
                foreach (CellUnit * cell, drawer->CellUnitList)
                {
                    if(cell->CellNum == indexCellNum)
                    {
                        cell->stockAmount += num;
                        qDebug()<<GetTotalCellNum();

                        //有效性检查
                        Rulecheck();

                        //更新界面
                        PrintData();

                        //按照逻辑调用

                        return true;
                    }
                }
            }
        }
        return false;
    }
    else
        return false;
}
bool ApplyUnit::SetOne(int indexBoxNum,
                     int indexDrawerNum,
                     int indexCellNum,
                     int increaseMount)
{
    if(boxNum == indexBoxNum)
    {
        foreach (DrawerUnit * drawer, DrawerUnitList)
        {
            if(drawer->DrawerNum == indexDrawerNum)
            {
                foreach (CellUnit * cell, drawer->CellUnitList)
                {
                    if(cell->CellNum == indexCellNum)
                    {
                        cell->increaseAmount = increaseMount;

                        //有效性检查
                        Rulecheck();

                        //(外)更新界面

                        //按照逻辑调用

                        return true;
                    }
                }
            }
        }
    }
    else
        return false;
}
bool ApplyUnit::RemoveOne(int indexBoxNum, int indexDrawerNum, int indexCellNum)
{
    if(boxNum == indexBoxNum)
    {
        foreach (DrawerUnit * drawer, DrawerUnitList)
        {
            if(drawer->DrawerNum == indexDrawerNum)
            {
                foreach (CellUnit * cell, drawer->CellUnitList)
                {
                    if(cell->CellNum == indexCellNum)
                    {
                        cell->increaseAmount--;
                        //有效性检查
                        Rulecheck();

                        return true;
                    }
                }
            }
        }
        return false;
    }
    else
        return false;
}
bool ApplyUnit::RemoveN(int indexBoxNum, int indexDrawerNum, int indexCellNum, int num)
{
    if(boxNum == indexBoxNum)
    {
        foreach (DrawerUnit * drawer, DrawerUnitList)
        {
            if(drawer->DrawerNum == indexDrawerNum)
            {
                foreach (CellUnit * cell, drawer->CellUnitList)
                {
                    if(cell->CellNum == indexCellNum)
                    {
                        cell->stockAmount -= num;
                        //有效性检查
                        Rulecheck();

                        return true;
                    }
                }
            }
        }
        return false;
    }
    else
        return false;
}
int ApplyUnit::GetTotalCellNum()
{
    int totalNum = 0;
    foreach (DrawerUnit * drawer, DrawerUnitList)
    {
        foreach (CellUnit * cell, drawer->CellUnitList)
        {
            totalNum += cell->increaseAmount;
        }
    }
    return totalNum;
}
int ApplyUnit::GetTotalCheckedCellNum()
{
    int totalNum = 0;
    foreach (DrawerUnit * drawer, DrawerUnitList)
    {
        foreach (CellUnit * cell, drawer->CellUnitList)
        {
            if(cell->checked == 1)
                totalNum ++;
        }
    }
    return totalNum;
}
/*
*   获得仓格的信息
*/
DrawerData ApplyUnit::GetDrawerData(int drawerNum)
{
    foreach (DrawerUnit * drawer, DrawerUnitList)
    {
        if(drawer->DrawerNum == drawerNum)
        {
            DrawerData data;
            data.checked = drawer->checked;
            data.categoryName = drawer->categoryName;
            return data;
        }
    }
}
/*
*   获得单元格的名字
*/
CellData ApplyUnit::GetCellData(int drawerNum, int cellNum)
{
    foreach (DrawerUnit * drawer, DrawerUnitList)
    {
        if(drawer->DrawerNum == drawerNum)
        {
            foreach (CellUnit * cell, drawer->CellUnitList)
            {
                if(cell->CellNum == cellNum)
                {
                    CellData data;
                    data.checked = cell->checked;
                    data.specsName = cell->specsName;
                    data.increaseMount = cell->increaseAmount;
                    data.stockMount = cell->stockAmount;
                    data.supplyAmount = cell->supplyAmount;
                    data.referStockAmount = cell->referStockAmount;
                    data.estimateStockAmount = cell->estimateStockAmount;
                    return data;
                }
            }
        }
    }
}
/*
 * 保存补充的货物/保存扣除的货物
 * 除了StockAmount!
*/
bool ApplyUnit::EStockToStock()
{
    checked = 1;
    foreach (DrawerUnit * drawer, DrawerUnitList)
    {
        if(drawer->checked == 1)
        {
            foreach (CellUnit * cell, drawer->CellUnitList)
            {
                if(cell->checked == 1)
                    cell->stockAmount = cell->estimateStockAmount;
            }
        }
    }
    return true;
}
/*
 * 清除所有计算痕迹
 * 除了StockAmount!
*/
bool ApplyUnit::RemoveAll()
{
    checked = 0;
    foreach (DrawerUnit * drawer, DrawerUnitList)
    {
        drawer->checked = 0;
        foreach (CellUnit * cell, drawer->CellUnitList)
        {
            cell->checked = 0;
            cell->increaseAmount = 0;
            cell->estimateStockAmount = 0;
        }
    }
    //(外)更新界面
}
/*
*   >0  <max check标志检查，规则检查
*/
bool ApplyUnit::Rulecheck()
{
    //1遍历 基本数字正确性
    foreach (DrawerUnit * drawer, DrawerUnitList)
    {
        foreach (CellUnit * cell, drawer->CellUnitList)
        {
            //stock保证正确
            if(cell->stockAmount < 0)
                cell->stockAmount = 0;

            //increase保证正确
            if(cell->increaseAmount < 0)
                cell->increaseAmount = 0;

            //预估实际库存量
            cell->estimateStockAmount = cell->stockAmount + cell->increaseAmount;

            //increase不能超过参考值
            if(cell->estimateStockAmount > cell->referStockAmount)
            {
                cell->estimateStockAmount = cell->referStockAmount;
                cell->increaseAmount = cell->referStockAmount - cell->estimateStockAmount;
            }

            //可补充数量
            cell->supplyAmount = cell->referStockAmount - cell->estimateStockAmount;
        }
    }
    //2遍历 逻辑正确性
    int boxCheckFlag = 0;
    foreach (DrawerUnit * drawer, DrawerUnitList)
    {
        int checkFlag = 0;
        foreach (CellUnit * cell, drawer->CellUnitList)
        {
            if(cell->increaseAmount > 0)
            {
                cell->checked = 1;
                checkFlag = 1;
            }
            else
            {
                cell->checked = 0;
            }
        }
        if(checkFlag == 1)
            drawer->checked = 1;
        else
            drawer->checked = 0;
    }
    if(boxCheckFlag == 0)
        checked = 0;
    else
        checked = 1;
}
/*
*   获得仓格的名字
*/
QVariant ApplyUnit::GetDrawerName(int drawerNum)
{
    //PrintData();
    DrawerData data;
    data.drawerNum = DrawerUnitList.at(drawerNum)->DrawerNum;
    data.checked = DrawerUnitList.at(drawerNum)->checked;
    data.categoryName = DrawerUnitList.at(drawerNum)->categoryName;
    return QVariant::fromValue(data);
    //容错判断。。带添加
    return QVariant();
}
/*
*   输入：applyUnit
*   输出：cell的列表和increasement ItemData
*/
QVector<ItemData> ApplyUnit::GetChechedCells()
{
    QVector<ItemData> vector;
    ItemData item;

    foreach (DrawerUnit * drawer, DrawerUnitList)
    {
        foreach (CellUnit * cell, drawer->CellUnitList)
        {
            //stock保证正确
            if(cell->checked == 1)
            {
                item.categoryName = drawer->categoryName;
                item.specsName =  cell->specsName;
                item.increaseMount = cell->increaseAmount;
                vector.append(item);
            }
        }
    }

    return vector;
}

/*构造与析构，*/
DrawerUnit::DrawerUnit(QObject *parent) : QObject(parent)
{
}
DrawerUnit::~DrawerUnit()
{
}
CellUnit::CellUnit(QObject *parent) : QObject(parent)
{
}
CellUnit::~CellUnit()
{
}
