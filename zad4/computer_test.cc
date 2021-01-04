#include "computer.h"

#include <cassert>
#include <cstdint>
#include <array>

namespace {
    // Operator == dla std::array jest constexpr dopiero od C++20.
    template<typename T, std::size_t N>
    constexpr bool compare(std::array<T, N> const& arg1, std::array<T, N> const& arg2) {
        for (std::size_t i = 0; i < N; ++i)
            if (arg1[i] != arg2[i]) return false;
        return true;
    }
};

// Testy 1xx: przykład.
// Testy 2xx: D, Id, Num, duże stałe, rzutowanie do typu słowa komputera.
// Testy 3xx: Mov, Mem, Lea.
// Testy 4xx: Add, Sub, Inc, Dec, obliczenia w typie słowa komputera.
// Testy 5xx: And, Or, Not.
// Testy 6xx: Label, Cmp, Jmp, Jz, Js.

int main() {

#if TEST_NUM == 101
    struct test {
        using tmpasm_move = Program<
                Mov<Mem<Num<0>>, Num<42>>>;

        using tmpasm_jump = Program<
                Inc<Mem<Num<0>>>,
                Jmp<Id("stop")>,
                Inc<Mem<Num<0>>>,
                Label<Id("stop")>>;

        using tmpasm_data = Program<
                Inc<Mem<Lea<Id("a")>>>,
                D<Id("a"), Num<0>>,
                D<Id("b"), Num<2>>,
                D<Id("c"), Num<3>>>;

        using tmpasm_operations = Program<
                D<Id("a"), Num<4>>,
                D<Id("b"), Num<3>>,
                D<Id("c"), Num<2>>,
                D<Id("d"), Num<1>>,
                Add<Mem<Lea<Id("a")>>, Mem<Lea<Id("c")>>>,
                Sub<Mem<Lea<Id("b")>>, Mem<Lea<Id("d")>>>,
                Mov<Mem<Lea<Id("c")>>, Num<0>>,
                Mov<Mem<Lea<Id("d")>>, Num<0>>>;

        using tmpasm_helloworld = Program<
                Mov<Mem<Mem<Num<10>>>, Num<'h'>>,
                Inc<Mem<Num<10>>>,
                Mov<Mem<Mem<Num<10>>>, Num<'e'>>,
                Inc<Mem<Num<10>>>,
                Mov<Mem<Mem<Num<10>>>, Num<'l'>>,
                Inc<Mem<Num<10>>>,
                Mov<Mem<Mem<Num<10>>>, Num<'l'>>,
                Inc<Mem<Num<10>>>,
                Mov<Mem<Mem<Num<10>>>, Num<'o'>>,
                Inc<Mem<Num<10>>>,
                Mov<Mem<Mem<Num<10>>>, Num<' '>>,
                Inc<Mem<Num<10>>>,
                Mov<Mem<Mem<Num<10>>>, Num<'w'>>,
                Inc<Mem<Num<10>>>,
                Mov<Mem<Mem<Num<10>>>, Num<'o'>>,
                Inc<Mem<Num<10>>>,
                Mov<Mem<Mem<Num<10>>>, Num<'r'>>,
                Inc<Mem<Num<10>>>,
                Mov<Mem<Mem<Num<10>>>, Num<'l'>>,
                Inc<Mem<Num<10>>>,
                Mov<Mem<Mem<Num<10>>>, Num<'d'>>>;
    };

    static_assert(compare(
            Computer<1, int>::boot<test::tmpasm_move>(),
            std::array<int, 1>({42})),
                  "Failed [tmpasp_move].");

    static_assert(compare(
            Computer<1, int>::boot<test::tmpasm_jump>(),
            std::array<int, 1>({1})),
                  "Failed [tmpasp_jump].");

    static_assert(compare(
            Computer<4, uint32_t>::boot<test::tmpasm_data>(),
            std::array<uint32_t, 4>({1, 2, 3, 0})),
                  "Failed [tmpasp_data].");

    static_assert(compare(
            Computer<5, int64_t>::boot<test::tmpasm_operations>(),
            std::array<int64_t, 5>({6, 2, 0, 0, 0})),
                  "Failed [tmpasp_operations].");

    static_assert(compare(
            Computer<11, char>::boot<test::tmpasm_helloworld>(),
            std::array<char, 11>({'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'})),
                  "Failed [tmpasm_helloworld].");
#endif


#if TEST_NUM == 201
    static_assert(Id("abcdef") == Id("ABCDEF"));
    static_assert(Id("0") == Id("0"));
    static_assert(Id("0") != Id("000"));
    static_assert(Id("012345") == Id("012345"));
    static_assert(Id("zz") == Id("ZZ"));
    static_assert(Id("a") != Id("aa"));
    static_assert(Id("A") != Id("AA"));
    static_assert(Id("z") != Id("zz"));
    static_assert(Id("Z") != Id("ZZ"));
    static_assert(Id("9") != Id("Z"));
#endif

#if TEST_NUM == 202
    struct test {
        using tmpasm_neg1 = Program<D<Id("a"), Num<-1>>>;
        using tmpasm_129 = Program<D<Id("a"), Num<129>>>;
        using tmpasm_255 = Program<D<Id("a"), Num<255>>>;
        using tmpasm_256 = Program<D<Id("a"), Num<256>>>;
        using tmpasm_max = Program<D<Id("a"), Num<UINT64_MAX>>>;
        using tmpasm_min = Program<D<Id("a"), Num<INT64_MIN>>>;
    };

    static_assert(compare(
            Computer<1, int8_t>::boot<test::tmpasm_neg1>(),
            std::array<int8_t, 1>({-1})));

    static_assert(compare(
            Computer<1, uint8_t>::boot<test::tmpasm_neg1>(),
            std::array<uint8_t, 1>({255})));

    static_assert(compare(
            Computer<1, int8_t>::boot<test::tmpasm_129>(),
            std::array<int8_t, 1>({-127})));

    static_assert(compare(
            Computer<1, uint8_t>::boot<test::tmpasm_129>(),
            std::array<uint8_t, 1>({129})));

    static_assert(compare(
            Computer<1, int8_t>::boot<test::tmpasm_255>(),
            std::array<int8_t, 1>({-1})));

    static_assert(compare(
            Computer<1, uint8_t>::boot<test::tmpasm_255>(),
            std::array<uint8_t, 1>({255})));

    static_assert(compare(
            Computer<1, int8_t>::boot<test::tmpasm_256>(),
            std::array<int8_t, 1>({0})));

    static_assert(compare(
            Computer<1, uint8_t>::boot<test::tmpasm_256>(),
            std::array<uint8_t, 1>({0})));

    static_assert(compare(
            Computer<1, int16_t>::boot<test::tmpasm_256>(),
            std::array<int16_t, 1>({256})));

    static_assert(compare(
            Computer<1, int32_t>::boot<test::tmpasm_256>(),
            std::array<int32_t, 1>({256})));

    static_assert(compare(
            Computer<1, int64_t>::boot<test::tmpasm_256>(),
            std::array<int64_t, 1>({256})));

    static_assert(compare(
            Computer<1, uint8_t>::boot<test::tmpasm_max>(),
            std::array<uint8_t, 1>({UINT8_MAX})));

    static_assert(compare(
            Computer<1, int8_t>::boot<test::tmpasm_max>(),
            std::array<int8_t, 1>({-1})));

    static_assert(compare(
            Computer<1, uint16_t>::boot<test::tmpasm_max>(),
            std::array<uint16_t, 1>({UINT16_MAX})));

    static_assert(compare(
            Computer<1, uint32_t>::boot<test::tmpasm_max>(),
            std::array<uint32_t, 1>({UINT32_MAX})));

    static_assert(compare(
            Computer<1, uint64_t>::boot<test::tmpasm_max>(),
            std::array<uint64_t, 1>({UINT64_MAX})));

    static_assert(compare(
            Computer<1, int64_t>::boot<test::tmpasm_min>(),
            std::array<int64_t, 1>({INT64_MIN})));

    static_assert(compare(
            Computer<1, uint64_t>::boot<test::tmpasm_min>(),
            std::array<uint64_t, 1>({9223372036854775808U})));

    static_assert(compare(
            Computer<1, int16_t>::boot<test::tmpasm_min>(),
            std::array<int16_t, 1>({0})));

    static_assert(compare(
            Computer<1, uint16_t>::boot<test::tmpasm_min>(),
            std::array<uint16_t, 1>({0})));
#endif

#if TEST_NUM == 203
    struct test {
        using tmpasm_1var = Program<D<Id("a"), Num<42>>>;
        using tmpasm_10diffvars = Program<
                D<Id("a1"), Num<1>>,
                D<Id("a2"), Num<2>>,
                D<Id("a3"), Num<3>>,
                D<Id("a4"), Num<4>>,
                D<Id("a5"), Num<5>>,
                D<Id("a6"), Num<6>>,
                D<Id("a7"), Num<7>>,
                D<Id("a8"), Num<8>>,
                D<Id("a9"), Num<9>>,
                D<Id("a10"), Num<10>>>;
        using tmpasm_10samevars = Program<
                D<Id("a"), Num<42>>,
                D<Id("a"), Num<42>>,
                D<Id("a"), Num<42>>,
                D<Id("a"), Num<42>>,
                D<Id("a"), Num<42>>,
                D<Id("a"), Num<42>>,
                D<Id("a"), Num<42>>,
                D<Id("a"), Num<42>>,
                D<Id("a"), Num<42>>,
                D<Id("a"), Num<42>>>;
    };

    static_assert(compare(
            Computer<11, int>::boot<test::tmpasm_1var>(),
            std::array<int, 11>({42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})));

    static_assert(compare(
            Computer<11, int>::boot<test::tmpasm_10diffvars>(),
            std::array<int, 11>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 0})));

    static_assert(Computer<11, int>::boot<test::tmpasm_10samevars>()[10] == 0);

    static_assert(compare(
            Computer<1, int>::boot<test::tmpasm_1var>(),
            std::array<int, 1>({42})));

    static_assert(compare(
            Computer<10, int>::boot<test::tmpasm_10diffvars>(),
            std::array<int, 10>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10})));
#endif

// Niepoprawne identyfikatory zmiennych - nie powinny się kompilować.
#if TEST_NUM == 204
    constexpr auto error = Computer<11, int>::boot<Program<D<Id(""), Num<42>>>>();
#endif

#if TEST_NUM == 205
    constexpr auto error = Computer<11, int>::boot<Program<D<Id("abcdefg"), Num<42>>>>();
#endif

#if TEST_NUM == 206
    constexpr auto error = Computer<11, int>::boot<Program<D<Id("0123456"), Num<42>>>>();
#endif

#if TEST_NUM == 207
    constexpr auto error = Computer<11, int>::boot<Program<D<Id("a!"), Num<42>>>>();
#endif

#if TEST_NUM == 208
    constexpr auto error = Computer<11, int>::boot<Program<D<Id("a@"), Num<42>>>>();
#endif

#if TEST_NUM == 209
    constexpr auto error = Computer<11, int>::boot<Program<D<Id("a#"), Num<42>>>>();
#endif

#if TEST_NUM == 210
    constexpr auto error = Computer<11, int>::boot<Program<D<Id("a$"), Num<42>>>>();
#endif

#if TEST_NUM == 211
    constexpr auto error = Computer<11, int>::boot<Program<D<Id("a%"), Num<42>>>>();
#endif

#if TEST_NUM == 212
    constexpr auto error = Computer<11, int>::boot<Program<D<Id("a^"), Num<42>>>>();
#endif

#if TEST_NUM == 213
    constexpr auto error = Computer<11, int>::boot<Program<D<Id("a&"), Num<42>>>>();
#endif

#if TEST_NUM == 214
    constexpr auto error = Computer<11, int>::boot<Program<D<Id("a*"), Num<42>>>>();
#endif

#if TEST_NUM == 215
    constexpr auto error = Computer<11, int>::boot<Program<D<Id("a("), Num<42>>>>();
#endif

#if TEST_NUM == 216
    constexpr auto error = Computer<11, int>::boot<Program<D<Id("a "), Num<42>>>>();
#endif

#if TEST_NUM == 217
    constexpr auto error = Computer<11, int>::boot<Program<D<Id("a_"), Num<42>>>>();
#endif

#if TEST_NUM == 218
    constexpr auto error = Computer<11, int>::boot<Program<D<Id("a-"), Num<42>>>>();
#endif

#if TEST_NUM == 219
    constexpr auto error = Computer<11, int>::boot<Program<D<Id("a+"), Num<42>>>>();
#endif

#if TEST_NUM == 220
    constexpr auto error = Computer<11, int>::boot<Program<D<Id("a."), Num<42>>>>();
#endif

#if TEST_NUM == 221
    constexpr auto error = Computer<11, int>::boot<Program<D<Id("a,"), Num<42>>>>();
#endif

// Niepoprawne Num oraz D - nie powinny się kompilować.
#if TEST_NUM == 222
    constexpr auto error = Computer<11, int>::boot<Program<D<Id("a"), Num<>>>>();
#endif

#if TEST_NUM == 223
    constexpr auto error = Computer<11, int>::boot<Program<D<Id("a"), Num<Num<1>>>>>();
#endif

#if TEST_NUM == 224
    constexpr auto error = Computer<11, int>::boot<Program<D<Id("a"), Lea<Id("a")>>>>();
#endif

#if TEST_NUM == 225
    constexpr auto error = Computer<11, int>::boot<Program<D<Id("a"), Mem<Num<0>>>>>();
#endif

// Przekroczenie rozmiaru pamięci - nie powinno się kompilować.
#if TEST_NUM == 226
    struct test {
        using tmpasm_10diffvars = Program<
                D<Id("a1"), Num<1>>,
                D<Id("a2"), Num<2>>,
                D<Id("a3"), Num<3>>,
                D<Id("a4"), Num<4>>,
                D<Id("a5"), Num<5>>,
                D<Id("a6"), Num<6>>,
                D<Id("a7"), Num<7>>,
                D<Id("a8"), Num<8>>,
                D<Id("a9"), Num<9>>,
                D<Id("a10"), Num<10>>>;
    };

    constexpr auto error = Computer<9, int>::boot<test::tmpasm_10diffvars>();
#endif

#if TEST_NUM == 301
    struct test {
        using tmpasm_move_mem = Program<
                Mov<Mem<Num<1>>, Num<2>>,
                Mov<Mem<Num<2>>, Num<9>>,
                Mov<Mem<Num<3>>, Mem<Num<1>>>,
                Mov<Mem<Num<4>>, Mem<Num<2>>>>;
        using tmpasm_move_mem_mem = Program<
                Mov<Mem<Num<0>>, Num<1>>,
                Mov<Mem<Num<1>>, Num<2>>,
                Mov<Mem<Num<2>>, Num<9>>,
                Mov<Mem<Num<3>>, Mem<Num<1>>>,
                Mov<Mem<Num<4>>, Mem<Num<2>>>,
                Mov<Mem<Num<5>>, Mem<Mem<Num<0>>>>,
                Mov<Mem<Mem<Num<0>>>, Num<42>>>;
    };

    static_assert(compare(
            Computer<8, int>::boot<test::tmpasm_move_mem>(),
            std::array<int, 8>({0, 2, 9, 2, 9, 0, 0, 0})));

    static_assert(compare(
            Computer<8, int>::boot<test::tmpasm_move_mem_mem>(),
            std::array<int, 8>({1, 42, 9, 2, 9, 2, 0, 0})));
#endif

#if TEST_NUM == 302
    struct test {
        using tmpasm_move_lea_1var = Program<
                D<Id("a"), Num<42>>,
                Mov<Mem<Num<1>>, Lea<Id("a")>>,
                Mov<Mem<Num<2>>, Mem<Lea<Id("a")>>>,
                Mov<Mem<Lea<Id("a")>>, Num<5>>>;
        using tmpasm_move_lea_3vars_read = Program<
                Mov<Mem<Num<5>>, Mem<Lea<Id("a")>>>,
                Mov<Mem<Num<6>>, Mem<Lea<Id("b")>>>,
                Mov<Mem<Num<7>>, Mem<Lea<Id("c")>>>,
                D<Id("a"), Num<7>>,
                D<Id("b"), Num<8>>,
                D<Id("c"), Num<9>>>;
        using tmpasm_move_lea_3vars_write = Program<
                Mov<Mem<Num<5>>, Mem<Lea<Id("a")>>>,
                D<Id("a"), Num<7>>,
                Mov<Mem<Num<6>>, Mem<Lea<Id("b")>>>,
                D<Id("b"), Num<8>>,
                Mov<Mem<Num<7>>, Mem<Lea<Id("c")>>>,
                Mov<Mem<Lea<Id("c")>>, Num<13>>,
                Mov<Mem<Lea<Id("b")>>, Num<14>>,
                D<Id("c"), Num<9>>,
                Mov<Mem<Lea<Id("a")>>, Num<14>>,
                Mov<Mem<Lea<Id("a")>>, Num<15>>>;
    };

    static_assert(compare(
            Computer<3, int>::boot<test::tmpasm_move_lea_1var>(),
            std::array<int, 3>({5, 0, 42})));

    static_assert(compare(
            Computer<8, int>::boot<test::tmpasm_move_lea_3vars_read>(),
            std::array<int, 8>({7, 8, 9, 0, 0, 7, 8, 9})));

    static_assert(compare(
            Computer<8, int>::boot<test::tmpasm_move_lea_3vars_write>(),
            std::array<int, 8>({15, 14, 13, 0, 0, 7, 8, 9})));
#endif

// Niepoprawne Mov, Mem, Lea - nie powinno się kompilować.
#if TEST_NUM == 303
    constexpr auto error = Computer<11, int>::boot<Program< Mov<Num<1>, Num<2>> >>();
#endif

#if TEST_NUM == 304
    constexpr auto error = Computer<11, int>::boot<Program< Mov<Lea<Id("a")>, Num<2>> >>();
#endif

#if TEST_NUM == 305
    constexpr auto error = Computer<11, int>::boot<Program< Mov<Id("a"), Num<2>> >>();
#endif

#if TEST_NUM == 306
    constexpr auto error = Computer<11, int>::boot<Program< Mov<Mem<Num<7>>, Id("a")> >>();
#endif

#if TEST_NUM == 307
    constexpr auto error = Computer<11, int>::boot<Program< Mov<Mem<Num<7>>, Lea<Num<2>>> >>();
#endif

#if TEST_NUM == 308
    constexpr auto error = Computer<11, int>::boot<Program< Mov<Mem<7>, Num<2>> >>();
#endif

// Odwołanie poza pamięć - nie powinno się kompilować.
#if TEST_NUM == 309
    constexpr auto error = Computer<11, int>::boot<Program< Mov<Mem<Num<100>>, Num<2>> >>();
#endif

#if TEST_NUM == 401
    struct test {
        using tmpasm_add_inc = Program<
                D<Id("a"), Num<3>>,
                D<Id("b"), Num<3>>,
                D<Id("c"), Num<3>>,
                D<Id("d"), Num<3>>,
                D<Id("e"), Num<3>>,
                Add<Mem<Num<0>>, Num<2>>,
                Add<Mem<Num<1>>, Mem<Num<4>>>,
                Add<Mem<Num<2>>, Lea<Id("e")>>,
                Inc<Mem<Lea<Id("e")>>>>;
        using tmpasm_sub_dec = Program<
                D<Id("a"), Num<3>>,
                D<Id("b"), Num<3>>,
                D<Id("c"), Num<3>>,
                D<Id("d"), Num<3>>,
                D<Id("e"), Num<3>>,
                Sub<Mem<Num<0>>, Num<2>>,
                Sub<Mem<Num<1>>, Mem<Num<4>>>,
                Sub<Mem<Num<2>>, Lea<Id("e")>>,
                Dec<Mem<Lea<Id("e")>>>>;
        using tmpasm_mixed = Program<
                D<Id("a"), Num<3>>,
                D<Id("b"), Num<3>>,
                D<Id("c"), Num<3>>,
                D<Id("d"), Num<3>>,
                D<Id("e"), Num<3>>,
                Add<Mem<Num<0>>, Num<2>>,
                Sub<Mem<Num<1>>, Mem<Num<4>>>,
                Add<Mem<Num<2>>, Mem<Num<4>>>,
                Sub<Mem<Num<2>>, Lea<Id("e")>>,
                Inc<Mem<Lea<Id("e")>>>,
                Dec<Mem<Lea<Id("e")>>>>;
        using tmpasm_min_max = Program<
                D<Id("a"), Num<127>>,
                D<Id("b"), Num<-128>>,
                D<Id("c"), Num<255>>,
                D<Id("d"), Num<0>>,
                D<Id("e"), Num<100>>,
                D<Id("f"), Num<100>>,
                Add<Mem<Num<0>>, Num<1>>,
                Sub<Mem<Num<1>>, Num<1>>,
                Add<Mem<Num<2>>, Num<1>>,
                Sub<Mem<Num<3>>, Num<1>>,
                Add<Mem<Num<4>>, Num<29>>,
                Add<Mem<Num<5>>, Num<156>>>;
    };

    static_assert(compare(
            Computer<8, int>::boot<test::tmpasm_add_inc>(),
            std::array<int, 8>({5, 6, 7, 3, 4, 0, 0, 0})));

    static_assert(compare(
            Computer<8, int>::boot<test::tmpasm_sub_dec>(),
            std::array<int, 8>({1, 0, -1, 3, 2, 0, 0, 0})));

    static_assert(compare(
            Computer<8, int>::boot<test::tmpasm_mixed>(),
            std::array<int, 8>({5, 0, 2, 3, 3, 0, 0, 0})));

    static_assert(compare(
            Computer<6, int>::boot<test::tmpasm_min_max>(),
            std::array<int, 6>({128, -129, 256, -1, 129, 256})));

    static_assert(compare(
            Computer<6, int8_t>::boot<test::tmpasm_min_max>(),
            std::array<int8_t, 6>({-128, 127, 0, -1, -127, 0})));

    static_assert(compare(
            Computer<6, uint8_t>::boot<test::tmpasm_min_max>(),
            std::array<uint8_t, 6>({128, 127, 0, 255, 129, 0})));
#endif

//  Niepoprawne Add, Sub, Inc, Dec - nie powinny się kompilować.
#if TEST_NUM == 402
    constexpr auto error = Computer<11, int>::boot<Program< Add<Num<1>, Num<2>> >>();
#endif

#if TEST_NUM == 403
    constexpr auto error = Computer<11, int>::boot<Program< Add<Lea<Id("a")>, Num<2>> >>();
#endif

#if TEST_NUM == 404
    constexpr auto error = Computer<11, int>::boot<Program< Sub<Num<1>, Num<2>> >>();
#endif

#if TEST_NUM == 405
    constexpr auto error = Computer<11, int>::boot<Program< Sub<Lea<Id("a")>, Num<2>> >>();
#endif

#if TEST_NUM == 406
    constexpr auto error = Computer<11, int>::boot<Program< Dec<Lea<Id("a")>> >>();
#endif

#if TEST_NUM == 407
    constexpr auto error = Computer<11, int>::boot<Program< Inc<Lea<Id("a")>> >>();
#endif

#if TEST_NUM == 408
    constexpr auto error = Computer<11, int>::boot<Program< Dec<Num<0>> >>();
#endif

#if TEST_NUM == 409
    constexpr auto error = Computer<11, int>::boot<Program< Inc<Num<0>> >>();
#endif

#if TEST_NUM == 501
    struct test {
        using tmpasm_and = Program<
                D<Id("a"), Num<0xff>>,
                D<Id("b"), Num<0xff>>,
                D<Id("c"), Num<0xf0>>,
                And<Mem<Num<0>>, Num<1>>,
                And<Mem<Num<1>>, Num<13>>,
                And<Mem<Num<2>>, Num<0x0f>>>;
        using tmpasm_or = Program<
                D<Id("a"), Num<0>>,
                D<Id("b"), Num<1>>,
                D<Id("c"), Num<0xf0>>,
                Or<Mem<Num<0>>, Num<1>>,
                Or<Mem<Num<1>>, Num<13>>,
                Or<Mem<Num<2>>, Num<0x0f>>>;
        using tmpasm_not = Program<
                D<Id("a"), Num<1>>,
                D<Id("b"), Num<0xff>>,
                D<Id("c"), Num<0xf0>>,
                Not<Mem<Num<0>>>,
                Not<Mem<Num<1>>>,
                Not<Mem<Num<2>>>>;
    };

    static_assert(compare(
            Computer<3, int>::boot<test::tmpasm_and>(),
            std::array<int, 3>({1, 13, 0})));

    static_assert(compare(
            Computer<3, int>::boot<test::tmpasm_or>(),
            std::array<int, 3>({1, 13, 0xff})));

    static_assert(compare(
            Computer<3, uint8_t >::boot<test::tmpasm_not>(),
            std::array<uint8_t, 3>({0xff - 1, 0, 0x0f})));

    static_assert(compare(
            Computer<3, uint16_t >::boot<test::tmpasm_not>(),
            std::array<uint16_t, 3>({0xffff - 1, 0xff00, 0xff0f})));
#endif

// Niepoprawne And, Or, Not - nie powinno się kompilować.
#if TEST_NUM == 502
    constexpr auto error = Computer<11, int>::boot<Program< And<Num<1>, Num<2>> >>();
#endif

#if TEST_NUM == 503
    constexpr auto error = Computer<11, int>::boot<Program< And<Lea<Id("a")>, Num<2>> >>();
#endif

#if TEST_NUM == 504
    constexpr auto error = Computer<11, int>::boot<Program< Or<Num<1>, Num<2>> >>();
#endif

#if TEST_NUM == 505
    constexpr auto error = Computer<11, int>::boot<Program< Or<Lea<Id("a")>, Num<2>> >>();
#endif

#if TEST_NUM == 506
    constexpr auto error = Computer<11, int>::boot<Program< Not<Lea<Id("a")>> >>();
#endif

#if TEST_NUM == 507
    constexpr auto error = Computer<11, int>::boot<Program< Not<Num<0>> >>();
#endif

#if TEST_NUM == 601
    struct test {
        using tmpasm_jmp = Program<
                Label<Id("l1")>,
                Inc<Mem<Num<0>>>,
                Jmp<Id("l3")>,
                Inc<Mem<Num<0>>>,
                Label<Id("l2")>,
                Inc<Mem<Num<0>>>,
                Label<Id("l3")>>;
        using tmpasm_label_var = Program<
                D<Id("a"), Num<0>>,
                Label<Id("b")>,
                Jmp<Id("a")>,
                Inc<Mem<Num<0>>>,
                Label<Id("a")>>;
        using tmpasm_same = Program<
                Label<Id("a")>,
                D<Id("a"), Num<0>>,
                Inc<Mem<Lea<Id("a")>>>>;
    };

    static_assert(compare(
            Computer<1, int>::boot<test::tmpasm_jmp>(),
            std::array<int, 1>({1})));

    static_assert(compare(
            Computer<1, int8_t>::boot<test::tmpasm_label_var>(),
            std::array<int8_t, 1>({0})));

    static_assert(compare(
            Computer<1, int8_t>::boot<test::tmpasm_same>(),
            std::array<int8_t, 1>({1})));
#endif

#if TEST_NUM == 602
    struct test {
        using tmpasm_cmp_jz = Program<
                Cmp<Num<1>, Num<1>>,
                Jz<Id("then")>,
                Jmp<Id("else")>,
                Label<Id("then")>,
                Inc<Mem<Num<0>>>,
                Jmp<Id("end")>,
                Label<Id("else")>,
                Dec<Mem<Num<0>>>,
                Label<Id("end")>>;
        using tmpasm_arithm_jz = Program<
                D<Id("a"), Num<1>>,
                Add<Mem<Num<0>>, Num<-1>>,
                Jz<Id("end")>,
                Inc<Mem<Num<0>>>,
                Label<Id("end")>>;
        using tmpasm_log_jz = Program<
                And<Mem<Num<0>>, Num<1>>,
                Jz<Id("end")>,
                Inc<Mem<Num<0>>>,
                Label<Id("end")>>;
    };

    static_assert(compare(
            Computer<1, int>::boot<test::tmpasm_cmp_jz>(),
            std::array<int, 1>({1})));

    static_assert(compare(
            Computer<1, int>::boot<test::tmpasm_arithm_jz>(),
            std::array<int, 1>({0})));

    static_assert(compare(
            Computer<1, int>::boot<test::tmpasm_log_jz>(),
            std::array<int, 1>({0})));
#endif

#if TEST_NUM == 603
    struct test {
        using tmpasm_cmp_js = Program<
                Cmp<Num<0>, Num<1>>,
                Js<Id("then")>,
                Jmp<Id("else")>,
                Label<Id("then")>,
                Inc<Mem<Num<0>>>,
                Jmp<Id("end")>,
                Label<Id("else")>,
                Dec<Mem<Num<0>>>,
                Label<Id("end")>>;
        using tmpasm_arithm_js = Program<
                Sub<Mem<Num<0>>, Num<1>>,
                Js<Id("end")>,
                Inc<Mem<Num<0>>>,
                Label<Id("end")>>;
        using tmpasm_log_js = Program<
                Or<Mem<Num<0>>, Num<128>>,
                Js<Id("end")>,
                Add<Mem<Num<0>>, Num<100>>,
                Label<Id("end")>>;
    };

    static_assert(compare(
            Computer<1, int>::boot<test::tmpasm_cmp_js>(),
            std::array<int, 1>({1})));

    static_assert(compare(
            Computer<1, int>::boot<test::tmpasm_arithm_js>(),
            std::array<int, 1>({-1})));

    static_assert(compare(
            Computer<1, int8_t>::boot<test::tmpasm_log_js>(),
            std::array<int8_t, 1>({-28})));
#endif

// Niepoprawne Label, Jmp, Js, Jz - nie powinno się kompilować.
#if TEST_NUM == 604
    constexpr auto error = Computer<11, int>::boot<Program<
                Jmp<Id("no")>
            >>();
#endif

#if TEST_NUM == 605
    constexpr auto error = Computer<11, int>::boot<Program<
                Cmp<Num<1>, Num<1>>,
                Jz<Id("no")>
            >>();
#endif

#if TEST_NUM == 606
    constexpr auto error = Computer<11, int>::boot<Program<
                Cmp<Num<0>, Num<1>>,
                Js<Id("no")>
            >>();
#endif

#if TEST_NUM == 607
    constexpr auto error = Computer<11, int>::boot<Program<
                Jmp<Id("no op")>,
                Label<Id("no op")>
            >>();
#endif

#if TEST_NUM == 608
    constexpr auto error = Computer<11, int>::boot<Program<
                Jz<Id("no op")>,
                Label<Id<"no op">>
            >>();
#endif

#if TEST_NUM == 609
    constexpr auto error = Computer<11, int>::boot<Program<
                Jmp<Id("label")>,
                Lea<Id("no")>,
                Label<Id("label")>
            >>();
#endif

#if TEST_NUM == 610
    constexpr auto error = Computer<11, int>::boot<Program<
                Jmp<Id("label")>,
                Jmp<Id("no op")>,
                Label<Id("label")>
            >>();
#endif

}
