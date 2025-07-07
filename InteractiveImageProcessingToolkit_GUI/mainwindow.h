#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QStack>

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

private:
    Ui::MainWindow *ui;
    QImage originalImage;
    QImage resultImage;
    QStack<QImage> undoStack;
    QStack<QImage> redoStack;

    void pushToUndoStack();
    void displayResult();
};
#endif // MAINWINDOW_H
