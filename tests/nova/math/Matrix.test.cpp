#include <doctest/doctest.h>

#include <nova/nova.h>
#include <nova/math/Matrix.h>

using namespace NOVA_NAMESPACE;

/// Most of the functions just wrap GLM, so the tests aren't too extensive
TEST_SUITE("Matrix4x4")
{
    TEST_CASE("Constructor, Identity Matrix")
    {
        Matrix<Float, 4, 4> matrix;
        CHECK(matrix[0][0] == 1);
        CHECK(matrix[0][1] == 0);
        CHECK(matrix[0][2] == 0);
        CHECK(matrix[0][3] == 0);
        CHECK(matrix[1][0] == 0);
        CHECK(matrix[1][1] == 1);
        CHECK(matrix[1][2] == 0);
        CHECK(matrix[1][3] == 0);
        CHECK(matrix[2][0] == 0);
        CHECK(matrix[2][1] == 0);
        CHECK(matrix[2][2] == 1);
        CHECK(matrix[2][3] == 0);
        CHECK(matrix[3][0] == 0);
        CHECK(matrix[3][1] == 0);
        CHECK(matrix[3][2] == 0);
        CHECK(matrix[3][3] == 1);
    }

    TEST_CASE("Vec column indexer")
    {
        Matrix<Float, 4, 4> matrix;
        CHECK(matrix[0] == Vec4f(1, 0, 0, 0));
        CHECK(matrix[1] == Vec4f(0, 1, 0, 0));
        CHECK(matrix[2] == Vec4f(0, 0, 1, 0));
        CHECK(matrix[3] == Vec4f(0, 0, 0, 1));
    }

    TEST_CASE("Create fromArray")
    {
        const Float matData[] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1,
        };
        auto matrix = Mat4f::fromArray(matData);

        CHECK(matrix[0][0] == 1);
        CHECK(matrix[0][1] == 0);
        CHECK(matrix[0][2] == 0);
        CHECK(matrix[0][3] == 0);
        CHECK(matrix[1][0] == 0);
        CHECK(matrix[1][1] == 1);
        CHECK(matrix[1][2] == 0);
        CHECK(matrix[1][3] == 0);
        CHECK(matrix[2][0] == 0);
        CHECK(matrix[2][1] == 0);
        CHECK(matrix[2][2] == 1);
        CHECK(matrix[2][3] == 0);
        CHECK(matrix[3][0] == 0);
        CHECK(matrix[3][1] == 0);
        CHECK(matrix[3][2] == 0);
        CHECK(matrix[3][3] == 1);
    }

    TEST_CASE("Scalar multiplication")
    {
        Mat4f matrixA{};

        auto matrixB = matrixA * 4.0f;
        auto matrixC = 4.0f * matrixA;

        CHECK(matrixB[0][0] == 4);
        CHECK(matrixB[0][1] == 0);
        CHECK(matrixB[0][2] == 0);
        CHECK(matrixB[0][3] == 0);
        CHECK(matrixB[1][0] == 0);
        CHECK(matrixB[1][1] == 4);
        CHECK(matrixB[1][2] == 0);
        CHECK(matrixB[1][3] == 0);
        CHECK(matrixB[2][0] == 0);
        CHECK(matrixB[2][1] == 0);
        CHECK(matrixB[2][2] == 4);
        CHECK(matrixB[2][3] == 0);
        CHECK(matrixB[3][0] == 0);
        CHECK(matrixB[3][1] == 0);
        CHECK(matrixB[3][2] == 0);
        CHECK(matrixB[3][3] == 4);

        CHECK(matrixB == matrixC);
    }

    TEST_CASE("Scalar division")
    {
        Mat4f matrix{};

        CHECK( &(matrix /= 4.0f) == &matrix);

        CHECK(matrix[0][0] == .25f);
        CHECK(matrix[0][1] == 0);
        CHECK(matrix[0][2] == 0);
        CHECK(matrix[0][3] == 0);
        CHECK(matrix[1][0] == 0);
        CHECK(matrix[1][1] == .25f);
        CHECK(matrix[1][2] == 0);
        CHECK(matrix[1][3] == 0);
        CHECK(matrix[2][0] == 0);
        CHECK(matrix[2][1] == 0);
        CHECK(matrix[2][2] == .25f);
        CHECK(matrix[2][3] == 0);
        CHECK(matrix[3][0] == 0);
        CHECK(matrix[3][1] == 0);
        CHECK(matrix[3][2] == 0);
        CHECK(matrix[3][3] == .25f);
    }
}
