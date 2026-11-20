#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Math.hpp"
#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Check.hpp"

#include "Empire.Engine/Engine.Math/Math.Matrix/DynMatrix.hpp"
#include "Empire.Engine/Engine.Math/Math.Matrix/Matrix.hpp"

namespace MM = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::MathMatrix;

void TestDynamicMatrix()
{
    SECTION("Jolt dynamic matrices");
    using Vector3 = MM::Vector<3>;
    using Matrix35 = MM::Matrix<3, 5>;
    using Matrix54 = MM::Matrix<5, 4>;
    using Matrix34 = MM::Matrix<3, 4>;
    using Matrix22 = MM::Matrix<2, 2>;
    using Matrix44 = MM::Matrix<4, 4>;

    Vector3 Vector = Vector3::sZero();
    CHECK(Vector.IsZero() && Vector.GetRows() == 3);
    Vector[0] = 3.0f;
    Vector[1] = -4.0f;
    Vector[2] = 12.0f;
    CHECK(Vector.LengthSq() == 169.0f);
    CHECK(Vector.Normalized().IsNormalized());
    Vector3 Copied = Vector3::sZero();
    Copied.CopyPart(Vector, 1, 2, 0);
    CHECK(Copied[0] == -4.0f && Copied[1] == 12.0f && Copied[2] == 0.0f);

    const Matrix35 Identity = Matrix35::sIdentity();
    CHECK(Identity.GetRows() == 3 && Identity.GetCols() == 5);
    CHECK(Identity(0, 0) == 1.0f && Identity(1, 1) == 1.0f && Identity(2, 2) == 1.0f);
    CHECK(Identity(0, 3) == 0.0f && Identity.IsIdentity());

    Matrix35 Left = Matrix35::sZero();
    Matrix54 Right = Matrix54::sZero();
    for (unsigned Row = 0; Row < 3; ++Row)
        for (unsigned Column = 0; Column < 5; ++Column)
            Left(Row, Column) = static_cast<float>(Row * 5 + Column + 1);
    for (unsigned Row = 0; Row < 5; ++Row)
        for (unsigned Column = 0; Column < 4; ++Column)
            Right(Row, Column) = static_cast<float>(Row * 4 + Column + 1);

    const Matrix34 Product = Left * Right;
    CHECK(Product(0, 0) == 175.0f && Product(1, 0) == 400.0f && Product(2, 0) == 625.0f);
    CHECK(Product(0, 3) == 220.0f && Product(1, 3) == 520.0f && Product(2, 3) == 820.0f);
    CHECK(Product.Transposed().Transposed() == Product);
    Vector3 Diagonal = Vector3::sZero();
    Diagonal[0] = 2.0f;
    Diagonal[1] = 3.0f;
    Diagonal[2] = 4.0f;
    CHECK((Matrix35::sDiagonal(Diagonal) * MM::Vector<5>::sZero()).IsZero());

    Matrix22 TwoByTwo;
    TwoByTwo(0, 0) = 1.0f;
    TwoByTwo(0, 1) = 2.0f;
    TwoByTwo(1, 0) = 3.0f;
    TwoByTwo(1, 1) = 4.0f;
    Matrix22 Inverse22;
    CHECK(Inverse22.SetInversed(TwoByTwo));
    CHECK(TwoByTwo * Inverse22 == Matrix22::sIdentity());
    TwoByTwo(0, 0) = 0.0f;
    TwoByTwo(1, 0) = 0.0f;
    CHECK(!Inverse22.SetInversed(TwoByTwo));

    Matrix44 Invertible = Matrix44::sZero();
    Invertible(1, 0) = 4.0f;
    Invertible(3, 0) = 8.0f;
    Invertible(0, 1) = 2.0f;
    Invertible(2, 1) = 16.0f;
    Invertible(1, 2) = 16.0f;
    Invertible(3, 2) = 4.0f;
    Invertible(0, 3) = 8.0f;
    Invertible(2, 3) = 2.0f;
    Matrix44 Inverse44;
    CHECK(Inverse44.SetInversed(Invertible));
    CHECK(Invertible * Inverse44 == Matrix44::sIdentity());
    Invertible(1, 0) = 0.0f;
    Invertible(3, 0) = 0.0f;
    CHECK(!Inverse44.SetInversed(Invertible));

    MM::DynMatrix Dynamic(2, 3);
    CHECK(Dynamic.GetRows() == 2 && Dynamic.GetCols() == 3);
    Dynamic(1, 2) = 5.0f;
    CHECK(Dynamic(1, 2) == 5.0f);
    const MM::DynMatrix CopiedDynamic = Dynamic;
    CHECK(CopiedDynamic(1, 2) == 5.0f);
}
