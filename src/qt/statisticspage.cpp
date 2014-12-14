#include "statisticspage.h"
#include "ui_statisticspage.h"
#include "main.h"
#include "wallet.h"
#include "base58.h"
#include "clientmodel.h"
#include "syncoinrpc.h"
#include "syncoinfunction.h"
#include <sstream>
#include <string>

using namespace json_spirit;

StatisticsPage::StatisticsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StatisticsPage)
{
    ui->setupUi(this);
    
    setFixedSize(400, 420);
    

    connect(ui->node, SIGNAL(pressed()), this, SLOT(updateNet()));
    connect(ui->add, SIGNAL(pressed()), this, SLOT(nodeAdd()));
}

int heightPrevious = -1;
int connectionPrevious = -1;
int volumePrevious = -1;
double rewardPrevious = -1;
double netPawratePrevious = -1;
double pawratePrevious = -1;
double hardnessPrevious = -1;

void StatisticsPage::updateNet()
{
    std::string nodes = getNodeInfo();
    QString QNodes = QString::fromUtf8(nodes.c_str());
    ui->nodes->setText(QNodes);
}

void StatisticsPage::setModel(ClientModel *model)
{

    this->model = model;


    updateNet();
}

void StatisticsPage::nodeAdd()
{
   std::string node = ui->addnode->text().toStdString();
    bool s = addnode(node);
    QString m;
    if(s)
    {
        m = "<b><font color=green>Successfully connected to node</font></b>";
    } else {
        m = "<b><font color=red>Node connection failed</font></b>";
    }
    ui->noderesult->setText(m);
    updateNet();

}



bool addnode(std::string node)
{
    std::string strNode = node;
    strNode.append(":24241");
    CAddress addr;
    bool exists = false;
    LOCK(cs_vNodes);
    int i = vNodes.size();
    while(i > 0)
    {
        i--;
        CNodeStats stats;
        CNode* pnode = vNodes[i];
        if(pnode->addrName == strNode)
        {
            exists = true;
            return false;
        }
    }
    if(!exists)
    {
    ConnectNode(addr, strNode.c_str());
    int i1 = vNodes.size();
    while(i1 > 0)
    {
        i1--;
        CNodeStats stats;
        CNode* pnode = vNodes[i1];
        if(pnode->addrName == strNode)
        {
            return true;
        }
    }
        return false;
    }
}


std::string getNodeInfo()
{
    LOCK(cs_vNodes);
    int i = vNodes.size();
    std::string str;
    while(i > 0)
    {
        i--;
        CNodeStats stats;
        CNode* pnode = vNodes[i];
        str.append(pnode->addrName);
        str.append(" [");
        str.append(pnode->strSubVer);
        str.append("]");
        str.append("\n");
    }
    return str;
}

StatisticsPage::~StatisticsPage()
{
    delete ui;
}
