#include "geometry.h"

#include <stdexcept>
#include <iostream>

namespace{
    inline int32_t overflowCheckAdd(int32_t a, int32_t b){
        if(a > 0){
            if(b > 0){
                if(a > INT32_MAX - b){
                    throw std::invalid_argument("Coordinate overflow");
                }
            }
        } else if(b < 0){
            if(a < INT32_MIN - b){
                throw std::invalid_argument("Coordinate overflow");
            }
        }
        return a + b;
    }
}


// class AbstractVector2D

AbstractVector2D::AbstractVector2D(int32_t x, int32_t y): xn(x), yn(y) {};

int32_t AbstractVector2D::x() const{
    return xn;
}

int32_t AbstractVector2D::y() const{
    return yn;
}

bool AbstractVector2D::operator==(const AbstractVector2D& vec) const{
    return xn == vec.xn && yn == vec.yn;
}

void AbstractVector2D::add(const AbstractVector2D& vec){
    xn = overflowCheckAdd(xn, vec.xn);
    yn = overflowCheckAdd(yn, vec.yn);
}

template <class ResultType>
ResultType AbstractVector2D::operator+(const AbstractVector2D& vec) const{
    return ResultType(overflowCheckAdd(xn, vec.xn), overflowCheckAdd(yn, vec.yn));
}

template <class ResultType>
ResultType AbstractVector2D::reflection() const{
    return ResultType(yn, xn);
}



// Class Position

Position::Position(int32_t x, int32_t y): AbstractVector2D(x, y) {};

Position::operator Vector() const{
    return Vector(xn, yn);
}

bool Position::operator==(const Position& pos) const{
    return AbstractVector2D::operator==(pos);
}

Position& Position::operator+=(const Vector& vec){
    add(vec);
    return *this;
}

Position Position::operator+(const Vector& vec) const{
    return AbstractVector2D::operator+<Position>(vec);
}

const Position& Position::origin(){
    static Position orig(0,0);
    return orig;
}

Position Position::reflection() const{
    return AbstractVector2D::reflection<Position>();
}



// class Vector

Vector::Vector(int32_t x, int32_t y): AbstractVector2D(x, y) {};

Vector Vector::operator+(const Vector& vec) const{
    return AbstractVector2D::operator+<Vector>(vec);
}

bool Vector::operator==(const Vector& pos) const{
    return AbstractVector2D::operator==(pos);
}

Vector& Vector::operator+=(const Vector& vec){
    add(vec);
    return *this;
}

Position Vector::operator+(const Position& pos) const{
    return pos + *this;
}

Rectangle Vector::operator+(const Rectangle& rect) const{
    return rect + *this;
}

Rectangles Vector::operator+(const Rectangles& rectangles) const{
    return rectangles + *this;
}

Rectangles Vector::operator+(Rectangles&& rectangles) const{
    return std::move(rectangles += *this);
}

Vector::operator Position() const{
    return Position(xn, yn);
}

Vector Vector::reflection() const{
    return AbstractVector2D::reflection<Vector>();
}



// class Rectangle

Rectangle::Rectangle(uint32_t width, uint32_t height, const Position& pos): widthn(width), heightn(height), corner(pos) {
    if(width == 0 || height == 0){
        throw std::invalid_argument("Rectangle width and height must be positive");
    }
};

Rectangle::Rectangle(uint32_t width, uint32_t height): Rectangle(width, height, {0,0}) {};

uint32_t Rectangle::width() const{
    return widthn;
}

uint32_t Rectangle::height() const{
    return heightn;
}

const Position& Rectangle::pos() const{
    return corner;
}

uint64_t Rectangle::area() const{
    return widthn * heightn;
}

Rectangle Rectangle::reflection() const{
    return Rectangle(heightn, widthn, corner.reflection());
}

bool Rectangle::operator==(const Rectangle& rect) const{
    return widthn == rect.widthn && heightn == rect.heightn && corner == rect.corner;
}

Rectangle& Rectangle::operator+=(const Vector& vec){
    corner += vec;
    return *this;
}

Rectangle Rectangle::operator+(const Vector& vec) const{
    return Rectangle(widthn, heightn, corner + vec);
}

bool Rectangle::connectUp(const Rectangle& rect){
    if (corner + Vector(0, heightn) == rect.corner && widthn == rect.widthn) {
        heightn = overflowCheckAdd(heightn, rect.heightn);
        return true;
    } else {
        return false;
    }
}

bool Rectangle::connectRight(const Rectangle& rect){
    if (corner + Vector(widthn, 0) == rect.corner && heightn == rect.heightn) {
        widthn = overflowCheckAdd(widthn, rect.widthn);
        return true;
    } else {
        return false;
    }
}



// class Rectangless

Rectangles::Rectangles(std::initializer_list<Rectangle> rectangles): storage(rectangles) {};

Rectangles::Rectangles(): storage() {};

Rectangles::Rectangles(Rectangles &&rectangles) : storage(move(rectangles.storage)) {}

Rectangles &Rectangles::operator=(Rectangles &&r) {
    storage = move(r.storage);
    return *this;
}

Rectangle& Rectangles::operator[](size_t i){
    if(i >= storage.size()){
        throw std::invalid_argument("Rectangle index out of bounds");
    }
    return storage[i];
}

const Rectangle& Rectangles::operator[](size_t i) const{
    if(i >= storage.size()){
        throw std::invalid_argument("Rectangle index out of bounds");
    }
    return storage[i];
}

size_t Rectangles::size() const{
    return storage.size();
}

bool Rectangles::operator==(const Rectangles& rects) const{
    size_t s = size();
    if(s != rects.size()){
        return false;
    }
    for(size_t i = 0; i < s; i++){
        if(!(storage[i] == rects[i])){
            return false;
        }
    }
    return true;
}

Rectangles& Rectangles::operator+=(const Vector& vec){
    for(Rectangle& r: storage){
        r += vec;
    }
    return *this;
}
    
Rectangles Rectangles::operator+(const Vector& vec) const &{
    Rectangles ret = *this;
    return ret += vec;
}

Rectangles Rectangles::operator+(const Vector& vec) &&{
    return std::move(*this += vec);
}

// Mergers

Rectangle merge_horizontally(const Rectangle& rect1, const Rectangle& rect2){
    Rectangle copy = rect1;
    if(!copy.connectUp(rect2)){
        throw std::invalid_argument("Rectangles non-mergable");
    }
    return copy;
}


Rectangle merge_vertically(const Rectangle& rect1, const Rectangle& rect2){
    Rectangle copy = rect1;
    if(!copy.connectRight(rect2)){
        throw std::invalid_argument("Rectangles non-mergable");
    }
    return copy;
}


Rectangle merge_all(const Rectangles& rectangles){
    size_t s = rectangles.size();
    if(s == 0){
        throw std::invalid_argument("Cannot merge empty collection of rectangles");
    }
    Rectangle ret = rectangles[0];
    for(size_t i = 1; i < s; i++){
        if(!(ret.connectUp(rectangles[i]) || ret.connectRight(rectangles[i]))){
            throw std::invalid_argument("Rectangle collection non-mergable");
        }
    }
    return ret;
}
