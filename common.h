#ifndef COMMON_H
#define COMMON_H

#include <QOpenGLFunctions>
#include <GL/gl.h>

#include "external/eigen/Eigen/Dense"
#include "external/eigen/Eigen/Sparse"

/* LBCUI.cpp */
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <QDebug>

/* glwidget.h */
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QDoubleSpinBox>

/* OpenGL.h */
#include <QGLFormat>
#include <QGLWidget>
#include <QMouseEvent>
#include <QEvent>
#include <QtGui/QOpenGLShaderProgram>

/* OpenGL.cpp */
#include <algorithm>

using namespace std;

namespace LBC{

typedef Eigen::MatrixXd DenseMatrix;
typedef Eigen::Matrix3Xd DenseMatrix3X;
typedef Eigen::VectorXd DenseVector;
typedef Eigen::MatrixXi DenseIndexMatrix;
typedef Eigen::VectorXi IndexVector;
typedef Eigen::Vector2d Vector2d;
typedef Eigen::Vector3d Vector3d;
typedef Eigen::Triplet<double> TripletD;
typedef Eigen::SparseMatrix<double> SparseMatrix;

}


#endif // COMMON_H
