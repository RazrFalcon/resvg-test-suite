#include <QPushButton>

#include "exportdialog.h"
#include "ui_exportdialog.h"

ExportDialog::ExportDialog(const QList<Backend> &backends, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ExportDialog)
{
    ui->setupUi(this);

    ui->chBoxBReference->setEnabled(backends.contains(Backend::Reference));
    ui->chBoxBResvgCairo->setEnabled(backends.contains(Backend::ResvgCairo));
    ui->chBoxBResvgQt->setEnabled(backends.contains(Backend::ResvgQt));
    ui->chBoxBResvgRaqote->setEnabled(backends.contains(Backend::ResvgRaqote));
    ui->chBoxBResvgSkia->setEnabled(backends.contains(Backend::ResvgSkia));
    ui->chBoxBChrome->setEnabled(backends.contains(Backend::Chrome));
    ui->chBoxBFirefox->setEnabled(backends.contains(Backend::Firefox));
    ui->chBoxBBatik->setEnabled(backends.contains(Backend::Batik));
    ui->chBoxBInkscape->setEnabled(backends.contains(Backend::Inkscape));
    ui->chBoxBLibrsvg->setEnabled(backends.contains(Backend::Librsvg));
    ui->chBoxBQtSvg->setEnabled(backends.contains(Backend::QtSvg));

    ui->chBoxBResvgCairo->setChecked(backends.contains(Backend::ResvgCairo));
    ui->chBoxBResvgQt->setChecked(backends.contains(Backend::ResvgQt));
    ui->chBoxBResvgRaqote->setChecked(backends.contains(Backend::ResvgRaqote));
    ui->chBoxBResvgSkia->setChecked(backends.contains(Backend::ResvgSkia));
    ui->chBoxBChrome->setChecked(backends.contains(Backend::Chrome));
    ui->chBoxBFirefox->setChecked(backends.contains(Backend::Firefox));
    ui->chBoxBBatik->setChecked(backends.contains(Backend::Batik));
    ui->chBoxBInkscape->setChecked(backends.contains(Backend::Inkscape));
    ui->chBoxBLibrsvg->setChecked(backends.contains(Backend::Librsvg));
    ui->chBoxBQtSvg->setChecked(backends.contains(Backend::QtSvg));

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
    if (ui->chBoxBResvgCairo->isChecked())  { opt.backends << Backend::ResvgCairo; }
    if (ui->chBoxBResvgQt->isChecked())     { opt.backends << Backend::ResvgQt; }
    if (ui->chBoxBResvgRaqote->isChecked()) { opt.backends << Backend::ResvgRaqote; }
    if (ui->chBoxBResvgSkia->isChecked())   { opt.backends << Backend::ResvgSkia; }
    if (ui->chBoxBChrome->isChecked())      { opt.backends << Backend::Chrome; }
    if (ui->chBoxBFirefox->isChecked())     { opt.backends << Backend::Firefox; }
    if (ui->chBoxBBatik->isChecked())       { opt.backends << Backend::Batik; }
    if (ui->chBoxBInkscape->isChecked())    { opt.backends << Backend::Inkscape; }
    if (ui->chBoxBLibrsvg->isChecked())     { opt.backends << Backend::Librsvg; }
    if (ui->chBoxBQtSvg->isChecked())       { opt.backends << Backend::QtSvg; }

    return opt;
}
