// Copyright (C) 2013 Manuel Riecke <api.beast@gmail.com>
//
// TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
// 0. You just DO WHAT THE FUCK YOU WANT TO.

#include "Angle.h"
#include "Helpers/Math.h"

float Angle::toDegree() const
{
	return Data / Tau * 360;
}

void Angle::sanitize()
{
	Data = PositiveModulo(Data, Tau);
	if(Data > Pi)
		Data -= Tau;
}

Angle::operator float() const
{
	return Data;
}

Angle Angle::operator+(Angle value) const
{
	return Angle(Data + value.Data);
}

Angle Angle::operator-(Angle value) const
{
	return Angle(Data - value.Data);
}

Angle Angle::operator*(float value) const
{
	return Angle(Data * value);
}

Angle Angle::operator/(float value) const
{
	return Angle(Data / value);
}

Angle& Angle::operator+=(Angle value)
{
	Data += value.Data;
	sanitize();
	return *this;
}

Angle& Angle::operator-=(Angle value)
{
	Data -= value.Data;
	sanitize();
	return *this;
}

Angle& Angle::operator*=(float value)
{
	Data *= value;
	sanitize();
	return *this;
}

Angle& Angle::operator/=(float value)
{
	Data /= value;
	sanitize();
	return *this;
}

Angle Angle::operator-() const
{
	return Angle(-Data);
}

Angle Angle::operator+() const
{
	return Angle(+Data);
}

bool Angle::operator<(Angle other) const
{
	return Angle(this->Data - other.Data).Data < 0.f;
}

bool Angle::operator>(Angle other) const
{
	return Angle(this->Data - other.Data).Data > 0.f;
}

bool Angle::operator==(Angle other) const
{
	return this->Data == other.Data;
}

bool Angle::operator!=(Angle other) const
{
	return this->Data != other.Data;
}

bool Angle::operator<=(Angle other) const
{
	return *this > other || *this == other;
}

bool Angle::operator>=(Angle other) const
{
	return *this < other || *this == other;
}

float Angle::toRadians() const
{
	return Data;
}

float Angle::toTurn() const
{
	return Data / Tau;
}

Angle Angle::FromBooleanDirectionMatrix(bool up, bool down, bool right, bool left)
{
	if(up   && left ) return - 45_deg;
	if(up   && right) return   45_deg;
	if(down && left ) return -135_deg;
	if(down && right) return  135_deg;
	if(down )         return  180_deg;
	if(up   )         return    0_deg;
	if(left )         return - 90_deg;
	if(right)         return   90_deg;
	return 0.0_deg;
}

Angle operator*(float value, Angle angle)
{
	return angle * value;
}

Angle operator/(float value, Angle angle)
{
	return angle / value;
}
