#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <prac/QFileDialog>

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

    void SetPixmap(const QString path) {
        active_pixmap = QPixmap(path);
    }

    void FitImage();

    void SetFolder(const QString& d);
    QString GetCurrentFile();
    void UpdateEnabled();

private slots:
    void on_btn_left_clicked();
    void on_btn_right_clicked();
    void slotCustomMenuRequested(QPoint pos);
    void slotUpWindows(bool checked);
    void slotUseResources();
    void slotChooseDir();

private:
    void resizeEvent(QResizeEvent *event) override;
    QPixmap GetImageByPath(QString path) const;
    std::pair<QPixmap, int> FindNextImage(int start_index, int direction) const;
    bool checkIsImgCorrect(const QString &path) const;
private:
    Ui::MainWindow *ui;
    QPixmap active_pixmap;
    QLabel lbl_new_{this};
    QString current_folder_;
    int cur_file_index_ = 0;
};
#endif // MAINWINDOW_H
