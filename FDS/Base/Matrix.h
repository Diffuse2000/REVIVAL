#ifndef REVIVAL_MATRIX_H
#define REVIVAL_MATRIX_H

// [36 Bytes]
typedef float Matrix[3][3];
typedef float Matrix4[4][4]; //stuff

struct alignas(16) XMMMatrix { float XMMMatrix[3][3]; };
struct alignas(16) XMMMatrix4 { float XMMMatrix4[4][4]; };


#endif //REVIVAL_MATRIX_H
