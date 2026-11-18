// Тесты Math: Scalar, Vector, Matrix, Quaternion, Transform, Geometry.
#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Math.hpp"
#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Check.hpp"

int RunMathUnitTests()
{
    TestScalar();
    TestVector2();
    TestVector3();
    TestVector4();
    TestMatrix2();
    TestMatrix3();
    TestMatrix4();
    TestQuaternion();
    TestTransform();
    TestGeometry();

    std::printf("\nпроверок: %d, провалов: %d\n", MathTests::Checks, MathTests::Failures);
    return MathTests::Failures == 0 ? 0 : 1;
}
