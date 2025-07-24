#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QStack>

#include "ImageSpatialTransformation.h"
#include "ImageIntensityTransformation.h"
#include "ImageConverter.h"
#include "ImageQtAdapter.h"
#include "ImageMorphology.h"
#include "ImageUtils.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionLoad_Image_triggered();

    void on_actionSave_Image_triggered();

    void on_pushButtonUndo_clicked();

    void on_pushButtonRedo_clicked();

    void on_actionIntensity_Transformation_triggered();

    void on_pbApplyNegative_clicked();

    void on_pbApplyLog_clicked();

    void on_pbApplyGamma_clicked();

    void on_actionSpatial_Transformations_triggered();

    void onLowHighPassKernelChanged(int index);

    void onBlurKernelChanged(int index);

    void on_pushButtonLhFilter_clicked();

    void on_pushButtonImageSharpening_clicked();

    void on_pushButtonUMHB_clicked();


    void on_actionRGB_to_Grayscale_triggered();

    void on_actionGrayscale_to_Binary_triggered();

    void on_pushButtonApplyGrayscaleToBinary_clicked();

    void on_methodGrayscaleToBinaryComboBox_currentIndexChanged(int index);

    void on_actionBasic_Morphological_triggered();

    void on_pushButtonApplyBasicMorphology_clicked();

private:
    Ui::MainWindow *ui;
    QImage originalImage;
    QImage resultImage;
    QStack<QImage> undoStack;
    QStack<QImage> redoStack;
    iipt::SpatialTransformation::PaddingType getPaddingFromString(const QString& str);


    void pushToUndoStack();
    void displayResult();
    void updateImageInfo();

    void showDoneMessage(int timeoutMs = 2000);




};
#endif // MAINWINDOW_H
