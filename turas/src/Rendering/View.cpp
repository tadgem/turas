//
// Created by liam_ on 7/10/2024.
//
#include "Rendering/View.h"
#include "Core/Utils.h"

turas::View::View(const String &name) : m_Name(name), m_Hash(Utils::Hash(name)) {

}