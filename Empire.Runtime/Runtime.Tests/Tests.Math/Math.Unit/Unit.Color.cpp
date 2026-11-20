#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Math.hpp"
#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Check.hpp"

#include "Empire.Engine/Engine.Math/Math.Color/Color.hpp"

namespace MC = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::MathColor;

void TestColor()
{
    SECTION("Color: RGBA packing and channels");
    const MC::Color Value(0x12, 0x34, 0x56, 0x78);
    CHECK(Value.GetUInt32() == 0x78563412u);
    CHECK(Value.GetRed() == 0x12);
    CHECK(Value.GetGreen() == 0x34);
    CHECK(Value.GetBlue() == 0x56);
    CHECK(Value.GetAlpha() == 0x78);
    CHECK(Value(0) == 0x12 && Value(1) == 0x34 && Value(2) == 0x56 && Value(3) == 0x78);
    CHECK(MC::Color(Value, 0xff).GetAlpha() == 0xff);

    SECTION("Color: channel mutation and comparisons");
    MC::Color Mutable(1, 2, 3, 4);
    Mutable.SetChannel(2, 9);
    CHECK(Mutable == MC::Color(1, 2, 9, 4));
    CHECK(Mutable != Value);

    SECTION("Color: multiplication and intensity");
    CHECK((MC::Color(255, 128, 64, 255) * MC::Color(128, 255, 128, 128)) == MC::Color(127, 127, 32, 127));
    CHECK((MC::Color(100, 80, 60, 200) * 0.5f) == MC::Color(50, 40, 30, 200));
    CHECK((2.0f * MC::Color(10, 20, 30, 40)) == MC::Color(20, 40, 60, 40));
    CHECK(MC::Color(255, 255, 255).GetIntensity() == 255);

    SECTION("Color: vector conversion and gradients");
    const auto Vector = MC::Color(255, 128, 0, 64).ToVec4();
    CHECK(Vector.X == 1.0f);
    CHECK(Vector.Y > 0.50f && Vector.Y < 0.51f);
    CHECK(Vector.Z == 0.0f);
    CHECK(Vector.W > 0.25f && Vector.W < 0.26f);
    CHECK(MC::Color::GreenRedGradient(-1.0f) == MC::Color::Green);
    CHECK(MC::Color::GreenRedGradient(0.0f) == MC::Color(0, 255, 0));
    CHECK(MC::Color::GreenRedGradient(0.5f) == MC::Color(255, 255, 0));
    CHECK(MC::Color::GreenRedGradient(1.0f) == MC::Color::Red);
    CHECK(MC::Color::GetDistinctColor(0) == MC::Color(255, 0, 0));
    CHECK(MC::Color::GetDistinctColor(36) == MC::Color(255, 0, 0));
}
