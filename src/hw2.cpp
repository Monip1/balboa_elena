#include "hw2.h"
#include "hw2_scenes.h"
#include "hw1.h"

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

    Vector2 p0pc { 0.0 - p0.x / p0.z , 0.0-p0.y/p0.z};
    Vector2 p1pc{ 0.0 - p1.x / p1.z , 0.0 - p1.y / p1.z };
    Vector2 p2pc{ 0.0 - p2.x / p2.z , 0.0 - p2.y / p2.z };
    Real a = (Real)img.width / (Real)img.height;
    Vector2 p0ss{img.width*(p0pc.x+s*a)/(2.0*s*a), img.height*(p0pc.y-s)/(0.0-2.0*s)};
    Vector2 p1ss{ img.width * (p1pc.x + s * a) / (2.0 * s * a), img.height * (p1pc.y - s) / (0.0 - 2.0 * s) };
    Vector2 p2ss{ img.width * (p2pc.x + s * a) / (2.0 * s * a), img.height * (p2pc.y - s) / (0.0 - 2.0 * s) };


    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{ 0.5, 0.5, 0.5 };
        }
    }

    if (0.0 - p0.z < z_near || 0.0 - p1.z < z_near || 0.0 - p2.z < z_near) {
        return img;
    }
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
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

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{1, 1, 1};
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

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{1, 1, 1};
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

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{1, 1, 1};
        }
    }
    return img;
}

