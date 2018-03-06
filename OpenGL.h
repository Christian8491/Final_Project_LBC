#ifndef OPENGL_H
#define OPENGL_H

#include "common.h"
#include "LBCUI.h"
#include "LBCUI_3D.h"

class COpenGL : public QGLWidget
{
    public:
        COpenGL(const QGLFormat &format, QWidget *parent = 0);
        ~COpenGL();
        void enableAll();
        void draw_control_points();
        void draw_control_points_3d();
        void draw_control_points_cage_3d();
        void draw_edges();
        void draw_edges_3d();
        void draw_edges_cage_3d();
        void draw_faces_3d();
        void draw_triangulation_3d();       // Like delaunay
        void draw_delaunay();
        void draw_texture();
        void Render();

    protected:
        void initializeGL();
        void resizeGL(int w, int h);
        void paintGL();
        void mousePressEvent(QMouseEvent *e);

        int selected;

    public:
        LBCUI *lbc;
        LBCUI_3D *lbc_3d;
        QOpenGLShaderProgram *m_program, *m_program_text;
        QOpenGLFunctions openGL_object;
        GLuint m_posAttr, m_posAttr_text;
        GLuint m_colorAttr;
        GLuint m_matrixUniform, m_matrixUniform_text;
        GLuint m_text_coord;
        GLuint texture_2D;
        QMatrix4x4 model, view, projection;
};

#endif // OPENGL_H
