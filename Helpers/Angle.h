// Copyright (C) 2013 Manuel Riecke <api.beast@gmail.com>
//
// TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
// 0. You just DO WHAT THE FUCK YOU WANT TO.

#pragma once

#include "Math.h"

//! A half turn in radians. Equal to the ratio of a circle's circumference to it's diameter.
constexpr float Pi  = 3.14159;
//! A full turn in radians. Equal to the ratio of a circle's circumference to it's radius.
constexpr float Tau = Pi * 2;

//! @ingroup Angle
//! @{
struct Angle
{
	explicit constexpr Angle(float value) : Data((PositiveModulo(value, Tau) > Pi) ? (PositiveModulo(value, Tau) - Tau) : PositiveModulo(value, Tau)){}
	Angle()=default;
	explicit operator float() const;
	Angle operator+(Angle value) const;
	Angle operator-(Angle value) const;
	Angle operator*(float value) const;
	Angle operator/(float value) const;
	Angle& operator+=(Angle value);
	Angle& operator-=(Angle value);
	Angle& operator*=(float value);
	Angle& operator/=(float value);
	Angle operator-() const;
	Angle operator+() const;
	bool operator >(Angle other) const;
	bool operator <(Angle other) const;
	bool operator==(Angle other) const;
	bool operator!=(Angle other) const;
	bool operator>=(Angle other) const;
	bool operator<=(Angle other) const;
	static constexpr Angle FromTurn  (float turns);
	static constexpr Angle FromRadians(float radians);
	static constexpr Angle FromDegree (float degree);
	float toRadians() const;
 	float toDegree() const;
	float toTurn() const;
	void sanitize();
	
	constexpr double sin();
	constexpr double cos();
	static constexpr Angle Atan2(double a, double b);
	static constexpr Angle Acos(double A);
	static constexpr Angle Asin(double A);
	
public:
	static Angle FromBooleanDirectionMatrix(bool up, bool right, bool down, bool left);
public:
	float Data=0.f;
};

Angle operator*(float value, Angle angle);
Angle operator/(float value, Angle angle);

constexpr Angle Angle::FromDegree(float degree)
{
	return Angle(degree * Tau / 360);
}

constexpr Angle Angle::FromRadians(float radians)
{
	return Angle(radians);
}

constexpr Angle Angle::FromTurn(float turns)
{
	return Angle(turns * Tau);
}

constexpr double Angle::cos()
{
	return std::cos(Data);
}

constexpr double Angle::sin()
{
	return std::sin(Data);
}

constexpr Angle Angle::Atan2(double a, double b)
{
	return Angle::FromRadians(std::atan2(a, b));
}

constexpr Angle Angle::Acos(double A)
{
	return Angle::FromRadians(std::acos(A));
}

constexpr Angle Angle::Asin(double A)
{
	return Angle::FromRadians(std::asin(A));
}

constexpr Angle operator "" _turn(long double num){ return Angle::FromTurn(num); }
constexpr Angle operator "" _turn(unsigned long long num){ return Angle::FromTurn(num); }
constexpr Angle operator "" _deg(long double num){ return Angle::FromDegree(num); }
constexpr Angle operator "" _deg(unsigned long long num){ return Angle::FromDegree(num); }
constexpr Angle operator "" _rad(long double num){ return Angle::FromRadians(num); }
constexpr Angle operator "" _rad(unsigned long long num){ return Angle::FromRadians(num); }

//! @}
