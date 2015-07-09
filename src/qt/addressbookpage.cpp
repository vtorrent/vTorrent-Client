#include "addressbookpage.h"
#include "ui_addressbookpage.h"

#include "addresstablemodel.h"
#include "optionsmodel.h"

#ifdef FULLCLIENT
#include "bitcoingui.h"
#else
#include "bitcoingui_c.h"
#endif

#include "editaddressdialog.h"
#include "csvmodelwriter.h"
#include "guiutil.h"

#include <QSortFilterProxyModel>
#include <QClipboard>
#include <QMessageBox>
#include <QMenu>

#ifdef USE_QRCODE
#include "qrcodedialog.h"
#endif

AddressBookPage::AddressBookPage(Mode mode, Tabs tab, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddressBookPage),
    model(0),
    optionsModel(0),
    mode(mode),
    tab(tab)
{
    ui->setupUi(this);

//#ifdef Q_OS_MAC // Icons on push buttons are very uncommon on Mac
    ui->newAddressButton->setIcon(QIcon());
    ui->copyToClipboard->setIcon(QIcon());
    ui->copyToClipboard_pub->setIcon(QIcon());
    ui->deleteButton->setIcon(QIcon());
//#endif

#ifndef USE_QRCODE
    ui->showQRCode->setVisible(false);
#endif

    switch(mode)
    {
    case ForSending:
        connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(accept()));
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableView->setFocus();
        break;
    case ForEditing:
        ui->buttonBox->setVisible(false);
        break;
    }
    switch(tab)
    {
    case SendingTab:
        ui->ReceiveTitle->setVisible(false);
        ui->deleteButton->setVisible(true);
        ui->signMessage->setVisible(false);
        ui->verifyMessage->setVisible(false);
        break;
    case SendingTabDialog:
        ui->AddressesTitle->setVisible(false);
        ui->ReceiveTitle->setVisible(false);
        ui->deleteButton->setVisible(true);
        ui->signMessage->setVisible(false);
        ui->verifyMessage->setVisible(false);
        break;
    case ReceivingTab:
        ui->AddressesTitle->setVisible(false);
        ui->deleteButton->setVisible(false);
        ui->signMessage->setVisible(true);
        break;
    case ReceivingTabDialog:
        ui->AddressesTitle->setVisible(false);
        ui->ReceiveTitle->setVisible(false);
        ui->deleteButton->setVisible(false);
        ui->signMessage->setVisible(true);
        break;
    }

    // Context menu actions
    QAction *copyLabelAction = new QAction(tr("Copy &Label"), this);
    QAction *copyAddressAction = new QAction(ui->copyToClipboard->text(), this);
    QAction *copyPubKeyAction = new QAction(tr("Copy &Public &Key"), this);
    QAction *editAction = new QAction(tr("&Edit"), this);
    #ifdef USE_QRCODE
        QAction *showQRCodeAction = new QAction(ui->showQRCode->text(), this);
    #endif
    QAction *signMessageAction = new QAction(ui->signMessage->text(), this);
    QAction *verifyMessageAction = new QAction(ui->verifyMessage->text(), this);
    deleteAction = new QAction(ui->deleteButton->text(), this);

    // Build context menu
    contextMenu = new QMenu();
    contextMenu->addAction(copyAddressAction);
    contextMenu->addAction(copyLabelAction);
    contextMenu->addAction(copyPubKeyAction);
    contextMenu->addAction(editAction);
    if(tab == SendingTab || tab == SendingTabDialog)
        contextMenu->addAction(deleteAction);
    contextMenu->addSeparator();
    #ifdef USE_QRCODE
        contextMenu->addAction(showQRCodeAction);
    #endif
    if(tab == ReceivingTab)
        contextMenu->addAction(signMessageAction);
    else if(tab == SendingTab || tab == SendingTabDialog)
        contextMenu->addAction(verifyMessageAction);

    // Connect signals for context menu actions
    connect(copyAddressAction, SIGNAL(triggered()), this, SLOT(on_copyToClipboard_clicked()));
    connect(copyLabelAction, SIGNAL(triggered()), this, SLOT(onCopyLabelAction()));
    connect(copyPubKeyAction, SIGNAL(triggered()), this, SLOT(onCopyPubKeyAction()));
    connect(editAction, SIGNAL(triggered()), this, SLOT(onEditAction()));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(on_deleteButton_clicked()));
    #ifdef USE_QRCODE
        connect(showQRCodeAction, SIGNAL(triggered()), this, SLOT(on_showQRCode_clicked()));
    #endif
    connect(signMessageAction, SIGNAL(triggered()), this, SLOT(on_signMessage_clicked()));
    connect(verifyMessageAction, SIGNAL(triggered()), this, SLOT(on_verifyMessage_clicked()));

    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextualMenu(QPoint)));

    // Pass through accept action from button box
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
}

AddressBookPage::~AddressBookPage()
{
    delete ui;
}

void AddressBookPage::setModel(AddressTableModel *model)
{
    this->model = model;
    if(!model)
        return;

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setDynamicSortFilter(true);
    proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    switch(tab)
    {
    case ReceivingTab:
        // Don't show stealth addresses in dialogues
        proxyModel->setFilterRole(AddressTableModel::AddressTypeRole);

        // Receive filter
        proxyModel->setFilterRole(AddressTableModel::TypeRole);
        proxyModel->setFilterFixedString(AddressTableModel::Receive);
        break;
    case ReceivingTabDialog:
        // Don't show stealth addresses in dialogues
        proxyModel->setFilterRole(AddressTableModel::AddressTypeRole);

        // Receive filter
        proxyModel->setFilterRole(AddressTableModel::TypeRole);
        proxyModel->setFilterFixedString(AddressTableModel::Receive);
        break;
    case SendingTab:
        // Don't show stealth addresses in dialogues
        proxyModel->setFilterRole(AddressTableModel::AddressTypeRole);

        // Send filter
        proxyModel->setFilterRole(AddressTableModel::TypeRole);
        proxyModel->setFilterFixedString(AddressTableModel::Send);
        break;
    case SendingTabDialog:
        // Don't show stealth addresses in dialogues
        proxyModel->setFilterRole(AddressTableModel::AddressTypeRole);

        // Send filter
        proxyModel->setFilterRole(AddressTableModel::TypeRole);
        proxyModel->setFilterFixedString(AddressTableModel::Send);
        break;
    }
    ui->tableView->setModel(proxyModel);
    ui->tableView->sortByColumn(0, Qt::AscendingOrder);

    // Set column widths
    ui->tableView->horizontalHeader()->resizeSection(
            AddressTableModel::Label, 140);
    ui->tableView->horizontalHeader()->setResizeMode(
            AddressTableModel::Address, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->resizeSection(
            AddressTableModel::Pubkey, 360);

    // Set Align/Style
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableView->horizontalHeader()->setStyleSheet("QHeaderView::section { border: 0px; padding: 4px; font-weight :bold}");

    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(selectionChanged()));

    // Select row for newly created address
    connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(selectNewAddress(QModelIndex,int,int)));

    selectionChanged();
}

void AddressBookPage::setOptionsModel(OptionsModel *optionsModel)
{
    this->optionsModel = optionsModel;
}

void AddressBookPage::on_copyToClipboard_clicked()
{
    GUIUtil::copyEntryData(ui->tableView, AddressTableModel::Address);
}

void AddressBookPage::on_copyToClipboard_pub_clicked()
{
    GUIUtil::copyEntryData(ui->tableView, AddressTableModel::Pubkey);
}

void AddressBookPage::onCopyLabelAction()
{
    GUIUtil::copyEntryData(ui->tableView, AddressTableModel::Label);
}

void AddressBookPage::onCopyPubKeyAction()
{
    GUIUtil::copyEntryData(ui->tableView, AddressTableModel::Pubkey);
}

void AddressBookPage::onEditAction()
{
    if(!ui->tableView->selectionModel())
        return;
    QModelIndexList indexes = ui->tableView->selectionModel()->selectedRows();
    if(indexes.isEmpty())
        return;

    EditAddressDialog dlg(
            (tab == SendingTab || tab == SendingTabDialog) ?
            EditAddressDialog::EditSendingAddress :
            EditAddressDialog::EditReceivingAddress);
    dlg.setModel(model);
    QModelIndex origIndex = proxyModel->mapToSource(indexes.at(0));
    dlg.loadRow(origIndex.row());
    dlg.exec();
}

void AddressBookPage::on_signMessage_clicked()
{
    QTableView *table = ui->tableView;
    QModelIndexList indexes = table->selectionModel()->selectedRows(AddressTableModel::Address);
    QString addr;

    foreach (QModelIndex index, indexes)
    {
        QVariant address = index.data();
        addr = address.toString();
    }

    emit signMessage(addr);
}

void AddressBookPage::on_verifyMessage_clicked()
{
    QTableView *table = ui->tableView;
    QModelIndexList indexes = table->selectionModel()->selectedRows(AddressTableModel::Address);
    QString addr;

    foreach (QModelIndex index, indexes)
    {
        QVariant address = index.data();
        addr = address.toString();
    }

    emit verifyMessage(addr);
}

void AddressBookPage::on_newAddressButton_clicked()
{
    if(!model)
        return;
    EditAddressDialog dlg(
            (tab == SendingTab || tab == SendingTabDialog) ?
            EditAddressDialog::NewSendingAddress :
            EditAddressDialog::NewReceivingAddress, this);
    dlg.setModel(model);
    if(dlg.exec())
    {
        newAddressToSelect = dlg.getAddress();
    }
}

void AddressBookPage::on_deleteButton_clicked()
{
    QTableView *table = ui->tableView;
    if(!table->selectionModel())
        return;
    QModelIndexList indexes = table->selectionModel()->selectedRows();
    if(!indexes.isEmpty())
    {
        table->model()->removeRow(indexes.at(0).row());
    }
}

void AddressBookPage::selectionChanged()
{
    // Set button states based on selected tab and selection
    QTableView *table = ui->tableView;
    if(!table->selectionModel())
        return;

    if(table->selectionModel()->hasSelection())
    {

        QModelIndexList indexes = table->selectionModel()->selectedRows(AddressTableModel::Address);
        QString addr;
        bool stealth_address;

        foreach (QModelIndex index, indexes)
        {
            QVariant address = index.data();
            addr = address.toString();
        }

        if (addr.length() < 35)
            stealth_address = false;
        else
            stealth_address = true;

        switch(tab)
        {
        case SendingTab:
            // In sending tab, allow deletion of selection
            ui->deleteButton->setEnabled(true);
            ui->deleteButton->setVisible(true);
            deleteAction->setEnabled(true);
            if (stealth_address == false) {
                ui->signMessage->setEnabled(false);
                ui->signMessage->setVisible(false);
                ui->verifyMessage->setEnabled(true);
                ui->verifyMessage->setVisible(true);
            } else {
                ui->signMessage->setEnabled(false);
                ui->signMessage->setVisible(false);
                ui->verifyMessage->setEnabled(false);
                ui->verifyMessage->setVisible(false);
            }
            break;
        case SendingTabDialog:
            // In sending tab, allow deletion of selection
            ui->deleteButton->setEnabled(true);
            ui->deleteButton->setVisible(true);
            deleteAction->setEnabled(true);
            if (stealth_address == false) {
                ui->signMessage->setEnabled(false);
                ui->signMessage->setVisible(false);
                ui->verifyMessage->setEnabled(true);
                ui->verifyMessage->setVisible(true);
            } else {
                ui->signMessage->setEnabled(false);
                ui->signMessage->setVisible(false);
                ui->verifyMessage->setEnabled(false);
                ui->verifyMessage->setVisible(false);
            }
            break;
        case ReceivingTab:
            // Deleting receiving addresses, however, is not allowed
            ui->deleteButton->setEnabled(false);
            ui->deleteButton->setVisible(false);
            deleteAction->setEnabled(false);
            if (stealth_address == false) {
                ui->signMessage->setEnabled(true);
                ui->signMessage->setVisible(true);
                ui->verifyMessage->setEnabled(false);
                ui->verifyMessage->setVisible(false);
            } else {
                ui->signMessage->setEnabled(false);
                ui->signMessage->setVisible(false);
                ui->verifyMessage->setEnabled(false);
                ui->verifyMessage->setVisible(false);
            }
            break;
        case ReceivingTabDialog:
            // Deleting receiving addresses, however, is not allowed
            ui->deleteButton->setEnabled(false);
            ui->deleteButton->setVisible(false);
            deleteAction->setEnabled(false);
            if (stealth_address == false) {
                ui->signMessage->setEnabled(true);
                ui->signMessage->setVisible(true);
                ui->verifyMessage->setEnabled(false);
                ui->verifyMessage->setVisible(false);
            } else {
                ui->signMessage->setEnabled(false);
                ui->signMessage->setVisible(false);
                ui->verifyMessage->setEnabled(false);
                ui->verifyMessage->setVisible(false);
            }
            break;
        }
        ui->copyToClipboard->setEnabled(true);
        ui->copyToClipboard_pub->setEnabled(true);
        ui->showQRCode->setEnabled(true);
    }
    else
    {
        ui->deleteButton->setEnabled(false);
        ui->showQRCode->setEnabled(false);
        ui->copyToClipboard->setEnabled(false);
        ui->copyToClipboard_pub->setEnabled(false);
        ui->signMessage->setEnabled(false);
        ui->verifyMessage->setEnabled(false);
    }
}

void AddressBookPage::done(int retval)
{
    QTableView *table = ui->tableView;
    if(!table->selectionModel() || !table->model())
        return;
    // When this is a tab/widget and not a model dialog, ignore "done"
    if(mode == ForEditing)
        return;

    // Figure out which address was selected, and return it
    QModelIndexList indexes = table->selectionModel()->selectedRows(AddressTableModel::Address);

    foreach (QModelIndex index, indexes)
    {
        QVariant address = table->model()->data(index);
        returnValue = address.toString();
    }

    if(returnValue.isEmpty())
    {
        // If no address entry selected, return rejected
        retval = Rejected;
    }

    QDialog::done(retval);
}

void AddressBookPage::exportClicked()
{
    // CSV is currently the only supported format
    QString filename = GUIUtil::getSaveFileName(
            this,
            tr("Export Address Book Data"), QString(),
            tr("Comma separated file (*.csv)"));

    if (filename.isNull()) return;

    CSVModelWriter writer(filename);

    // name, column, role
    writer.setModel(proxyModel);
    writer.addColumn("Label", AddressTableModel::Label, Qt::EditRole);
    writer.addColumn("Address", AddressTableModel::Address, Qt::EditRole);

    if(!writer.write())
    {
        QMessageBox::critical(this, tr("Error exporting"), tr("Could not write to file %1.").arg(filename),
                              QMessageBox::Abort, QMessageBox::Abort);
    }
}

void AddressBookPage::on_showQRCode_clicked()
{
#ifdef USE_QRCODE
    QTableView *table = ui->tableView;
    QModelIndexList indexes = table->selectionModel()->selectedRows(AddressTableModel::Address);

    foreach (QModelIndex index, indexes)
    {
        QString address = index.data().toString(), label = index.sibling(index.row(), 0).data(Qt::EditRole).toString();

        QRCodeDialog *dialog = new QRCodeDialog(address, label, tab == ReceivingTab, this);
        if(optionsModel)
            dialog->setModel(optionsModel);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog->show();
    }
#endif
}

void AddressBookPage::contextualMenu(const QPoint &point)
{
    QModelIndex index = ui->tableView->indexAt(point);
    if(index.isValid())
    {
        contextMenu->exec(QCursor::pos());
    }
}

void AddressBookPage::selectNewAddress(const QModelIndex &parent, int begin, int end)
{
    QModelIndex idx = proxyModel->mapFromSource(model->index(begin, AddressTableModel::Address, parent));
    if(idx.isValid() && (idx.data(Qt::EditRole).toString() == newAddressToSelect))
    {
        // Select row of newly created address, once
        ui->tableView->setFocus();
        ui->tableView->selectRow(idx.row());
        newAddressToSelect.clear();
    }
}
