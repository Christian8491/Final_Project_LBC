#include "LBCSolver.h"
#include "LBCUI_3D.h"
#include "common.h"

#ifdef SINGLE
#define REAL float
#else /* not SINGLE */
#define REAL double
#endif /* not SINGLE */


// VOID macro required for the default triangle library
#ifndef VOID
#define VOID void
#endif

// TRIVOID macro required for the win64 triangle library
#ifndef TRIVOID
#define TRIVOID void
#endif


extern "C"{
#include "external/triangle/triangle.h"
}

/* Show the data of cage_3d */
void LBCUI_3D::show_m_cage_3d()
{
    std::cout << "Num rows: " << cage_3d.rows() << std::endl;
    std::cout << "Num cols: " << cage_3d.cols() << std::endl;
    std::cout << "Data: " << std::endl;

    for (int j = 0; j < cage_3d.cols(); ++j) {
        for (int i = 0; i < cage_3d.rows(); ++i) {
            std::cout << cage_3d(i,j) << "  ";
        }
        std::cout << std::endl;
    }
}

/* Read and .obj file -> v: vertex and f: faces */
bool LBCUI_3D::read_cage_3d(const std::string& filename)
{
    std::ifstream vertexFile;
    vertexFile.open(filename.c_str());
    if (!vertexFile.is_open()) {
        std::cerr << "Unable to open file" << std::endl;
        return false;
    }

    /* Read vertex - "v" */
    std::vector<double> coordinates;
    std::vector<int> faces;
    while (vertexFile.good()) {
        std::string line;
        std::getline(vertexFile, line);

        if ((!vertexFile.fail()) && (!line.substr(0, 2).compare("v "))) {
            std::stringstream ss(line.substr(2));
            double x, y, z;
            ss >> x >> y >> z;

            if(!ss.fail()){
                coordinates.push_back(x);
                coordinates.push_back(y);
                coordinates.push_back(z);
            }
        }

        if ((!vertexFile.fail()) && (!line.substr(0, 2).compare("f "))) {
            std::stringstream ss(line.substr(2));
            int f1, f2, f3, r;
            //ss >> f1 >> f1 >> f1 >> f2 >> f2 >> f2 >> f3 >> f3 >> f3;      // FORMAT: f3
            //ss >> f1 >> f1 >> f2 >> f2 >> f3 >> f3;                        // FORMAT: f2
            //ss >> f1 >> r >> f2 >> r >> f3 >> r;                           // FORMAT: f2_r
            ss >> f1 >> f2 >> f3;                                            // armadillo and cage
            if(!ss.fail()){
                faces.push_back(f1);
                faces.push_back(f2);
                faces.push_back(f3);
            }
        }
    }

    has_cage_3d = coordinates.size() > 6;
    if (!has_cage_3d) {
        std::cerr << "Invalid cage file" << std::endl;
        return false;
    }

    int n_cage_vtx = coordinates.size() / 3;
    int n_faces = faces.size() / 3;

    cage_3d = Eigen::Map<Eigen::Matrix3Xd>(&(coordinates[0]), 3, n_cage_vtx);
    faces_3d = Eigen::Map<Eigen::MatrixXi>(&(faces[0]), 3, n_faces);

    std::cout << "cage_3d: " << cage_3d.rows() << " " << cage_3d.cols() << std::endl;
    std::cout << "faces_3d: " << faces_3d.rows() << " " << faces_3d.cols() << std::endl;

    /* Obtain the minimum and maximum coordinates */
    Eigen::Vector3d min_coord = cage_3d.rowwise().minCoeff();
    Eigen::Vector3d max_coord = cage_3d.rowwise().maxCoeff();

    center_3d = (min_coord + max_coord) * 0.5;
    scale_3d = (max_coord - min_coord).norm() * 0.5;

    /* Sum the total length of edges and find the average */
    for (int i = 0; i < n_cage_vtx / 3; ++i) {
        avg_cage_edge_length_3d += (cage_3d.col(3 * i + 0) - cage_3d.col(3 * i + 1)).norm();
        avg_cage_edge_length_3d += (cage_3d.col(3 * i + 1) - cage_3d.col(3 * i + 2)).norm();
        avg_cage_edge_length_3d += (cage_3d.col(3 * i + 2) - cage_3d.col(3 * i + 0)).norm();
    }
    avg_cage_edge_length_3d /= n_cage_vtx;

    /* Contains scaled cage_3d coordinates */
    cage_display_3d = (cage_3d.colwise() - center_3d) / scale_3d;

    fill_vertex_data_3d();
    fill_faces_data_3d();
    fill_edges_data_3d();

    return true;
}

/* Draw the contour and control points (coordinates of the obj) */
void LBCUI_3D::draw_3d(int pixel_scale)
{
    if(!has_cage_3d) return;

    double deafult_width = 2.0;

    /* To triangulation
    double triangle_line_width = std::min(deafult_width, pixel_scale * 0.04 * edge_length_3d / scale_3d);
    glLineWidth(triangle_line_width);

    if (has_delaunay_3d && mouse_picking_3d && w_3d.rows() == delaunay_3d.cols() && w_3d.cols() == cage_3d.cols()
        && selected_point_3d >= 0 && selected_point_3d < static_cast<int>(cage_3d.cols()))
    {
        draw_delaunay_texture_3d();
    }
    */
}


/* Calculate all the process to triangulate */
void LBCUI_3D::my_Triangulation_3d()
{
    fill_triangulation_data_3d();
    has_delaunay_3d = true;
    /* IT's Neccessary ??
    std::cout << "Delaunay Found!" << std::endl;

    double average_area = total_area_3d() / n_triangles * 1.6;
    edge_length_3d = sqrt(2.0 / sqrt(3) * average_area) * 1.6;

    std::cout << "Triangulation processed:" << std::endl;
    std::cout << "Average Area: " << average_area << std::endl;
    std::cout << "Edge length:  " << edge_length_3d << std::endl;
    std::cout << "Done" << std::endl;

    int n_boundary_vertex = 0;
    std::vector<double> position_x;
    std::vector<double> position_y;
    int n_vertex = cage_3d.cols();
    for (int i=0; i< n_vertex; ++i) {
        int next_i = (i + 1) % n_vertex;
        int n_segment = std::ceil((cage_3d.col(i) - cage_3d.col(next_i)).norm() / edge_length_3d);
        n_boundary_vertex += n_segment;

        for (int j=0; j< n_segment; j++) {
            Eigen::Vector3d boundary_pt = cage_3d.col(i) + j * (cage_3d.col(next_i) - cage_3d.col(i)) / n_segment;
            position_x.push_back(boundary_pt(0));
            position_y.push_back(boundary_pt(1));
        }
    }

    struct triangulateio in, out;
    */

}

void LBCUI_3D::lbc_solver_3d(int weighting_scheme, int max_iter, double relaxation_alpha, double penalty_weight,
                             double primal_residual_tolerance, double dual_residual_tolerance)
{
    if(!has_cage_3d) {
        std::cerr << "Error: no cage data" << std::endl;
        return;
    }

    std::cout << "Setting up input data" << std::endl;

    LBC::DenseMatrix sample_points = delaunay_3d.block(0, 0, 3, delaunay_3d.cols());
    LBC::DenseIndexMatrix &cell_vertices = delaunay_faces_3d;

    /* control_point_idx -- QUE VERTICES SON CONTROL POINTS -> TODOS EN 3D */
    LBC::IndexVector control_point_idx(cage_3d.cols());
    std::cout << "cage_3d.cols(): " << cage_3d.cols() << std::endl;
    for (int i = 0; i < cage_3d.cols(); ++i) {
        LBC::Vector3d vector_3d_col = cage_3d.block(0, i, 3, 1);
        int min_idx = -1;
        (sample_points.colwise() - vector_3d_col).colwise().norm().minCoeff(&min_idx);
        control_point_idx[i] = min_idx;
    }

    /* boundary_facet_info */
    std::vector< LBC::DataSetup::CageBoundaryFacetInfo > boundary_facet_info;
    int n_control_pt = cage_3d.cols();

    for (int i = 0; i < n_control_pt; ++i) {
        int next_i = (i+1) % n_control_pt;

    }

}


/* fill all vertex into a) vertex_3d & b) delaunay_3d */
void LBCUI_3D::fill_vertex_data_3d()
{
    /* a) vertex_3d */
    int size = cage_display_3d.cols() * cage_display_3d.rows() * 2;
    vertex_3d = new GLfloat[size];
    vertex_duplicate_3d = new GLfloat[size];

    int pos = 0, pos2 = 0;
    for (int j = 0; j < cage_display_3d.cols(); ++j) {
        for (int i = 0; i < cage_display_3d.rows(); ++i) {
            vertex_3d[pos++] = cage_display_3d(i, j);
            vertex_duplicate_3d[pos2++] = cage_display_3d(i, j) * 1.3;      // NOT FOUND
        }
    }

    /* b) delaunay_3d */
    delaunay_3d = cage_3d; //cage_display_3d;

}

/* fill all faces into a) faces_indices_3d & b) delaunay_faces_3d */
void LBCUI_3D::fill_faces_data_3d()
{
    /* a) faces_indices_3d */
    int size = faces_3d.cols() * faces_3d.rows();
    faces_indices_3d = new int[size];
    faces_indices_duplicate_3d = new int[size];

    int pos = 0, pos2 = 0;
    for (int j = 0; j < faces_3d.cols(); ++j) {
        for (int i = 0; i < faces_3d.rows(); ++i) {
            faces_indices_3d[pos++] = faces_3d(i, j);
            faces_indices_duplicate_3d[pos2++] = faces_3d(i, j);
        }
    }

    /* b) delaunay_faces_3d */
    delaunay_faces_3d = faces_3d;
}

/* Fill all edges in order of faces to draw with GL_TRIANGLES */
void LBCUI_3D::fill_edges_data_3d()
{
    int size = get_total_edges() * 3;
    edges_3d = new GLfloat[size];
    edges_duplicate_3d = new GLfloat[size];

    int cont = 0, cont2 = 0;
    for (int i = 0; i < get_total_edges() ; ++i) {
        int face_i = faces_indices_3d[i];
        edges_3d[cont++] = vertex_3d[face_i * 3 - 3];
        edges_3d[cont++] = vertex_3d[face_i * 3 - 2];
        edges_3d[cont++] = vertex_3d[face_i * 3 - 1];

        edges_duplicate_3d[cont2++] = vertex_duplicate_3d[face_i * 3 - 3];
        edges_duplicate_3d[cont2++] = vertex_duplicate_3d[face_i * 3 - 2];
        edges_duplicate_3d[cont2++] = vertex_duplicate_3d[face_i * 3 - 1];
    }
}

/* When Display Weights is activates or deactivated */
void LBCUI_3D::set_mouse_select_3d()
{
    mouse_picking_3d = !mouse_picking_3d;
}

/* Calculate the area of the 3D model (surface) */
double LBCUI_3D::total_area_3d()
{
    double total_area = 0.0;
    int n = cage_3d.cols();

    for (int i = 0; i < n - 2; ++i) {
        total_area += cage_3d.block(0, i, 3, 3).determinant();
    }

    Eigen::Matrix3d last_block_1, last_block_2;
    last_block_1.col(0) = cage_3d.block(0, n - 2, 3, 1);
    last_block_1.col(1) = cage_3d.block(0, n - 1, 3, 1);
    last_block_1.col(2) = cage_3d.block(0, 0, 3, 1);
    total_area += last_block_1.determinant();

    last_block_2.col(0) = cage_3d.block(0, n - 1, 3, 1);
    last_block_2.col(1) = cage_3d.block(0, 0, 3, 1);
    last_block_2.col(2) = cage_3d.block(0, 1, 3, 1);
    total_area += last_block_2.determinant();

    std::cout << "Total Area: "  << total_area << std::endl;
    return 0.5 * std::abs(total_area);;
}

/* Return the total of vertex */
int LBCUI_3D::get_total_vertex()
{
    return cage_3d.cols();
}

/* Return the total of edges */
int LBCUI_3D::get_total_edges()
{
    return faces_3d.rows() * faces_3d.cols();
}

/* Fill data to create new faces to triangulate */
void LBCUI_3D::fill_triangulation_data_3d()
{
    int size = faces_3d.cols() * 36;
    vertex_triangulation_3d = new GLfloat[size];
    vertex_triangulation_display_3d = new GLfloat[faces_3d.cols() * 9];

    int cont = 0 , cont2 = 0;
    int face_i[3];
    float vectorA[3], vectorB[3], vectorC[3];

    for (int i = 0; i < faces_3d.cols(); ++i) {
        face_i[0] = faces_indices_3d[3 * i + 0];
        face_i[1] = faces_indices_3d[3 * i + 1];
        face_i[2] = faces_indices_3d[3 * i + 2];

        vectorA[0] = (vertex_3d[face_i[0] * 3 - 3] + vertex_3d[face_i[1] * 3 - 3]) / 2;
        vectorA[1] = (vertex_3d[face_i[0] * 3 - 2] + vertex_3d[face_i[1] * 3 - 2]) / 2;
        vectorA[2] = (vertex_3d[face_i[0] * 3 - 1] + vertex_3d[face_i[1] * 3 - 1]) / 2;

        vectorB[0] = (vertex_3d[face_i[1] * 3 - 3] + vertex_3d[face_i[2] * 3 - 3]) / 2;
        vectorB[1] = (vertex_3d[face_i[1] * 3 - 2] + vertex_3d[face_i[2] * 3 - 2]) / 2;
        vectorB[2] = (vertex_3d[face_i[1] * 3 - 1] + vertex_3d[face_i[2] * 3 - 1]) / 2;

        vectorC[0] = (vertex_3d[face_i[2] * 3 - 3] + vertex_3d[face_i[0] * 3 - 3]) / 2;
        vectorC[1] = (vertex_3d[face_i[2] * 3 - 2] + vertex_3d[face_i[0] * 3 - 2]) / 2;
        vectorC[2] = (vertex_3d[face_i[2] * 3 - 1] + vertex_3d[face_i[0] * 3 - 1]) / 2;

        /* Face 1 */
        for (int j = 0; j < 3; ++j) {
            vertex_triangulation_3d[cont++] = vertex_3d[face_i[0] * 3 - (3 - j)];
        }
        for (int j = 0; j < 3; ++j) {
            vertex_triangulation_3d[cont++] = vectorA[j];
        }
        for (int j = 0; j < 3; ++j) {
            vertex_triangulation_3d[cont++] = vectorC[j];
        }

        /* Face 2 */
        for (int j = 0; j < 3; ++j) {
            vertex_triangulation_3d[cont++] = vectorA[j];
        }
        for (int j = 0; j < 3; ++j) {
            vertex_triangulation_3d[cont++] = vertex_3d[face_i[1] * 3 - (3 - j)];
        }
        for (int j = 0; j < 3; ++j) {
            vertex_triangulation_3d[cont++] = vectorB[j];
        }

        /* Face 3 */
        for (int j = 0; j < 3; ++j) {
            vertex_triangulation_3d[cont++] = vectorB[j];
        }
        for (int j = 0; j < 3; ++j) {
            vertex_triangulation_3d[cont++] = vertex_3d[face_i[2] * 3 - (3 - j)];
        }
        for (int j = 0; j < 3; ++j) {
            vertex_triangulation_3d[cont++] = vectorC[j];
        }

        /* Face 4 */
        for (int j = 0; j < 3; ++j) {
            vertex_triangulation_display_3d[cont2++] = vectorA[j];
            vertex_triangulation_3d[cont++] = vectorA[j];
        }
        for (int j = 0; j < 3; ++j) {
            vertex_triangulation_display_3d[cont2++] = vectorB[j];
            vertex_triangulation_3d[cont++] = vectorB[j];
        }
        for (int j = 0; j < 3; ++j) {
            vertex_triangulation_display_3d[cont2++] = vectorC[j];
            vertex_triangulation_3d[cont++] = vectorC[j];
        }
    }

    std::cout << cont << std::endl;

}
