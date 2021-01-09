#ifndef CUSTOM_DATATYPE_DIALOG_H
#define CUSTOM_DATATYPE_DIALOG_H

#include <QDialog>

#include "bt_editor_base.h"

namespace Ui {
class CustomDataTypeDialog;
}

class CustomDataTypeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CustomDataTypeDialog(const DataTypes& datatypes, QWidget *parent = nullptr);
    ~CustomDataTypeDialog();

    DataTypes getDataTypes() const;

private slots:
    void on_tableWidget_itemSelectionChanged();

    void on_pushButtonAdd_pressed();

    void on_pushButtonRemove_pressed();

private:
    void postprocessValues();
    void checkValid();
    QStringList splitAndTrimValues(QString values_as_string) const;

private:
    Ui::CustomDataTypeDialog *ui;
};

#endif // CUSTOM_DATATYPE_DIALOG_H
