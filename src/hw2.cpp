#include "hw2.h"
#include "hw2_scenes.h"
#include "hw1.h"
#include <limits>

using namespace hw2;

#define divisions 4
//bool halfPlane(Vector2 p0, Vector2 p1, Vector2 q) {
//    Vector2 n = Vector2(p1.y - p0.y, p0.x - p1.x);
//    if (dot(q - p0, n) > 0) {
//        return true;
//    }
//    return false;
//}
bool halfPlane(Vector2 p0, Vector2 p1, Vector2 q) {
    Vector2 n = Vector2(p1.y - p0.y, p0.x - p1.x);
    if (dot(q - p0, n) > 0) {
        return true;
    }
    return false;
}

Vector3 avgCol(Vector3 colors[divisions][divisions]) {
    Vector3 acc = { 0,0,0 };
    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            acc += colors[i][j];
        }
    }
    return (acc / (divisions * divisions * 1.0));
}
Image3 hw_2_1(const std::vector<std::string> &params) {
    // Homework 2.1: render a single 3D triangle

    Image3 img(640 /* width */, 480 /* height */);

    Vector3 p0{0, 0, -1};
    Vector3 p1{1, 0, -1};
    Vector3 p2{0, 1, -1};
    Real s = 1; // scaling factor of the view frustrum
    Vector3 color = Vector3{1.0, 0.5, 0.5};
    Real z_near = 1e-6; // distance of the near clipping plane
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-p0") {
            p0.x = std::stof(params[++i]);
            p0.y = std::stof(params[++i]);
            p0.z = std::stof(params[++i]);
        } else if (params[i] == "-p1") {
            p1.x = std::stof(params[++i]);
            p1.y = std::stof(params[++i]);
            p1.z = std::stof(params[++i]);
        } else if (params[i] == "-p2") {
            p2.x = std::stof(params[++i]);
            p2.y = std::stof(params[++i]);
            p2.z = std::stof(params[++i]);
        } else if (params[i] == "-color") {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            color = Vector3{r, g, b};
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        }
    }

   
    //triangle code
    // calculate points in projected camera space
    Vector2 p0pc { 0.0 - p0.x / p0.z , 0.0-p0.y/p0.z};
    Vector2 p1pc{ 0.0 - p1.x / p1.z , 0.0 - p1.y / p1.z };
    Vector2 p2pc{ 0.0 - p2.x / p2.z , 0.0 - p2.y / p2.z };
    
    //calculate the screen space points
    Real a = (Real)img.width / (Real)img.height;
    Vector2 p0ss{img.width*(p0pc.x+s*a)/(2.0*s*a), img.height*(p0pc.y-s)/(0.0-2.0*s)};
    Vector2 p1ss{ img.width * (p1pc.x + s * a) / (2.0 * s * a), img.height * (p1pc.y - s) / (0.0 - 2.0 * s) };
    Vector2 p2ss{ img.width * (p2pc.x + s * a) / (2.0 * s * a), img.height * (p2pc.y - s) / (0.0 - 2.0 * s) };

    // paint background
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{ 0.5, 0.5, 0.5 };
        }
    }

    // if any point is behind clipping plane, ignore this triangle completely
    if (0.0 - p0.z < z_near || 0.0 - p1.z < z_near || 0.0 - p2.z < z_near) {
        return img;
    }
    //for each point in the screen space
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {

            //divide that pixel into 16 subdivisions (4x4) and compute color
            Vector3 subColors[divisions][divisions];
            for (int i = 0; i < divisions; i++) {
                for (int j = 0; j < divisions; j++) {
                    Vector2 q = Vector2(x + (Real(1.0 / divisions) * i) + Real(1.0 / (divisions * 2.0)), y + (Real(1.0 / divisions) * j) + Real(1.0 / (divisions * 2.0)));
                    bool hp01 = halfPlane(p0ss, p1ss, q);
                    bool hp12 = halfPlane(p1ss, p2ss, q);
                    bool hp20 = halfPlane(p2ss, p0ss, q);
                    if ((hp01 && hp12 && hp20) || !(hp01 || hp12 || hp20)) {
                        subColors[i][j] = color;
                    }
                    else {
                        subColors[i][j] = img(x, y);
                    }
                }
            }
            img(x, y) = avgCol(subColors);
        }
    }
    
    return img;
}

Real absV(Real x) {
    if (x < 0) {
        return -x;
    }
    else {
        return x;
    }
}

Real area(Vector3 p0, Vector3 p1, Vector3 p2) {
    return absV(length(cross(p1 - p0, p2 - p0)) / 2.0);
}


Vector3 baryVals(Vector3 p0, Vector3 p1, Vector3 p2, Vector3 p0p, Vector3 p1p, Vector3 p2p, Vector3 p) {
    Real denom = area(p0p, p1p, p2p);
    Real b0p = area(p, p1p, p2p) / denom;
    Real b1p = area(p0p, p, p2p) / denom;
    Real b2p = area(p0p, p1p, p) / denom;

    Real denom_p = (b0p / p0.z) + (b1p / p1.z) + (b2p / p2.z);
    Real b0 = (b0p / p0.z) / denom_p;
    Real b1 = (b1p / p1.z) / denom_p;
    Real b2 = (b2p / p2.z) / denom_p;

    return Vector3(b0, b1, b2);
}


Real depth(Vector3 p0, Vector3 p1, Vector3 p2, Vector3 p0p, Vector3 p1p, Vector3 p2p, Vector3 p) {
    Real denom = area(p0p, p1p, p2p);
    Real b0p = area(p, p1p,p2p) / denom;
    Real b1p = area(p0p,p,p2p) / denom;
    Real b2p = area(p0p,p1p,p)/ denom;

    Real denom_p = (b0p / p0.z) + (b1p / p1.z) + (b2p / p2.z);
    Real b0 = (b0p / p0.z) / denom_p;
    Real b1 = (b1p / p1.z) / denom_p;
    Real b2 = (b2p / p2.z) / denom_p;

    //std::cout << b0 + b1 + b2 << "\n";
    return (b0*p0.z + b1*p1.z+b2*p2.z);
}

Image3 hw_2_2(const std::vector<std::string> &params) {
    // Homework 2.2: render a triangle mesh

    Image3 img(640 /* width */, 480 /* height */);

    Real s = 1; // scaling factor of the view frustrum
    Real z_near = 1e-6; // distance of the near clipping plane
    int scene_id = 0;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        } else if (params[i] == "-scene_id") {
            scene_id = std::stoi(params[++i]);
        }
    }
    // silence warnings, feel free to remove these
    UNUSED(s); 
    UNUSED(z_near);
    UNUSED(scene_id);

    TriangleMesh mesh = meshes[scene_id];
    UNUSED(mesh); // silence warning, feel free to remove this
 
    // paint background
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{ 0.5, 0.5, 0.5 };
        }
    }

  
    //for each point in the screen space
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            //divide that pixel into 16 subdivisions (4x4) and compute color
            Vector3 subColors[divisions][divisions];
            for (int i = 0; i < divisions; i++) {
                for (int j = 0; j < divisions; j++) {
                    Real z_min = 0.0-std::numeric_limits<Real>::max();
                    int kset = -1;
                    //std::cout<< z_min;
                    //set subpixel to background or prevous color
                    subColors[i][j] = img(x, y);
                    for (int k = 0; k < mesh.faces.size(); k++) {

                        Vector3i face = mesh.faces.at(k);
                        Vector3 p0 = mesh.vertices.at(face.x);
                        Vector3 p1 = mesh.vertices.at(face.y);
                        Vector3 p2 = mesh.vertices.at(face.z);
                        // if any point is behind clipping plane, ignore this triangle completely
                        if (0.0 - p0.z < z_near || 0.0 - p1.z < z_near || 0.0 - p2.z < z_near) {
                            return img;
                        }
                        // calculate points in projected camera space
                        Vector2 p0pc{ 0.0 - p0.x / p0.z , 0.0 - p0.y / p0.z };
                        Vector2 p1pc{ 0.0 - p1.x / p1.z , 0.0 - p1.y / p1.z };
                        Vector2 p2pc{ 0.0 - p2.x / p2.z , 0.0 - p2.y / p2.z };
                        Vector2 q = Vector2(x + (Real(1.0 / divisions) * i) + Real(1.0 / (divisions * 2.0)), y + (Real(1.0 / divisions) * j) + Real(1.0 / (divisions * 2.0)));
                         //calculate the screen space points
                        Real a = (Real)img.width / (Real)img.height;
                        Vector2 p0ss{ (Real)img.width * (p0pc.x + s * a) / (2.0 * s * a), (Real)img.height * (p0pc.y - s) / (0.0 - 2.0 * s) };
                        Vector2 p1ss{ (Real)img.width * (p1pc.x + s * a) / (2.0 * s * a), (Real)img.height * (p1pc.y - s) / (0.0 - 2.0 * s) };
                        Vector2 p2ss{ (Real)img.width * (p2pc.x + s * a) / (2.0 * s * a), (Real)img.height * (p2pc.y - s) / (0.0 - 2.0 * s) };


                        bool hp01 = halfPlane(p0ss, p1ss, q);
                        bool hp12 = halfPlane(p1ss, p2ss, q);
                        bool hp20 = halfPlane(p2ss, p0ss, q);
                        if ((hp01 && hp12 && hp20) || !(hp01 || hp12 || hp20)) {
                            Vector3 qp = Vector3( (2.0*q.x*s*a/(Real)img.width)-(s*a), (0.0-2.0*s*q.y/(Real)img.height) + s, 0.0);
                            Real this_depth = depth(p0, p1, p2, Vector3(p0pc.x, p0pc.y, 0.0), Vector3(p1pc.x, p1pc.y, 0.0), Vector3(p2pc.x, p2pc.y, 0.0), qp);
                            //Real this_depth = depth(p0, p1, p2, Vector3(p0ss.x, p0ss.y, 0.0), Vector3(p1ss.x, p1ss.y, 0.0), Vector3(p2ss.x, p2ss.y, 0.0), Vector3(q.x,q.y,0.0));
                            if(this_depth > z_min){
                                if (k == 8 || k == 9) {
                                    //std::cout << "zmin = " << z_min << " and this_depth = " << this_depth << " set in " << kset << "\n";
                                }
                                subColors[i][j] = mesh.face_colors.at(k);
                                z_min = this_depth;
                                kset = k;
                            }
                            
                        }
                      
                    }
                }
            }
            img(x, y) = avgCol(subColors);
        }
    }

    return img;
}

Image3 hw_2_3(const std::vector<std::string> &params) {
    // Homework 2.3: render a triangle mesh with vertex colors

    Image3 img(640 /* width */, 480 /* height */);

    Real s = 1; // scaling factor of the view frustrum
    Real z_near = 1e-6; // distance of the near clipping plane
    int scene_id = 0;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        } else if (params[i] == "-scene_id") {
            scene_id = std::stoi(params[++i]);
        }
    }
    // silence warnings, feel free to remove these
    UNUSED(s); 
    UNUSED(z_near);
    UNUSED(scene_id);

    TriangleMesh mesh = meshes[scene_id];
    UNUSED(mesh); // silence warning, feel free to remove this
    // paint background
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{ 0.5, 0.5, 0.5 };
        }
    }


    //for each point in the screen space
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            //divide that pixel into 16 subdivisions (4x4) and compute color
            Vector3 subColors[divisions][divisions];
            for (int i = 0; i < divisions; i++) {
                for (int j = 0; j < divisions; j++) {
                    Real z_min = 0.0 - std::numeric_limits<Real>::max();
                    int kset = -1;
                    //std::cout<< z_min;
                    //set subpixel to background or prevous color
                    subColors[i][j] = img(x, y);
                    for (int k = 0; k < mesh.faces.size(); k++) {

                        Vector3i face = mesh.faces.at(k);
                        Vector3 p0 = mesh.vertices.at(face.x);
                        Vector3 p1 = mesh.vertices.at(face.y);
                        Vector3 p2 = mesh.vertices.at(face.z);
                        // if any point is behind clipping plane, ignore this triangle completely
                        if (0.0 - p0.z < z_near || 0.0 - p1.z < z_near || 0.0 - p2.z < z_near) {
                            return img;
                        }
                        // calculate points in projected camera space
                        Vector2 p0pc{ 0.0 - p0.x / p0.z , 0.0 - p0.y / p0.z };
                        Vector2 p1pc{ 0.0 - p1.x / p1.z , 0.0 - p1.y / p1.z };
                        Vector2 p2pc{ 0.0 - p2.x / p2.z , 0.0 - p2.y / p2.z };
                        Vector2 q = Vector2(x + (Real(1.0 / divisions) * i) + Real(1.0 / (divisions * 2.0)), y + (Real(1.0 / divisions) * j) + Real(1.0 / (divisions * 2.0)));
                        //calculate the screen space points
                        Real a = (Real)img.width / (Real)img.height;
                        Vector2 p0ss{ (Real)img.width * (p0pc.x + s * a) / (2.0 * s * a), (Real)img.height * (p0pc.y - s) / (0.0 - 2.0 * s) };
                        Vector2 p1ss{ (Real)img.width * (p1pc.x + s * a) / (2.0 * s * a), (Real)img.height * (p1pc.y - s) / (0.0 - 2.0 * s) };
                        Vector2 p2ss{ (Real)img.width * (p2pc.x + s * a) / (2.0 * s * a), (Real)img.height * (p2pc.y - s) / (0.0 - 2.0 * s) };


                        bool hp01 = halfPlane(p0ss, p1ss, q);
                        bool hp12 = halfPlane(p1ss, p2ss, q);
                        bool hp20 = halfPlane(p2ss, p0ss, q);
                        if ((hp01 && hp12 && hp20) || !(hp01 || hp12 || hp20)) {
                            Vector3 qp = Vector3((2.0 * q.x * s * a / (Real)img.width) - (s * a), (0.0 - 2.0 * s * q.y / (Real)img.height) + s, 0.0);
                            //Real this_depth = depth(p0, p1, p2, Vector3(p0pc.x, p0pc.y, 0.0), Vector3(p1pc.x, p1pc.y, 0.0), Vector3(p2pc.x, p2pc.y, 0.0), qp);
                            Vector3 bary = baryVals(p0, p1, p2, Vector3(p0ss.x, p0ss.y, 0.0), Vector3(p1ss.x, p1ss.y, 0.0), Vector3(p2ss.x, p2ss.y, 0.0), Vector3(q.x, q.y, 0.0));
                            Real this_depth = depth(p0, p1, p2, Vector3(p0ss.x, p0ss.y, 0.0), Vector3(p1ss.x, p1ss.y, 0.0), Vector3(p2ss.x, p2ss.y, 0.0), Vector3(q.x,q.y,0.0));
                            if (this_depth > z_min) {
                                subColors[i][j] = bary.x * mesh.vertex_colors.at(face.x) + bary.y * mesh.vertex_colors.at(face.y) + bary.z * mesh.vertex_colors.at(face.z);
                                z_min = this_depth;
                                kset = k;
                            }
                        }
                    }
                }
            }
            img(x, y) = avgCol(subColors);
        }
    }

    return img;
}

Image3 hw_2_4(const std::vector<std::string> &params) {
    // Homework 2.4: render a scene with transformation
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.camera.resolution.x,
               scene.camera.resolution.y);

    Real z_near = scene.camera.z_near;
    Real s = scene.camera.s;
    for (TriangleMesh mesh : scene.meshes) {
        UNUSED(mesh); // silence warning, feel free to remove this
        // paint background
        for (int y = 0; y < img.height; y++) {
            for (int x = 0; x < img.width; x++) {
                img(x, y) = Vector3{ 0.5, 0.5, 0.5 };
            }
        }


        //for each point in the screen space
        for (int y = 0; y < img.height; y++) {
            for (int x = 0; x < img.width; x++) {
                //divide that pixel into 16 subdivisions (4x4) and compute color
                Vector3 subColors[divisions][divisions];
                for (int i = 0; i < divisions; i++) {
                    for (int j = 0; j < divisions; j++) {
                        Real z_min = 0.0 - std::numeric_limits<Real>::max();
                        int kset = -1;
                        //std::cout<< z_min;
                        //set subpixel to background or prevous color
                        subColors[i][j] = img(x, y);
                        for (int k = 0; k < mesh.faces.size(); k++) {

                            Vector3i face = mesh.faces.at(k);
                            Vector3 p0 = mesh.vertices.at(face.x);
                            Vector3 p1 = mesh.vertices.at(face.y);
                            Vector3 p2 = mesh.vertices.at(face.z);
                            // if any point is behind clipping plane, ignore this triangle completely
                            if (0.0 - p0.z < z_near || 0.0 - p1.z < z_near || 0.0 - p2.z < z_near) {
                                return img;
                            }

                            //Vector3 p = Vector3(x + (Real(1.0 / divisions) * i) + Real(1.0 / (divisions * 2.0)), y + (Real(1.0 / divisions) * j) + Real(1.0 / (divisions * 2.0)));
                            Vector4 tp0 = inverse(scene.camera.cam_to_world) * Vector4(p0.x, p0.y, p0.z, 1.0);
                            Vector4 tp1 = inverse(scene.camera.cam_to_world) * Vector4(p1.x, p1.y, p1.z, 1.0);
                            Vector4 tp2 = inverse(scene.camera.cam_to_world) * Vector4(p2.x, p2.y, p2.z, 1.0);

                            // calculate points in projected camera space
                            Vector2 p0pc{ 0.0 - tp0.x / tp0.z , 0.0 - tp0.y / tp0.z };
                            Vector2 p1pc{ 0.0 - tp1.x / tp1.z , 0.0 - tp1.y / tp1.z };
                            Vector2 p2pc{ 0.0 - tp2.x / tp2.z , 0.0 - tp2.y / tp2.z };
                            Vector2 q = Vector2(x + (Real(1.0 / divisions) * i) + Real(1.0 / (divisions * 2.0)), y + (Real(1.0 / divisions) * j) + Real(1.0 / (divisions * 2.0)));
                            //calculate the screen space points
                            Real a = (Real)img.width / (Real)img.height;
                            Vector2 p0ss{ (Real)img.width * (p0pc.x + s * a) / (2.0 * s * a), (Real)img.height * (p0pc.y - s) / (0.0 - 2.0 * s) };
                            Vector2 p1ss{ (Real)img.width * (p1pc.x + s * a) / (2.0 * s * a), (Real)img.height * (p1pc.y - s) / (0.0 - 2.0 * s) };
                            Vector2 p2ss{ (Real)img.width * (p2pc.x + s * a) / (2.0 * s * a), (Real)img.height * (p2pc.y - s) / (0.0 - 2.0 * s) };


                            bool hp01 = halfPlane(p0ss, p1ss, q);
                            bool hp12 = halfPlane(p1ss, p2ss, q);
                            bool hp20 = halfPlane(p2ss, p0ss, q);
                            if ((hp01 && hp12 && hp20) || !(hp01 || hp12 || hp20)) {
                                Vector3 qp = Vector3((2.0 * q.x * s * a / (Real)img.width) - (s * a), (0.0 - 2.0 * s * q.y / (Real)img.height) + s, 0.0);
                                //Real this_depth = depth(p0, p1, p2, Vector3(p0pc.x, p0pc.y, 0.0), Vector3(p1pc.x, p1pc.y, 0.0), Vector3(p2pc.x, p2pc.y, 0.0), qp);
                                Vector3 bary = baryVals(p0, p1, p2, Vector3(p0ss.x, p0ss.y, 0.0), Vector3(p1ss.x, p1ss.y, 0.0), Vector3(p2ss.x, p2ss.y, 0.0), Vector3(q.x, q.y, 0.0));
                                Real this_depth = depth(p0, p1, p2, Vector3(p0ss.x, p0ss.y, 0.0), Vector3(p1ss.x, p1ss.y, 0.0), Vector3(p2ss.x, p2ss.y, 0.0), Vector3(q.x, q.y, 0.0));
                                if (this_depth > z_min) {
                                    subColors[i][j] = bary.x * mesh.vertex_colors.at(face.x) + bary.y * mesh.vertex_colors.at(face.y) + bary.z * mesh.vertex_colors.at(face.z);
                                    z_min = this_depth;
                                    kset = k;
                                }
                            }
                        }
                    }
                }
                img(x, y) = avgCol(subColors);
            }
        }
    }
    return img;
}
