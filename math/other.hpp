#pragma once

constexpr float PI = 3.1415926f;

constexpr
float radians(float degrees) noexcept {
	return degrees * (PI / 180.f);
}

constexpr
float degrees(float radians) noexcept {
	return radians * (180.f / PI);
}

constexpr
float clamp(float value, float min, float max) noexcept {
	return value < min ? min : value > max ? max : value;
}