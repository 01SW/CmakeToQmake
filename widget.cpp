/**
 * @file widget.h
 * @author ws01
 * @email 01@ws01.ltd
 * @date 2021/5/10
 */

#include "widget.h"
#include "ui_widget.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDir>

Widget::Widget(QWidget *parent) :
        QWidget(parent), ui(new Ui::Widget) {
    ui->setupUi(this);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    ui->pushButton_5->setEnabled(false);

    connect(ui->pushButton, &QPushButton::clicked, this, &Widget::selectSourceDir);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &Widget::toCmake);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &Widget::toQmake);
    connect(ui->pushButton_4, &QPushButton::clicked, this, &Widget::selectQtDir);
    connect(ui->pushButton_5, &QPushButton::clicked, this, &Widget::updateData);
}

Widget::~Widget() {
    delete ui;
}

void Widget::selectSourceDir() {
    auto dir = QFileDialog::getExistingDirectory(this, "选择文件夹");
    if(dir.isEmpty()){
        return;
    }
    sourceList_.clear();
    headerList_.clear();
    resourceList_.clear();
    uiList_.clear();
    proPath_.clear();

    rootDirPath_ = dir;
    ui->label->setText(rootDirPath_);
    findFile(dir);
    ui->pushButton_3->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_5->setEnabled(true);
}

void Widget::toCmake() {
    auto cmakePath = rootDirPath_ + "/CMakeLists.txt";
    QFile file(cmakePath);
    file.open(QIODevice::ReadWrite);
    QString data;
    bool s = false, h = false, r = false, u = false;
    while(!file.atEnd()){
        auto line = file.readLine();
        data.append(line);
        line = line.trimmed();
        if(line.isEmpty() || line.at(0) == '#'){
            continue;
        }
        if(line == "set(SourceFiles"){
            QString str;
            while(!file.atEnd()){
                auto text = file.readLine().trimmed();
                if(text.isEmpty() || text.at(0) == '#'){
                    str.append(text + "\n");
                    continue;
                }
                if(sourceList_.indexOf(text) != -1){
                    continue;
                }
                if(text == ")"){
                    str.append(text + "\n");
                    break;
                }
            }
            for(auto &source : sourceList_){
                data.append(source + "\n");
            }
            data.append(str);
            s = true;
        }else if(line == "set(HeaderFiles"){
            QString str;
            while(!file.atEnd()){
                auto text = file.readLine().trimmed();
                if(text.isEmpty() || text.at(0) == '#'){
                    str.append(text + "\n");
                    continue;
                }
                if(headerList_.indexOf(text) != -1){
                    continue;
                }
                if(text == ")"){
                    str.append(text + "\n");
                    break;
                }
            }
            for(auto &txt : headerList_){
                data.append(txt + "\n");
            }
            data.append(str);
            h = true;
        }else if(line == "set(ResourceFiles"){
            QString str;
            while(!file.atEnd()){
                auto text = file.readLine().trimmed();
                if(text.isEmpty() || text.at(0) == '#'){
                    str.append(text + "\n");
                    continue;
                }
                if(resourceList_.indexOf(text) != -1){
                    continue;
                }
                if(text == ")"){
                    str.append(text + "\n");
                    break;
                }
            }
            for(auto &txt : resourceList_){
                data.append(txt + "\n");
            }
            data.append(str);
            r = true;
        }else if(line == "set(UIFiles"){
            QString str;
            while(!file.atEnd()){
                auto text = file.readLine().trimmed();
                if(text.isEmpty() || text.at(0) == '#'){
                    str.append(text + "\n");
                    continue;
                }
                if(uiList_.indexOf(text) != -1){
                    continue;
                }
                if(text == ")"){
                    str.append(text + "\n");
                    break;
                }
            }
            for(auto &txt : uiList_){
                data.append(txt + "\n");
            }
            data.append(str);
            u = true;
        }
    }

    QString text;
    if(!s && !sourceList_.isEmpty()){
        QString sourceStr = "set(SourceFiles \n";
        for(auto &str : sourceList_){
            sourceStr.append(str + "\n");
        }
        sourceStr.append(")\n");
        text.append(sourceStr);
        s = true;
    }
    if(!h && !headerList_.isEmpty()){
        QString headerStr = "set(HeaderFiles \n";
        for(auto &str : headerList_){
            headerStr.append(str + "\n");
        }
        headerStr.append(")\n");
        text.append(headerStr);
        h = true;
    }
    if(!r && !resourceList_.isEmpty()){
        QString resourceStr = "set(ResourceFiles \n";
        for(auto &str : resourceList_){
            resourceStr.append(str + "\n");
        }
        resourceStr.append(")\n");
        text.append(resourceStr);
        r = true;
    }
    if(!u && !uiList_.isEmpty()){
        QString uiStr = "set(UIFiles \n";
        for(auto &str : uiList_){
            uiStr.append(str + "\n");
        }
        uiStr.append(")\n");
        text.append(uiStr);
        r = true;
    }

    bool isEmpty = false;
    if(data.isEmpty()){
        data.append("cmake_minimum_required(VERSION 3.10)\n");
        data.append("project(" + rootDirPath_.mid(rootDirPath_.lastIndexOf("/")+1) + ")\n\n");
        data.append("set(CMAKE_CXX_STANDARD 11)\n");
        auto qtDir = ui->lineEdit->text();
        if(!qtDir.isEmpty()){
            data.append("set(CMAKE_PREFIX_PATH \"" + qtDir + "\")\n");
        }
        auto str = "set(CMAKE_AUTOMOC ON)\n"
                   "set(CMAKE_AUTORCC ON)\n"
                   "set(CMAKE_AUTOUIC ON)\n"
                   "\n"
                   "set(QT_VERSION 5)\n"
                   "set(REQUIRED_LIBS Core Gui Widgets)\n"
                   "set(REQUIRED_LIBS_QUALIFIED Qt5::Core Qt5::Gui Qt5::Widgets)\n\n"
                   "if (NOT CMAKE_PREFIX_PATH)\n"
                   "    message(WARNING \"CMAKE_PREFIX_PATH is not defined, you may need to set it \"\n"
                   "            \"(-DCMAKE_PREFIX_PATH=\\\"path/to/Qt/lib/cmake\\\" or -DCMAKE_PREFIX_PATH=/usr/include/{host}/qt{version}/ on Ubuntu)\")\n"
                   "endif ()\n"
                   "\n"
                   "find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)\n\n";
        data.append(str);
        isEmpty = true;
    }

    if(s || h || r || u){
        if(!text.isEmpty()) {
            data.append(text);

            if(isEmpty){
                data.append("include_directories(.)\n");
            }
        }
        if(isEmpty){
            data.append("\nadd_executable(${PROJECT_NAME}");
            if(s){
                data.append(" ${SourceFiles}");
            }
            if(h){
                data.append(" ${HeaderFiles}");
            }
            if(r){
                data.append(" ${ResourceFiles}");
            }
            if(u){
                data.append(" ${UIFiles}");
            }
            data.append(")\ntarget_link_libraries(${PROJECT_NAME} ${REQUIRED_LIBS_QUALIFIED})\n");
        }
        file.seek(0);
        file.write(data.toUtf8().data());
    }
    QMessageBox::about(this, "提示", "转换完成");
}

void Widget::toQmake() {
    if(proPath_.isEmpty()){
        proPath_ = rootDirPath_ + "/" + rootDirPath_.mid(rootDirPath_.lastIndexOf("/")+1) + ".pro";
    }
    QFile file(proPath_);
    file.open(QIODevice::ReadWrite);
    QString data;
    bool s = false, h = false, r = false, u = false;
    while(!file.atEnd()){
        auto line = file.readLine();
        data.append(line);
        line = line.trimmed();
        if(line.isEmpty() || line.at(0) == '#'){
            continue;
        }
        if(line == "SOURCES += \\"){
            while(!file.atEnd()){
                auto text = file.readLine().trimmed();
                if(text.isEmpty()){
                    break;
                }
                auto txt = text;
                if(text.lastIndexOf(" \\") != -1){
                    txt = text.mid(0, text.lastIndexOf(" \\"));
                }
                if(sourceList_.indexOf(txt) != -1){
                    continue;
                }
            }
            for(int i = 0; i < sourceList_.size(); i++){
                auto &txt = sourceList_.at(i);
                if(i + 1 >= sourceList_.size()) {
                    data.append(txt + "\n");
                }else{
                    data.append(txt + " \\\n");
                }
            }
            data.append("\n");
            s = true;
        }else if(line == "HEADERS += \\"){
            while(!file.atEnd()){
                auto text = file.readLine().trimmed();
                if(text.isEmpty()){
                    break;
                }
                auto txt = text;
                if(text.lastIndexOf(" \\") != -1){
                    txt = text.mid(0, text.lastIndexOf(" \\"));
                }
                if(headerList_.indexOf(txt) != -1){
                    continue;
                }
            }
            for(int i = 0; i < headerList_.size(); i++){
                auto &txt = headerList_.at(i);
                if(i + 1 >= headerList_.size()) {
                    data.append(txt + "\n");
                }else{
                    data.append(txt + " \\\n");
                }
            }
            data.append("\n");
            h = true;
        }else if(line == "FORMS += \\"){
            while(!file.atEnd()){
                auto text = file.readLine().trimmed();
                if(text.isEmpty()){
                    break;
                }
                auto txt = text;
                if(text.lastIndexOf(" \\") != -1){
                    txt = text.mid(0, text.lastIndexOf(" \\"));
                }
                if(uiList_.indexOf(txt) != -1){
                    continue;
                }
            }
            for(int i = 0; i < uiList_.size(); i++){
                auto &txt = uiList_.at(i);
                if(i + 1 >= uiList_.size()) {
                    data.append(txt + "\n");
                }else{
                    data.append(txt + " \\\n");
                }
            }
            data.append("\n");
            u = true;
        }else if(line == "RESOURCES += \\"){
            while(!file.atEnd()){
                auto text = file.readLine().trimmed();
                if(text.isEmpty()){
                    break;
                }
                auto txt = text;
                if(text.lastIndexOf(" \\") != -1){
                    txt = text.mid(0, text.lastIndexOf(" \\"));
                }
                if(resourceList_.indexOf(txt) != -1){
                    continue;
                }
            }
            for(int i = 0; i < resourceList_.size(); i++){
                auto &txt = resourceList_.at(i);
                if(i + 1 >= resourceList_.size()) {
                    data.append(txt + "\n");
                }else{
                    data.append(txt + " \\\n");
                }
            }
            data.append("\n");
            r = true;
        }
    }

    QString text;
    if(!s && !sourceList_.isEmpty()){
        QString str = "SOURCES += \\\n";
        for(int i = 0; i < sourceList_.size(); i++){
            auto &txt = sourceList_.at(i);
            if(i + 1 >= sourceList_.size()) {
                str.append(txt + "\n");
            }else{
                str.append(txt + " \\\n");
            }
        }
        str.append("\n");
        text.append(str);
        s = true;
    }
    if(!h && !headerList_.isEmpty()){
        QString str = "HEADERS += \\\n";
        for(int i = 0; i < headerList_.size(); i++){
            auto &txt = headerList_.at(i);
            if(i + 1 >= headerList_.size()) {
                str.append(txt + "\n");
            }else{
                str.append(txt + " \\\n");
            }
        }
        str.append("\n");
        text.append(str);
        h = true;
    }
    if(!r && !resourceList_.isEmpty()){
        QString str = "RESOURCES += \\\n";
        for(int i = 0; i < resourceList_.size(); i++){
            auto &txt = resourceList_.at(i);
            if(i + 1 >= resourceList_.size()) {
                str.append(txt + "\n");
            }else{
                str.append(txt + " \\\n");
            }
        }
        str.append("\n");
        text.append(str);
        r = true;
    }
    if(!u && !uiList_.isEmpty()){
        QString str = "FORMS += \\\n";
        for(int i = 0; i < uiList_.size(); i++){
            auto &txt = uiList_.at(i);
            if(i + 1 >= uiList_.size()) {
                str.append(txt + "\n");
            }else{
                str.append(txt + " \\\n");
            }
        }
        str.append("\n");
        text.append(str);
        u = true;
    }

    bool isEmpty = false;
    if(data.isEmpty()){
        data.append("QT       += core gui\ngreaterThan(QT_MAJOR_VERSION, 4): QT += widgets\n\n");
        data.append("CONFIG += c++11\nDEFINES += QT_DEPRECATED_WARNINGS\n\n");

        isEmpty = true;
    }

    if(s || h || r || u){
        if(!text.isEmpty()){
            data.append(text);
        }

        if(isEmpty){
            auto str = "\nqnx: target.path = /tmp/$${TARGET}/bin\n"
                       "else: unix:!android: target.path = /opt/$${TARGET}/bin\n"
                       "!isEmpty(target.path): INSTALLS += target";
            data.append(str);
        }
        file.seek(0);
        file.write(data.toUtf8().data());
    }
    QMessageBox::about(this, "提示", "转换完成");
}

void Widget::findFile(const QString &dirPath) {
    QDir dir(dirPath);
    if(!dir.exists()){
        return;
    }

    dir.setFilter(QDir::NoDot | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Files | QDir::Dirs);

    auto infoList = dir.entryInfoList();
    for(auto &info : infoList){
        if(info.isDir()){
            if(info.fileName().indexOf("build") != -1){
                continue;
            }
            findFile(info.filePath());
        }else{
            auto suffix = info.suffix();
            if(suffix != "c" && suffix != "cpp" && suffix != "h" &&
               suffix != "ui" && suffix != "qrc"){
                if(suffix == "pro" && dirPath == rootDirPath_){
                    proPath_ = info.filePath();
                }
                continue;
            }
            auto path = info.filePath().mid(rootDirPath_.size()+1);
            if(suffix == "c" || suffix == "cpp"){
                sourceList_.append(path);
            }else if(suffix == "h"){
                headerList_.append(path);
            }else if(suffix == "ui"){
                uiList_.append(path);
            }else if(suffix == "qrc"){
                resourceList_.append(path);
            }
        }
    }
}

void Widget::selectQtDir() {
    auto dir = QFileDialog::getExistingDirectory(this, "选择CMake路径");
    if(dir.isEmpty()){
        return;
    }
    ui->lineEdit->setText(dir);
}

void Widget::updateData() {
    sourceList_.clear();
    headerList_.clear();
    resourceList_.clear();
    uiList_.clear();
    proPath_.clear();

    findFile(rootDirPath_);
}
