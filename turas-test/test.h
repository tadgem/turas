//
// Created by liam_ on 7/4/2024.
//

#pragma once
#include "Core/Log.h"
#include "Core/Engine.h"
#include "Core/ECS.h"
#include "Core/Utils.h"
#include "STL/Vector.h"
#include "STL/Functional.h"
#include "Systems/Transform.h"

inline static turas::Vector<turas::Procedure> s_Tests = {};

#define BEGIN_TESTS() int main(int argc, char** argv) {
#define TEST(X) s_Tests.emplace_back([]() X);

#define RUN_TESTS() for(auto& t : s_Tests) { t(); } };