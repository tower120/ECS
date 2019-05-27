// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <functional>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/transform.hpp>
#include <range/v3/view/unbounded.hpp>
#include "../simple_test.hpp"
#include "../test_iterators.hpp"

using namespace std::placeholders;

RANGES_DIAGNOSTIC_IGNORE_DEPRECATED_DECLARATIONS

template<class InIter, class OutIter>
void
test1()
{
    {
        int ia[] = {0, 1, 2, 3, 4};
        constexpr auto sa = ranges::size(ia);
        int ib[sa] = {0};
        ranges::unary_transform_result<InIter, OutIter> r =
            ranges::transform(InIter(ia), sentinel<int const *>(ia+sa), OutIter(ib),
                              std::bind(std::plus<int>(), _1, 1));
        CHECK(base(r.in) == ia + sa);
        CHECK(base(r.out) == ib + sa);
        CHECK(ib[0] == 1);
        CHECK(ib[1] == 2);
        CHECK(ib[2] == 3);
        CHECK(ib[3] == 4);
        CHECK(ib[4] == 5);
    }

    {
        int ia[] = {0, 1, 2, 3, 4};
        constexpr auto sa = ranges::size(ia);
        int ib[sa] = {0};
        auto rng = ranges::make_subrange(InIter(ia), sentinel<int const *>(ia + sa));
        ranges::unary_transform_result<InIter, OutIter> r =
            ranges::transform(rng, OutIter(ib),
                              std::bind(std::plus<int>(), _1, 1));
        CHECK(base(r.in) == ia + sa);
        CHECK(base(r.out) == ib + sa);
        CHECK(ib[0] == 1);
        CHECK(ib[1] == 2);
        CHECK(ib[2] == 3);
        CHECK(ib[3] == 4);
        CHECK(ib[4] == 5);
    }

    {
        int ia[] = {0, 1, 2, 3, 4};
        constexpr auto sa = ranges::size(ia);
        int ib[sa] = {0};
        auto rng = ranges::make_subrange(InIter(ia), sentinel<int const *>(ia + sa));
        auto r =
            ranges::transform(std::move(rng), OutIter(ib),
                              std::bind(std::plus<int>(), _1, 1));
        CHECK(base(r.in) == ia + sa);
        CHECK(base(r.out) == ib + sa);
        CHECK(ib[0] == 1);
        CHECK(ib[1] == 2);
        CHECK(ib[2] == 3);
        CHECK(ib[3] == 4);
        CHECK(ib[4] == 5);
    }
}

template<class InIter1, class InIter2, class OutIter>
void
test2()
{
    {
        int ia[] = {0, 1, 2, 3, 4};
        constexpr auto sa = ranges::size(ia);
        int ib[sa] = {1, 2, 3, 4, 5};
        ranges::binary_transform_result<InIter1, InIter2, OutIter> r =
            ranges::transform(InIter1(ib), sentinel<int const *>(ib + sa), InIter2(ia),
                              OutIter(ib), std::minus<int>());
        CHECK(base(r.in1) == ib + sa);
        CHECK(base(r.in2) == ia + sa);
        CHECK(base(r.out) == ib + sa);
        CHECK(ib[0] == 1);
        CHECK(ib[1] == 1);
        CHECK(ib[2] == 1);
        CHECK(ib[3] == 1);
        CHECK(ib[4] == 1);
    }

    {
        int ia[] = {0, 1, 2, 3, 4};
        constexpr auto sa = ranges::size(ia);
        int ib[sa] = {1, 2, 3, 4, 5};
        ranges::binary_transform_result<InIter1, InIter2, OutIter> r =
            ranges::transform(InIter1(ib), sentinel<int const *>(ib + sa),
                              InIter2(ia), sentinel<int const *>(ia + sa),
                              OutIter(ib), std::minus<int>());
        CHECK(base(r.in1) == ib + sa);
        CHECK(base(r.in2) == ia + sa);
        CHECK(base(r.out) == ib + sa);
        CHECK(ib[0] == 1);
        CHECK(ib[1] == 1);
        CHECK(ib[2] == 1);
        CHECK(ib[3] == 1);
        CHECK(ib[4] == 1);
    }

    {
        int ia[] = {0, 1, 2, 3, 4};
        constexpr auto sa = ranges::size(ia);
        int ib[sa] = {1, 2, 3, 4, 5};
        auto rng0 = ranges::make_subrange(InIter1(ib), sentinel<int const *>(ib + sa));
        ranges::binary_transform_result<InIter1, InIter2, OutIter> r =
            ranges::transform(rng0, InIter2(ia),
                              OutIter(ib), std::minus<int>());
        CHECK(base(r.in1) == ib + sa);
        CHECK(base(r.in2) == ia + sa);
        CHECK(base(r.out) == ib + sa);
        CHECK(ib[0] == 1);
        CHECK(ib[1] == 1);
        CHECK(ib[2] == 1);
        CHECK(ib[3] == 1);
        CHECK(ib[4] == 1);
    }

    {
        int ia[] = {0, 1, 2, 3, 4};
        constexpr auto sa = ranges::size(ia);
        int ib[sa] = {1, 2, 3, 4, 5};
        auto rng0 = ranges::make_subrange(InIter1(ib), sentinel<int const *>(ib + sa));
        auto r =
            ranges::transform(std::move(rng0), InIter2(ia),
                              OutIter(ib), std::minus<int>());
        CHECK(base(r.in1) == ib + sa);
        CHECK(base(r.in2) == ia + sa);
        CHECK(base(r.out) == ib + sa);
        CHECK(ib[0] == 1);
        CHECK(ib[1] == 1);
        CHECK(ib[2] == 1);
        CHECK(ib[3] == 1);
        CHECK(ib[4] == 1);
    }

    {
        int ia[] = {0, 1, 2, 3, 4};
        constexpr auto sa = ranges::size(ia);
        int ib[sa] = {1, 2, 3, 4, 5};
        auto rng0 = ranges::make_subrange(InIter1(ib), sentinel<int const *>(ib + sa));
        auto rng1 = ranges::make_subrange(InIter2(ia), sentinel<int const *>(ia + sa));
        ranges::binary_transform_result<InIter1, InIter2, OutIter> r =
            ranges::transform(rng0, rng1, OutIter(ib), std::minus<int>());
        CHECK(base(r.in1) == ib + sa);
        CHECK(base(r.in2) == ia + sa);
        CHECK(base(r.out) == ib + sa);
        CHECK(ib[0] == 1);
        CHECK(ib[1] == 1);
        CHECK(ib[2] == 1);
        CHECK(ib[3] == 1);
        CHECK(ib[4] == 1);
    }

    {
        int ia[] = {0, 1, 2, 3, 4};
        constexpr auto sa = ranges::size(ia);
        int ib[sa] = {1, 2, 3, 4, 5};
        auto rng0 = ranges::make_subrange(InIter1(ib), sentinel<int const *>(ib + sa));
        auto rng1 = ranges::make_subrange(InIter2(ia), sentinel<int const *>(ia + sa));
        auto r =
            ranges::transform(std::move(rng0), std::move(rng1), OutIter(ib), std::minus<int>());
        CHECK(base(r.in1) == ib + sa);
        CHECK(base(r.in2) == ia + sa);
        CHECK(base(r.out) == ib + sa);
        CHECK(ib[0] == 1);
        CHECK(ib[1] == 1);
        CHECK(ib[2] == 1);
        CHECK(ib[3] == 1);
        CHECK(ib[4] == 1);
    }
}

struct S
{
    int i;
};

int main()
{
    test1<input_iterator<const int*>, output_iterator<int*> >();
    test1<input_iterator<const int*>, input_iterator<int*> >();
    test1<input_iterator<const int*>, forward_iterator<int*> >();
    test1<input_iterator<const int*>, bidirectional_iterator<int*> >();
    test1<input_iterator<const int*>, random_access_iterator<int*> >();
    test1<input_iterator<const int*>, int*>();

    test1<forward_iterator<const int*>, output_iterator<int*> >();
    test1<forward_iterator<const int*>, input_iterator<int*> >();
    test1<forward_iterator<const int*>, forward_iterator<int*> >();
    test1<forward_iterator<const int*>, bidirectional_iterator<int*> >();
    test1<forward_iterator<const int*>, random_access_iterator<int*> >();
    test1<forward_iterator<const int*>, int*>();

    test1<bidirectional_iterator<const int*>, output_iterator<int*> >();
    test1<bidirectional_iterator<const int*>, input_iterator<int*> >();
    test1<bidirectional_iterator<const int*>, forward_iterator<int*> >();
    test1<bidirectional_iterator<const int*>, bidirectional_iterator<int*> >();
    test1<bidirectional_iterator<const int*>, random_access_iterator<int*> >();
    test1<bidirectional_iterator<const int*>, int*>();

    test1<random_access_iterator<const int*>, output_iterator<int*> >();
    test1<random_access_iterator<const int*>, input_iterator<int*> >();
    test1<random_access_iterator<const int*>, forward_iterator<int*> >();
    test1<random_access_iterator<const int*>, bidirectional_iterator<int*> >();
    test1<random_access_iterator<const int*>, random_access_iterator<int*> >();
    test1<random_access_iterator<const int*>, int*>();

    test1<const int*, output_iterator<int*> >();
    test1<const int*, input_iterator<int*> >();
    test1<const int*, forward_iterator<int*> >();
    test1<const int*, bidirectional_iterator<int*> >();
    test1<const int*, random_access_iterator<int*> >();
    test1<const int*, int*>();


    test2<input_iterator<const int*>, input_iterator<const int*>, output_iterator<int*> >();
    test2<input_iterator<const int*>, input_iterator<const int*>, input_iterator<int*> >();
    test2<input_iterator<const int*>, input_iterator<const int*>, forward_iterator<int*> >();
    test2<input_iterator<const int*>, input_iterator<const int*>, bidirectional_iterator<int*> >();
    test2<input_iterator<const int*>, input_iterator<const int*>, random_access_iterator<int*> >();
    test2<input_iterator<const int*>, input_iterator<const int*>, int*>();

    test2<input_iterator<const int*>, forward_iterator<const int*>, output_iterator<int*> >();
    test2<input_iterator<const int*>, forward_iterator<const int*>, input_iterator<int*> >();
    test2<input_iterator<const int*>, forward_iterator<const int*>, forward_iterator<int*> >();
    test2<input_iterator<const int*>, forward_iterator<const int*>, bidirectional_iterator<int*> >();
    test2<input_iterator<const int*>, forward_iterator<const int*>, random_access_iterator<int*> >();
    test2<input_iterator<const int*>, forward_iterator<const int*>, int*>();

    test2<input_iterator<const int*>, bidirectional_iterator<const int*>, output_iterator<int*> >();
    test2<input_iterator<const int*>, bidirectional_iterator<const int*>, input_iterator<int*> >();
    test2<input_iterator<const int*>, bidirectional_iterator<const int*>, forward_iterator<int*> >();
    test2<input_iterator<const int*>, bidirectional_iterator<const int*>, bidirectional_iterator<int*> >();
    test2<input_iterator<const int*>, bidirectional_iterator<const int*>, random_access_iterator<int*> >();
    test2<input_iterator<const int*>, bidirectional_iterator<const int*>, int*>();

    test2<input_iterator<const int*>, random_access_iterator<const int*>, output_iterator<int*> >();
    test2<input_iterator<const int*>, random_access_iterator<const int*>, input_iterator<int*> >();
    test2<input_iterator<const int*>, random_access_iterator<const int*>, forward_iterator<int*> >();
    test2<input_iterator<const int*>, random_access_iterator<const int*>, bidirectional_iterator<int*> >();
    test2<input_iterator<const int*>, random_access_iterator<const int*>, random_access_iterator<int*> >();
    test2<input_iterator<const int*>, random_access_iterator<const int*>, int*>();

    test2<input_iterator<const int*>, const int*, output_iterator<int*> >();
    test2<input_iterator<const int*>, const int*, input_iterator<int*> >();
    test2<input_iterator<const int*>, const int*, forward_iterator<int*> >();
    test2<input_iterator<const int*>, const int*, bidirectional_iterator<int*> >();
    test2<input_iterator<const int*>, const int*, random_access_iterator<int*> >();
    test2<input_iterator<const int*>, const int*, int*>();

    test2<forward_iterator<const int*>, input_iterator<const int*>, output_iterator<int*> >();
    test2<forward_iterator<const int*>, input_iterator<const int*>, input_iterator<int*> >();
    test2<forward_iterator<const int*>, input_iterator<const int*>, forward_iterator<int*> >();
    test2<forward_iterator<const int*>, input_iterator<const int*>, bidirectional_iterator<int*> >();
    test2<forward_iterator<const int*>, input_iterator<const int*>, random_access_iterator<int*> >();
    test2<forward_iterator<const int*>, input_iterator<const int*>, int*>();

    test2<forward_iterator<const int*>, forward_iterator<const int*>, output_iterator<int*> >();
    test2<forward_iterator<const int*>, forward_iterator<const int*>, input_iterator<int*> >();
    test2<forward_iterator<const int*>, forward_iterator<const int*>, forward_iterator<int*> >();
    test2<forward_iterator<const int*>, forward_iterator<const int*>, bidirectional_iterator<int*> >();
    test2<forward_iterator<const int*>, forward_iterator<const int*>, random_access_iterator<int*> >();
    test2<forward_iterator<const int*>, forward_iterator<const int*>, int*>();

    test2<forward_iterator<const int*>, bidirectional_iterator<const int*>, output_iterator<int*> >();
    test2<forward_iterator<const int*>, bidirectional_iterator<const int*>, input_iterator<int*> >();
    test2<forward_iterator<const int*>, bidirectional_iterator<const int*>, forward_iterator<int*> >();
    test2<forward_iterator<const int*>, bidirectional_iterator<const int*>, bidirectional_iterator<int*> >();
    test2<forward_iterator<const int*>, bidirectional_iterator<const int*>, random_access_iterator<int*> >();
    test2<forward_iterator<const int*>, bidirectional_iterator<const int*>, int*>();

    test2<forward_iterator<const int*>, random_access_iterator<const int*>, output_iterator<int*> >();
    test2<forward_iterator<const int*>, random_access_iterator<const int*>, input_iterator<int*> >();
    test2<forward_iterator<const int*>, random_access_iterator<const int*>, forward_iterator<int*> >();
    test2<forward_iterator<const int*>, random_access_iterator<const int*>, bidirectional_iterator<int*> >();
    test2<forward_iterator<const int*>, random_access_iterator<const int*>, random_access_iterator<int*> >();
    test2<forward_iterator<const int*>, random_access_iterator<const int*>, int*>();

    test2<forward_iterator<const int*>, const int*, output_iterator<int*> >();
    test2<forward_iterator<const int*>, const int*, input_iterator<int*> >();
    test2<forward_iterator<const int*>, const int*, forward_iterator<int*> >();
    test2<forward_iterator<const int*>, const int*, bidirectional_iterator<int*> >();
    test2<forward_iterator<const int*>, const int*, random_access_iterator<int*> >();
    test2<forward_iterator<const int*>, const int*, int*>();

    test2<bidirectional_iterator<const int*>, input_iterator<const int*>, output_iterator<int*> >();
    test2<bidirectional_iterator<const int*>, input_iterator<const int*>, input_iterator<int*> >();
    test2<bidirectional_iterator<const int*>, input_iterator<const int*>, forward_iterator<int*> >();
    test2<bidirectional_iterator<const int*>, input_iterator<const int*>, bidirectional_iterator<int*> >();
    test2<bidirectional_iterator<const int*>, input_iterator<const int*>, random_access_iterator<int*> >();
    test2<bidirectional_iterator<const int*>, input_iterator<const int*>, int*>();

    test2<bidirectional_iterator<const int*>, forward_iterator<const int*>, output_iterator<int*> >();
    test2<bidirectional_iterator<const int*>, forward_iterator<const int*>, input_iterator<int*> >();
    test2<bidirectional_iterator<const int*>, forward_iterator<const int*>, forward_iterator<int*> >();
    test2<bidirectional_iterator<const int*>, forward_iterator<const int*>, bidirectional_iterator<int*> >();
    test2<bidirectional_iterator<const int*>, forward_iterator<const int*>, random_access_iterator<int*> >();
    test2<bidirectional_iterator<const int*>, forward_iterator<const int*>, int*>();

    test2<bidirectional_iterator<const int*>, bidirectional_iterator<const int*>, output_iterator<int*> >();
    test2<bidirectional_iterator<const int*>, bidirectional_iterator<const int*>, input_iterator<int*> >();
    test2<bidirectional_iterator<const int*>, bidirectional_iterator<const int*>, forward_iterator<int*> >();
    test2<bidirectional_iterator<const int*>, bidirectional_iterator<const int*>, bidirectional_iterator<int*> >();
    test2<bidirectional_iterator<const int*>, bidirectional_iterator<const int*>, random_access_iterator<int*> >();
    test2<bidirectional_iterator<const int*>, bidirectional_iterator<const int*>, int*>();

    test2<bidirectional_iterator<const int*>, random_access_iterator<const int*>, output_iterator<int*> >();
    test2<bidirectional_iterator<const int*>, random_access_iterator<const int*>, input_iterator<int*> >();
    test2<bidirectional_iterator<const int*>, random_access_iterator<const int*>, forward_iterator<int*> >();
    test2<bidirectional_iterator<const int*>, random_access_iterator<const int*>, bidirectional_iterator<int*> >();
    test2<bidirectional_iterator<const int*>, random_access_iterator<const int*>, random_access_iterator<int*> >();
    test2<bidirectional_iterator<const int*>, random_access_iterator<const int*>, int*>();

    test2<bidirectional_iterator<const int*>, const int*, output_iterator<int*> >();
    test2<bidirectional_iterator<const int*>, const int*, input_iterator<int*> >();
    test2<bidirectional_iterator<const int*>, const int*, forward_iterator<int*> >();
    test2<bidirectional_iterator<const int*>, const int*, bidirectional_iterator<int*> >();
    test2<bidirectional_iterator<const int*>, const int*, random_access_iterator<int*> >();
    test2<bidirectional_iterator<const int*>, const int*, int*>();

    test2<random_access_iterator<const int*>, input_iterator<const int*>, output_iterator<int*> >();
    test2<random_access_iterator<const int*>, input_iterator<const int*>, input_iterator<int*> >();
    test2<random_access_iterator<const int*>, input_iterator<const int*>, forward_iterator<int*> >();
    test2<random_access_iterator<const int*>, input_iterator<const int*>, bidirectional_iterator<int*> >();
    test2<random_access_iterator<const int*>, input_iterator<const int*>, random_access_iterator<int*> >();
    test2<random_access_iterator<const int*>, input_iterator<const int*>, int*>();

    test2<random_access_iterator<const int*>, forward_iterator<const int*>, output_iterator<int*> >();
    test2<random_access_iterator<const int*>, forward_iterator<const int*>, input_iterator<int*> >();
    test2<random_access_iterator<const int*>, forward_iterator<const int*>, forward_iterator<int*> >();
    test2<random_access_iterator<const int*>, forward_iterator<const int*>, bidirectional_iterator<int*> >();
    test2<random_access_iterator<const int*>, forward_iterator<const int*>, random_access_iterator<int*> >();
    test2<random_access_iterator<const int*>, forward_iterator<const int*>, int*>();

    test2<random_access_iterator<const int*>, bidirectional_iterator<const int*>, output_iterator<int*> >();
    test2<random_access_iterator<const int*>, bidirectional_iterator<const int*>, input_iterator<int*> >();
    test2<random_access_iterator<const int*>, bidirectional_iterator<const int*>, forward_iterator<int*> >();
    test2<random_access_iterator<const int*>, bidirectional_iterator<const int*>, bidirectional_iterator<int*> >();
    test2<random_access_iterator<const int*>, bidirectional_iterator<const int*>, random_access_iterator<int*> >();
    test2<random_access_iterator<const int*>, bidirectional_iterator<const int*>, int*>();

    test2<random_access_iterator<const int*>, random_access_iterator<const int*>, output_iterator<int*> >();
    test2<random_access_iterator<const int*>, random_access_iterator<const int*>, input_iterator<int*> >();
    test2<random_access_iterator<const int*>, random_access_iterator<const int*>, forward_iterator<int*> >();
    test2<random_access_iterator<const int*>, random_access_iterator<const int*>, bidirectional_iterator<int*> >();
    test2<random_access_iterator<const int*>, random_access_iterator<const int*>, random_access_iterator<int*> >();
    test2<random_access_iterator<const int*>, random_access_iterator<const int*>, int*>();

    test2<random_access_iterator<const int*>, const int*, output_iterator<int*> >();
    test2<random_access_iterator<const int*>, const int*, input_iterator<int*> >();
    test2<random_access_iterator<const int*>, const int*, forward_iterator<int*> >();
    test2<random_access_iterator<const int*>, const int*, bidirectional_iterator<int*> >();
    test2<random_access_iterator<const int*>, const int*, random_access_iterator<int*> >();
    test2<random_access_iterator<const int*>, const int*, int*>();

    test2<const int*, input_iterator<const int*>, output_iterator<int*> >();
    test2<const int*, input_iterator<const int*>, input_iterator<int*> >();
    test2<const int*, input_iterator<const int*>, forward_iterator<int*> >();
    test2<const int*, input_iterator<const int*>, bidirectional_iterator<int*> >();
    test2<const int*, input_iterator<const int*>, random_access_iterator<int*> >();
    test2<const int*, input_iterator<const int*>, int*>();

    test2<const int*, forward_iterator<const int*>, output_iterator<int*> >();
    test2<const int*, forward_iterator<const int*>, input_iterator<int*> >();
    test2<const int*, forward_iterator<const int*>, forward_iterator<int*> >();
    test2<const int*, forward_iterator<const int*>, bidirectional_iterator<int*> >();
    test2<const int*, forward_iterator<const int*>, random_access_iterator<int*> >();
    test2<const int*, forward_iterator<const int*>, int*>();

    test2<const int*, bidirectional_iterator<const int*>, output_iterator<int*> >();
    test2<const int*, bidirectional_iterator<const int*>, input_iterator<int*> >();
    test2<const int*, bidirectional_iterator<const int*>, forward_iterator<int*> >();
    test2<const int*, bidirectional_iterator<const int*>, bidirectional_iterator<int*> >();
    test2<const int*, bidirectional_iterator<const int*>, random_access_iterator<int*> >();
    test2<const int*, bidirectional_iterator<const int*>, int*>();

    test2<const int*, random_access_iterator<const int*>, output_iterator<int*> >();
    test2<const int*, random_access_iterator<const int*>, input_iterator<int*> >();
    test2<const int*, random_access_iterator<const int*>, forward_iterator<int*> >();
    test2<const int*, random_access_iterator<const int*>, bidirectional_iterator<int*> >();
    test2<const int*, random_access_iterator<const int*>, random_access_iterator<int*> >();
    test2<const int*, random_access_iterator<const int*>, int*>();

    test2<const int*, const int*, output_iterator<int*> >();
    test2<const int*, const int*, input_iterator<int*> >();
    test2<const int*, const int*, forward_iterator<int*> >();
    test2<const int*, const int*, bidirectional_iterator<int*> >();
    test2<const int*, const int*, random_access_iterator<int*> >();
    test2<const int*, const int*, int*>();

    int *p = nullptr;
    auto unary = [](int i){return i + 1; };
    auto binary = [](int i, int j){return i + j; };
    S const s[] = {S{1}, S{2}, S{3}, S{4}};
    int const i[] = {1, 2, 3, 4};
    static_assert(std::is_same<ranges::unary_transform_result<S const*, int*>,
        decltype(ranges::transform(s, p, unary, &S::i))>::value, "");
    static_assert(std::is_same<ranges::binary_transform_result<S const*, int const *, int*>,
        decltype(ranges::transform(s, i, p, binary, &S::i))>::value, "");
    static_assert(std::is_same<ranges::binary_transform_result<S const*, S const *, int*>,
        decltype(ranges::transform(s, s, p, binary, &S::i, &S::i))>::value, "");

    return ::test_result();
}
