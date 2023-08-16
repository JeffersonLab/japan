// Helper functions
//
// Created by Ole Hansen on 8/16/23.
//

#ifndef QWANALYSIS_QWUTIL_H
#define QWANALYSIS_QWUTIL_H

#include <algorithm>
#include <iterator>

// Copy C-style array a to b
template<typename T>
void QwCopyArray( const T& a, T& b ) {
  std::copy(std::begin(a), std::end(a), std::begin(b));
}

#endif //QWANALYSIS_QWUTIL_H
