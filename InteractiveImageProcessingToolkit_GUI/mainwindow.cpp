#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QPixmap>
#include <QMessageBox>
#include <QSettings>

#include "ImageIntensityTransformation.h"
#include "ImageSpatialTransformation.h"
#include "ImageQtAdapter.h"
#include "ImageConverter.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->kernelTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onLowHighPassKernelChanged);

    connect(ui->blurKernelComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onBlurKernelChanged);

    connect(ui->horizontalSliderSeSize, &QSlider::valueChanged,
            ui->spinBoxSeSize, &QSpinBox::setValue);
    connect(ui->spinBoxSeSize, QOverload<int>::of(&QSpinBox::valueChanged),
            ui->horizontalSliderSeSize, &QSlider::setValue);


    ui->sigmaDoubleSpinBox->setEnabled(false);
    ui->sigmaLabel->setEnabled(false);
    ui->sigmaDoubleSpinBox_2->setEnabled(false);
    ui->sigmaLabel_2->setEnabled(false);

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
        updateImageInfo();

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

// -------------------- Top Menus -----------------------

void MainWindow::on_actionIntensity_Transformation_triggered()
{
    ui->stackedWidget->setCurrentWidget(ui->page_intensityTransformations);
}

void MainWindow::on_actionSpatial_Transformations_triggered()
{
    ui->stackedWidget->setCurrentWidget(ui->page_spatialTransformations);
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

//----------------- Stacked Pages --------------------------------

// --------------- Image Information page -------------------------

void MainWindow::updateImageInfo()
{
    if (resultImage.isNull()) {
        ui->labelImageInfo->setText("No image loaded.");
        return;
    }

    QString info;
    info += "Width: " + QString::number(resultImage.width()) + "\n";
    info += "Height: " + QString::number(resultImage.height()) + "\n";
    info += "Depth: " + QString::number(resultImage.depth()) + " bpp\n";

    if (resultImage.format() == QImage::Format_Grayscale8) {
        info += "Format: Grayscale";
    } else {
        info += "Format: RGB";
    }

    ui->labelImageInfo->setText(info);
}

// ------------ Spatial Transformation


void MainWindow::onLowHighPassKernelChanged(int index)
{
    QString type = ui->kernelTypeComboBox->itemText(index);
    bool enableSigma = (type == "Gaussian");
    ui->sigmaDoubleSpinBox->setEnabled(enableSigma);
    ui->sigmaLabel->setEnabled(enableSigma);
}

void MainWindow::onBlurKernelChanged(int index)
{
    QString type = ui->blurKernelComboBox->itemText(index);
    bool enableSigma = (type == "Gaussian");
    ui->sigmaDoubleSpinBox_2->setEnabled(enableSigma);
    ui->sigmaLabel_2->setEnabled(enableSigma);
}



void MainWindow::on_pushButtonLhFilter_clicked()
{
    if (resultImage.isNull()) return;


    pushToUndoStack();
    iipt::Image img = ImageQtAdapter::fromQImage(resultImage);

    QString kernelType = ui->kernelTypeComboBox->currentText();
    int kSize = ui->kernelSizeSpinBox->value();
    float sigma = ui->sigmaDoubleSpinBox->value();
    QString paddingStr = ui->paddingTypeComboBox->currentText();
    iipt::SpatialTransformation::PaddingType padding = getPaddingFromString(paddingStr);

    if (kernelType == "Box") {
        iipt::SpatialTransformation::applyBoxFilter(img, kSize, padding);
    } else if (kernelType == "Gaussian") {
        iipt::SpatialTransformation::applyGaussianFilter(img, kSize, sigma, padding);
    } else if (kernelType == "Median") {
        iipt::SpatialTransformation::applyMedianFilter(img, kSize, padding);
    }


    resultImage = ImageQtAdapter::toQImage(img);
    displayResult();
    showDoneMessage();
}


void MainWindow::on_pushButtonImageSharpening_clicked()
{
    if (resultImage.isNull()) return;

    pushToUndoStack();
    iipt::Image img = ImageQtAdapter::fromQImage(resultImage);

    QString method = ui->sharpeningMethodComboBox->currentText();
    QString paddingStr = ui->paddingTypeComboBox_2->currentText();
    iipt::SpatialTransformation::PaddingType padding = getPaddingFromString(paddingStr);

    iipt::SpatialTransformation::applySharpening(img, method.toStdString(), padding);

    resultImage = ImageQtAdapter::toQImage(img);
    displayResult();
}


void MainWindow::on_pushButtonUMHB_clicked()
{
    if (resultImage.isNull()) return;

    pushToUndoStack();
    iipt::Image img = ImageQtAdapter::fromQImage(resultImage);

    QString kernelType = ui->blurKernelComboBox->currentText();
    int kSize = ui->kernelSizeSpinBox_3->value();
    float sigma = ui->sigmaDoubleSpinBox_2->value();
    float gain = ui->gainDoubleSpinBox->value();
    QString paddingStr = ui->paddingTypeComboBox_3->currentText();
    iipt::SpatialTransformation::PaddingType padding = getPaddingFromString(paddingStr);

    if (gain <= 1.0f) {
        iipt::SpatialTransformation::applyUnsharpMasking(img, kernelType.toStdString(), kSize, sigma, padding);
    } else {
        iipt::SpatialTransformation::applyHighboostFiltering(img, kernelType.toStdString(), kSize, gain, sigma, padding);
    }

    resultImage = ImageQtAdapter::toQImage(img);
    displayResult();
}

iipt::SpatialTransformation::PaddingType MainWindow::getPaddingFromString(const QString& str) {
    if (str == "Zero") return iipt::SpatialTransformation::PaddingType::Zero;
    if (str == "Replicate") return iipt::SpatialTransformation::PaddingType::Replicate;
    if (str == "Mirror") return iipt::SpatialTransformation::PaddingType::Mirror;
    return iipt::SpatialTransformation::PaddingType::None;
}



void MainWindow::showDoneMessage(int timeoutMs)
{
    ui->statusbar->showMessage("Done", timeoutMs);
    QApplication::restoreOverrideCursor();
}





void MainWindow::on_actionRGB_to_Grayscale_triggered()
{
    if (resultImage.isNull()) return;

    pushToUndoStack();
    iipt::Image img = ImageQtAdapter::fromQImage(resultImage);

    iipt::RGBToGrayscaleConverter::convert(img);
    resultImage = ImageQtAdapter::toQImage(img);
    displayResult();
}


void MainWindow::on_actionGrayscale_to_Binary_triggered()
{
    ui->stackedWidget->setCurrentWidget(ui->pageGrayscaleToBinary);
    ui->methodGrayscaleToBinaryComboBox->setCurrentIndex(0);
    ui->thresholdValueSpinBox->setValue(128);
    ui->blockSizeSpinBox->setValue(11);
    ui->cSpinBox->setValue(2);

    ui->thresholdValueLabel->setVisible(true);
    ui->thresholdValueSpinBox->setVisible(true);
    ui->blockSizeLabel->setVisible(false);
    ui->blockSizeSpinBox->setVisible(false);
    ui->cLabel->setVisible(false);
    ui->cSpinBox->setVisible(false);

}

void MainWindow::on_methodGrayscaleToBinaryComboBox_currentIndexChanged(int index)
{
    // Show/hide relevant input fields
    bool isFixed = (index == 0);
    bool isAdaptive = (index == 2 || index == 3);

    ui->thresholdValueLabel->setVisible(isFixed);
    ui->thresholdValueSpinBox->setVisible(isFixed);

    ui->blockSizeLabel->setVisible(isAdaptive);
    ui->blockSizeSpinBox->setVisible(isAdaptive);

    ui->cLabel->setVisible(isAdaptive);
    ui->cSpinBox->setVisible(isAdaptive);
}



void MainWindow::on_pushButtonApplyGrayscaleToBinary_clicked()
{
    if (resultImage.isNull()) return;

    pushToUndoStack();
    iipt::Image img = ImageQtAdapter::fromQImage(resultImage);

    int methodIndex = ui->methodGrayscaleToBinaryComboBox->currentIndex();

    switch (methodIndex) {
    case 0: // Fixed
        iipt::GrayscaleToBinaryConverter::fixedThreshold(
            img,
            ui->thresholdValueSpinBox->value()
            );
        break;

    case 1: // Otsu
        iipt::GrayscaleToBinaryConverter::otsuThreshold(img);
        break;

    case 2: // Adaptive Mean
        iipt::GrayscaleToBinaryConverter::adaptiveMeanThreshold(
            img,
            ui->blockSizeSpinBox->value(),
            ui->cSpinBox->value()
            );
        break;

    case 3: // Adaptive Gaussian
        iipt::GrayscaleToBinaryConverter::adaptiveGaussianThreshold(
            img,
            ui->blockSizeSpinBox->value(),
            ui->cSpinBox->value()
            );
        break;
    }

    resultImage = ImageQtAdapter::toQImage(img);
    displayResult();
}


void MainWindow::on_actionBasic_Morphological_triggered()
{
    ui->stackedWidget->setCurrentWidget(ui->pageMorphologicalBasic);
}


void MainWindow::on_pushButtonApplyBasicMorphology_clicked()
{
    if (resultImage.isNull()) return;

    // 1. convert to our Image class (must be 1‑channel for morphology)
    iipt::Image img = ImageQtAdapter::fromQImage(resultImage);
    if (img.channels != 1) {
        QMessageBox::warning(this, "Morphology",
                             "Please convert to grayscale/binary before applying morphology.");
        return;
    }

    pushToUndoStack();

    /* ------- SE shape & size ------- */
    QString shapeStr = "square";
    if      (ui->radioButtonSquare->isChecked())           shapeStr = "square";
    else if (ui->radioButtonCross->isChecked())            shapeStr = "cross";
    else if (ui->radioButtonCircle->isChecked())           shapeStr = "circle";
    else if (ui->radioButtonHorizontalLine->isChecked())   shapeStr = "line_horizontal";
    else if (ui->radioButtonVerticalLine->isChecked())     shapeStr = "line_vertical";

    int seSize = ui->spinBoxSeSize->value();               // odd numbers only
    auto se = iipt::ImageUtils::createStructuringElement(shapeStr.toStdString(), seSize);


    /*----- Padding Type ---------------*/
    iipt::ImageUtils::PaddingType pad = iipt::ImageUtils::PaddingType::None;
    if(ui->radioButtonNone->isChecked())        pad = iipt::ImageUtils::PaddingType::None;
    if (ui->radioButtonZero->isChecked())       pad = iipt::ImageUtils::PaddingType::Zero;
    if (ui->radioButtonReplicate->isChecked())  pad =  iipt::ImageUtils::PaddingType::Replicate;
    if (ui->radioButtonMirror->isChecked())     pad =  iipt::ImageUtils::PaddingType::Mirror;


    /*********** Basic Morphological Operation -------------------*/

    if      (ui->radioButtonErosion->isChecked())               iipt::ImageMorphology::erosion(img, se, pad);
    else if (ui->radioButtonDilation->isChecked())              iipt::ImageMorphology::dilation(img, se, pad);
    else if (ui->radioButtonOpening->isChecked())               iipt::ImageMorphology::opening(img, se, pad);
    else if (ui->radioButtonClosing->isChecked())               iipt::ImageMorphology::closing(img, se, pad);
    else if (ui->radioButtonBoundaryExtraction->isChecked())    iipt::ImageMorphology::boundaryExtract(img, se, pad);


   resultImage = ImageQtAdapter::toQImage(img);
   displayResult();
   showDoneMessage();



}

