#include "hw1.h"
#include "hw1_scenes.h"
#include <stdio.h>

using namespace hw1;

/*int min (int a, int b){
    return a < b ?  a : b;
}
int max(int a, int b) {
    return a > b ? a : b;
}*/
int constrain(Real num, int min, int max) {
    if (num < min) {
        return min;
    }
    else if (num > max) {
        return max;
    }
    return num;
}

Image3 hw_1_1(const std::vector<std::string> &params) {
    // Homework 1.1: render a circle at the specified
    // position, with the specified radius and color.
    //printf("suffereing\n");
    Image3 img(640 /* width */, 480 /* height */);

    Vector2 center = Vector2{img.width / 2 + Real(0.5), img.height / 2 + Real(0.5)};
    Real radius = 100.0;
    Vector3 color = Vector3{1.0, 0.5, 0.5};
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-center") {
            Real x = std::stof(params[++i]);
            Real y = std::stof(params[++i]);
            center = Vector2{x, y};
        } else if (params[i] == "-radius") {
            radius = std::stof(params[++i]);
        } else if (params[i] == "-color") {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            color = Vector3{r, g, b};
        }
    }

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Vector2 toPoint = Vector2{ center.x - x, center.y - y };
            Real dist = length(toPoint);
            if (dist <= radius) {
                img(x, y) = color;
            }
            else {
                img(x, y) = Vector3{ 0.5, 0.5, 0.5 };
            }
        }
    }
    return img;
}

Image3 hw_1_2(const std::vector<std::string> &params) {
    // Homework 1.2: render multiple circles
    if (params.size() == 0) {
        return Image3(0, 0);
    }
    //printf("Hello");
    int scene_id = std::stoi(params[0]);
    const CircleScene &scene = hw1_2_scenes[scene_id];

    Image3 img(scene.resolution.x, scene.resolution.y);

    /*# For each circle, check all pixels
        for each circle:
            bounding_box = \
                BBox(circle.center - radius,
                    circle.center + radius)
            for each pixel in bounding_box:
                # check if the pixel center hits the circle
                # overwrite color if the circle is closer
        */
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = scene.background;
        }
    }
    for (Circle circ : scene.objects) {
    
        //force the box to be within the bounds of the image 
        int minX = constrain(circ.center.x - circ.radius, 0, img.width-1);
        int maxX = constrain(circ.center.x + circ.radius, 0, img.width-1);
        int minY = constrain(circ.center.y - circ.radius, 0, img.height-1);
        int maxY = constrain(circ.center.y + circ.radius, 0, img.height-1);
        for (int y = minY; y <= maxY; y++) {
            for (int x = minX; x <= maxX; x++) {
                Real dist = length(Vector2{ circ.center.x - x, circ.center.y - y });
                if (dist <= circ.radius) {
                    img(x, y) = circ.color;
                }
            }
        }
    }
    //original, this works!
    /*for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = scene.background;
            for (Circle circ : scene.objects) {
                Real dist = length(Vector2{ circ.center.x - x, circ.center.y - y });
                if (dist <= circ.radius) {
                    img(x, y) = circ.color;
                }
            }
        }
    }*/
    return img;
}

bool halfPlane(Vector2 p0, Vector2 p1, Vector2 q) {
    Vector2 n = Vector2(p1.y - p0.y, p0.x - p1.x);
    if (dot(q - p0, n) > 0) {
        return true;
    }
    return false;
}
Image3 hw_1_3(const std::vector<std::string> &params) {
    // Homework 1.3: render multiple shapes
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = scene.background;
        }
    }
    for (Shape shape : scene.shapes) {

        //force the box to be within the bounds of the image 
        
        if (auto* circ = std::get_if<Circle>(&shape)) {
            int minX = constrain(circ->center.x - circ->radius, 0, img.width - 1);
            int maxX = constrain(circ->center.x + circ->radius, 0, img.width - 1);
            int minY = constrain(circ->center.y - circ->radius, 0, img.height - 1);
            int maxY = constrain(circ->center.y + circ->radius, 0, img.height - 1);
            for (int y = minY; y <= maxY; y++) {
                for (int x = minX; x <= maxX; x++) {
                    Real dist = length(Vector2{ circ->center.x - x, circ->center.y - y });
                    if (dist <= circ->radius) {
                        img(x, y) = circ->color;
                    }
                }
            }
            //printf("I am a circle\n");
        }
        else if (auto* rectangle = std::get_if<Rectangle>(&shape)) {
            for (int y = rectangle->p_min.y; y < rectangle->p_max.y; y++) {
                for (int x = rectangle->p_min.x; x < rectangle->p_max.x; x++) {
                    img(x, y) = rectangle->color;
                }
            }
        }
        else if (auto* triangle = std::get_if<Triangle>(&shape)) {
            for (int y = 0; y < img.height; y++) {
                for (int x = 0; x < img.width; x++) {
                    Vector2 q = Vector2(x, y);
                    bool hp01 = halfPlane(triangle->p0, triangle->p1, q);
                    bool hp12 = halfPlane(triangle->p1, triangle->p2, q);
                    bool hp20 = halfPlane(triangle->p2, triangle->p0, q);
                    if ( hp01 && hp12 && hp20) {
                        img(x, y) = triangle->color;
                    }
                    else if (!(hp01 || hp12 || hp20)) {
                        img(x, y) = triangle->color;
                    }
                }
            }
            //printf("I'm a triangle\n");
        }
    }
    
    return img;
}

Image3 hw_1_4(const std::vector<std::string> &params) {
    // Homework 1.4: render transformed shapes
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = scene.background;
        }
    }
    for (Shape shape : scene.shapes) {

        if (auto* circ = std::get_if<Circle>(&shape)) {
            
            for (int y = 0; y < img.height; y++) {
                for (int x = 0; x < img.width; x++) {

                    Vector3 tp = inverse(circ->transform) * Vector3(x, y, 1);
                    Real dist = length(Vector2{ circ->center.x - tp.x , circ->center.y - tp.y });
                    if (dist <= circ->radius) {
                        img(x, y) = circ->color;
                    }
                }
            }
            //printf("I am a circle\n");
        }
        else if (auto* rectangle = std::get_if<Rectangle>(&shape)) {
            for (int y = 0; y < img.height; y++) {
                for (int x = 0; x < img.width; x++) {
                    Vector3 tp = inverse(rectangle->transform) * Vector3(x, y, 1);
                    if (tp.x >= rectangle->p_min.x && tp.x <= rectangle->p_max.x && tp.y >= rectangle->p_min.y && tp.y <= rectangle->p_max.y) {
                        img(x,y) = rectangle->color;
                    }
                }
            }
            printf("a rectangle\n");
        }
        else if (auto* triangle = std::get_if<Triangle>(&shape)) {
            for (int y = 0; y < img.height; y++) {
                for (int x = 0; x < img.width; x++) {
                    Vector3 tp = inverse(triangle->transform) * Vector3(x, y, 1);
                    Vector2 q = Vector2(tp.x, tp.y);
                    bool hp01 = halfPlane(triangle->p0, triangle->p1, q);
                    bool hp12 = halfPlane(triangle->p1, triangle->p2, q);
                    bool hp20 = halfPlane(triangle->p2, triangle->p0, q);
                    if (hp01 && hp12 && hp20) {
                        img(x, y) = triangle->color;
                    }
                    else if (!(hp01 || hp12 || hp20)) {
                        img(x, y) = triangle->color;
                    }
                }
            }
            printf("I'm a triangle\n");
        }
    }
    return img;
}

Image3 hw_1_5(const std::vector<std::string> &params) {
    // Homework 1.5: antialiasing
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{1, 1, 1};
        }
    }
    return img;
}

Image3 hw_1_6(const std::vector<std::string> &params) {
    // Homework 1.6: alpha blending
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{1, 1, 1};
        }
    }
    return img;
}
