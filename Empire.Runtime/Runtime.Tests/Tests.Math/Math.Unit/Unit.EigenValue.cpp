#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Math.hpp"
#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Check.hpp"

#include "Empire.Engine/Engine.Math/Math.Matrix/EigenValueSymmetric.hpp"
#include "Empire.Engine/Engine.Math/Math.Matrix/Matrix.hpp"

namespace MM = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::MathMatrix;

void TestSymmetricEigenvalues()
{
    SECTION("Jolt symmetric eigenvalue solver");
    using Matrix3 = MM::Matrix<3, 3>;
    using Vector3 = MM::Vector<3>;

    Matrix3 Symmetric = Matrix3::sZero();
    Symmetric(0, 0) = 4.0f;
    Symmetric(1, 1) = 4.0f;
    Symmetric(2, 2) = 2.0f;
    Symmetric(0, 1) = Symmetric(1, 0) = 1.0f;

    Matrix3 EigenVectors = Matrix3::sIdentity();
    Vector3 EigenValues = Vector3::sZero();
    CHECK(MM::EigenValueSymmetric(Symmetric, EigenVectors, EigenValues));

    for (unsigned Column = 0; Column < 3; ++Column)
    {
        CHECK(EigenVectors.GetColumn(Column).IsNormalized());
        const Vector3 MatrixEigenVector = Symmetric * EigenVectors.GetColumn(Column);
        const Vector3 EigenValueEigenVector = EigenValues[Column] * EigenVectors.GetColumn(Column);
        CHECK(MatrixEigenVector.IsClose(EigenValueEigenVector, 1.0e-12f));
    }
    CHECK((EigenValues[0] == 5.0f || EigenValues[1] == 5.0f || EigenValues[2] == 5.0f));
    CHECK((EigenValues[0] == 3.0f || EigenValues[1] == 3.0f || EigenValues[2] == 3.0f));
    CHECK((EigenValues[0] == 2.0f || EigenValues[1] == 2.0f || EigenValues[2] == 2.0f));
}
