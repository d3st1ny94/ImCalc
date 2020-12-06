#pragma once

#include <iostream>
#include "imgui.h"

std::ostream& operator<<(std::ostream& stream, const ImVec4& color) {
    stream << color.x << ' ' << color.y << ' ' << color.z << ' ' << color.w;
    return stream;
}
std::istream& operator>>(std::istream& stream, ImVec4& color) {
    if (stream >> color.x >> color.y >> color.z >> color.w)
        return stream;
    color.x = 0.5f;
    color.y = 0.5f;
    color.z = 0.5f;
    color.w = 0.5f;
    return stream;
}