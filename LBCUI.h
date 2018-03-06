#ifndef LBCUI_H
#define LBCUI_H

#include "common.h"
#include <QImage>

class LBCUI
{
public:
    LBCUI()
    :m_edge_length(1.0), m_scale(1.0), m_avg_cage_edge_length(1.0), m_mouse_picking(false), m_has_cage(false),
    m_selected_point(-1), active_2d(false), m_texture_initialized(false), total_coordinates(0), m_has_delaunay(false){}
    ~LBCUI();

public:
    bool read_cage(const std::string& filename);
    void draw(int pixel_scale);
    void delaunay_Triangulation(int numTriangles);
    void set_mouse_select();
    void lbc_solver(int weighting_scheme, int max_iter, double relaxation_alpha, double penalty_weight,
                    double primal_residual_tolerance, double dual_residual_tolerance);
    int select(double x, double y);
    void setup_texture();

private:
    double domain_area();
    void draw_delaunay_texture();
    double map_value_to_tex_coord(double value, bool log_scale = true);
    void fill_data_points_edges();
    void fill_data_triangle();
    void generate_buffer();

private:
    LBC::DenseMatrix3X m_delaunay, m_delaunay_display;
    LBC::DenseIndexMatrix m_delaunay_faces;
    LBC::Vector3d m_center;
    LBC::DenseMatrix3X m_cage, m_cage_display;              // coordinates of the cage model
    LBC::DenseMatrix m_w;

    double m_edge_length;
    double m_scale;
    double m_avg_cage_edge_length;                          // average of edge's lenght

    bool m_mouse_picking;                                   // when click in control point
    bool m_has_cage;                                        // when read_cage() is valid
    int m_selected_point;

public:
    /* To openGL modern */
    bool active_2d;
    QImage m_texture;
    GLuint m_TextureID;
    bool m_texture_initialized;
    float* coordinates_scaled;
    int total_coordinates;
    bool m_has_delaunay;                                    // when delaunay_Triangulation() is call
    float* triang_coordinates;
    int total_triangles;
};


#endif // LBCUI_H
