#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>

// Функция подгоняет изображение под нужный размер окна.
[[nodiscard("Не игнорируйте результат функции")]] QPixmap ResizeImgToFit(const QPixmap &src, int window_width, int window_height) {
    int img_w = src.width();
    int img_h = src.height();

    double w_ratio = double(img_w) / window_width;
    double h_ratio = double(img_h) / window_height;

    if ( w_ratio > h_ratio ) {
        return src.scaledToHeight(window_height);
    } else {
        return src.scaledToWidth(window_width);
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    SetPixmap(":/cats/images/cat1.jpg");
    FitImage();
    SetFolder(":/cats/images/");

    ui->menuBar->hide();
    // Разрешаем вызов контекстного меню на всём окне.
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    // Соединяем сигнал вызова меню со слотом,
    // который напишем далее.
    connect(this, &QMainWindow::customContextMenuRequested,
            this, &MainWindow::slotCustomMenuRequested);
    connect(ui->action_up_windows, &QAction::triggered,
            this, &MainWindow::slotUpWindows);
    connect(ui->action_choose_dir, &QAction::triggered,
            this, &MainWindow::slotChooseDir);
    connect(ui->action_use_resources, &QAction::triggered,
            this, &MainWindow::slotUseResources);
    connect(ui->action_close, &QAction::triggered,
            this, &MainWindow::close);

    ui->action_up_windows->setChecked(true);
    setWindowFlags(windowFlags().setFlag(Qt::WindowStaysOnTopHint, true));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::FitImage()
{
    Q_ASSERT(!active_pixmap.isNull());

    // Напишите этот метод.
    // 1. Вызовите ResizeImgToFit.
    // 2. Поместите изображение в lbl_img.
    // 3. Измените размер lbl_img.
    // 4. Переместите lbl_img, пользуясь формулами из условия.

    active_pixmap = ResizeImgToFit(active_pixmap, width(), height());
    lbl_new_.setPixmap(active_pixmap);
    lbl_new_.resize(active_pixmap.size());

    int lbl_x = (width() - active_pixmap.width()) / 2; // Координата x.
    int lbl_y = (height() - active_pixmap.height()) / 2; // Координата y.
    lbl_new_.move(lbl_x, lbl_y);

}

void MainWindow::SetFolder(const QString &d)
{
    current_folder_ = d;
    cur_file_index_ = 0;
    SetPixmap(GetCurrentFile());
    UpdateEnabled();
}

QString MainWindow::GetCurrentFile()
{
    QDir dir(current_folder_);
    // QStringList list_files = dir.entryList();

    auto entryList = dir.entryInfoList();
    QStringList list_files;// = dir.entryList();

    for(QFileInfo temp: entryList) {
        if (checkIsImgCorrect(temp.absoluteFilePath()))
            list_files << temp.absoluteFilePath();
    }

    /* if(list_files.size() > 2) {
        // Пропустим первые два элемента
        // и прочитаем сразу третий (с индексом 2).
    }*/

    // Сохраним количество файлов.
    int total = int(list_files.size());

    int file_index = std::min(cur_file_index_, total - 1);
    file_index = std::max(0, file_index);

    // Используем file_index, а не cur_file_index_.
    return dir.filePath(list_files[file_index]);
}

void MainWindow::UpdateEnabled() {
    // Количество изображений в папке.
    int max_images = QDir(current_folder_).entryList().size();

    // Устанавливаем активность кнопки «влево».
    ui->btn_left->setEnabled(cur_file_index_ > 0);

    // Устанавливаем активность кнопки «вправо».
    ui->btn_right->setEnabled(cur_file_index_ < max_images - 1);
}

void MainWindow::resizeEvent(QResizeEvent*)
{
    FitImage();
}

void MainWindow::on_btn_left_clicked()
{
    --cur_file_index_;
    const auto pait = FindNextImage(cur_file_index_, -1);
    cur_file_index_ = pait.second;
    active_pixmap = pait.first;
    FitImage();
    UpdateEnabled();
}


void MainWindow::on_btn_right_clicked()
{
    ++cur_file_index_;
    const auto pait = FindNextImage(cur_file_index_, 1);
    cur_file_index_ = pait.second;
    active_pixmap = pait.first;
    FitImage();
    UpdateEnabled();
}

void MainWindow::slotCustomMenuRequested(QPoint pos)
{
    ui->menu->popup(this->mapToGlobal(pos));
}

void MainWindow::slotUpWindows(bool checked)
{
    setWindowFlags(windowFlags().setFlag(Qt::WindowStaysOnTopHint, checked));
}

void MainWindow::slotUseResources()
{
    SetPixmap(":/cats/images/cat1.jpg");
    FitImage();
    SetFolder(":/cats/images/");
    UpdateEnabled();
}

QPixmap MainWindow::GetImageByPath(QString path) const {
    QFileInfo file_info(path);
    if (!file_info.isFile()) {
        return {};
    }
    return QPixmap(path);
}
bool MainWindow::checkIsImgCorrect(const QString &path) const
{
    QPixmap pixmap{path};
    return !pixmap.isNull();
}

std::pair<QPixmap, int> MainWindow::FindNextImage(int start_index, int direction) const {
    QDir dir(current_folder_);
    auto entryList = dir.entryInfoList();
    QStringList file_list = dir.entryList();

    int count = file_list.size();
    int cur_image = start_index;

    for(int steps = 0; steps < count; ++steps, cur_image += direction) {
        cur_image = ((cur_image % count) + count) % count;

        auto pixmap = GetImageByPath(dir.filePath(file_list[cur_image]));
        if (!pixmap.isNull()) {
            return {pixmap, cur_image};
        }
    }
    return {{}, -1};
}
void MainWindow::slotChooseDir()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    QString("Открыть папку"),
                                                    QDir::currentPath(),
                                                    QFileDialog::DontResolveSymlinks);
    QDir im_dir(dir);
    const auto entryList = im_dir.entryInfoList();
    current_folder_ = dir;
    if (entryList.isEmpty())
    {
        lbl_new_.clear();
        ui->btn_left->setEnabled(false);
        ui->btn_right->setEnabled(false);
    }
    else
    {

        auto pair = FindNextImage(0, 1);
        if (pair.second != -1)
        {
            cur_file_index_ = pair.second;
            active_pixmap = pair.first;
            FitImage();
            ui->btn_left->setEnabled(true);
            ui->btn_right->setEnabled(true);
            UpdateEnabled();
        }
        else
        {
            lbl_new_.clear();
            ui->btn_left->setEnabled(false);
            ui->btn_right->setEnabled(false);
        }
    }
}

