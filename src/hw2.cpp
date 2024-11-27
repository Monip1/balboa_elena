#include "hw2.h"
#include "hw2_scenes.h"

using namespace hw2;

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
    Real a = Real(img.width) / Real(img.height);

    auto to_screen = [&](const Vector3 &p) {
        Vector2 pp = Vector2{-p.x / p.z, -p.y / p.z};
        Vector2 screen_pp = Vector2{img.width * (pp.x + s * a) / (2 * s * a),
                                    -img.height * (pp.y - s) / (2 * s)};
        return screen_pp;
    };
    Vector2 sp0 = to_screen(p0);
    Vector2 sp1 = to_screen(p1);
    Vector2 sp2 = to_screen(p2);
    Vector2 e01 = sp1 - sp0, e12 = sp2 - sp1, e20 = sp0 - sp2;
    auto normal = [](const Vector2 &v) {
        return Vector2{v.y, -v.x};
    };
    Vector2 n01 = normal(e01),
            n12 = normal(e12),
            n20 = normal(e20);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Vector3 sum{0, 0, 0};
            if (p0.z < -z_near && p1.z < -z_near && p2.z < -z_near) {
                int n = 4;
                for (int dx = 0; dx < n; dx++) {
                    for (int dy = 0; dy < n; dy++) {
                        Real xoff = (dx + Real(0.5)) / Real(n);
                        Real yoff = (dy + Real(0.5)) / Real(n);
                        Vector2 pixel_center{x + xoff, y + yoff};
                        bool s01 = dot(pixel_center - sp0, n01) > 0;
                        bool s12 = dot(pixel_center - sp1, n12) > 0;
                        bool s20 = dot(pixel_center - sp2, n20) > 0;
                        if ((s01 && s12 && s20) || (!s01 && !s12 && !s20)) {
                            sum += color;
                        } else {
                            sum += Vector3{0.5, 0.5, 0.5};
                        }
                    }
                }
                img(x, y) = sum / Real(n * n);
            } else {
                img(x, y) = Vector3{0.5, 0.5, 0.5};
            }
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

    Real a = Real(img.width) / Real(img.height);

    TriangleMesh mesh = meshes[scene_id];

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Vector3 sum{0, 0, 0};
            int n = 4;
            for (int dx = 0; dx < n; dx++) {
                for (int dy = 0; dy < n; dy++) {
                    Real xoff = (dx + Real(0.5)) / Real(n);
                    Real yoff = (dy + Real(0.5)) / Real(n);
                    Vector2 pixel_center{x + xoff, y + yoff};
                    Vector3 color = Vector3{0.5, 0.5, 0.5}; // background color
                    Real z_min = infinity<Real>();
                    for (int i = 0; i < (int)mesh.faces.size(); i++) {
                        Vector3 p0 = mesh.vertices[mesh.faces[i][0]];
                        Vector3 p1 = mesh.vertices[mesh.faces[i][1]];
                        Vector3 p2 = mesh.vertices[mesh.faces[i][2]];

                        // We don't implement clipping here, if even one of the 
                        // vertex is behind the clipping plane, we skip the triangle
                        if (p0.z > -z_near || p1.z > -z_near || p2.z > -z_near) {
                            continue;
                        }

                        auto to_screen = [&](const Vector3 &p) {
                            Vector2 pp = Vector2{-p.x / p.z, -p.y / p.z};
                            Vector2 screen_pp = Vector2{img.width * (pp.x + s * a) / (2 * s * a),
                                                        -img.height * (pp.y - s) / (2 * s)};
                            return screen_pp;
                        };
                        Vector2 sp0 = to_screen(p0);
                        Vector2 sp1 = to_screen(p1);
                        Vector2 sp2 = to_screen(p2);
                        Vector2 e01 = sp1 - sp0, e12 = sp2 - sp1, e20 = sp0 - sp2;
                        auto normal = [](const Vector2 &v) {
                            return Vector2{v.y, -v.x};
                        };
                        Vector2 n01 = normal(e01),
                                n12 = normal(e12),
                                n20 = normal(e20);

                        bool s01 = dot(pixel_center - sp0, n01) > 0;
                        bool s12 = dot(pixel_center - sp1, n12) > 0;
                        bool s20 = dot(pixel_center - sp2, n20) > 0;
                        if ((s01 && s12 && s20) || (!s01 && !s12 && !s20)) {
                            // Inside the triangle
                            // Interpolate Z
                            
                            // 1. convert pixel center from screen space to camera space
                            auto to_camera = [&](const Vector2 &p) {
                                // camera to screen:
                                // x'' = w ((x' + sa)/ (2sa))
                                // y'' = -h ((y' - s)/ (2s))
                                // screen to camera:
                                // x' = 2sa x'' / w - sa
                                // y' = - 2s y'' / h + s
                                return Vector2{(2 * s * a) * p.x / img.width - s * a,
                                               - 2 * s * p.y / img.height + s};
                            };
                            Vector2 pp = to_camera(pixel_center);
                            Vector2 pp0 = to_camera(sp0);
                            Vector2 pp1 = to_camera(sp1);
                            Vector2 pp2 = to_camera(sp2);

                            // 2. compute projected barycentric coordinates
                            auto area = [&](const Vector2 pp0,
                                            const Vector2 pp1,
                                            const Vector2 pp2) {
                                // lift things to 3D to take cross product
                                Vector3 p0 = Vector3{pp0.x, pp0.y, Real(0)};
                                Vector3 p1 = Vector3{pp1.x, pp1.y, Real(0)};
                                Vector3 p2 = Vector3{pp2.x, pp2.y, Real(0)};
                                return length(cross(p1 - p0, p2 - p0)) / 2;
                            };

                            Real a0 = area(pp, pp1, pp2);
                            Real a1 = area(pp0, pp, pp2);
                            Real a2 = area(pp0, pp1, pp);
                            Real denom = area(pp0, pp1, pp2);
                            Real b0p = a0 / denom;
                            Real b1p = a1 / denom;
                            Real b2p = a2 / denom;

                            // 3. compute the original barycentric coordinates
                            b0p /= p0.z; 
                            b1p /= p1.z;
                            b2p /= p2.z;
                            Real b0 = b0p / (b0p + b1p + b2p);
                            Real b1 = b1p / (b0p + b1p + b2p);
                            Real b2 = b2p / (b0p + b1p + b2p);

                            // 4. interpolate and obtain Z
                            Real z = b0 * p0.z + b1 * p1.z + b2 * p2.z;
                            // Remember to negate to make Z positive
                            z = fabs(z);

                            // Now, test if Z is smaller
                            if (z < z_min) {
                                color = mesh.face_colors[i];
                                z_min = z;
                            }
                        }
                    }
                    sum += color;
                }
            }
            img(x, y) = sum / Real(n * n);
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

    Real a = Real(img.width) / Real(img.height);

    TriangleMesh mesh = meshes[scene_id];

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Vector3 sum{0, 0, 0};
            int n = 4;
            for (int dx = 0; dx < n; dx++) {
                for (int dy = 0; dy < n; dy++) {
                    Real xoff = (dx + Real(0.5)) / Real(n);
                    Real yoff = (dy + Real(0.5)) / Real(n);
                    Vector2 pixel_center{x + xoff, y + yoff};
                    Vector3 color = Vector3{0.5, 0.5, 0.5}; // background color
                    Real z_min = infinity<Real>();
                    for (int i = 0; i < (int)mesh.faces.size(); i++) {
                        Vector3 p0 = mesh.vertices[mesh.faces[i][0]];
                        Vector3 p1 = mesh.vertices[mesh.faces[i][1]];
                        Vector3 p2 = mesh.vertices[mesh.faces[i][2]];

                        // We don't implement clipping here, if even one of the 
                        // vertex is behind the clipping plane, we skip the triangle
                        if (p0.z > -z_near || p1.z > -z_near || p2.z > -z_near) {
                            continue;
                        }

                        auto to_screen = [&](const Vector3 &p) {
                            Vector2 pp = Vector2{-p.x / p.z, -p.y / p.z};
                            Vector2 screen_pp = Vector2{img.width * (pp.x + s * a) / (2 * s * a),
                                                        -img.height * (pp.y - s) / (2 * s)};
                            return screen_pp;
                        };
                        Vector2 sp0 = to_screen(p0);
                        Vector2 sp1 = to_screen(p1);
                        Vector2 sp2 = to_screen(p2);
                        Vector2 e01 = sp1 - sp0, e12 = sp2 - sp1, e20 = sp0 - sp2;
                        auto normal = [](const Vector2 &v) {
                            return Vector2{v.y, -v.x};
                        };
                        Vector2 n01 = normal(e01),
                                n12 = normal(e12),
                                n20 = normal(e20);

                        bool s01 = dot(pixel_center - sp0, n01) > 0;
                        bool s12 = dot(pixel_center - sp1, n12) > 0;
                        bool s20 = dot(pixel_center - sp2, n20) > 0;
                        if ((s01 && s12 && s20) || (!s01 && !s12 && !s20)) {
                            // Inside the triangle
                            // Interpolate Z
                            
                            // 1. convert pixel center from screen space to camera space
                            auto to_camera = [&](const Vector2 &p) {
                                // camera to screen:
                                // x'' = w ((x' + sa)/ (2sa))
                                // y'' = -h ((y' - s)/ (2s))
                                // screen to camera:
                                // x' = 2sa x'' / w - sa
                                // y' = - 2s y'' / h + s
                                return Vector2{(2 * s * a) * p.x / img.width - s * a,
                                               - 2 * s * p.y / img.height + s};
                            };
                            Vector2 pp = to_camera(pixel_center);
                            Vector2 pp0 = to_camera(sp0);
                            Vector2 pp1 = to_camera(sp1);
                            Vector2 pp2 = to_camera(sp2);

                            // 2. compute projected barycentric coordinates
                            auto area = [&](const Vector2 pp0,
                                            const Vector2 pp1,
                                            const Vector2 pp2) {
                                // lift things to 3D to take cross product
                                Vector3 p0 = Vector3{pp0.x, pp0.y, Real(0)};
                                Vector3 p1 = Vector3{pp1.x, pp1.y, Real(0)};
                                Vector3 p2 = Vector3{pp2.x, pp2.y, Real(0)};
                                return length(cross(p1 - p0, p2 - p0)) / 2;
                            };

                            Real a0 = area(pp, pp1, pp2);
                            Real a1 = area(pp0, pp, pp2);
                            Real a2 = area(pp0, pp1, pp);
                            Real denom = area(pp0, pp1, pp2);
                            Real b0p = a0 / denom;
                            Real b1p = a1 / denom;
                            Real b2p = a2 / denom;

                            // 3. compute the original barycentric coordinates
                            b0p /= p0.z; 
                            b1p /= p1.z;
                            b2p /= p2.z;
                            Real b0 = b0p / (b0p + b1p + b2p);
                            Real b1 = b1p / (b0p + b1p + b2p);
                            Real b2 = b2p / (b0p + b1p + b2p);

                            // 4. interpolate and obtain Z
                            Real z = b0 * p0.z + b1 * p1.z + b2 * p2.z;
                            // Remember to negate to make Z positive
                            z = fabs(z);

                            // Now, test if Z is smaller
                            if (z < z_min) {
                                // Interpolate vertex colors
                                Vector3 C0 = mesh.vertex_colors[mesh.faces[i][0]];
                                Vector3 C1 = mesh.vertex_colors[mesh.faces[i][1]];
                                Vector3 C2 = mesh.vertex_colors[mesh.faces[i][2]];

                                color = b0 * C0 + b1 * C1 + b2 * C2;
                                z_min = z;
                            }
                        }
                    }
                    sum += color;
                }
            }
            img(x, y) = sum / Real(n * n);
        }
    }
    return img;
}

Vector3 xform_point(const Matrix4x4 &m, const Vector3 &p) {
    Vector4 ph = Vector4{p.x, p.y, p.z, Real(1)};
    Vector4 mph = m * ph;
    return Vector3{mph.x / mph.w,
                   mph.y / mph.w,
                   mph.z / mph.w};
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
    // Construct perspective projection matrix
    Real w = scene.camera.resolution.x;
    Real h = scene.camera.resolution.y;
    Real s = scene.camera.s;
    Real a = Real(w) / Real(h);

    Matrix4x4 projection;
    projection(0, 0) = 1;
    projection(1, 1) = 1;
    projection(2, 3) = 1;
    projection(3, 2) = -1;
    Matrix4x4 camera_to_screen;
    camera_to_screen(0, 0) = w / (2 * s * a);
    camera_to_screen(0, 3) = w / 2;
    camera_to_screen(1, 1) = -h / (2 * s);
    camera_to_screen(1, 3) = h / 2;
    camera_to_screen(2, 2) = 1;
    camera_to_screen(3, 3) = 1;

    Matrix4x4 world_to_screen =
        camera_to_screen * projection * inverse(scene.camera.cam_to_world);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Vector3 sum{0, 0, 0};
            int n = 4;
            for (int dx = 0; dx < n; dx++) {
                for (int dy = 0; dy < n; dy++) {
                    Real xoff = (dx + Real(0.5)) / Real(n);
                    Real yoff = (dy + Real(0.5)) / Real(n);
                    Vector2 pixel_center{x + xoff, y + yoff};
                    Vector3 color = scene.background; // background color
                    Real z_min = infinity<Real>();
                    for (const auto &mesh : scene.meshes) {
                        // construct the MVP matrix
                        Matrix4x4 MVP = world_to_screen * mesh.model_matrix;
                        for (int i = 0; i < (int)mesh.faces.size(); i++) {
                            Vector3 p0 = mesh.vertices[mesh.faces[i][0]];
                            Vector3 p1 = mesh.vertices[mesh.faces[i][1]];
                            Vector3 p2 = mesh.vertices[mesh.faces[i][2]];
                            Vector3 screen_p0 = xform_point(MVP, p0);
                            Vector3 screen_p1 = xform_point(MVP, p1);
                            Vector3 screen_p2 = xform_point(MVP, p2);

                            // We don't implement clipping here, if even one of the 
                            // vertex is behind the clipping plane, we skip the triangle
                            // screen_pi.z stores 1/pi.z
                            Real inv_z_near = (1/scene.camera.z_near);
                            if (screen_p0.z > inv_z_near ||
                                    screen_p1.z > inv_z_near ||
                                    screen_p2.z > inv_z_near) {
                                continue;
                            }

                            Vector2 sp0 = Vector2{screen_p0.x, screen_p0.y};
                            Vector2 sp1 = Vector2{screen_p1.x, screen_p1.y};
                            Vector2 sp2 = Vector2{screen_p2.x, screen_p2.y};

                            Vector2 e01 = sp1 - sp0, e12 = sp2 - sp1, e20 = sp0 - sp2;
                            auto normal = [](const Vector2 &v) {
                                return Vector2{v.y, -v.x};
                            };
                            Vector2 n01 = normal(e01),
                                    n12 = normal(e12),
                                    n20 = normal(e20);

                            bool s01 = dot(pixel_center - sp0, n01) > 0;
                            bool s12 = dot(pixel_center - sp1, n12) > 0;
                            bool s20 = dot(pixel_center - sp2, n20) > 0;
                            if ((s01 && s12 && s20) || (!s01 && !s12 && !s20)) {
                                // Inside the triangle
                                // Interpolate Z
                                
                                // 1. convert pixel center from screen space to camera space
                                auto to_camera = [&](const Vector2 &p) {
                                    // camera to screen:
                                    // x'' = w ((x' + sa)/ (2sa))
                                    // y'' = -h ((y' - s)/ (2s))
                                    // screen to camera:
                                    // x' = 2sa x'' / w - sa
                                    // y' = - 2s y'' / h + s
                                    return Vector2{(2 * s * a) * p.x / img.width - s * a,
                                                   - 2 * s * p.y / img.height + s};
                                };
                                Vector2 pp = to_camera(pixel_center);
                                Vector2 pp0 = to_camera(sp0);
                                Vector2 pp1 = to_camera(sp1);
                                Vector2 pp2 = to_camera(sp2);

                                // 2. compute projected barycentric coordinates
                                auto area = [&](const Vector2 pp0,
                                                const Vector2 pp1,
                                                const Vector2 pp2) {
                                    // lift things to 3D to take cross product
                                    Vector3 p0 = Vector3{pp0.x, pp0.y, Real(0)};
                                    Vector3 p1 = Vector3{pp1.x, pp1.y, Real(0)};
                                    Vector3 p2 = Vector3{pp2.x, pp2.y, Real(0)};
                                    return length(cross(p1 - p0, p2 - p0)) / 2;
                                };

                                Real a0 = area(pp, pp1, pp2);
                                Real a1 = area(pp0, pp, pp2);
                                Real a2 = area(pp0, pp1, pp);
                                Real denom = area(pp0, pp1, pp2);
                                Real b0p = a0 / denom;
                                Real b1p = a1 / denom;
                                Real b2p = a2 / denom;

                                // 3. compute the original barycentric coordinates
                                b0p *= screen_p0.z;
                                b1p *= screen_p1.z;
                                b2p *= screen_p2.z;
                                Real b0 = b0p / (b0p + b1p + b2p);
                                Real b1 = b1p / (b0p + b1p + b2p);
                                Real b2 = b2p / (b0p + b1p + b2p);

                                // 4. interpolate and obtain Z
                                Real z = b0 / screen_p0.z +
                                         b1 / screen_p1.z +
                                         b2 / screen_p2.z;
                                // Remember to negate to make Z positive
                                z = fabs(z);

                                // Now, test if Z is smaller
                                if (z < z_min) {
                                    // Interpolate vertex colors
                                    Vector3 C0 = mesh.vertex_colors[mesh.faces[i][0]];
                                    Vector3 C1 = mesh.vertex_colors[mesh.faces[i][1]];
                                    Vector3 C2 = mesh.vertex_colors[mesh.faces[i][2]];

                                    color = b0 * C0 + b1 * C1 + b2 * C2;
                                    z_min = z;
                                }
                            }
                        }
                    }
                    sum += color;
                }
            }
            img(x, y) = sum / Real(n * n);
        }
    }
    return img;
}

