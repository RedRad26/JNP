#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>
#include <cstdint>
#include <cstdlib>
#include <initializer_list>

#include <iostream>

class Position;
class Vector;
class Rectangle;
class Rectangles;

class AbstractVector2D{
protected:
    int32_t xn, yn;

    AbstractVector2D(int32_t x, int32_t y);

    void add(const AbstractVector2D& vec);

    bool operator==(const AbstractVector2D& vec) const;

    template <class ResultType>
    ResultType reflection() const;

    template <class ResultType>
    ResultType operator+(const AbstractVector2D& vec) const;

public:
    int32_t x() const;
    int32_t y() const;
};

class Position: public AbstractVector2D{
public:
    Position(int32_t x, int32_t y);

    bool operator==(const Position& pos) const;
    Position& operator+=(const Vector& vec);
    Position operator+(const Vector& vec) const;

    Position reflection() const;

    static const Position& origin();

    explicit operator Vector() const;
};

class Vector: public AbstractVector2D{
public:
    Vector(int32_t x, int32_t y);

    bool operator==(const Vector& vec) const;

    Vector& operator+=(const Vector& vec);

    Position operator+(const Position& pos) const;
    Vector operator+(const Vector& vec) const;
    Rectangle operator+(const Rectangle& rect) const;
    Rectangles operator+(const Rectangles& rect) const;

    Vector reflection() const;

    explicit operator Position() const;
};

class Rectangle{
private:
    uint32_t widthn, heightn;
    Position corner;
    
    bool connectRight(const Rectangle& rect);
    bool connectUp(const Rectangle& rect);

public:
    Rectangle(uint32_t width, uint32_t height, const Position& pos);
    Rectangle(uint32_t width, uint32_t height);

    uint32_t width() const;
    uint32_t height() const;
    const Position& pos() const;

    uint64_t area() const;

    Rectangle reflection() const;

    bool operator==(const Rectangle& rect) const;
    Rectangle& operator+=(const Vector& vec);
    Rectangle operator+(const Vector& vec) const;

    friend Rectangle merge_horizontally(const Rectangle& rect1, const Rectangle& rect2);
    friend Rectangle merge_vertically(const Rectangle& rect1, const Rectangle& rect2);
    friend Rectangle merge_all(const Rectangles& rectangles);
};

class Rectangles{
    private:
    std::vector<Rectangle> storage;

    void addRectangle(Rectangle&& rect);

    public:
    Rectangles();
    Rectangles(std::initializer_list<Rectangle> rectangles);

    const Rectangle& operator[](size_t i) const;
    Rectangle& operator[](size_t i);
    
    size_t size() const;

    bool operator==(const Rectangles& rects) const;
    Rectangles& operator+=(const Vector& vec);
    Rectangles operator+(const Vector& vec) const;

};

Rectangle merge_horizontally(const Rectangle& rect1, const Rectangle& rect2);
Rectangle merge_vertically(const Rectangle& rect1, const Rectangle& rect2);
Rectangle merge_all(const Rectangles& rectangles);

std::ostream& operator<<(std::ostream& s, const Position& v);

std::ostream& operator<<(std::ostream& s, const Vector& v);

std::ostream& operator<<(std::ostream& s, const Rectangle& r);

std::ostream& operator<<(std::ostream& s, const Rectangles& rs);

#endif /* GEOMETRY_H */