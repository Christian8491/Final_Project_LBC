#ifndef LBCUI_3D_H
#define LBCUI_3D_H

#include "common.h"
#include <QImage>

class LBCUI_3D {

public:
    LBCUI_3D(): avg_cage_edge_length_3d(0.0), scale_3d(1.0), selected_point_3d(-1), has_cage_3d(false), edge_length_3d(1.0),
    mouse_picking_3d(false), has_delaunay_3d(false), active_3d(false){}
    ~LBCUI_3D();

public:
    bool read_cage_3d(const std::string& filename);
    void draw_3d(int pixel_scale);
    void my_Triangulation_3d();
    void set_mouse_select_3d();
    void show_m_cage_3d();
    int get_total_vertex();                                     // return the total of vertex
    int get_total_edges();
    void lbc_solver_3d(int weighting_scheme, int max_iter, double relaxation_alpha, double penalty_weight,
                       double primal_residual_tolerance, double dual_residual_tolerance);

private:
    void draw_delaunay_texture_3d();
    void fill_vertex_data_3d();
    void fill_faces_data_3d();
    void fill_edges_data_3d();
    void fill_triangulation_data_3d();
    double total_area_3d();

private:
    LBC::DenseMatrix3X delaunay_3d; //, delaunay_display_3d;
    LBC::DenseIndexMatrix delaunay_faces_3d;
    //LBC::DenseMatrix w_3d;
    LBC::DenseIndexMatrix faces_3d;
    LBC::Vector3d center_3d;                                  // center of the model
    LBC::DenseMatrix3X cage_3d, cage_display_3d;              // coordinates of the cage model (and the scaled cage model)

    double avg_cage_edge_length_3d;                           // average of edge's lenght
    double scale_3d;
    int selected_point_3d;
    bool has_cage_3d;                                         // when has a minimum of vertex (9)
    double edge_length_3d;
    bool mouse_picking_3d;                                    // when click in control point


public:
    bool has_delaunay_3d;                                     // when delaunay_Triangulation() is call
    bool active_3d;

    float *vertex_3d, *vertex_duplicate_3d;                                                 // Contains all vertex (scaled) -- Read "v "
    float *vertex_triangulation_3d, *vertex_triangulation_display_3d;
    int *faces_indices_3d, *faces_indices_triangulation_3d, *faces_indices_duplicate_3d;    // Contains the order of faces -- Read "f "
    float *edges_3d, *edges_triangulation_3d, *edges_duplicate_3d;                          // Contains triangles in order to draw
};

#endif // LBCUI_3D_H
