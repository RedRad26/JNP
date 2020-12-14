#include "geometry.h"
#include <type_traits>
#include <utility>
#include <functional>
#include <limits>
#include <iostream>

#ifdef NDEBUG
#undef NDEBUG
#endif // NDEBUG

#include <cassert>

int main() {
    std::cout << "Starting testing procedure." << std::endl;

    // ------------- TEMPLATE TYPE TRAITS CHECKS -------------

    // Default konstruktory
    assert(!std::is_default_constructible_v<Position>);
    assert(!std::is_default_constructible_v<Vector>);
    assert(!std::is_default_constructible_v<Rectangle>);
    assert(std::is_default_constructible_v<Rectangles>);

    // Copy construction
    assert(std::is_copy_constructible_v<Position>);
    assert(std::is_copy_constructible_v<Vector>);
    assert(std::is_copy_constructible_v<Rectangle>);
    assert(std::is_copy_constructible_v<Rectangles>);

    // Copy assignment
    assert(std::is_copy_assignable_v<Position>);
    assert(std::is_copy_assignable_v<Vector>);
    assert(std::is_copy_assignable_v<Rectangle>);
    assert(std::is_copy_assignable_v<Rectangles>);

    // Move construction
    assert(std::is_move_constructible_v<Rectangles>);

    // Move assignment
    assert(std::is_move_assignable_v<Rectangles>);

    // Destruction
    assert(std::is_destructible_v<Position>);
    assert(std::is_destructible_v<Vector>);
    assert(std::is_destructible_v<Rectangle>);
    assert(std::is_destructible_v<Rectangles>);

    // Dzialanie konstuktora Vector(scalar, scalar).
    Vector vec3{-300, -400};
    Vector vec4{0, 999};

    // Dzialanie x() i y()
    Vector vec5(-333, 444);
    assert(vec5.x() == -333);
    assert(vec5.y() == 444);

    const Vector vec6(-519, 0);
    assert(vec6.x() == -519);
    assert(vec6.y() == 0);

    // Dzialanie reflection
    Vector vec7(813, -129);
    assert(vec7.x() == vec7.reflection().reflection().x());
    assert(vec7.y() == vec7.reflection().reflection().y());

    assert(vec7.reflection().x() == vec7.reflection().x());
    assert(vec7.reflection().y() == vec7.reflection().y());

    const Vector vec8(1, 3);
    assert(vec8.x() == vec8.reflection().reflection().x());
    assert(vec8.y() == vec8.reflection().reflection().y());

    assert(vec8.reflection().x() == vec8.reflection().x());
    assert(vec8.reflection().y() == vec8.reflection().y());

    // Dzialanie ==
    Vector vec11(3, 4);
    Vector vec12(3, 4);

    assert(vec11 == vec12);

    const Vector vec13(3, 4);
    const Vector vec14(3, 4);

    assert(vec13 == vec14);

    assert(vec11 == vec13);
    assert(vec13 == vec11);

    Vector vec15(3, 7);
    Vector vec16(7, 4);

    assert(!(vec11 == vec15));
    assert(!(vec11 == vec16));

    const Vector vec17{-4, -3};

    // Istnienie Vector::Vector(const Vector &)
    Vector vec18(vec17);

    // Istnienie Vector::operator=(const Vector &)
    Vector vec19 = vec18; // Side note: ta linijka to nie operator= tylko wywolanie konstruktora.
    vec19 = vec17;

    Vector vec20{5, 6};
    Vector vec21(6, 9);

    // Czy nie dano przypadkiem Vector::Vector(Vector &&) = delete.
    Vector vec22 = std::move(vec20);

    Vector vec23{-1, -7};

    // Czy nie dano przypadkiem Vector::operator=(Vector &&) = delete.
    vec23 = std::move(vec21);

    // Istnienie Vector::~Vector().
    vec23.~Vector();

// POSITION!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    // Dzialanie konstuktora Position(scalar, scalar).
    Position pos3{-300, -400};
    Position pos4{0, 999};

    // Dzialanie x() i y()
    Position pos5(-333, 444);
    assert(pos5.x() == -333);
    assert(pos5.y() == 444);

    const Position pos6(-519, 0);
    assert(pos6.x() == -519);
    assert(pos6.y() == 0);

    // Dzialanie reflection
    Position pos7(813, -129);
    assert(pos7.x() == pos7.reflection().reflection().x());
    assert(pos7.y() == pos7.reflection().reflection().y());

    assert(pos7.reflection().x() == pos7.reflection().x());
    assert(pos7.reflection().y() == pos7.reflection().y());

    const Position pos8(1, 3);
    assert(pos8.x() == pos8.reflection().reflection().x());
    assert(pos8.y() == pos8.reflection().reflection().y());

    assert(pos8.reflection().x() == pos8.reflection().x());
    assert(pos8.reflection().y() == pos8.reflection().y());

    // Dzialanie Position::origin
    Position pos9 = Position::origin();
    const Position pos10 = Position::origin();

    assert(Position::origin().x() == 0);
    assert(Position::origin().y() == 0);

    // Dzialanie ==
    Position pos11(3, 4);
    Position pos12(3, 4);

    assert(pos11 == pos12);

    const Position pos13(3, 4);
    const Position pos14(3, 4);

    assert(pos13 == pos14);

    assert(pos11 == pos13);
    assert(pos13 == pos11);

    Position pos15(3, 7);
    Position pos16(7, 4);

    assert(!(pos11 == pos15));
    assert(!(pos11 == pos16));

    assert((Position::origin() == Position(0, 0)));

    const Position pos17{-4, -3};

    // Istnienie Position::Position(const Position &)
    Position pos18(pos17);

    // Istnienie Position::operator=(const Position &)
    Position pos19 = pos18; // Side note: ta linijka to nie operator= tylko wywolanie konstruktora.
    pos19 = pos17;

    Position pos20{5, 6};
    Position pos21(6, 9);

    // Czy nie dano przypadkiem Position::Position(Position &&) = delete.
    Position pos22 = std::move(pos20);

    Position pos23{-1, -7};

    // Czy nie dano przypadkiem Position::operator=(Position &&) = delete.
    pos23 = std::move(pos21);

    // Istnienie Position::~Position().
    pos23.~Position();

    // Przesuwanie

    Position pos24{100, 300};
    const Vector vec24(-1, -1);

    pos24 += vec24;

    assert(pos24 == Position(99, 299));

    Vector vec25{15, 37};

    vec25 += vec24;

    assert(vec25 == Vector(14, 36));

    // Przypisywanie

    // Test z tresci
    Position pos26(1, 2);
    Vector vec26(pos26);
    Vector vec27(2, 3);
    Position pos27(vec27);
    pos27 = Position(vec26);
    vec26 = Vector(pos27);

    // Rozne

    Position pos29 = Position(-5, 6);
    Vector vec29 = Vector(-3, 4);

    // Sprawdza czy nie dano czasem consta do typu zwracanego.
    pos29.reflection() = {3, 5};
    vec29.reflection() = {7, 3};

    assert((pos29 == Position{-5, 6}));
    assert((vec29 == Vector{-3, 4}));

    Vector vec30{51, 63};
    Vector vec31{-3, -4};

    vec30 += Vector(0, 0);
    assert(vec30 == vec30);

    auto minScalar = std::numeric_limits<int32_t>::min();
    auto maxScalar = std::numeric_limits<int32_t>::max();

    Position pos32{minScalar, maxScalar};
    Vector vec32{minScalar, maxScalar};

    assert(pos32.x() == minScalar);
    assert(pos32.y() == maxScalar);

    assert(vec32.x() == minScalar);
    assert(vec32.y() == maxScalar);

    Vector vec33{-215, 341};
    assert(vec33.reflection() == Vector(341, -215));

    Position pos33{-215, 341};
    assert(pos33.reflection() == Position(341, -215));

// ------------------- RECTANGLE -------------------

    // UWAGA: Zakladam reprezentacje
    // scalar width, scalar height, Position pos
    // Moim zdaniem, tresc wymaga takiej reprezentacji, ze wzgledu na
    // gettery width(), height(), pos() oraz zwyczajne reusability.

    Rectangle rec7{300, 400};
    const Rectangle rec8{500, 600, {-2561, -39143}};

    assert(rec7.width() == 300);
    assert(rec7.height() == 400);
    assert(rec7.pos() == Position::origin());

    assert(rec8.width() == 500);
    assert(rec8.height() == 600);
    assert(rec8.pos() == Position(-2561, -39143));

    assert(rec7.area() == 300 * 400);
    assert(rec8.area() == 500 * 600);

    assert(rec7.reflection() == Rectangle(400, 300, {0, 0}));
    assert(rec8.reflection() == Rectangle(600, 500, {-39143, -2561}));

    rec8.reflection() = Rectangle{1, 2};

    const Rectangle rec9{300, 400};
    Rectangle rec10{500, 600, {-2561, -39143}};

    assert(rec7 == rec7);
    assert(rec8 == rec8);

    assert(rec7 == rec9);
    assert(rec8 == rec10);

    assert(!(rec7 == rec10));
    assert(!(rec8 == rec9));

    assert(!(Rectangle(3, 7, {2, 1}) == Rectangle(3, 7)));
    assert(!(Rectangle(3, 7, {2, 1}) == Rectangle(3, 7, {2, 0})));
    assert(!(Rectangle(3, 7, {2, 1}) == Rectangle(3, 7, {0, 1})));
    assert(!(Rectangle(3, 7, {2, 1}) == Rectangle(7, 7, {2, 1})));
    assert(!(Rectangle(3, 7, {2, 1}) == Rectangle(3, 3, {2, 1})));

    const Rectangle rec11(3, 7, {3, 3});
    assert(rec11.reflection() == Rectangle(7, 3, {3, 3}));

    assert(Rectangle(45, 37, {1, 2}).reflection() == Rectangle(37, 45, {2, 1}));
    assert(Rectangle(45, 37, {-1, -2}).reflection() == Rectangle(37, 45, {-2, -1}));
    assert(Rectangle(4519, 37431, {-314, -1938}).reflection() == Rectangle(37431, 4519, {-1938, -314}));

    Rectangle rec12{5, 6, {-2, -1}};
    rec12 += {2, 1};
    assert(rec12 == Rectangle(5, 6));

    Rectangle rec20{5, 6, {1, -3}};
    Rectangle rec21{6, 9, {-4, 0}};

    // Czy nie dano przypadkiem Rectangle::Rectangle(Rectangle &&) = delete.
    Rectangle rec22 = std::move(rec20);

    Rectangle rec23{1, 7};

    // Czy nie dano przypadkiem Rectangle::operator=(Rectangle &&) = delete.
    rec23 = std::move(rec21);

    // Istnienie Rectangle::~Rectangle().
    rec23.~Rectangle();

// ------------------- RECTANGLES -------------------

    // Ma istniec konstruktor domyslny
    Rectangles recs1;
    const Rectangles recs2{};

    assert(recs1.size() == 0);
    assert(recs2.size() == 0);

    const Rectangle rec101(3, 5);
    const Rectangle rec102(26, 31, {-31, -52});
    const Rectangle rec103(591, 411, {15, 9});

    // Sprawdza czy wykorzystano initializer_list w konstruktorze.
    Rectangles recs3{rec101, rec102, rec103};

    const Rectangles recs4({rec101, rec102, rec103});

    assert(recs3.size() == 3);
    assert(recs3.size() == 3);

    assert(recs3 == recs4);

    assert(!(recs1 == recs3));

    recs3[0] = rec102;
    assert(recs4[0].area() == 3 * 5);

    recs3 += Vector(1, 1);

    // Przenoszenie i kopiowanie

    // Konstruktor kopiujacy.
    Rectangles recs5 = recs4;

    // operator= kopiujacy.
    Rectangles recs6{rec103, rec102};
    recs6 = recs4;

    // Konstruktor przenoszacy
    Rectangles recs7{rec103, rec102};
    Rectangles recs8 = std::move(recs7);

    // operator= przenoszacy
    Rectangles recs9{rec103, rec102};
    Rectangles recs10{rec101};
    recs10 = std::move(recs9);

// ------------ VARIOUS ---------------

    // Dodawanie
    Position p{3, 5};
    const Position cp{-4, -9};

    Rectangle r{9, 7, {-61, -13}};
    const Rectangle cr{15, 12, {17, -21}};

    Rectangles rs{Rectangle(8, 1, {-4, 8}),
                  Rectangle(71, 23, {5, 6}),
                  Rectangle(9, 15, {43, 12})};

    const Rectangles crs{Rectangle(8, 1, {-4, 8}),
                         Rectangle(71, 23, {5, 6}),
                         Rectangle(9, 15, {43, 12})};

    Vector v{-21, 34};
    const Vector cv{12, 5};

    p + v;
    cp + v;
    p + cv;
    cp + cv;

    v + p;
    v + cp;
    cv + p;
    cv + cp;

    v + v;
    cv + v;
    cv + cv;

    r + v;
    cr + v;
    r + cv;
    cr + cv;

    v + r;
    cv + r;
    v + cr;
    cv + cr;

    rs + v;
    crs + v;
    rs + cv;
    crs + cv;

    Rectangles tm1 = rs;
    Rectangles tm2 = crs;
    Rectangles tm3 = rs;
    Rectangles tm4 = crs;

    std::move(tm1) + cv;
    cv + std::move(tm2);
    std::move(tm3) + v;
    v + std::move(tm4);

// ------------- MERGE -------------

    const Rectangle mr1{4, 5, {3, 7}};
    const Rectangle mr2{4, 1, {3, 12}};

    assert(merge_horizontally(mr1, mr2) == Rectangle(4, 6, {3, 7}));

    const Rectangle mr3{4, 1, {3, 13}};
    const Rectangle mr4{4, 1, {4, 13}};

    const Rectangle mr5{4, 5};
    const Rectangle mr6{1, 5, {4, 0}};

    assert(merge_vertically(mr5, mr6) == Rectangle(5, 5, {0, 0}));

    const Rectangle mr7{1, 5, {4, 1}};

    const Rectangle mr8{1, 5, {5, 1}};

    Rectangle ret_all_1 = merge_all({Rectangle(2, 1),
                                     Rectangle(2, 1, {0, 1}),
                                     Rectangle(2, 2, {2, 0}),
                                     Rectangle(4, 2, {0, 2}),
                                     Rectangle(2, 4, {4, 0}),
                                     Rectangle(6, 1, {0, 4})});

    const Rectangle mr9{3, 4, {-105, -213}};

    const Rectangle mr10{3, 2, {-105, -209}};
    assert(merge_horizontally(mr9, mr10) == Rectangle(3, 6, {-105, -213}));

    const Rectangle mr11(2, 4, {-102, -213});
    assert(merge_vertically(mr9, mr11) == Rectangle(5, 4, {-105, -213}));

    assert(ret_all_1 == Rectangle(6, 5));

    // Czy typ zwracany przez origin() zawiera consta.
    assert(std::is_const_v<std::remove_reference_t<decltype(Position::origin())>>);

    // Position& Position::operator+=(const Vector&)
    assert((std::is_same_v<std::invoke_result_t<decltype(&Position::operator+=), Position, const Vector &>, Position &>));

    // Vector& Vector::operator+=(const Vector&)
    assert((std::is_same_v<std::invoke_result_t<decltype(&Vector::operator+=), Vector, const Vector &>, Vector &>));

    // Rectangle& Rectangle::operator+=(const Vector&)
    assert((std::is_same_v<std::invoke_result_t<decltype(&Rectangle::operator+=), Rectangle, const Vector &>, Rectangle &>));

    // Rectangles& Rectangles::operator+=(const Vector&)
    assert((std::is_same_v<std::invoke_result_t<decltype(&Rectangles::operator+=), Rectangles, const Vector &>, Rectangles &>));

    // DNC: pos27 = vec26;
    // DNC: vec26 = pos27;
    // DNC: Position pos28 = vec27;
    // DNC: Vector vec28 = pos27;
    // DNC: recs4[0] = rec102;
    // DNC: Rectangle &recRef1 = recs4[0];

    // DNR: recs3[3];
    // DNR: recs4[3];
    // DNR: Rectangle rec3{0, 0};
    // DNR: Rectangle rec4{3, 0};
    // DNR: Rectangle rec5{0, 5};
    // DNR: Rectangle rec6{-100, -100};
     DNR: Rectangle rec300(-100, 200);
    // DNR: Rectangle rec301(100, -200);
    // DNR: merge_horizontally(mr1, mr3);
    // DNR: merge_horizontally(mr1, mr4);
    // DNR: merge_vertically(mr5, mr7);
    // DNR: merge_vertically(mr5, mr8);

    /* DNR: Rectangle ret_all_2 = merge_all({Rectangle(2, 1),
                                     Rectangle(2, 1, {0, 1}),
                                     Rectangle(2, 2, {2, 0}),
                                     Rectangle(4, 2, {0, 2}),
                                     Rectangle(2, 4, {3, 0}),
                                     Rectangle(6, 1, {0, 4})}); */

    std::cout << "All tests passed!" << std::endl;
}