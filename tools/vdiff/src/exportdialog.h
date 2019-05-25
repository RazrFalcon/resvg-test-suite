#pragma once

#include <QDialog>

#include "tests.h"

namespace Ui { class ExportDialog; }

class ExportDialog : public QDialog
{
    Q_OBJECT

public:
    struct Options
    {
        bool showTitle = false;
        bool indicateStatus = false;
        bool showDiff = false;
        QVector<Backend> backends;
    };

    explicit ExportDialog(const QList<Backend> &backends, QWidget *parent = nullptr);
    ~ExportDialog();

    Options options() const;

private:
    Ui::ExportDialog *ui;
};
