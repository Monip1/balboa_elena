#include "hw1.h"
#include "hw1_scenes.h"
#include <stdio.h>


using namespace hw1;

#define divisions 8
/*int min (int a, int b){
    return a < b ?  a : b;
}
int max(int a, int b) {
    return a > b ? a : b;
}*/
int bBox[4]; //minx miny maxx maxy
int constrain(Real num, int min, int max) {
    if (num < min) {
        return min;
    }
    else if (num > max) {
        return max;
    }
    return num;
}
bool tHalfPlane(Vector2 p0, Vector2 p1, Vector2 q) {
    Vector2 n = Vector2(p1.y - p0.y, p0.x - p1.x);
    if (dot(q - p0, n) > 0) {
        return true;
    }
    return false;
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
            Vector2 toPoint = Vector2{ center.x - (x + Real(0.5)), center.y - (y +Real(0.5))};
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
                Real dist = length(Vector2{ circ.center.x - (x +Real(0.5)), circ.center.y - (y + Real(0.5))});
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
        }\
        
        {"scale": [100, 16.666666]},
                {"translate": [0, 150]},
                {"rotate" : -56.30993},
                {"translate": [300, 200]}
    }*/
    return img;
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
                    Vector2 p = Vector2(x + Real(0.5), y + Real(0.5));
                    Real dist = length(Vector2{ circ->center.x - p.x, circ->center.y - p.y});
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
                    Vector2 q = Vector2(x+Real(0.5), y+Real(0.5));
                    bool hp01 = tHalfPlane(triangle->p0, triangle->p1, q);
                    bool hp12 = tHalfPlane(triangle->p1, triangle->p2, q);
                    bool hp20 = tHalfPlane(triangle->p2, triangle->p0, q);
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

                    Vector3 tp = inverse(circ->transform) * Vector3(x+Real(0.5), y+Real(0.5), 1.0);
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
                    Vector3 tp = inverse(rectangle->transform) * Vector3(x+Real(0.5), y+Real(0.5), 1.0);
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
                    Vector3 tp = inverse(triangle->transform) * Vector3(x+Real(0.5), y+Real(0.5), 1.0);
                    Vector2 q = Vector2(tp.x, tp.y);
                    bool hp01 = tHalfPlane(triangle->p0, triangle->p1, q);
                    bool hp12 = tHalfPlane(triangle->p1, triangle->p2, q);
                    bool hp20 = tHalfPlane(triangle->p2, triangle->p0, q);
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
Vector3 avgCol1(Vector3 colors[divisions][divisions]) {
    Vector3 acc = { 0,0,0 };
    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            acc += colors[i][j];
        }
    }
    return (acc / (divisions * divisions*1.0));
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
            img(x, y) = scene.background;
        }
    }
    for (Shape shape : scene.shapes) {

        if (auto* circ = std::get_if<Circle>(&shape)) {
            
            for (int y = 0; y < img.height; y++) {
                for (int x = 0; x < img.width; x++) {
                    Vector3 subColors[divisions][divisions];
                    for (int i = 0; i < divisions; i++) {
                        for (int j = 0; j < divisions; j++) {
                            Vector2 p = Vector2(x + (Real(1.0 / divisions) * i) + Real(1.0 / (divisions * 2.0)), y + (Real(1.0 / divisions) * j) + Real(1.0 / (divisions * 2.0)));
                            Vector3 tp = inverse(circ->transform) * Vector3(p.x,p.y, 1.0);
                            Real dist = length(Vector2{ circ->center.x - tp.x , circ->center.y - tp.y });
                            if (dist <= circ->radius) {
                                subColors[i][j] = circ->color;
                            }
                            else {
                                subColors[i][j] = img(x, y);
                            }
                        }
                    }

                    img(x, y) = avgCol1(subColors);
                }
            }
            printf("I am a circle\n");
        }
        else if (auto* rectangle = std::get_if<Rectangle>(&shape)) {
            for (int y = 0; y < img.height; y++) {
                for (int x = 0; x < img.width; x++) {
                    Vector3 subColors[divisions][divisions];
                    for (int i = 0; i < divisions; i++) {
                        for (int j = 0; j < divisions; j++) {
                            Vector2 p = Vector2(x + (Real(1.0 / divisions) * i) + Real(1.0 / (divisions * 2.0)), y + (Real(1.0 / divisions) * j) + Real(1.0 / (divisions * 2.0)));
                            Vector3 tp = inverse(rectangle->transform) * Vector3(p.x, p.y, 1.0);
                            if (tp.x >= rectangle->p_min.x && tp.x <= rectangle->p_max.x && tp.y >= rectangle->p_min.y && tp.y <= rectangle->p_max.y) {
                                subColors[i][j] = rectangle->color;
                            }
                            else {
                                subColors[i][j] = img(x, y);
                            }
                        }
                    }
                    img(x, y) = avgCol1(subColors);
                }
            }
            printf("a rectangle yay\n");
        }
        else if (auto* triangle = std::get_if<Triangle>(&shape)) {
            for (int y = 0; y < img.height; y++) {
                for (int x = 0; x < img.width; x++) {
                    //Vector3 tp = inverse(triangle->transform) * Vector3(x, y, 1);
                    
                    Vector3 subColors[divisions][divisions];
                    for (int i = 0; i < divisions; i++) {
                        for (int j = 0; j < divisions; j++) {
                            Vector2 p = Vector2(x + (Real(1.0 / divisions) * i) + Real(1.0 / (divisions * 2.0)), y + (Real(1.0 / divisions) * j) + Real(1.0 / (divisions * 2.0)));
                            Vector3 tp = inverse(triangle->transform) * Vector3(p.x,p.y, 1.0);
                            Vector2 q = Vector2(tp.x, tp.y);
                            bool hp01 = tHalfPlane(triangle->p0, triangle->p1, q);
                            bool hp12 = tHalfPlane(triangle->p1, triangle->p2, q);
                            bool hp20 = tHalfPlane(triangle->p2, triangle->p0, q);
                            if ((hp01 && hp12 && hp20) || !(hp01 || hp12 || hp20)) {
                                subColors[i][j] = triangle->color;
                            }
                            else {
                                subColors[i][j] = img(x, y);
                            }
                        }
                    }
                    img(x, y) = avgCol1(subColors);
                }
            }
            printf("I'm a triangle yayyyy\n");
        }
    }


   return img;
}

void setBox(Vector3 p0, Vector3 p1, Vector3 p2, Vector3 p3, Matrix3x3 F, int width, int height){

    Vector3 tp0 = F * p0;
    Vector3 tp1 = F * p1;
    Vector3 tp2 = F * p2;
    Vector3 tp3 = F * p3;
    printf("F is: \n%d %d %d\n%d %d %d\n%d %d %d\n\n\n", F(0, 0), F(0, 1), F(0, 2), F(1, 0), F(1, 1), F(1, 2), F(2, 0), F(2, 1), F(2, 2));
    //printf("tp0 = %d, %d, %d\n", tp0.x, tp0.y, tp0.z);
    //printf("tp1 = %d, %d, %d\n", tp1.x, tp1.y, tp1.z);
    //printf("tp2 = %d, %d, %d\n", tp2.x, tp2.y, tp2.z);
    //printf("tp3 = %d, %d, %d\n", tp3.x, tp3.y, tp3.z);
    bBox[0] = min(min(tp0.x, tp1.x), min(tp2.x, tp3.x))-1;
    bBox[1] = max(max(tp0.x, tp1.x), max(tp2.x, tp3.x))+1;
    bBox[2] = min(min(tp0.y, tp1.y), min(tp2.y, tp3.y))-1;
    bBox[3] = max(max(tp0.y, tp1.y), max(tp2.y, tp3.y))+1;

    bBox[0] = max(bBox[0], 0);
    bBox[1] = min(bBox[1], width);
    bBox[2] = max(bBox[2], 0);
    bBox[3] = min(bBox[3], height);
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
            img(x, y) = scene.background;
            //img(x, y) = Vector3(1.0-((0.0+ x + y) / (0.0+img.height + img.width)), (0.0+x)/(0.0+img.width), (0.0+y)/img.height);
        }
    }
    for (Shape shape : scene.shapes) {

        if (auto* circ = std::get_if<Circle>(&shape)) {
            //setBox(Vector3(-1, -1, 1), Vector3(-1, 1, 1), Vector3(1, -1, 1), Vector3(1, 1, 1), circ->transform, img.width, img.height);
            //printf("Bounding box is [0]: %d, [1]: %d, [2]: %d, [3]: %d\n", bBox[0], bBox[1], bBox[2], bBox[3]);
            Matrix3x3 F = circ->transform;
            //printf("F is: \n%d %d %d\n%d %d %d\n%d %d %d\n\n\n", F(0, 0), F(0, 1), F(0, 2), F(1, 0), F(1, 1), F(1, 2), F(2, 0), F(2, 1), F(2, 2));

            //printf("F is : %s", toString(circ->transform));
            for (int y = 0; y < img.height; y++) {
                for (int x = 0; x < img.width; x++) {
                    Vector3 subColors[divisions][divisions];
                    for (int i = 0; i < divisions; i++) {
                        for (int j = 0; j < divisions; j++) {
                            Vector2 p = Vector2(x + (Real(1.0 / divisions) * i) + Real(1.0 / (divisions * 2.0)), y + (Real(1.0 / divisions) * j) + Real(1.0 / (divisions * 2.0)));
                            Vector3 tp = inverse(circ->transform) * Vector3(p.x, p.y, 1.0);
                            Real dist = length(Vector2{ circ->center.x - tp.x , circ->center.y - tp.y });
                            if (dist <= circ->radius) {
                                subColors[i][j] = circ->color * circ->alpha + (1 - circ->alpha) * img(x, y);
                            }
                            else {
                                subColors[i][j] = img(x, y);
                            }
                        }
                    }

                    img(x, y) = avgCol1(subColors);
                }
            }
            printf("I am a circle\n");
        }
        else if (auto* rectangle = std::get_if<Rectangle>(&shape)) {
            //setBox(Vector3(0, 0, 1), Vector3(0, 1, 1), Vector3(1, 0, 1), Vector3(1, 1, 1), rectangle->transform, img.width, img.height);
            /*for (int y = bBox[2]; y < bBox[3]; y++) {
                for (int x = bBox[0]; x < bBox[1]; x++) {*/
            for (int y = 0; y < img.height; y++) {
                for (int x = 0; x < img.width; x++) {
                    Vector3 subColors[divisions][divisions];
                    for (int i = 0; i < divisions; i++) {
                        for (int j = 0; j < divisions; j++) {
                            Vector2 p = Vector2(x + (Real(1.0 / divisions) * i) + Real(1.0 / (divisions * 2.0)), y + (Real(1.0 / divisions) * j) + Real(1.0 / (divisions * 2.0)));
                            Vector3 tp = inverse(rectangle->transform) * Vector3(p.x, p.y, 1.0);
                            if (tp.x >= rectangle->p_min.x && tp.x <= rectangle->p_max.x && tp.y >= rectangle->p_min.y && tp.y <= rectangle->p_max.y) {
                                /*subColors[i][j] =             Vector3(1.0-((0.0+ x + y) / (0.0+img.height + img.width)), (0.0+x)/(0.0+img.width), (0.0+y)/img.height) * rectangle->alpha + (1-rectangle->alpha)*img(x,y);*/
                                subColors[i][j] = rectangle->color * rectangle->alpha + (1 - rectangle->alpha) * img(x, y);
                            }
                            else {
                                subColors[i][j] = img(x, y);
                            }
                        }
                    }
                    img(x, y) = avgCol1(subColors);
                }
            }
            printf("a rectangle yay\n");
        }
        else if (auto* triangle = std::get_if<Triangle>(&shape)) {
            //setBox(Vector3(0, 0, 1), Vector3(0, 1, 1), Vector3(1, 0, 1), Vector3(0, 0, 1), triangle->transform, img.width, img.height);
            for (int y = 0; y < img.height; y++) {
                for (int x = 0; x < img.width; x++) {
                    //Vector3 tp = inverse(triangle->transform) * Vector3(x, y, 1);
                 
                    Vector3 subColors[divisions][divisions];
                    for (int i = 0; i < divisions; i++) {
                        for (int j = 0; j < divisions; j++) {
                            Vector2 p = Vector2(x + (Real(1.0 / divisions) * i) + Real(1.0 / (divisions * 2.0)), y + (Real(1.0 / divisions) * j) + Real(1.0 / (divisions * 2.0)));
                            Vector3 tp = inverse(triangle->transform) * Vector3(p.x, p.y, 1.0);
                            Vector2 q = Vector2(tp.x, tp.y);
                            bool hp01 = tHalfPlane(triangle->p0, triangle->p1, q);
                            bool hp12 = tHalfPlane(triangle->p1, triangle->p2, q);
                            bool hp20 = tHalfPlane(triangle->p2, triangle->p0, q);
                            if ((hp01 && hp12 && hp20) || !(hp01 || hp12 || hp20)) {
                                subColors[i][j] = triangle->color * triangle->alpha + (1 - triangle->alpha) * img(x, y);
                            }
                            else {
                                subColors[i][j] = img(x, y);
                            }
                        }
                    }
                    img(x, y) = avgCol1(subColors);
                }
            }
            printf("I'm a triangle yayyyy\n");
        }
    }


    return img;
}
