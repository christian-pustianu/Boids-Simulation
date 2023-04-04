#pragma once

constexpr float PI = 3.1415926f;

/**
* @brief Takes an angle in degrees and converts it to radians.
*/
constexpr
float radians(float degrees) noexcept {
	return degrees * (PI / 180.f);
}

/**
* @brief Takes an angle in radians and converts it to degrees.
*/
constexpr
float degrees(float radians) noexcept {
	return radians * (180.f / PI);
}

/**
* @brief Clamps a value between a minimum and maximum value.
*/
constexpr
float clamp(float value, float min, float max) noexcept {
	return value < min ? min : value > max ? max : value;
}
