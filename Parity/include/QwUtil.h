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
  for (size_t i=0; i<a.size(); i++){
    b.at(i).CopyFrom(a.at(i));
  }
}

#endif //QWANALYSIS_QWUTIL_H
