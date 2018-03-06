#include "shaders.h"
#include "OpenGL.h"

COpenGL::COpenGL(const QGLFormat &format, QWidget *parent) : QGLWidget(format, parent)
{
    lbc = new LBCUI;
    lbc_3d = new LBCUI_3D;
    selected = -1;
}

COpenGL::~COpenGL(){}

void COpenGL::enableAll()
{
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glShadeModel(GL_SMOOTH);                                //  select flat or smooth shading
    glEnable(GL_DOUBLEBUFFER);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_DEPTH_TEST);                                // Enable depth buffer
    glPointSize(3.0);       // 13.0
}

/* Sets up the OpenGL resources and state - load shaders */
void COpenGL::initializeGL()
{
    enableAll();

    /* Initialize OpenGL */
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
    m_posAttr = m_program->attributeLocation("posAttr");
    m_colorAttr = m_program->uniformLocation("colorAttr");
    m_matrixUniform = m_program->uniformLocation("matrix");

    m_program_text = new QOpenGLShaderProgram(this);
    m_program_text->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexTexture);
    m_program_text->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentTexture);
    m_program_text->link();
    m_posAttr_text = m_program_text->attributeLocation("posAttr");
    m_matrixUniform_text = m_program_text->attributeLocation("matrix");
    m_text_coord = m_program_text->attributeLocation("vTextCoord");
    texture_2D = m_program_text->attributeLocation("texture_2D");

    /* Model - View - Projection */
    view.lookAt(QVector3D(0.0, 0.0, 1.0), QVector3D(0, 0, 0), QVector3D(0, 1, 0));      // QVector3D(0.6, 0.7, 1.0)
    projection.perspective(45.0, 4.0f/3.0f, 0.1f, 100.0f);
    projection.translate(0.0, 0.0, -2.0); // 0.0, 0.0, -2.0
    projection.scale(1.4, 1.2, 1);

    /* Texture */
    lbc->m_texture.load("./colorbar_texture.png");
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1,&(lbc->m_TextureID));
    //glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, lbc->m_TextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, lbc->m_texture.depth()/8, lbc->m_texture.width(),
         lbc->m_texture.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, lbc->m_texture.bits());
    glEnable(GL_TEXTURE_2D);
    openGL_object.initializeOpenGLFunctions();
}

/* Update projection matrix and other size related settings */
void COpenGL::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);                     // Use the entire window for rendering.
}


/* ------------------------------------------ CONTROL POINTS ----------------------------------------------- */

/* Draw 2D control points */
void COpenGL::draw_control_points()
{
    m_program->bind();
    glPointSize(12.0);

    /* Uniforms */
    m_program->setUniformValue(m_colorAttr, QVector4D(1.0, 0.5, 0.0, 1.0));             // orange color
    m_program->setUniformValue(m_matrixUniform, projection * view * model);

    /* Attributes */
    openGL_object.glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, lbc->coordinates_scaled);
    openGL_object.glEnableVertexAttribArray(0);
    glDrawArrays(GL_POINTS, 0, lbc->total_coordinates / 3);
    openGL_object.glDisableVertexAttribArray(0);

    m_program->release();
}

/* Draw 3D control points */
void COpenGL::draw_control_points_3d()
{
    m_program->bind();
    glPointSize(10.0);

    /* Uniforms */
    m_program->setUniformValue(m_colorAttr, QVector4D(1.0, 0.5, 0.0, 1.0));     // orange color
    m_program->setUniformValue(m_matrixUniform, projection * view * model);

    /* Attributes */
    openGL_object.glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, lbc_3d->vertex_3d);
    openGL_object.glEnableVertexAttribArray(0);
    glDrawArrays(GL_POINTS, 0, lbc_3d->get_total_vertex());
    openGL_object.glDisableVertexAttribArray(0);

    m_program->release();
}

/* Draw 3D control points for the cage generation */
void COpenGL::draw_control_points_cage_3d()
{
    m_program->bind();

    /* Uniforms */
    m_program->setUniformValue(m_colorAttr, QVector4D(1.0, 0.5, 0.0, 1.0));     // orange color
    m_program->setUniformValue(m_matrixUniform, projection * view * model);

    /* Attributes */
    openGL_object.glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, lbc_3d->vertex_duplicate_3d);
    openGL_object.glEnableVertexAttribArray(0);
    glDrawArrays(GL_POINTS, 0, lbc_3d->get_total_vertex());
    openGL_object.glDisableVertexAttribArray(0);

    m_program->release();
}
/* ------------------------------------------------- EDGES -------------------------------------------------- */

/* Draw 2D edges */
void COpenGL::draw_edges()
{
    m_program->bind();

    glLineWidth(3.0);

    /* Uniforms */
    m_program->setUniformValue(m_colorAttr, QVector4D(0.2, 0.2, 0.2, 1.0));     // gray color
    m_program->setUniformValue(m_matrixUniform, projection * view * model);

    /* Attributes */
    openGL_object.glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, lbc->coordinates_scaled);
    openGL_object.glEnableVertexAttribArray(0);
    glDrawArrays(GL_LINE_LOOP, 0, lbc->total_coordinates / 3);
    openGL_object.glDisableVertexAttribArray(0);

    m_program->release();
}

/* Draw 3D edges */
void COpenGL::draw_edges_3d()
{
    m_program->bind();

    glLineWidth(4.0);// 6.0
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    /* Uniforms */
    m_program->setUniformValue(m_colorAttr, QVector4D(0, 0, 0, 1.0));
    m_program->setUniformValue(m_matrixUniform, projection * view * model);

    /* Attributes */
    openGL_object.glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, lbc_3d->edges_3d);
    openGL_object.glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, lbc_3d->get_total_edges());
    openGL_object.glDisableVertexAttribArray(0);

    m_program->release();
}

/* Draw 3D edges for the cage */
void COpenGL::draw_edges_cage_3d()
{
    m_program->bind();
    glLineWidth(2.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    /* Uniforms */
    m_program->setUniformValue(m_colorAttr, QVector4D(0.1, 0.1, 0.1, 1.0));
    m_program->setUniformValue(m_matrixUniform, projection * view * model);

    /* Attributes */
    openGL_object.glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, lbc_3d->edges_duplicate_3d);
    openGL_object.glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, lbc_3d->get_total_edges());
    openGL_object.glDisableVertexAttribArray(0);

    m_program->release();
}


/* ------------------------------------------------ FACES ------------------------------------------------- */

/* Only for 3d models */
void COpenGL::draw_faces_3d()
{
    m_program->bind();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    /* Uniforms */
    m_program->setUniformValue(m_colorAttr, QVector4D(0.82, 0.82, 0.82, 1.0));
    m_program->setUniformValue(m_matrixUniform, projection * view * model);

    /* Attributes */
    openGL_object.glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, lbc_3d->edges_3d);
    openGL_object.glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, lbc_3d->get_total_edges());
    openGL_object.glDisableVertexAttribArray(0);

    m_program->release();
}


/* ------------------------------------------------ DELAUNAY ------------------------------------------------- */

/* Draw delaunay triangulation */
void COpenGL::draw_delaunay()
{
    m_program->bind();
    glLineWidth(1.2);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    /* Uniforms */
    m_program->setUniformValue(m_colorAttr, QVector4D(0.0, 0.1, 0.7, 1.0));     // blue color
    m_program->setUniformValue(m_matrixUniform, projection * view * model);

    /* Attributes */
    openGL_object.glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, lbc->triang_coordinates);
    openGL_object.glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, lbc->total_triangles * 3);
    openGL_object.glDisableVertexAttribArray(0);

    m_program->release();
}

void COpenGL::draw_triangulation_3d()
{
    m_program->bind();
    glLineWidth(2.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    /* Uniforms */
    m_program->setUniformValue(m_colorAttr, QVector4D(0, 0, 1.0, 1.0));
    m_program->setUniformValue(m_matrixUniform, projection * view * model);
    /* Attributes */
    openGL_object.glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, lbc_3d->vertex_triangulation_display_3d);
    openGL_object.glEnableVertexAttribArray(0);

    glDrawArrays(GL_TRIANGLES, 0, 3 * 216);
    openGL_object.glDisableVertexAttribArray(0);

    m_program->release();
}

/* Draw texture */
void COpenGL::draw_texture()
{
    m_program->bind();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    /* Uniforms */
    m_program_text->setUniformValue(m_matrixUniform_text, projection * view * model);
    m_program_text->setUniformValue(texture_2D, lbc->m_TextureID);

    /* Attributes */
    openGL_object.glVertexAttribPointer(m_posAttr_text, 3, GL_FLOAT, GL_FALSE, 0, lbc->triang_coordinates);
    openGL_object.glEnableVertexAttribArray(0);
    openGL_object.glVertexAttribPointer(m_text_coord, 2, GL_FLOAT, GL_FALSE, 1, &(lbc->m_texture));
    openGL_object.glEnableVertexAttribArray(1);
    glDrawArrays(GL_TRIANGLES, 0, lbc->total_triangles * 2);
    openGL_object.glDisableVertexAttribArray(0);
    openGL_object.glDisableVertexAttribArray(1);

    /*  #define BUFFER_OFFSET( offset )   ((GLvoid*) (offset)) */

    m_program->release();
}

/* --------------------------------------------- Draw the scene ------------------------------------------- */

void COpenGL::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* To 2D render */
    if (lbc->active_2d == true) {
        lbc_3d->active_3d = false;
        if (lbc->total_coordinates > 0) {
            draw_control_points();
            draw_edges();
        }
        if (lbc->m_has_delaunay) draw_delaunay();
        //if (lbc->m_texture_initialized) draw_texture();
        lbc->draw(std::min(width(), height()));
    }

    /* To 3D render */
    if (lbc_3d->active_3d == true) {
        lbc->active_2d = false;
        if (lbc_3d->get_total_vertex() > 0) {
            draw_faces_3d();
            draw_edges_3d();
            draw_control_points_3d();
        }
        if (lbc_3d->has_delaunay_3d == true) draw_triangulation_3d();
    }

}

void COpenGL::Render()
{
    lbc->draw(std::min(width(), height()));
}

void COpenGL::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        double x = e->pos().x();
        double y = e->pos().y();
        double base_length = std::min(width(), height());

        selected = lbc->select((2*x-width())/base_length, (-2*y+height())/base_length);
        updateGL();
    }
}
