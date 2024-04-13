#include "Logic/capplicationcontroller.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CApplicationController application_controller;
    return a.exec();
}
