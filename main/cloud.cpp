#include "cloud.h"
#include "core.h"
#include "ui_mainwindow.h"

Cloud::Cloud(Ui::MainWindow *ui, QWidget *parent)
:QMainWindow(parent)
{
    this->ui = ui;
    client = new Client();
    connect(ui->uploadBtn, SIGNAL(clicked()), this, SLOT(openfiles()));
}

Cloud::~Cloud()
{
    delete client;
}

void Cloud::openfiles()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open File"),
                            "", "CSV Files(*.csv)", 0);
    if (fileNames.isEmpty()) {
        return;
    }
    string fileName;
    vector<Sequence> sequences;
    for (int i = 0; i < fileNames.length(); i++)
        {
            fileName = fileNames[i].toLocal8Bit().data();
            ifstream fin2(fileName.c_str());
            Csv format(fin2);
            Sequence temp;
            try{
                getSquFromFile(&format,&temp);
            }
            catch(int i)
            {
                qDebug() << "Error Code is" << i;
                return;
            }
            sequences.push_back(temp);
        }
    client->upload(sequences);
}

