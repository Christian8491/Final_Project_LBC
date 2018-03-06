#ifndef UI_GLWIDGET_H
#define UI_GLWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GLWIDGETClass
{
public:
    QWidget *centralWidget;

    void setupUi(QMainWindow *GLWIDGETClass)
    {
        if (GLWIDGETClass->objectName().isEmpty())
            GLWIDGETClass->setObjectName(QStringLiteral("GLWIDGETClass"));
        GLWIDGETClass->resize(850, 600);
        GLWIDGETClass->setWindowTitle(QApplication::translate("GLWIDGETClass", "Course: Graphics, Project:LBC", 0));
        centralWidget = new QWidget(GLWIDGETClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        GLWIDGETClass->setCentralWidget(centralWidget);
        
        QMetaObject::connectSlotsByName(GLWIDGETClass);
    }

};

namespace Ui {
    class GLWIDGETClass: public Ui_GLWIDGETClass {};
} // namespace Ui

QT_END_NAMESPACE


#endif // UI_GLWIDGET_H
