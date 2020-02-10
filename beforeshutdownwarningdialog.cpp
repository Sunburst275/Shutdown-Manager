#include "beforeshutdownwarningdialog.h"
#include "ui_beforeshutdownwarningdialog.h"

BeforeShutdownWarningDialog::BeforeShutdownWarningDialog(int minutesBeforeShutdown, bool* acknowledge, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BeforeShutdownWarningDialog),
    acknowledgeReturn(acknowledge)
{
    ui->setupUi(this);

    std::stringstream ss;
    ss << "Warning:\n";
    ss << "Your computer will be shut down in less than " << minutesBeforeShutdown << " minutes.";

    ui->warningLabel->setText(QString::fromStdString(ss.str()));
}

BeforeShutdownWarningDialog::~BeforeShutdownWarningDialog()
{
    delete ui;
}

void BeforeShutdownWarningDialog::on_buttonBox_accepted()
{
    *acknowledgeReturn = true;
    this->close();
}
