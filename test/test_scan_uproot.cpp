/* test_scan_uproot.cpp              -*-C++-*-
 *
 *************************************************************************
 *
 * Copyright (C) 2015 Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Intel Corporation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************/


/**
 * @file test_scan_uproot.cpp
 *
 * @brief Extra tests of parallel scan with uproot. 
 *
 *
 */


#include "cilkpub/scan.h"
#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <vector>


#include "cilktest_harness.h"
#include "cilktest_timing.h"


// An abstract reduction value, represented as a half-open interval [first,last)
class Red {             
    int first;
    int last;
    Red(int first_, int last_) : first(first_), last(last_) {} 
public:
    Red() = default;
    static Red leaf(int first_, int last_) {return Red(first_, last_);}
    static Red merge(const Red& x, const Red& y) {
        TEST_ASSERT_EQ(x.last, y.first);
        return Red(x.first,y.last);
    }
    void assert_equal(int first_, int last_) const {
        TEST_ASSERT_EQ(first, first_);
        TEST_ASSERT_EQ(last, last_);
    }
};


void test_scan_uproot(void) {

    const int max_n = 1000;
    std::atomic<char> reduce_mark[max_n];
    std::atomic<char> scan_mark[max_n];

    CILKTEST_PRINTF(1, "test_scan_uproot, n in [0, %d) ", max_n);
    for( size_t n=0; n<=max_n; ++n ) {
        // Clear flags
        std::fill_n( reduce_mark, n, 0 );
        std::fill_n( scan_mark, n, 0 );
        std::atomic<int> call_count = 0;
        const size_t tile_size = 4;

        int PRINT_INTERVAL = 50;
        if (n % PRINT_INTERVAL == 0) {
            CILKTEST_PRINTF(2, "\nN = %d ..", n);
        }
        else {
            CILKTEST_PRINTF(2, ".");
        }

        // Tile size of 4 is much too low for typical production use, but useful for correctness testing.
        cilkpub::parallel_scan( n, Red::leaf(-1,0), tile_size,  
                                // Reduce [first...first+size) 
                                [&](size_t first, size_t size) -> Red {
                                    TEST_ASSERT(size<=tile_size);
                                    TEST_ASSERT(first+size<=n);
                                    for( size_t i=first; i<first+size; ++i )
                                        reduce_mark[i]++;
                                    return Red::leaf(first, first+size);
                                },
                                // Combine two reduction values.
                                [](Red x, Red y) {
                                    return Red::merge(x,y);
                                },
                                // Do scan over [first...first+size).  j is reduction value for [0...first).
                                [&](size_t first, size_t size, Red j) {
                                    TEST_ASSERT(size<=tile_size);
                                    TEST_ASSERT(first+size<=n);
                                    j.assert_equal( -1, first );
                                    for( size_t i=first; i<first+size; ++i )
                                        scan_mark[i]++;
                                },
                                // Consume top-level reduction value.
                                [&](Red r) {
                                    r.assert_equal( -1, n );
                                    TEST_ASSERT_EQ_MSG(std::count( reduce_mark, reduce_mark+n, 1 ),
                                                       n,
                                                       "reduce did not process each item exactly once");
                                    TEST_ASSERT_EQ_MSG(std::count( scan_mark, scan_mark+n, 0 ),
                                                       n,
                                                       "scan ran prematurely" );
                                    call_count++;
                                }
            );
        TEST_ASSERT_EQ_MSG( call_count,
                            1,
                            "uproot functor not called exactly once" );
        TEST_ASSERT_EQ_MSG(std::count( scan_mark, scan_mark+n, 1 ),
                           n,
                           "scan did not process each item exactly once" );
    }
    CILKTEST_PRINTF(1, "\ntest_scan_uproot complete\n");
}

int main(int argc, char* argv[]) {
    CILKTEST_PARSE_TEST_ARGS(argc, argv);
    CILKTEST_BEGIN("scan_uproot");
    
    test_scan_uproot();
    
    return CILKTEST_END("scan_uproot");
}
