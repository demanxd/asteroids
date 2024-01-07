/* MIT License
 * 
 * Copyright (c) 2024 Dmitry Shapovalov
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include "Engine.h"
#include <stdlib.h>
#include <memory.h>
#include <string>
#include <vector>
#include <cmath>
#include <random>
#include <ctime>

//  is_key_pressed(int button_vk_code) - check if a key is pressed,
//                                       use keycodes (VK_SPACE, VK_RIGHT, VK_LEFT, VK_UP, VK_DOWN, 'A', 'B')
//
//  get_cursor_x(), get_cursor_y() - get mouse cursor position
//  is_mouse_button_pressed(int button) - check if mouse button is pressed (0 - left button, 1 - right button)
//  clear_buffer() - set all pixels in buffer to 'black'
//  is_window_active() - returns true if window is active
//  schedule_quit_game() - quit game after act()


namespace Constants {
    const short size_unit = 10;
    const float speed_unit = 10.0;
    const float ball_speed_unit = 100.0;
    const short border_width = 10;
    const uint32_t color_borders = 0x000000;
    const uint32_t color_desk = 0x77AA33;
    const uint32_t color_bricks = 0xAA2233;
    const uint32_t color_ball = 0x2200FF;
    const uint32_t color_projectile = 0xFF0022;
    const uint32_t color_asteroid1 = 0x550066;
    const uint32_t color_asteroid2 = 0x552222;
    const uint32_t color_asteroid3 = 0x557722;
    const short brick_height_unit = size_unit * 1.5;
    const short brick_width_unit = brick_height_unit * 2.5;
    const float ball_direction = 2;
    const uint32_t bonus_color_wider = 0xFF1177;
    const uint32_t bonus_color_shorter = 0x11FF77;

    const float rotate_degree = 20;

};

namespace Global {
    float desk_wide = 5.5;
    int   life_count = 3;
}

struct Point2DF
{
public:
    Point2DF() { this->_x = 0.0; this->_y = 0.0; };
    Point2DF(const float x, const float y) : _x(x), _y(y) {};
    Point2DF(const Point2DF& point) { this->_x = point.get_x(); this->_y = point.get_y(); };
    ~Point2DF() {};

    float get_x() const { return _x; };
    float get_y() const { return _y; };

    void set_x(const float x) { this->_x = x; };
    void set_y(const float y) { this->_y = y; };

    Point2DF& operator=(const Point2DF& point) { this->_x = point.get_x(); this->_y = point.get_y(); return *this; };
    //Point2DF& operator=(const Point2D& point) { this->_x = point.get_x(); this->_y = point.get_y(); return *this; };
    bool operator==(const Point2DF& point) {
        return ((this->get_x() == point.get_x()) && (this->get_y() == point.get_y())) ? true : false;
    };

    bool operator!=(const Point2DF& point) {
        return (!(*this == point));
    }

    Point2DF operator+(const Point2DF& point) {
        Point2DF tmp(*this);
        tmp.set_x(tmp.get_x() + point.get_x());
        tmp.set_y(tmp.get_y() + point.get_y());
        return tmp;
    };

    Point2DF operator-(const Point2DF& point) {
        Point2DF tmp(*this);
        tmp.set_x(tmp.get_x() - point.get_x());
        tmp.set_y(tmp.get_y() - point.get_y());
        return tmp;
    };

    Point2DF operator*(const float& value) {
        Point2DF tmp(*this);
        tmp.set_x(tmp.get_x() * value);
        tmp.set_y(tmp.get_y() * value);
        return tmp;
    };

    Point2DF& operator+=(const Point2DF& point) { this->_x += point.get_x(); this->_y += point.get_y(); return *this; };
private:
    float _x;
    float _y;
};

struct Point2D
{
public:
    Point2D() { this->_x = 0.0; this->_y = 0.0; };
    Point2D(const uint32_t x, const uint32_t y) : _x(x), _y(y) {};
    Point2D(const Point2DF& point) { this->_x = point.get_x(); this->_y = point.get_y(); };
    Point2D(const Point2D& point) { this->_x = point.get_x(); this->_y = point.get_y(); };
    ~Point2D() {};

    uint32_t get_x() const { return _x; };
    uint32_t get_y() const { return _y; };

    void set_x(const uint32_t x) { this->_x = x; };
    void set_y(const uint32_t y) { this->_y = y; };

    Point2D& operator=(const Point2D& point) { this->_x = point.get_x(); this->_y = point.get_y(); return *this; };

    Point2D operator+(const Point2D& point) {
        Point2D tmp(*this);
        tmp.set_x(tmp.get_x() + point.get_x());
        tmp.set_y(tmp.get_y() + point.get_y());
        return tmp;
    };

    Point2D operator+(const Point2DF& point) {
        Point2D tmp(*this);
        tmp.set_x(tmp.get_x() + point.get_x());
        tmp.set_y(tmp.get_y() + point.get_y());
        return tmp;
    };

    Point2D operator-(const Point2D& point) {
        Point2D tmp(*this);
        if ((this->_x > point.get_x()) && (this->_y > point.get_y())) {
            tmp.set_x(tmp.get_x() - point.get_x());
            tmp.set_y(tmp.get_y() - point.get_y());
        }
        return tmp;
    };

    Point2D& operator+=(const Point2D& point) { this->_x += point.get_x(); this->_y += point.get_y(); return *this; };
    Point2D& operator+=(const Point2DF& point) { this->_x += point.get_x(); this->_y += point.get_y(); return *this; };

    bool operator==(const Point2D& point) {
        return ((this->get_x() == point.get_x()) && (this->get_y() == point.get_y())) ? true : false;
    };

    bool operator!=(const Point2D& point) {
        return (!(*this == point));
    }

private:
    uint32_t _x;
    uint32_t _y;
};

enum Angle {
    BottomLeft_e,
    BottomRight_e,
    TopLeft_e,
    TopRight_e
};

enum ShapeType {
    PrimitiveShape_e,
    Rectangle_e,
    Circle_e,
    RightTriangle_e
};


struct PrimitiveShape
{
public:
    PrimitiveShape() {};
    PrimitiveShape(const PrimitiveShape& primitiveShape) {
        this->_coordinate = primitiveShape.get_coordinate();
        this->_size = primitiveShape.get_size();
        this->_color = primitiveShape.get_color();
        this->_currentAngle = primitiveShape.get_current_angle();
    };
    virtual ~PrimitiveShape() {};

    Point2DF get_coordinate() const { return this->_coordinate; };
    Point2DF get_size() const { return this->_size; };
    uint32_t get_color() const { return this->_color; };
    Angle get_current_angle() const { return this->_currentAngle; };


    void set_coordinate(const Point2DF& coordinate) { this->_coordinate = coordinate; };
    void set_size(const Point2DF& size) { this->_size = size; };
    void set_color(const uint32_t& color) { this->_color = color; };
    void set_current_angle(const Angle& angle) { this->_currentAngle = angle; };

    virtual void draw() = 0;
    virtual void rotate_right() = 0;
    virtual void mirror_shape() = 0;


    bool rotate_right_around(Point2DF point) {
        bool isXMore;
        bool isYMore;
        Point2DF deltaPoint;
        Point2DF newCoordinate;
        Point2DF size(this->get_size());

        if (point.get_x() <= this->get_coordinate().get_x()) {
            isXMore = true;
            deltaPoint.set_x(this->get_coordinate().get_x() - point.get_x());
        }
        else {
            isXMore = false;
            deltaPoint.set_x(point.get_x() - this->get_coordinate().get_x());
        }
        if (point.get_y() <= this->get_coordinate().get_y()) {
            isYMore = true;
            deltaPoint.set_y(this->get_coordinate().get_y() - point.get_y());
        }
        else {
            isYMore = false;
            deltaPoint.set_y(point.get_y() - (this->get_coordinate().get_y()));
        }
        if (isXMore) {
            if (isYMore) {
                if ((((int64_t)point.get_y() - ((int64_t)deltaPoint.get_x() + size.get_x())) < 0)
                            || (((int64_t)point.get_x() + ((int64_t)deltaPoint.get_y() + size.get_y())) > SCREEN_HEIGHT))
                    return false;
                else {
                    newCoordinate.set_x(point.get_x() + deltaPoint.get_y());
                    newCoordinate.set_y(point.get_y() - (deltaPoint.get_x() + size.get_x()));
                }
            }
            else {
                if ((((int64_t)point.get_x() - ((int64_t)deltaPoint.get_x() + size.get_x())) < 0)
                            || (((int64_t)point.get_y() - ((int64_t)deltaPoint.get_y() + size.get_y())) < 0))
                    return false;
                else {
                    newCoordinate.set_x(point.get_x() - (deltaPoint.get_y()));
                    newCoordinate.set_y(point.get_y() - (deltaPoint.get_x() + size.get_x()));
                }
            }
        }
        else {
            if (isYMore) {
                if ((((int64_t)point.get_y() + ((int64_t)deltaPoint.get_x() + size.get_x())) > SCREEN_WIDTH)
                            || (((int64_t)point.get_x() + ((int64_t)deltaPoint.get_y() + size.get_y())) > SCREEN_HEIGHT))
                    return false;
                else {
                    newCoordinate.set_x(point.get_x() + deltaPoint.get_y());
                    newCoordinate.set_y(point.get_y() + (deltaPoint.get_x() - size.get_x()));
                }
            }
            else {
                if ((((int64_t)point.get_x() - ((int64_t)deltaPoint.get_y() + size.get_y())) < 0)
                            || (((int64_t)point.get_y() + ((int64_t)deltaPoint.get_x() + size.get_x())) > SCREEN_WIDTH))
                    return false;
                else {
                    newCoordinate.set_x(point.get_x() - (deltaPoint.get_y()));
                    newCoordinate.set_y(point.get_y() - (size.get_x() - deltaPoint.get_x()));
                }
            }
        }

        this->set_coordinate(newCoordinate);
        this->rotate_right();
        return true;
    };

    bool move_shape(const Point2DF coordinate) {
        Point2DF tmpPoint = coordinate;
        tmpPoint += this->_coordinate;

        if ((tmpPoint.get_x() < SCREEN_HEIGHT) && (tmpPoint.get_y() < SCREEN_WIDTH))
        {
            this->_coordinate = tmpPoint;
            return true;
        }

        return false;
    };

    bool move_shape(const uint32_t x, const uint32_t y) {
        Point2DF tmpPoint = Point2DF(x, y);
        tmpPoint += this->_coordinate;

        if ((tmpPoint.get_x() < SCREEN_HEIGHT) && (tmpPoint.get_y() < SCREEN_WIDTH))
        {
            this->_coordinate = tmpPoint;
            return true;
        }

        return false;
    };

    Point2DF get_center() {
        return Point2DF((this->get_coordinate().get_x() + (this->get_coordinate().get_x() + this->get_size().get_x())) / 2,
            (this->get_coordinate().get_y() + (this->get_coordinate().get_y() + this->get_size().get_y())) / 2);
    }


    PrimitiveShape& operator=(const PrimitiveShape& primitiveShape) {
        this->_coordinate = primitiveShape.get_coordinate();
        this->_size = primitiveShape.get_size();
        this->_color = primitiveShape.get_color();
        this->_currentAngle = primitiveShape.get_current_angle();
        return *this;
    };

    bool is_collided_with_shape(PrimitiveShape* shape) {
        Point2DF maskedBodyTopLeft = this->get_coordinate();
        Point2DF maskedBodyBottomRight = this->get_coordinate() + this->get_size();
        Point2DF layeredBodyTopLeft = shape->get_coordinate();
        Point2DF layeredBodyBottomRight = shape->get_coordinate() + shape->get_size();

        bool leftUpInnerX = ((maskedBodyTopLeft.get_x() < layeredBodyTopLeft.get_x())
            && (layeredBodyTopLeft.get_x() < maskedBodyBottomRight.get_x())) ? true : false;

        bool leftUpInnerY = ((maskedBodyTopLeft.get_y() < layeredBodyTopLeft.get_y())
            && (layeredBodyTopLeft.get_y() < maskedBodyBottomRight.get_y())) ? true : false;

        bool rightBottomInnerX = ((maskedBodyTopLeft.get_x() < layeredBodyBottomRight.get_x())
            && (layeredBodyBottomRight.get_x() < maskedBodyBottomRight.get_x())) ? true : false;

        bool rightBottomInnerY = ((maskedBodyTopLeft.get_y() < layeredBodyBottomRight.get_y())
            && (layeredBodyBottomRight.get_y() < maskedBodyBottomRight.get_y())) ? true : false;

        bool pointLeftUpInner = (leftUpInnerX && leftUpInnerY == true) ? true : false;
        bool pointLeftBottomInner = (leftUpInnerX && rightBottomInnerY == true) ? true : false;
        bool pointRightUpInner = (rightBottomInnerX && leftUpInnerY == true) ? true : false;
        bool pointRightBottomInner = (rightBottomInnerX && rightBottomInnerY == true) ? true : false;


        if (pointLeftUpInner || pointLeftBottomInner || pointRightUpInner || pointRightBottomInner)
            return true;

        return false;
    }


    virtual ShapeType get_shapeType() = 0;

protected:
    Point2DF _coordinate;
    Point2DF _size;
    uint32_t _color;

    Angle _currentAngle = Angle::BottomLeft_e;
};

struct FullSideShape : public PrimitiveShape {
public:
    FullSideShape() : PrimitiveShape() {};
    FullSideShape(const PrimitiveShape& shape) : PrimitiveShape(shape) {};
    ~FullSideShape() {};
    void rotate_right() {
        this->set_size(Point2DF(this->get_size().get_y(), this->get_size().get_x()));
    };

    void mirror_shape() {};
};

struct Rectangle : FullSideShape
{
public:
    Rectangle() : FullSideShape() {};
    Rectangle(const PrimitiveShape& shape) : FullSideShape(shape) {};
    ~Rectangle() {};

    void draw() {
        uint32_t startX = this->_coordinate.get_x();
        uint32_t startY = this->_coordinate.get_y();
        uint32_t sizeX = this->_size.get_x() + startX;
        uint32_t sizeY = this->_size.get_y() + startY;

        for (uint32_t j = startY; j < sizeY; j++) {
            for (uint32_t i = startX; i < sizeX; i++) {
                buffer[i][j] = this->_color;
            }
        }
    };

    ShapeType get_shapeType() { return ShapeType::Rectangle_e; };
};

struct Circle : public FullSideShape
{
public:
    Circle() : FullSideShape() {};
    Circle(const PrimitiveShape& shape) : FullSideShape(shape) {};
    ~Circle() {};

    void draw() {
        uint32_t startX = this->_coordinate.get_x();
        uint32_t startY = this->_coordinate.get_y();

        int32_t a = this->_size.get_x() / 2;
        int32_t b = this->_size.get_y() / 2;

        //1
        for (int32_t i = 0; i < a; i++) {
            for (int32_t j = 0; j < std::sqrt(((a * a - i * i) * b * b) / (a * a)); j++) {
                buffer[i + startX + a][j + startY + b] = this->_color;
            }
        }
        //2
        for (int32_t i = 0; i < a; i++) {
            for (int32_t j = -std::sqrt(((a * a - i * i) * b * b) / (a * a)); j < 0; j++) {
                buffer[i + startX + a][j + startY + b] = this->_color;
            }
        }
        //3
        for (int32_t i = 0; i > -a; i--) {
            for (int32_t j = -std::sqrt(((a * a - i * i) * b * b) / (a * a)); j < 0; j++) {
                buffer[i + startX + a][j + startY + b] = this->_color;
            }
        }
        //4
        for (int32_t i = 0; i > -a; i--) {
            for (int32_t j = 0; j < std::sqrt(((a * a - i * i) * b * b) / (a * a)); j++) {
                buffer[i + startX + a][j + startY + b] = this->_color;
            }
        }
    };

    ShapeType get_shapeType() { return ShapeType::Circle_e; };
};

struct RightTriangle : public PrimitiveShape
{
    //90 degree angle in bottom left by default
public:
    RightTriangle() : PrimitiveShape() {};
    RightTriangle(const PrimitiveShape& shape) : PrimitiveShape(shape) {};
    ~RightTriangle() {};

    void draw() {
        uint32_t startX = this->_coordinate.get_x();
        uint32_t startY = this->_coordinate.get_y();

        float_t g;

        int32_t step = 1;

        g = (float_t)this->_size.get_y() / this->_size.get_x();


        switch (_currentAngle) {
        case Angle::BottomLeft_e:
            for (uint32_t i = 0; i < this->_size.get_x(); i++) {
                for (uint32_t j = 0; j < i * g; j++) {
                    buffer[i + startX][j + startY] = this->_color;
                }
            }
            break;
        case Angle::BottomRight_e:
            for (uint32_t i = 0; i < this->_size.get_x(); i++) {
                for (uint32_t j = this->_size.get_y(); j > (this->_size.get_y() - (i * g)); j--) {
                    buffer[i + startX][j + startY] = this->_color;
                }
            }
            break;
        case Angle::TopLeft_e:
            for (uint32_t i = 0; i < this->_size.get_x(); i++) {
                for (uint32_t j = 0; j < (this->_size.get_y() - (i * g)); j++) {
                    buffer[i + startX][j + startY] = this->_color;
                }
            }
            break;
        case Angle::TopRight_e:
            for (uint32_t i = 0; i < this->_size.get_x(); i++) {
                for (uint32_t j = (i * g); j < this->_size.get_y(); j++) {
                    buffer[i + startX][j + startY] = this->_color;
                }
            }
            break;
        }

    };

    void rotate_right() {
        this->set_size(Point2DF(this->get_size().get_y(), this->get_size().get_x()));
        switch (_currentAngle) {
        case Angle::BottomLeft_e:
            _currentAngle = Angle::TopLeft_e;
            break;
        case Angle::BottomRight_e:
            _currentAngle = Angle::BottomLeft_e;
            break;
        case Angle::TopLeft_e:
            _currentAngle = Angle::TopRight_e;
            break;
        case Angle::TopRight_e:
            _currentAngle = Angle::BottomRight_e;
            break;
        }
    }

    void mirror_shape() {
        switch (_currentAngle) {
        case Angle::BottomLeft_e:
            _currentAngle = Angle::BottomRight_e;
            break;
        case Angle::BottomRight_e:
            _currentAngle = Angle::BottomLeft_e;
            break;
        case Angle::TopLeft_e:
            _currentAngle = Angle::TopRight_e;
            break;
        case Angle::TopRight_e:
            _currentAngle = Angle::TopLeft_e;
            break;
        }
    }

    ShapeType get_shapeType() { return ShapeType::RightTriangle_e; };
};

struct CompositeShape
{
public:
    CompositeShape() {};
    ~CompositeShape() {
        for (auto shape : this->_shapes) {
            delete shape;
        }
        this->_shapes.clear();
    };

    void clear() {
        this->_shapes.clear();
    }

    void add_shape(Rectangle shape) {
        PrimitiveShape* tmp = new Rectangle(shape);
        this->_shapes.push_back(tmp);
    };

    void add_shape(Circle shape) {
        PrimitiveShape* tmp = new Circle(shape);
        this->_shapes.push_back(tmp);
    };

    void add_shape(RightTriangle shape) {
        PrimitiveShape* tmp = new RightTriangle(shape);
        this->_shapes.push_back(tmp);
    };

    void remove_shape(uint16_t id) {
        delete this->_shapes.at(id);
        this->_shapes.erase(this->_shapes.begin() + id);
    };

    void add_composite_shape(CompositeShape* compositeShape) {
        Rectangle rectTmp;
        Circle circTmp;
        RightTriangle rightTriangleTmp;
        for (auto shape : compositeShape->_shapes) {
            switch (shape->get_shapeType()) {
            case ShapeType::Rectangle_e:
                rectTmp = *shape;
                this->add_shape(rectTmp);
                break;
            case ShapeType::Circle_e:
                circTmp = *shape;
                this->add_shape(circTmp);
                break;
            case ShapeType::RightTriangle_e:
                rightTriangleTmp = *shape;
                this->add_shape(rightTriangleTmp);
                break;
            default:
                return;
            }
        }
    };

    void draw() {
        for (auto i : _shapes) {
            i->draw();
        }
    };

    bool rotate_right_around(Point2DF point) {
        std::vector<PrimitiveShape*> shapes = this->_shapes;
        for (auto shape : shapes) {
            Rectangle tmp(*shape);
            if (tmp.rotate_right_around(point))
                continue;
            return false;
        }
        shapes.clear();
        for (auto shape : this->_shapes) {
            shape->rotate_right_around(point);
        }
        return true;
    }
    bool rotate_right_around_self() {
        std::vector<PrimitiveShape*> shapes = this->_shapes;
        Point2DF point;
        uint64_t tmp_x = 0;
        uint64_t tmp_y = 0;

        for (auto shape : shapes) {
            tmp_x += shape->get_center().get_x();
            tmp_y += shape->get_center().get_y();
        }
        tmp_x /= shapes.size();
        tmp_y /= shapes.size();
        point.set_x(tmp_x);
        point.set_y(tmp_y);

        for (auto shape : shapes) {
            Rectangle tmp(*shape);
            if (tmp.rotate_right_around(point))
                continue;
            else
                return false;
        }

        shapes.clear();
        for (auto shape : this->_shapes)
            shape->rotate_right_around(point);
        return true;
    }

    const PrimitiveShape* get_shape_at(size_t id) {
        return this->_shapes.at(id);
    };

    size_t get_size() {
        return this->_shapes.size();
    };

    void move_on(Point2DF direct) {
        for (auto shape : this->_shapes) {
            shape->set_coordinate(shape->get_coordinate() + direct);
        }
    };

    bool check_for_collide(PrimitiveShape* shape) {
        for (auto primShape : _shapes) {
            if (primShape->is_collided_with_shape(shape) == true)
                return true;
        }
        return false;
    }

    int32_t get_collided_shape_id(Point2DF coord, Point2DF size) {
        PrimitiveShape* shape = new Rectangle;
        shape->set_coordinate(Point2DF(coord));
        shape->set_size(size);
        for (size_t it = 0; it < this->_shapes.size(); it++) {
            if (this->_shapes.at(it)->is_collided_with_shape(shape) == true) {
                delete shape;
                return it;
            }
        }
        delete shape;
        return -1;
    }

    Point2DF get_coordinate_of_shape_at(size_t id) {
        return this->_shapes.at(id)->get_coordinate();
    }
    Point2DF get_size_of_shape_at(size_t id) {
        return this->_shapes.at(id)->get_size();
    }

private:
    std::vector<PrimitiveShape*> _shapes;
};


enum NormalDirection {
    NORMAL_UP,
    NORMAL_RIGHT,
    NORMAL_DOWN,
    NORMAL_LEFT
};

enum CollideDirection {
    COLLIDE_UP,
    COLLIDE_RIGHT,
    COLLIDE_DOWN,
    COLLIDE_LEFT,
    COLLIDE_UP_RIGHT,
    COLLIDE_UP_LEFT,
    COLLIDE_DOWN_RIGHT,
    COLLIDE_DOWN_LEFT
};


struct Body2D {
public:
    Body2D() {
        this->_compShape = new CompositeShape;
        this->_size = Point2DF((SCREEN_HEIGHT + 1), (SCREEN_WIDTH + 1));
        this->_coordinate = Point2DF(this->_size.get_x(), this->_size.get_y());
        this->_collisionLayer = 0x00;
        this->_collisionMask = 0x00;
        this->_deletable = false;
    };
    Body2D(const Body2D& body) {
        this->_coordinate = body._coordinate;
        this->_size = body._size;
        this->_compShape = body._compShape;
        this->_normalDir = body._normalDir;
        this->_collisionLayer = body._collisionLayer; //From -> with what masks that body interructed
        this->_collisionMask = body._collisionMask; //With -> with what kind of "From" that body interructed
        this->_deletable = false;
    }
    virtual ~Body2D() { delete _compShape; };
    virtual void init() = 0;
    void draw() { _compShape->draw(); };
    virtual void act(float dt) = 0;

    void add_shape(Rectangle shape) {
        if (recalculate_self_coordinates(shape.get_coordinate(), shape.get_size()))
            this->_compShape->add_shape(shape);
    };

    void add_shape(Circle shape) {
        if (recalculate_self_coordinates(shape.get_coordinate(), shape.get_size()))
            this->_compShape->add_shape(shape);
    };

    void add_shape(RightTriangle shape) {
        if (recalculate_self_coordinates(shape.get_coordinate(), shape.get_size()))
            this->_compShape->add_shape(shape);
    };

    void add_shape(CompositeShape* compositeShape) {
        this->_compShape->add_composite_shape(compositeShape);
    };

    void remove_shape(uint16_t id) {
        this->_compShape->remove_shape(id);
    }

    void calculate_self_coordinates() {
        Point2DF leftUp;
        Point2DF rightBottom;

        leftUp = this->_compShape->get_coordinate_of_shape_at(0);
        rightBottom = leftUp + this->_compShape->get_size_of_shape_at(0);

        if (this->_compShape->get_size() == 0) {
            this->_coordinate = leftUp;
            this->_size = rightBottom - leftUp;
            return;
        }

        for (int i = 0; i < this->_compShape->get_size(); i++) {
            if (leftUp.get_x() > this->_compShape->get_coordinate_of_shape_at(i).get_x())
                leftUp.set_x(this->_compShape->get_coordinate_of_shape_at(i).get_x());

            if (leftUp.get_y() > this->_compShape->get_coordinate_of_shape_at(i).get_y())
                leftUp.set_y(this->_compShape->get_coordinate_of_shape_at(i).get_y());

            if (rightBottom.get_x() < leftUp.get_x() + this->_compShape->get_size_of_shape_at(i).get_x())
                rightBottom.set_x(leftUp.get_x() + this->_compShape->get_size_of_shape_at(i).get_x());

            if (rightBottom.get_y() < leftUp.get_y() + this->_compShape->get_size_of_shape_at(i).get_y())
                rightBottom.set_y(leftUp.get_y() + this->_compShape->get_size_of_shape_at(i).get_y());
        }
        this->_coordinate = leftUp;
        this->_size = rightBottom - leftUp;
        return;
    }
    bool recalculate_self_coordinates(Point2DF coordinate, Point2DF size) {
        Point2DF leftUpSelf = this->_coordinate;
        Point2DF rightDownSelf = this->_coordinate + Point2DF(this->_size.get_x(), this->_size.get_y());

        Point2DF leftUp = coordinate;
        Point2DF rightDown = coordinate + Point2DF(size.get_x(), size.get_y());

        if (!coordinate_checker(coordinate))
            return false;
        if (!coordinate_checker(size))
            return false;

        if (this->_coordinate.get_x() == SCREEN_HEIGHT + 1)
            this->_coordinate.set_x(coordinate.get_x());
        else if (this->_coordinate.get_x() > coordinate.get_x())
            this->_coordinate.set_x(coordinate.get_x());
        if (this->_coordinate.get_y() == SCREEN_WIDTH + 1)
            this->_coordinate.set_y(coordinate.get_y());
        else if (this->_coordinate.get_y() > coordinate.get_y())
            this->_coordinate.set_y(coordinate.get_y());

        if (this->_size.get_x() == SCREEN_HEIGHT + 1)
            this->_size.set_x(size.get_x());
        else if ((leftUpSelf.get_x() > leftUp.get_x()) || (rightDownSelf.get_x() < rightDown.get_x()))
            if ((leftUpSelf.get_x() > leftUp.get_x()))
                this->_size.set_x(this->_size.get_x() + (leftUpSelf.get_x() - leftUp.get_x()));
            else
                this->_size.set_x(this->_size.get_x() + (rightDown.get_x() - rightDownSelf.get_x()));

        if (this->_size.get_y() == SCREEN_WIDTH + 1)
            this->_size.set_y(size.get_y());

        else if ((leftUpSelf.get_y() > leftUp.get_y()) || (rightDownSelf.get_y() < rightDown.get_y()))
            if ((leftUpSelf.get_y() > leftUp.get_y()))
                this->_size.set_y(this->_size.get_y() + (leftUpSelf.get_y() - leftUp.get_y()));
            else
                this->_size.set_y(this->_size.get_y() + (rightDown.get_y() - rightDownSelf.get_y()));
        return true;
    }
    bool coordinate_checker(Point2DF point) {
        if ((point.get_x() > SCREEN_HEIGHT) || (point.get_x() < 0))
            return false;
        if ((point.get_y() > SCREEN_WIDTH) || (point.get_y() < 0))
            return false;

        return true;
    }
    bool is_move_acceptible(Point2DF direct) {
        Point2DF tmp;

        if (direct.get_x() < 0) {
            if (this->_coordinate.get_x() < (-direct.get_x()))
                return false;

            tmp.set_x(_coordinate.get_x() + direct.get_x());
        }
        else
            tmp.set_x(this->_coordinate.get_x() + direct.get_x());

        if (direct.get_y() < 0) {
            if (this->_coordinate.get_y() < (-direct.get_y()))
                return false;

            tmp.set_y(_coordinate.get_y() + direct.get_y());
        }
        else
            tmp.set_y(this->_coordinate.get_y() + direct.get_y());

        if (!coordinate_checker(tmp))
            return false;

        if (direct.get_x() < 0)
            tmp.set_x(this->_size.get_x() + direct.get_x() + this->_coordinate.get_x());
        else 
            tmp.set_x(this->_size.get_x() + direct.get_x() + this->_coordinate.get_x());
        if (direct.get_y() < 0)
            tmp.set_y(this->_size.get_y() + direct.get_y() + this->_coordinate.get_y());
        else
            tmp.set_y(this->_size.get_y() + direct.get_y() + this->_coordinate.get_y());

        if (!coordinate_checker(tmp))
            return false;

        return true;
    }

    bool move_on(Point2DF direct) {
        Point2DF tmpPoint(this->_coordinate + direct);
        if (!is_move_acceptible(direct))
            return false;

        this->_compShape->move_on(Point2DF(direct.get_x(), direct.get_y()));
        this->_coordinate = Point2DF(tmpPoint.get_x(), tmpPoint.get_y());
        return true;
    };
    bool move_immedeatly(Point2DF direct) {
        Point2DF tmpPoint(direct);
        if (!is_move_acceptible(direct))
            return false;

        this->_compShape->move_on(Point2DF(direct.get_x(), direct.get_y()));
        this->_coordinate = Point2DF(this->get_coordinate() + tmpPoint);
        return true;
    };

    Point2DF get_coordinate() { return this->_coordinate; }

    Point2DF get_size() { return this->_size; }

    NormalDirection get_normalDir() { return this->_normalDir; };

    void set_normalDir(NormalDirection dir) { this->_normalDir = dir; };

    CompositeShape* get_compShape() { return _compShape; };

    void set_collision_layer(uint16_t mask) {
        this->_collisionLayer = mask;
    }

    uint16_t get_collision_layer() {
        return this->_collisionLayer;
    }

    void set_collision_mask(uint16_t mask) {
        this->_collisionMask = mask;
    }

    uint16_t get_collision_mask() {
        return this->_collisionMask;
    }

    bool is_collided(uint16_t mask) {
        return ((this->_collisionLayer & mask) != 0x00) ? true : false;
    }

    bool is_collidable(Body2D* body) {
        return ((body->_collisionLayer & this->get_collision_mask()) != 0x00) ? true : false;
    }

    bool is_box_collided(Body2D* body) {
        Point2DF maskedBodyTopLeft = body->get_coordinate();
        Point2DF maskedBodyBottomRight = body->get_coordinate() + body->get_size();
        Point2DF layeredBodyTopLeft = this->get_coordinate();
        Point2DF layeredBodyBottomRight = this->get_coordinate() + this->get_size();

        bool leftUpInnerX = ((maskedBodyTopLeft.get_x() < layeredBodyTopLeft.get_x())
            && (layeredBodyTopLeft.get_x() < maskedBodyBottomRight.get_x())) ? true : false;

        bool leftUpInnerY = ((maskedBodyTopLeft.get_y() < layeredBodyTopLeft.get_y())
            && (layeredBodyTopLeft.get_y() < maskedBodyBottomRight.get_y())) ? true : false;

        bool rightBottomInnerX = ((maskedBodyTopLeft.get_x() < layeredBodyBottomRight.get_x())
            && (layeredBodyBottomRight.get_x() < maskedBodyBottomRight.get_x())) ? true : false;

        bool rightBottomInnerY = ((maskedBodyTopLeft.get_y() < layeredBodyBottomRight.get_y())
            && (layeredBodyBottomRight.get_y() < maskedBodyBottomRight.get_y())) ? true : false;

        bool pointLeftUpInner = (leftUpInnerX && leftUpInnerY == true) ? true : false;
        bool pointLeftBottomInner = (leftUpInnerX && rightBottomInnerY == true) ? true : false;
        bool pointRightUpInner = (rightBottomInnerX && leftUpInnerY == true) ? true : false;
        bool pointRightBottomInner = (rightBottomInnerX && rightBottomInnerY == true) ? true : false;


        if (pointLeftUpInner || pointLeftBottomInner || pointRightUpInner || pointRightBottomInner)
            return true;

        return false;
    }
    int32_t get_collided_shape_id(Body2D* body) {
        return this->_compShape->get_collided_shape_id(body->get_coordinate(), body->get_size());
    }

    void set_direction(Point2DF newDir) { _direction = newDir; }

    Point2DF get_direction() { return _direction; }

    void set_speed(float newSpeed) { _speed = newSpeed; }

    float get_speed() { return _speed; }

    void delete_request() { this->_deletable = true; }

    bool is_deletable() { return this->_deletable; }

    virtual void collision_signal(std::string operationName, int32_t shape_id) {};

    virtual void collision_act(CollideDirection direction, Body2D* maskedBody, int32_t shape_id) {};

    void procedure_collision(Body2D* maskedBody, int32_t shape_id) {
        Point2DF maskedBodyTopLeft = maskedBody->get_compShape()->get_coordinate_of_shape_at(shape_id);
        Point2DF maskedBodyBottomRight = maskedBody->get_compShape()->get_coordinate_of_shape_at(shape_id)
            + maskedBody->get_compShape()->get_size_of_shape_at(shape_id);

        Point2DF layeredBodyTopLeft = this->get_coordinate();
        Point2DF layeredBodyBottomRight = this->get_coordinate() + this->get_size();

        bool leftUpInnerX = ((maskedBodyTopLeft.get_x() < layeredBodyTopLeft.get_x())
            && (layeredBodyTopLeft.get_x() < maskedBodyBottomRight.get_x())) ? true : false;
        bool leftUpInnerY = ((maskedBodyTopLeft.get_y() < layeredBodyTopLeft.get_y())
            && (layeredBodyTopLeft.get_y() < maskedBodyBottomRight.get_y())) ? true : false;
        bool rightBottomInnerX = ((maskedBodyTopLeft.get_x() < layeredBodyBottomRight.get_x())
            && (layeredBodyBottomRight.get_x() < maskedBodyBottomRight.get_x())) ? true : false;
        bool rightBottomInnerY = ((maskedBodyTopLeft.get_y() < layeredBodyBottomRight.get_y())
            && (layeredBodyBottomRight.get_y() < maskedBodyBottomRight.get_y())) ? true : false;

        bool pointLeftUpInner = (leftUpInnerX && leftUpInnerY == true) ? true : false;
        bool pointLeftBottomInner = (rightBottomInnerX && leftUpInnerY == true) ? true : false;
        bool pointRightUpInner = (leftUpInnerX && rightBottomInnerY == true) ? true : false;
        bool pointRightBottomInner = (rightBottomInnerX && rightBottomInnerY == true) ? true : false;


        if (pointLeftUpInner && pointRightUpInner)
            this->collision_act(CollideDirection::COLLIDE_UP, maskedBody, shape_id);
        else if (pointLeftUpInner && pointLeftBottomInner)
            this->collision_act(CollideDirection::COLLIDE_LEFT, maskedBody, shape_id);
        else if (pointRightUpInner && pointRightBottomInner)
            this->collision_act(CollideDirection::COLLIDE_RIGHT, maskedBody, shape_id);
        else if (pointRightBottomInner && pointLeftBottomInner)
            this->collision_act(CollideDirection::COLLIDE_DOWN, maskedBody, shape_id);
        else if (pointRightBottomInner)
            this->collision_act(CollideDirection::COLLIDE_DOWN_RIGHT, maskedBody, shape_id);
        else if (pointLeftBottomInner)
            this->collision_act(CollideDirection::COLLIDE_DOWN_LEFT, maskedBody, shape_id);
        else if (pointLeftUpInner)
            this->collision_act(CollideDirection::COLLIDE_UP_LEFT, maskedBody, shape_id);
        else if (pointRightUpInner)
            this->collision_act(CollideDirection::COLLIDE_UP_RIGHT, maskedBody, shape_id);
    }
    virtual Point2DF get_start_point() { return Point2DF(0, 0); }

    void set_coordinate(Point2DF point) { this->_coordinate = point; }

private:
    Point2DF _coordinate;
    Point2DF _size;
    CompositeShape* _compShape;
    NormalDirection _normalDir;
    uint16_t _collisionLayer;
    uint16_t _collisionMask;

    bool _deletable;

    Point2DF _direction;
    float _speed;
};

struct BordersLeft : Body2D {
    void init() {
        Rectangle leftBorder;
        leftBorder.set_coordinate(Point2DF((0), (0)));
        leftBorder.set_size(Point2DF(SCREEN_HEIGHT, Constants::border_width));
        leftBorder.set_color(Constants::color_borders);
        this->add_shape(leftBorder);
        this->set_collision_mask(0x01);
    };

    void act(float dt) { };
};
struct BordersRight : Body2D {
    void init() {
        Rectangle rightBorder;
        rightBorder.set_size(Point2DF(SCREEN_HEIGHT, Constants::border_width));
        rightBorder.set_color(Constants::color_borders);
        rightBorder.set_coordinate(Point2DF((0), (SCREEN_WIDTH - rightBorder.get_size().get_y())));
        this->add_shape(rightBorder);
        this->set_collision_mask(0x01);
    };

    void act(float dt) { };
};
struct BordersTop : Body2D {
    void init() {
        Rectangle topBorder;
        topBorder.set_coordinate(Point2DF((0), (0)));
        topBorder.set_size(Point2DF(Constants::border_width, SCREEN_WIDTH));
        topBorder.set_color(Constants::color_borders);
        this->add_shape(topBorder);
        this->set_collision_mask(0x01);
    };

    void act(float dt) { };
};
struct BordersBottom : Body2D {
    void init() {
        Rectangle topBorder;
        topBorder.set_coordinate(Point2DF((SCREEN_HEIGHT - Constants::border_width), (Constants::border_width)));
        topBorder.set_size(Point2DF(Constants::border_width, SCREEN_WIDTH - 2 * Constants::border_width));
        topBorder.set_color(Constants::color_borders);
        this->add_shape(topBorder);
        this->set_collision_mask(0x01);
    };

    void act(float dt) { };
};

struct Ship : Body2D {
    void init() {
        Circle center;
        center.set_size(Point2DF(Constants::size_unit * 3, Constants::size_unit * 3));
        center.set_coordinate(Point2DF(((SCREEN_HEIGHT / 2) - center.get_size().get_x() / 2), 
            ((SCREEN_WIDTH / 2) - center.get_size().get_y() / 2)));

        center.set_color(Constants::color_desk);
        this->add_shape(center);
        Circle center2(center);
        this->add_shape(center2);
        Circle circle;
        circle.set_coordinate(Point2DF((center.get_coordinate().get_x()), 
            (center.get_coordinate().get_y() + center.get_size().get_y() - Constants::size_unit / 2 - 2)));

        circle.set_size(Point2DF(Constants::size_unit, Constants::size_unit));
        circle.set_color(center.get_color() + 0x663300);
        this->add_shape(circle);
        this->set_normalDir(NormalDirection::NORMAL_UP);
        this->set_collision_layer(0x1D);
        this->set_collision_mask(0x02);
        this->set_direction(Point2DF(center.get_center() - circle.get_center()));
    };

    void act(float dt) {
        Point2DF moveUnits;
        if (is_key_pressed(VK_LEFT)) {
            Circle circ(*(Circle*)this->get_compShape()->get_shape_at(2));
            Circle main(*(Circle*)this->get_compShape()->get_shape_at(0));

            Point2DF point = Point2DF(main.get_center() - circ.get_center());

            Point2DF newPoint = Point2DF(point.get_x() * cos(Constants::rotate_degree * dt / 2) - point.get_y() * sin(Constants::rotate_degree * dt / 2),
                point.get_x() * sin(Constants::rotate_degree * dt / 2) + point.get_y() * cos(Constants::rotate_degree * dt / 2));

            this->remove_shape(2);
            circ.set_coordinate(newPoint + main.get_center());
            this->add_shape(circ);
            this->set_direction(Point2DF( main.get_center() - circ.get_center()));
        }
        if (is_key_pressed(VK_RIGHT)) {
            Circle circ(*(Circle*)this->get_compShape()->get_shape_at(2));
            Circle main(*(Circle*)this->get_compShape()->get_shape_at(0));

            Point2DF point = Point2DF(main.get_center() - circ.get_center());

            Point2DF newPoint = Point2DF(point.get_x() * cos(Constants::rotate_degree * dt / 2) + point.get_y() * sin(Constants::rotate_degree * dt / 2),
                -point.get_x() * sin(Constants::rotate_degree * dt / 2) + point.get_y() * cos(Constants::rotate_degree * dt / 2));

            this->remove_shape(2);
            circ.set_coordinate(newPoint + main.get_center());
            this->add_shape(circ);
            this->set_direction(main.get_center() - circ.get_center());
        }

        if (is_key_pressed(VK_DOWN))
            moveUnits = (this->get_direction() * dt * Constants::speed_unit);
        if (is_key_pressed(VK_UP))
            moveUnits = (this->get_direction() * (-dt) * Constants::speed_unit);

        if (moveUnits != Point2DF(0.0, 0.0))
            this->move_on(moveUnits);
    };
    void collision_act(CollideDirection direction, Body2D* maskedBody, int32_t shape_id) {
        Point2DF moveUnits(0, 0);
        uint16_t mask = this->get_collision_layer() & maskedBody->get_collision_mask();
        if ((mask == 0x01)) {
            if ((direction == CollideDirection::COLLIDE_RIGHT) || (direction == CollideDirection::COLLIDE_LEFT))
                if (direction == CollideDirection::COLLIDE_RIGHT)
                    moveUnits = Point2DF(0, -(this->get_coordinate().get_y() - Constants::border_width - 1));
                else
                    moveUnits = Point2DF(0, (SCREEN_WIDTH -  2 * Constants::border_width - this->get_size().get_y()));

            if ((direction == CollideDirection::COLLIDE_UP) || (direction == CollideDirection::COLLIDE_DOWN))
                if (direction == CollideDirection::COLLIDE_UP)
                    moveUnits = Point2DF((SCREEN_HEIGHT - 2 * Constants::border_width - this->get_size().get_x()), 0);
                else
                    moveUnits = Point2DF(-(this->get_coordinate().get_x() - Constants::border_width - 1), 0);
        }
        if ((mask & 0x1C) != 0x00){
            mark:
            moveUnits = Point2DF(Constants::size_unit + rand() % (int)(2 * Constants::speed_unit), 
                Constants::size_unit + rand() % (int)(2 * Constants::speed_unit));
            if (!is_move_acceptible(moveUnits))
                goto mark;

            Global::life_count--;
        }
        this->move_immedeatly(moveUnits);
    };
    Point2DF get_start_point() {
        Circle circ(*(Circle*)this->get_compShape()->get_shape_at(2));
        return circ.get_center();
    }
};

struct Projectile : Body2D {
    void init() {
        Circle center;
        center.set_size(Point2DF(Constants::size_unit, Constants::size_unit));
        center.set_coordinate(this->get_coordinate());
        center.set_color(Constants::color_projectile);
        this->add_shape(center);
        this->set_normalDir(NormalDirection::NORMAL_UP);
        this->set_collision_layer(0x01);
        this->set_collision_mask(0x02);
    };
    void act(float dt) {
        Point2DF moveUnits;
        moveUnits = this->get_direction() * dt * 10;
        if (moveUnits != Point2DF(0.0, 0.0))
            this->move_on(moveUnits);
    };
    void collision_act(CollideDirection direction, Body2D* maskedBody, int32_t shape_id) {
        Point2DF moveUnits(0, 0);
        uint16_t mask = this->get_collision_layer() & maskedBody->get_collision_mask();

        if ((mask == 0x01))
            if ((direction == CollideDirection::COLLIDE_RIGHT) || (direction == CollideDirection::COLLIDE_LEFT) 
                    || (direction == CollideDirection::COLLIDE_UP) || (direction == CollideDirection::COLLIDE_DOWN))
                this->delete_request();
    };
};

struct Asteroid1 : Body2D {
    void init() {
        Circle center;
        center.set_size(Point2DF(Constants::size_unit * 10, Constants::size_unit * 10));
        center.set_coordinate(this->get_coordinate());
        center.set_color(Constants::color_asteroid1);
        this->add_shape(center);
        this->set_normalDir(NormalDirection::NORMAL_UP);
        this->set_collision_layer(0x03);
        this->set_collision_mask(0x04);
    };

    void act(float dt) {
        Point2DF moveUnits;
        moveUnits = this->get_direction() * dt * 10;
        if (moveUnits != Point2DF(0.0, 0.0))
            this->move_on(moveUnits);
    };

    void collision_act(CollideDirection direction, Body2D* maskedBody, int32_t shape_id) {
        Point2DF moveUnits(0, 0);
        uint16_t mask = this->get_collision_layer() & maskedBody->get_collision_mask();
        if ((mask == 0x02))
            this->delete_request();

        if ((mask == 0x01)) {
            if ((direction == CollideDirection::COLLIDE_RIGHT) || (direction == CollideDirection::COLLIDE_LEFT))
                if (direction == CollideDirection::COLLIDE_RIGHT)
                    moveUnits = Point2DF(0, -(this->get_coordinate().get_y() - Constants::border_width - 1));
                else
                    moveUnits = Point2DF(0, (SCREEN_WIDTH - 2 * Constants::border_width - this->get_size().get_y()));

            if ((direction == CollideDirection::COLLIDE_UP) || (direction == CollideDirection::COLLIDE_DOWN))
                if (direction == CollideDirection::COLLIDE_UP)
                    moveUnits = Point2DF((SCREEN_HEIGHT - 2 * Constants::border_width - this->get_size().get_x()), 0);
                else
                    moveUnits = Point2DF(-(this->get_coordinate().get_x() - Constants::border_width - 1), 0);

            this->move_immedeatly(moveUnits);
        }
    };
};

struct Asteroid2 : Body2D {
    void init() {
        Circle center;
        center.set_size(Point2DF(Constants::size_unit * 5, Constants::size_unit * 5));
        center.set_coordinate(this->get_coordinate());
        center.set_color(Constants::color_asteroid2);
        this->add_shape(center);
        this->set_normalDir(NormalDirection::NORMAL_UP);
        this->set_collision_layer(0x03);
        this->set_collision_mask(0x08);
    };
    void act(float dt) {
        Point2DF moveUnits;
        moveUnits = this->get_direction() * dt * 10;
        if (moveUnits != Point2DF(0.0, 0.0))
            this->move_on(moveUnits);
    };

    void collision_act(CollideDirection direction, Body2D* maskedBody, int32_t shape_id) {
        Point2DF moveUnits(0, 0);
        uint16_t mask = this->get_collision_layer() & maskedBody->get_collision_mask();
        if ((mask == 0x02))
                this->delete_request();

        if ((mask == 0x01)) {
            if ((direction == CollideDirection::COLLIDE_RIGHT) || (direction == CollideDirection::COLLIDE_LEFT))
                if (direction == CollideDirection::COLLIDE_RIGHT)
                    moveUnits = Point2DF(0, -(this->get_coordinate().get_y() - Constants::border_width - 1));
                else
                    moveUnits = Point2DF(0, (SCREEN_WIDTH - 2 * Constants::border_width - this->get_size().get_y()));

            if ((direction == CollideDirection::COLLIDE_UP) || (direction == CollideDirection::COLLIDE_DOWN))
                if (direction == CollideDirection::COLLIDE_UP)
                    moveUnits = Point2DF((SCREEN_HEIGHT - 2 * Constants::border_width - this->get_size().get_x()), 0);
                else
                    moveUnits = Point2DF(-(this->get_coordinate().get_x() - Constants::border_width - 1), 0);

            this->move_immedeatly(moveUnits);
        }
    };
};

struct Asteroid3 : Body2D {
    void init() {
        Circle center;
        center.set_size(Point2DF(Constants::size_unit * 2, Constants::size_unit * 2));
        center.set_coordinate(this->get_coordinate());
        center.set_color(Constants::color_asteroid3);
        this->add_shape(center);
        this->set_normalDir(NormalDirection::NORMAL_UP);
        this->set_collision_layer(0x03);
        this->set_collision_mask(0x10);
    };

    void act(float dt) {
        Point2DF moveUnits;
        moveUnits = this->get_direction() * dt * 10;
        if (moveUnits != Point2DF(0.0, 0.0))
            this->move_on(moveUnits);
    };

    void collision_act(CollideDirection direction, Body2D* maskedBody, int32_t shape_id) {
        Point2DF moveUnits(0, 0);
        uint16_t mask = this->get_collision_layer() & maskedBody->get_collision_mask();
        if ((mask == 0x02))
            this->delete_request();

        if ((mask == 0x01)) {
            if ((direction == CollideDirection::COLLIDE_RIGHT) || (direction == CollideDirection::COLLIDE_LEFT))
                if (direction == CollideDirection::COLLIDE_RIGHT)
                    moveUnits = Point2DF(0, -(this->get_coordinate().get_y() - Constants::border_width - 1));
                else
                    moveUnits = Point2DF(0, (SCREEN_WIDTH - 2 * Constants::border_width - this->get_size().get_y()));

            if ((direction == CollideDirection::COLLIDE_UP) || (direction == CollideDirection::COLLIDE_DOWN))
                if (direction == CollideDirection::COLLIDE_UP) 
                    moveUnits = Point2DF((SCREEN_HEIGHT - 2 * Constants::border_width - this->get_size().get_x()), 0);
                else
                    moveUnits = Point2DF(-(this->get_coordinate().get_x() - Constants::border_width - 1), 0);

            this->move_immedeatly(moveUnits);
        }
    };
};

struct ShipIcon1 : Body2D {
    void init() {
        Circle center;
        center.set_size(Point2DF(Constants::size_unit * 3, Constants::size_unit * 3));
        center.set_coordinate(Point2DF(10, 15));
        center.set_color(Constants::color_desk);
        this->add_shape(center);
        Circle circle;
        circle.set_coordinate(Point2DF((center.get_coordinate().get_x()), 
            (center.get_coordinate().get_y() + center.get_size().get_y() - Constants::size_unit / 2 - 2)));

        circle.set_size(Point2DF(Constants::size_unit, Constants::size_unit));
        circle.set_color(center.get_color() + 0x663300);
        this->add_shape(circle);
        this->set_normalDir(NormalDirection::NORMAL_UP);
        this->set_collision_layer(0x00);
        this->set_collision_mask(0x00);
    };

    void act(float dt) {
    };
    void collision_act(CollideDirection direction, Body2D* maskedBody, int32_t shape_id) {
    };
};

struct ShipIcon2 : Body2D {
    void init() {
        Circle center;
        center.set_size(Point2DF(Constants::size_unit * 3, Constants::size_unit * 3));
        center.set_coordinate(Point2DF(10, 65));
        center.set_color(Constants::color_desk);
        this->add_shape(center);
        Circle circle;
        circle.set_coordinate(Point2DF((center.get_coordinate().get_x()), 
            (center.get_coordinate().get_y() + center.get_size().get_y() - Constants::size_unit / 2 - 2)));

        circle.set_size(Point2DF(Constants::size_unit, Constants::size_unit));
        circle.set_color(center.get_color() + 0x663300);
        this->add_shape(circle);
        this->set_normalDir(NormalDirection::NORMAL_UP);
        this->set_collision_layer(0x00);
        this->set_collision_mask(0x00);
    };

    void act(float dt) {
    };
    void collision_act(CollideDirection direction, Body2D* maskedBody, int32_t shape_id) {
    };
};

struct ShipIcon3 : Body2D {
    void init() {
        Circle center;
        center.set_size(Point2DF(Constants::size_unit * 3, Constants::size_unit * 3));
        center.set_coordinate(Point2DF(10, 115));
        center.set_color(Constants::color_desk);
        this->add_shape(center);
        Circle circle;
        circle.set_coordinate(Point2DF((center.get_coordinate().get_x()), 
            (center.get_coordinate().get_y() + center.get_size().get_y() - Constants::size_unit / 2 - 2)));

        circle.set_size(Point2DF(Constants::size_unit, Constants::size_unit));
        circle.set_color(center.get_color() + 0x663300);
        this->add_shape(circle);
        this->set_normalDir(NormalDirection::NORMAL_UP);
        this->set_collision_layer(0x00);
        this->set_collision_mask(0x00);
    };

    void act(float dt) {
    };
    void collision_act(CollideDirection direction, Body2D* maskedBody, int32_t shape_id) {
    };
};

struct Bodies {
public:
    Bodies() {};
    ~Bodies() {
        for (auto body : this->_bodies) {
            delete body;
        }
        this->_bodies.clear();
    };

    void add_body2d(Body2D* body) {
        _bodies.push_back(body);
    }

    void init() {
        for (auto body : this->_bodies) {
            body->init();
        }
    }
    void draw() {
        for (auto body : this->_bodies) {
            body->draw();
        }
    }

    void act(float dt) {
        for (auto body : this->_bodies) {
            body->act(dt);
        }
        for (int i = 0; i < this->_bodies.size(); i++) {
            if (this->_bodies.at(i)->is_deletable() == true) {
                if (this->_bodies.at(i)->get_collision_mask() == 0x04) {
                    for (int k = 0; k < 4; k++) {
                        Point2DF coord = this->_bodies.at(i)->get_coordinate();
                        Point2DF siz = this->_bodies.at(i)->get_size();
                        Body2D* asteroid = new Asteroid2;
                        asteroid->set_coordinate(Point2DF(float(coord.get_x() + rand()% (int) siz.get_x()),
                            float(coord.get_y() + rand() % (int)siz.get_y())));

                        asteroid->init();
                        asteroid->set_direction(Point2DF(1 + rand() % ((int)Constants::speed_unit - 2), 
                            1 + rand() % ((int)Constants::speed_unit - 2)));
                        this->add_body2d(asteroid);
                    }
                }
                if (this->_bodies.at(i)->get_collision_mask() == 0x08) {
                    for (int k = 0; k < 4; k++) {
                        Point2DF coord = this->_bodies.at(i)->get_coordinate();
                        Point2DF siz = this->_bodies.at(i)->get_size();
                        Body2D* asteroid = new Asteroid3;
                        asteroid->set_coordinate(Point2DF(float(coord.get_x() + rand() % (int)siz.get_x()),
                            float(coord.get_y() + rand() % (int)siz.get_y())));
                        asteroid->init();
                        asteroid->set_direction(Point2DF(1 + rand() % ((int)Constants::speed_unit - 2), 
                            1 + rand() % ((int)Constants::speed_unit - 2)));
                        this->add_body2d(asteroid);
                    }
                }
                delete this->_bodies.at(i);
                this->_bodies.erase(this->_bodies.begin() + i);
            }
        }

        check_collision();
    }

    void check_collision() {
        int32_t id = -1;
        for (auto bodyLayer : this->_bodies) {
            for (auto bodyMask : this->_bodies) {
                if ((bodyLayer != bodyMask) && (bodyMask->is_collidable(bodyLayer))) {
                    if (bodyLayer->is_box_collided(bodyMask)) {
                        id = bodyMask->get_collided_shape_id(bodyLayer);
                        if (id == -1)
                            continue;
                        else
                            procedure_collision(bodyLayer, bodyMask, id);
                    }
                }
            }
        }
    }

    void procedure_collision(Body2D* layeredBody, Body2D* maskedBody, int32_t shape_id) {
        layeredBody->procedure_collision(maskedBody, shape_id);
    }

    Body2D* get_body_at(int id) {
        return _bodies.at(id);
    }

    size_t get_size() {
        return this->_bodies.size();
    }

private:
    std::vector<Body2D*> _bodies;
};


struct NativeBody : Bodies {
    NativeBody() {
        Body2D* p_ship = new Ship;
        Body2D* p_borderRight = new BordersRight;
        Body2D* p_borderTop = new BordersTop;
        Body2D* p_borderLeft = new BordersLeft;
        Body2D* p_borderBottom = new BordersBottom;


        this->add_body2d(p_ship);
        this->add_body2d(p_borderRight);
        this->add_body2d(p_borderTop);
        this->add_body2d(p_borderLeft);
        this->add_body2d(p_borderBottom);
    }
};

struct Lifes : Bodies {
    Lifes() {
        Body2D* p_ship1 = new ShipIcon1;
        Body2D* p_ship2 = new ShipIcon2;
        Body2D* p_ship3 = new ShipIcon3;

        this->add_body2d(p_ship1);
        this->add_body2d(p_ship2);
        this->add_body2d(p_ship3);
    }
};


uint16_t aster1_count = 3;
uint16_t aster2_count = 6;
uint16_t aster3_count = 8;
Bodies* scene_bodies;

Bodies* lifes;

// initialize game data in this function
void initialize()
{
    srand(time(0));
    scene_bodies = new NativeBody;
    scene_bodies->init();
    for (int i = 0; i < aster1_count; i++) {
        Body2D* asteroid = new Asteroid1;
        asteroid->set_coordinate(Point2DF(float(Constants::border_width + rand() % SCREEN_HEIGHT - Constants::size_unit * 15), 
            float(Constants::border_width + rand() % SCREEN_WIDTH - Constants::size_unit * 15)));

        asteroid->init();
        asteroid->set_direction(Point2DF( 1 + rand() % ((int)Constants::speed_unit), 1 + rand() % ((int)Constants::speed_unit)));
        scene_bodies->add_body2d(asteroid);
    }
    for (int i = 0; i < aster2_count; i++) {
        Body2D* asteroid = new Asteroid2;
        asteroid->set_coordinate(Point2DF(float(Constants::border_width + rand() % SCREEN_HEIGHT - Constants::size_unit * 15),
            float(Constants::border_width + rand() % SCREEN_WIDTH - Constants::size_unit * 15)));

        asteroid->init();
        asteroid->set_direction(Point2DF(1 + rand() % ((int)Constants::speed_unit - 2), 1 + rand() % ((int)Constants::speed_unit - 2)));
        scene_bodies->add_body2d(asteroid);
    }
    for (int i = 0; i < aster3_count; i++) {
        Body2D* asteroid = new Asteroid3;
        asteroid->set_coordinate(Point2DF(float(Constants::border_width + rand() % SCREEN_HEIGHT - Constants::size_unit * 15),
            float(Constants::border_width + rand() % SCREEN_WIDTH - Constants::size_unit * 15)));

        asteroid->init();
        asteroid->set_direction(Point2DF(1 + rand() % ((int)Constants::speed_unit - 2), 1 + rand() % ((int)Constants::speed_unit - 2)));
        scene_bodies->add_body2d(asteroid);
    }
    lifes = new Lifes;
    lifes->init();
}

// this function is called to update game data,
// dt - time elapsed since the previous update (in seconds)
void act(float dt)
{
  if (is_key_pressed(VK_ESCAPE))
    schedule_quit_game();

  if (is_key_pressed(VK_SPACE)) {
      Body2D* projectile = new Projectile;
      projectile->set_coordinate(scene_bodies->get_body_at(0)->get_start_point());
      projectile->init();
      projectile->set_direction(Point2DF(0,0) - scene_bodies->get_body_at(0)->get_direction());
      scene_bodies->add_body2d(projectile);
  }
  scene_bodies->act(dt);

  if (scene_bodies->get_size() == 5)
      finalize();

  if (Global::life_count < lifes->get_size())
      lifes->get_body_at(lifes->get_size() - 1)->delete_request();

  if (Global::life_count < 1)
      finalize();

  lifes->act(dt);
}

// fill buffer in this function
// uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH] - is an array of 32-bit colors (8 bits per R, G, B)
void draw()
{
  // clear backbuffer
  memset(buffer, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));
  scene_bodies->draw();
  lifes->draw();
}

// free game data in this function
void finalize()
{
    delete scene_bodies;
    delete lifes;
}

