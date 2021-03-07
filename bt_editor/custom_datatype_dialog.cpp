#include "custom_datatype_dialog.h"
#include "ui_custom_datatype_dialog.h"

#include <QString>
#include <iostream>

CustomDataTypeDialog::CustomDataTypeDialog(const DataTypes& datatypes, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CustomDataTypeDialog)
{
    ui->setupUi(this);
    setWindowTitle("Custom DataType Editor");

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // populate QTableWidget
    ui->tableWidget->setRowCount(datatypes.size());
    int i = 0;
    for(const auto& datatype_it: datatypes)
    {
        const auto& datatype_name = datatype_it.first;  // QString
        const auto& datatype_values = datatype_it.second; // QStringList

        // Join list with comma and endline: ,\n
        QString datatype_values_text = datatype_values.join(",\n");

        ui->tableWidget->setItem(i, 0, new QTableWidgetItem( datatype_name ));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem( datatype_values_text ));

        i++;
    }

    connect( ui->tableWidget, &QTableWidget::cellChanged,
             this, &CustomDataTypeDialog::postprocessValues );

    checkValid();
}

CustomDataTypeDialog::~CustomDataTypeDialog()
{
    delete ui;
}

DataTypes CustomDataTypeDialog::getDataTypes() const
{
    DataTypes result;

    for (int row=0; row < ui->tableWidget->rowCount(); row++ )
    {
        auto datatype_name = ui->tableWidget->item(row,0)->text();
        auto datatype_values = ui->tableWidget->item(row,1)->text();

        QStringList list = splitAndTrimValues(datatype_values);

        result.insert( std::make_pair(datatype_name, list) );
    }

    return result;
}

void CustomDataTypeDialog::on_tableWidget_itemSelectionChanged()
{
    QModelIndexList selected_rows = ui->tableWidget->selectionModel()->selectedRows();
    ui->pushButtonRemove->setEnabled( selected_rows.count() != 0);
}

void CustomDataTypeDialog::on_pushButtonAdd_pressed()
{
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->setRowCount(row+1);

    ui->tableWidget->setItem(row,0, new QTableWidgetItem( "datatype_name" ));

    /*
    QComboBox* combo_direction = new QComboBox;

    combo_direction->addItem("Input");
    combo_direction->addItem("Output");
    combo_direction->addItem("In/Out");

    ui->tableWidget->setCellWidget(row, 1, combo_direction);
    */

    ui->tableWidget->setItem(row,1, new QTableWidgetItem());

    checkValid();
}

void CustomDataTypeDialog::on_pushButtonRemove_pressed()
{
    auto selected = ui->tableWidget->selectionModel()->selectedRows();
    for( const auto& index: selected)
    {
        ui->tableWidget->removeRow( index.row() );
    }
    checkValid();
}

void CustomDataTypeDialog::postprocessValues()
{
    for (int row=0; row < ui->tableWidget->rowCount(); row++ )
    {
        auto datatype_values = ui->tableWidget->item(row,1);
        if (datatype_values)
        {
            auto datatype_values_text = datatype_values->text();

            QStringList list = splitAndTrimValues(datatype_values_text);

            // Join list with comma and endline: ,\n
            QString new_datatype_values = list.join(",\n");

            ui->tableWidget->blockSignals(true);
            datatype_values->setText(new_datatype_values);
            ui->tableWidget->blockSignals(false);
        }
    }

    checkValid();
}

void CustomDataTypeDialog::checkValid()
{
    bool valid = true;
    int pos = 0;

    // Regexp for a valid datatype name
    QRegExp rx("[\\w:]+");
    QValidator* validator = new QRegExpValidator(rx, this);

    // Do checks, set warning text and valid to FALSE if NOK

    std::set<QString> datatype_names;

    for (int row=0; row < ui->tableWidget->rowCount(); row++ )
    {
        // Validate datatype names
        auto datatype_name = ui->tableWidget->item(row,0)->text();

        if( validator->validate(datatype_name, pos) != QValidator::Acceptable)
        {
            valid = false;
            ui->labelWarning->setText("Invalid name: use only letters, digits and underscores");
        }


        // Validate datatype values
        auto datatype_values = ui->tableWidget->item(row,1);
        if (datatype_values)
        {
            auto datatype_values_text = datatype_values->text();

            QStringList list = splitAndTrimValues(datatype_values_text);

            bool stringlist_valid = true;
            for (auto str : list)
            {
                if( validator->validate(str, pos) != QValidator::Acceptable)
                {
                    stringlist_valid = false;
                }
            }

            if (!stringlist_valid)
            {
                valid = false;
                ui->labelWarning->setText("Invalid values: use only letters, digits and underscores");
                datatype_values->setBackgroundColor(Qt::red);
            }
            else
            {
                // use the background color from the datatype name to restore the normal color
                datatype_values->setBackgroundColor(Qt::white);
            }
        }

        datatype_names.insert(datatype_name);
        
    }

    if( datatype_names.size() < ui->tableWidget->rowCount() )
    {
        valid = false;
        ui->labelWarning->setText("Duplicated NodeParameter key");
    }

    if(valid)
    {
        ui->labelWarning->setText("OK");
        ui->labelWarning->setStyleSheet("color: rgb(78, 154, 6)");
    }
    else
    {
        ui->labelWarning->setStyleSheet("color: rgb(204, 0, 0)");
    }

    ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( valid );


    delete validator;
}


QStringList CustomDataTypeDialog::splitAndTrimValues(QString values_as_string) const
{
    // values_as_string = "val1, val2, val3"

    // Remove all whitespace (spaces and newlines)
    values_as_string = values_as_string.replace(QRegExp("\\s*"), "");

    // Split text on comma ,
    // The smart split gets rid of empty values
    QStringList list = values_as_string.split(QLatin1Char(','), QString::SkipEmptyParts);

    return list;
}