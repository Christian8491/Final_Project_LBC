#include "glwidget.h"
#include <iostream>
#include <QVBoxLayout>
#include <QFileDialog>

GLWIDGET::GLWIDGET(QWidget *parent)
    : QMainWindow(parent), triangulation_ready_(false), solver_ready_(false), display_ready_(false),
      triangulation_ready_3d(false), solver_ready_3d(false), display_ready_3d(false)
{
    ui.setupUi(this);                               // Initialize

    /* OpenGL */
    QGLFormat glf = QGLFormat::defaultFormat();     // It specifies the display format of an OpenGL rendering
    glf.setSampleBuffers(true);
    gl_Widget_ = new COpenGL(glf);

    /* Setting values to QspinBox and QDoubleSpinBox */
    spinBox_max_iter_ = new QSpinBox;
    spinBox_max_iter_->setMinimum(100);
    spinBox_max_iter_->setMaximum(1000000);
    spinBox_max_iter_->setValue(10000);
    spinBox_max_iter_->setSingleStep(100);

    dSpinBox_relaxation_alpha_ = new QDoubleSpinBox;
    dSpinBox_relaxation_alpha_->setMinimum(1.0);
    dSpinBox_relaxation_alpha_->setMaximum(2.0);
    dSpinBox_relaxation_alpha_->setValue(1.65);
    dSpinBox_relaxation_alpha_->setSingleStep(0.01);

    dSpinBox_penalty_ = new QDoubleSpinBox;
    dSpinBox_penalty_->setMinimum(1.0);
    dSpinBox_penalty_->setMaximum(10000.0);
    dSpinBox_penalty_->setValue(10);
    dSpinBox_penalty_->setSingleStep(1.0);

    dSpinBox_primal_res_tolerance_ = new QDoubleSpinBox;
    dSpinBox_primal_res_tolerance_->setMinimum(0.0000000001);
    dSpinBox_primal_res_tolerance_->setMaximum(0.01);
    dSpinBox_primal_res_tolerance_->setSingleStep(0.0000000001);
    dSpinBox_primal_res_tolerance_->setDecimals(10);
    dSpinBox_primal_res_tolerance_->setValue(0.0000001);

    dSpinBox_dual_res_tolerance_ = new QDoubleSpinBox;
    dSpinBox_dual_res_tolerance_->setMinimum(0.000000001);
    dSpinBox_dual_res_tolerance_->setMaximum(0.01);
    dSpinBox_dual_res_tolerance_->setSingleStep(0.000000001);
    dSpinBox_dual_res_tolerance_->setDecimals(9);
    dSpinBox_dual_res_tolerance_->setValue(0.000001);

    /* Slots */
    pushButton_Cage_ = new QPushButton(tr("Load 2D Cage"));
    connect(pushButton_Cage_, SIGNAL(clicked()), this, SLOT(load_cage()));

    pushButton_Cage_3D = new QPushButton(tr("Load 3D Cage"));
    connect(pushButton_Cage_3D, SIGNAL(clicked()), this, SLOT(load_cage_3D()));

    pushButton_LBC_solver_ = new QPushButton(tr("Compute LBC"));
    connect(pushButton_LBC_solver_, SIGNAL(clicked()), this, SLOT(lbc_solver()));

    checkBox_DisplayWeights_ = new QCheckBox(tr("Display Weights"));
    connect(checkBox_DisplayWeights_, SIGNAL(clicked()), this, SLOT(select()));

    /* GroupBox Triangulation */
    label_numTriangles_ = new QLabel(tr("Target No. of Cells:"));
    spinBox_numTriangles_ = new QSpinBox;
    spinBox_numTriangles_->setMinimum(10);
    spinBox_numTriangles_->setMaximum(1000000);
    spinBox_numTriangles_->setValue(2000);
    spinBox_numTriangles_->setSingleStep(10);
    pushButton_triangulation_ = new QPushButton(tr("Triangulate"));
    connect(pushButton_triangulation_, SIGNAL(clicked()), this, SLOT(triangulation()));

    QVBoxLayout *triangulation_layout = new QVBoxLayout;
    triangulation_layout->addWidget(label_numTriangles_);
    triangulation_layout->addWidget(spinBox_numTriangles_);
    triangulation_layout->addSpacing(5);
    triangulation_layout->addWidget(pushButton_triangulation_);

    groupbox_triangulation_ = new QGroupBox(tr("Discretization"));
    groupbox_triangulation_->setLayout(triangulation_layout);

    /* GroupBox Solver */
    label_weighting_ = new QLabel("Weighting");
    combobox_weighting_ = new QComboBox;
    combobox_weighting_->addItem(tr("Constant"));
    combobox_weighting_->addItem(tr("Linear"));
    combobox_weighting_->addItem(tr("Quadratic"));
    combobox_weighting_->addItem(tr("Sqrt"));
    combobox_weighting_->setCurrentIndex(2);                        // Select by default

    QVBoxLayout *solver_layout = new QVBoxLayout;
    solver_layout->addWidget(label_weighting_);
    solver_layout->addWidget(combobox_weighting_);
    solver_layout->addSpacing(5);
    solver_layout->addWidget(new QLabel(tr("Max. Iterations")));
    solver_layout->addWidget(spinBox_max_iter_);
    solver_layout->addSpacing(5);
    solver_layout->addWidget(new QLabel(tr("Relaxation Alpha")));
    solver_layout->addWidget(dSpinBox_relaxation_alpha_);
    solver_layout->addSpacing(5);
    solver_layout->addWidget(new QLabel(tr("Penalty Weight")));
    solver_layout->addWidget(dSpinBox_penalty_);
    solver_layout->addSpacing(5);
    solver_layout->addWidget(new QLabel(tr("Primal Res. Tol.")));
    solver_layout->addWidget(dSpinBox_primal_res_tolerance_);
    solver_layout->addSpacing(5);
    solver_layout->addWidget(new QLabel(tr("Dual Res. Tol.")));
    solver_layout->addWidget(dSpinBox_dual_res_tolerance_);
    solver_layout->addSpacing(5);
    solver_layout->addWidget(pushButton_LBC_solver_);
    groupbox_solver_ = new QGroupBox(tr("Solver"));
    groupbox_solver_->setLayout(solver_layout);

    /* Build interface */
    QVBoxLayout *layout_left = new QVBoxLayout;
    layout_left->addWidget(pushButton_Cage_);
    layout_left->addSpacing(10);
    layout_left->addWidget(pushButton_Cage_3D);
    layout_left->addSpacing(10);
    layout_left->addWidget(groupbox_triangulation_);
    layout_left->addSpacing(10);
    layout_left->addWidget(groupbox_solver_);
    layout_left->addSpacing(10);
    layout_left->addWidget(checkBox_DisplayWeights_);
    layout_left->addStretch();

    QHBoxLayout *layout_main = new QHBoxLayout;

    update_controls();

    layout_main->addLayout(layout_left);
    layout_main->addWidget(gl_Widget_);
    layout_main->setStretch(1, 1);
    this->centralWidget()->setLayout(layout_main);
}

GLWIDGET::~GLWIDGET(){}


/* --------------------------------------------- Load Cages ---------------------------------------------- */

void GLWIDGET::load_cage()
{
    QString path_2d = "D:/Computer_Science/Ciclo_II/Graphics/Final_Project/LBC_shaders/data/2D/";
    QString filename = QFileDialog::
            getOpenFileName(this, tr("Read Mesh"), path_2d, tr("Meshes (*.obj)"));

    if (filename.isEmpty()) {
        std::cerr << "Read Cage Failed!" << std::endl;
        return;
    }

    gl_Widget_->lbc->read_cage(filename.toStdString());
    gl_Widget_->updateGL();

    triangulation_ready_ = true;
    update_controls();

    update();

    gl_Widget_->lbc_3d->active_3d = false;
    gl_Widget_->lbc->active_2d = true;
}

void GLWIDGET::load_cage_3D()
{
    QString path_3d = "D:/Computer_Science/Ciclo_II/Graphics/Final_Project/LBC_shaders/data/3D/";
    QString filename = QFileDialog::
            getOpenFileName(this, tr("Read Mesh"), path_3d, tr("Meshes (*.obj)"));

    if (filename.isEmpty()) {
        std::cerr << "Read Cage Failed!" << std::endl;
        return;
    }

    gl_Widget_->lbc_3d->read_cage_3d(filename.toStdString());
    gl_Widget_->updateGL();
    triangulation_ready_3d = true;
    solver_ready_3d = true;
    update_controls_3d();

    update();

    gl_Widget_->lbc->active_2d = false;
    gl_Widget_->lbc_3d->active_3d = true;
}


/* --------------------------------- Triangulation (only for 2D) ------------------------------------ */

void GLWIDGET::triangulation()
{
    if (gl_Widget_->lbc->active_2d == true) {
        gl_Widget_->lbc->delaunay_Triangulation(spinBox_numTriangles_->value());
        solver_ready_ = true;
        update_controls();
        gl_Widget_->updateGL();
        update();
    }

    if (gl_Widget_->lbc_3d->active_3d == true){
        gl_Widget_->lbc_3d->my_Triangulation_3d();
        solver_ready_3d = true;
        update_controls_3d();
        gl_Widget_->updateGL();
        update();
    }

}

/* --------------------------------------------- Solver ---------------------------------------------- */

void GLWIDGET::lbc_solver()
{
    if (gl_Widget_->lbc->active_2d == true) {
        gl_Widget_->lbc->lbc_solver(combobox_weighting_->currentIndex(), spinBox_max_iter_->value(),
                                dSpinBox_relaxation_alpha_->value(), dSpinBox_penalty_->value(),
                                dSpinBox_primal_res_tolerance_->value(), dSpinBox_dual_res_tolerance_->value());

        display_ready_ = true;
        update_controls();
        update();
    }

    if (gl_Widget_->lbc_3d->active_3d == true) {
        gl_Widget_->lbc_3d->lbc_solver_3d(combobox_weighting_->currentIndex(), spinBox_max_iter_->value(),
                                          dSpinBox_relaxation_alpha_->value(), dSpinBox_penalty_->value(),
                                          dSpinBox_primal_res_tolerance_->value(), dSpinBox_dual_res_tolerance_->value());
        display_ready_3d = true;
        update_controls_3d();
        update();
    }
}


/* --------------------------------------------- Select ---------------------------------------------- */

void GLWIDGET::select()
{
    gl_Widget_->lbc->set_mouse_select();
    gl_Widget_->updateGL();
}

void GLWIDGET::update_controls()
{
    groupbox_triangulation_->setEnabled(triangulation_ready_);
    checkBox_DisplayWeights_->setEnabled(display_ready_);
    groupbox_solver_->setEnabled(solver_ready_);
}

void GLWIDGET::update_controls_3d()
{
    groupbox_triangulation_->setEnabled(triangulation_ready_3d);
    checkBox_DisplayWeights_->setEnabled(display_ready_3d);
    groupbox_solver_->setEnabled(solver_ready_3d);
}
