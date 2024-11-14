#include <QPushButton>

#include "exportdialog.h"
#include "ui_exportdialog.h"

ExportDialog::ExportDialog(const QList<Backend> &backends, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ExportDialog)
{
    ui->setupUi(this);

    ui->chBoxBReference->setEnabled(backends.contains(Backend::Reference));
    ui->chBoxBResvg->setEnabled(backends.contains(Backend::Resvg));
    ui->chBoxBChrome->setEnabled(backends.contains(Backend::Chrome));
    ui->chBoxBFirefox->setEnabled(backends.contains(Backend::Firefox));
    ui->chBoxBSafari->setEnabled(backends.contains(Backend::Safari));
    ui->chBoxBBatik->setEnabled(backends.contains(Backend::Batik));
    ui->chBoxBInkscape->setEnabled(backends.contains(Backend::Inkscape));
    ui->chBoxBLibrsvg->setEnabled(backends.contains(Backend::Librsvg));
    ui->chBoxBQtSvg->setEnabled(backends.contains(Backend::QtSvg));
    ui->chBoxBLadybird->setEnabled(backends.contains(Backend::Ladybird));

    ui->chBoxBResvg->setChecked(backends.contains(Backend::Resvg));
    ui->chBoxBChrome->setChecked(backends.contains(Backend::Chrome));
    ui->chBoxBFirefox->setChecked(backends.contains(Backend::Firefox));
    ui->chBoxBSafari->setChecked(backends.contains(Backend::Safari));
    ui->chBoxBBatik->setChecked(backends.contains(Backend::Batik));
    ui->chBoxBInkscape->setChecked(backends.contains(Backend::Inkscape));
    ui->chBoxBLibrsvg->setChecked(backends.contains(Backend::Librsvg));
    ui->chBoxBQtSvg->setChecked(backends.contains(Backend::QtSvg));
    ui->chBoxBLadybird->setChecked(backends.contains(Backend::Ladybird));

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Export");

    adjustSize();
}

ExportDialog::~ExportDialog()
{
    delete ui;
}

ExportDialog::Options ExportDialog::options() const
{
    Options opt;
    opt.showTitle = ui->chBoxShowTitle->isChecked();
    opt.indicateStatus = ui->chBoxIndicateStatus->isChecked();
    opt.showDiff = ui->chBoxShowDiff->isChecked();

    if (ui->chBoxBReference->isChecked())   { opt.backends << Backend::Reference; }
    if (ui->chBoxBResvg->isChecked())       { opt.backends << Backend::Resvg; }
    if (ui->chBoxBChrome->isChecked())      { opt.backends << Backend::Chrome; }
    if (ui->chBoxBFirefox->isChecked())     { opt.backends << Backend::Firefox; }
    if (ui->chBoxBSafari->isChecked())      { opt.backends << Backend::Safari; }
    if (ui->chBoxBBatik->isChecked())       { opt.backends << Backend::Batik; }
    if (ui->chBoxBInkscape->isChecked())    { opt.backends << Backend::Inkscape; }
    if (ui->chBoxBLibrsvg->isChecked())     { opt.backends << Backend::Librsvg; }
    if (ui->chBoxBQtSvg->isChecked())       { opt.backends << Backend::QtSvg; }
    if (ui->chBoxBLadybird->isChecked())    { opt.backends << Backend::Ladybird; }


    return opt;
}
