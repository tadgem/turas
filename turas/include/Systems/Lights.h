//
// Created by liam_ on 7/27/2024.
//

#pragma once
#include "glm/glm.hpp"
#include "Core/Serialization.h"

namespace turas {

struct DirectionalLight {
	glm::vec4	Direction;
	glm::vec4	Ambient;
	glm::vec4	Colour;

	template<typename Archive>
		void serialize(Archive &ar) {
		ZoneScoped;
		ar(
				Direction.x, Direction.y, Direction.z, Direction.w,
				Ambient.x, Ambient.y, Ambient.z, Ambient.w,
				Colour.x, Colour.y, Colour.z, Colour.w
		);
	}

	TURAS_IMPL_ALLOC (DirectionalLight)
};

struct PointLight {
	glm::vec4	PositionRadius;
	glm::vec4	Ambient;
	glm::vec4	Colour;

	template<typename Archive>
		void serialize(Archive &ar) {
		ZoneScoped;
		ar(
				PositionRadius.x, PositionRadius.y, PositionRadius.z, PositionRadius.w,
				Ambient.x, Ambient.y, Ambient.z, Ambient.w,
				Colour.x, Colour.y, Colour.z, Colour.w
		);
	}

	TURAS_IMPL_ALLOC (PointLight)
};

struct SpotLight {
	glm::vec4	PositionRadius;
	glm::vec4	DirectionAngle;
	glm::vec4	Ambient;
	glm::vec4	Colour;

	template<typename Archive>
		void serialize(Archive &ar) {
		ZoneScoped;
		ar(
				PositionRadius.x, PositionRadius.y, PositionRadius.z, PositionRadius.w,
				DirectionAngle.x, DirectionAngle.y, DirectionAngle.z, DirectionAngle.w,
				Ambient.x, Ambient.y, Ambient.z, Ambient.w,
				Colour.x, Colour.y, Colour.z, Colour.w
		);
	}

	TURAS_IMPL_ALLOC (SpotLight)
};

}