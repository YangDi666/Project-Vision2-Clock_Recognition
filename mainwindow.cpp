#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<string>
#include "clock.h"
#include<QFileDialog>
#include<QMessageBox>

using namespace cv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->label->setText("No Image");
    fileName_="";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    fileName_ = QFileDialog::getOpenFileName(this,tr("Open Image"),".",tr("Image File(*.png *.jpg *.jpeg *.bmp)"));
    Mat image = cv::imread(fileName_.toLatin1().data());
    cv::cvtColor(image,image,CV_BGR2RGB);
    QImage img = QImage((const unsigned char*)(image.data),image.cols,image.rows,QImage::Format_RGB888);
    ui->graphicsView->resize(img.width(), img.height());
    QGraphicsScene *scene = new QGraphicsScene;
    scene->addPixmap(QPixmap::fromImage(img));
    ui->graphicsView->setScene(scene);
    ui->graphicsView->adjustSize();
    ui->graphicsView->show();
}

void MainWindow::on_pushButton_2_clicked()
{
    string path_img=fileName_.toLatin1().data();
    if(path_img=="")
        QMessageBox::warning(NULL, "warning", "Load image first, please!");
    else
    {
        Clock c(path_img);
        c.traitement();
      //  c.dispay();
        ui->label->setText("It is : "+QString::number(c.getHour())+" : "+QString::number(c.getMin())+" : "+QString::number(c.getSec()));
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    close();
}
