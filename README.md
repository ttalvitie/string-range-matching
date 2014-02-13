string-range-matching
=====================

String Range Matching library

Library for string range matching designed in the style of C++ standard library: uses templates and iterators to support many types of containers. Depends on only the C++ standard library. Requires C++0x/C++11.

For more details about the algorithms, see the papers referred to by the documentation comments. The notation used in the code is mostly compatible.

Tests using randomized input strings are found in randomtest.cpp. Exhaustive test of all range matching functions for strings of length 0-6 in three-letter alphabet is in smalltest.cpp. To compile all the tests, run ./compile_tests.sh, and to run the tests, run ./smalltest and ./randomtest.
