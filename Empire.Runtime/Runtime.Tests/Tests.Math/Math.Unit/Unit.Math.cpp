// Tests for the currently ported Math modules.
#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Math.hpp"
#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Check.hpp"

int RunMathUnitTests()
{
    TestScalar();
    TestColor();
    TestVectorStorage();
    TestVector3();
    TestVector4();
    TestMatrix4();
    TestDynamicMatrix();
    TestSymmetricEigenvalues();
    TestAABB();
    TestConvexHull2D();
    TestConvexHull3D();
    TestQuaternion();

    std::printf("\nпроверок: %d, провалов: %d\n", MathTests::Checks, MathTests::Failures);
    return MathTests::Failures == 0 ? 0 : 1;
}
