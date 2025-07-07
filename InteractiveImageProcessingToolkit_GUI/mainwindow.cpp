#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QPixmap>
#include <QMessageBox>
#include <QSettings>

#include "ImageIntensityTransformation.h"
#include "ImageQtAdapter.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineLogScalingFactor->setText("1");
    ui->lineGammaScalingFactor->setText("1");
    ui->lineGamma->setText("1");
    ui->frame_2->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ------------------------- Helpers -------------------------

void MainWindow::pushToUndoStack()
{
    if (!resultImage.isNull()) {
        undoStack.push(resultImage);
        redoStack.clear();
    }
}

void MainWindow::displayResult()
{
    ui->labelResult->setPixmap(QPixmap::fromImage(resultImage).scaled(
        ui->labelResult->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation));
}

// ------------------- Load & Save --------------------------

void MainWindow::on_actionLoad_Image_triggered()
{
    QSettings settings("Truoyon", "Interactive_Image_Processing_Toolkit");
    QString lastDir = settings.value("lastImageDir", QDir::homePath()).toString();

    QString fileName = QFileDialog::getOpenFileName(
        this, "Open Image", "",
        "Images (*.bmp)");

    if (!fileName.isEmpty()) {
        QImage image(fileName);
        if (image.isNull()) {
            QMessageBox::warning(this, "Load Image", "Could not load the selected image.");
            return;
        }

        settings.setValue("lastImageDir", QFileInfo(fileName).absolutePath());

        originalImage = image;
        resultImage = image;

        ui->labelOriginal->setPixmap(QPixmap::fromImage(originalImage).scaled(
            ui->labelOriginal->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

        displayResult();

        undoStack.clear();
        redoStack.clear();
    }
}

void MainWindow::on_actionSave_Image_triggered()
{
    if (resultImage.isNull()) {
        QMessageBox::warning(this, "Save Image", "No image to save.");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(
        this, "Save Image", "",
        "BMP Image (*.bmp)");

    if (!fileName.isEmpty()) {
        if (!resultImage.save(fileName)) {
            QMessageBox::warning(this, "Save Image", "Failed to save the image.");
        }
    }
}

// -------------------- Undo / Redo --------------------------

void MainWindow::on_pushButtonUndo_clicked()
{
    if (!undoStack.isEmpty()) {
        redoStack.push(resultImage);
        resultImage = undoStack.pop();
        displayResult();
    }
}

void MainWindow::on_pushButtonRedo_clicked()
{
    if (!redoStack.isEmpty()) {
        undoStack.push(resultImage);
        resultImage = redoStack.pop();
        displayResult();
    }
}

// -------------------- Dock Visibility -----------------------

void MainWindow::on_actionIntensity_Transformation_triggered()
{
    ui->frame_2->setVisible(
        !ui->frame_2->isVisible()
        );
}

// --------------------- Transformations ----------------------

void MainWindow::on_pbApplyNegative_clicked()
{
    if (resultImage.isNull()) return;

    pushToUndoStack();
    iipt::Image img = ImageQtAdapter::fromQImage(resultImage);
    iipt::ImageIntensityTransformation::applyNegative(img);
    resultImage = ImageQtAdapter::toQImage(img);
    displayResult();
}

void MainWindow::on_pbApplyLog_clicked()
{
    if (resultImage.isNull()) return;

    bool ok;
    float c = ui->lineLogScalingFactor->text().toFloat(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a valid scaling factor.");
        return;
    }

    pushToUndoStack();
    iipt::Image img = ImageQtAdapter::fromQImage(resultImage);
    iipt::ImageIntensityTransformation::applyLog(img, c);
    resultImage = ImageQtAdapter::toQImage(img);
    displayResult();
}

void MainWindow::on_pbApplyGamma_clicked()
{
    if (resultImage.isNull()) return;

    bool ok1, ok2;
    float c = ui->lineGammaScalingFactor->text().toFloat(&ok1);
    float gamma = ui->lineGamma->text().toFloat(&ok2);

    if (!ok1 || !ok2) {
        QMessageBox::warning(this, "Invalid Input", "Please enter valid values for scaling factor and gamma.");
        return;
    }

    pushToUndoStack();
    iipt::Image img = ImageQtAdapter::fromQImage(resultImage);
    iipt::ImageIntensityTransformation::applyGamma(img, gamma, c);
    resultImage = ImageQtAdapter::toQImage(img);
    displayResult();
}
