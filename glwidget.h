#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "OpenGL.h"
#include "ui_glwidget.h"
#include "common.h"

class GLWIDGET : public QMainWindow
{
    Q_OBJECT

    public:
        GLWIDGET(QWidget *parent = 0);
        ~GLWIDGET();

    private:
        Ui::GLWIDGETClass ui;                                  // Only for initialize (ui_glwidget.h)
        COpenGL     *gl_Widget_;                               // To OpenGL

        QLabel      *label_numTriangles_;                      // "Target No. of Cells:"
        QLabel      *label_weighting_;                         // "Weighting"
        QCheckBox   *checkBox_DisplayWeights_;                 // "Display Weights"
        QPushButton *pushButton_Cage_;                         // "Load 2D Cage"
        QPushButton *pushButton_Cage_3D;                       // "Load 3D Cage"
        QPushButton *pushButton_triangulation_;                // "Triangulate"
        QPushButton *pushButton_LBC_solver_;                   // "Compute LBC"
        QComboBox   *combobox_weighting_;                      // "Constant", "Linear", "Quadratic", ..
        QGroupBox   *groupbox_triangulation_;                  // "Discretization"
        QGroupBox   *groupbox_solver_;                         // "Solver"
        QSpinBox    *spinBox_numTriangles_;                    // 2000 (in Discretizacion)
        QSpinBox    *spinBox_max_iter_;                        // Max. iterations
        QDoubleSpinBox    *dSpinBox_relaxation_alpha_;         // "Relaxation Alpha"
        QDoubleSpinBox    *dSpinBox_penalty_;                  // "Penalty Weight"
        QDoubleSpinBox    *dSpinBox_primal_res_tolerance_;     // "Primal Res. Tol."
        QDoubleSpinBox    *dSpinBox_dual_res_tolerance_;       // "Dual Res. Tol."

    private slots:
        void load_cage();
        void load_cage_3D();
        void triangulation();
        void lbc_solver();
        void select();

    private:
        bool triangulation_ready_, solver_ready_, display_ready_;
        bool triangulation_ready_3d, solver_ready_3d, display_ready_3d;
        void update_controls();
        void update_controls_3d();
};


#endif // GLWIDGET_H
