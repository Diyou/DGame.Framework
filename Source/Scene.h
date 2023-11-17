#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace DGame
{
	struct Scene
	{
#define DGameSceneSetter(Type, Name, Property)                                                                         \
	Type Property;                                                                                                     \
	Scene &Name(Type Property)                                                                                         \
	{                                                                                                                  \
		this->Property = Property;                                                                                     \
		return *this;                                                                                                  \
	}

		const char *Name;
		DGameSceneSetter(std::vector<float>, Vertices, vertices);

		DGameSceneSetter(std::vector<uint16_t>, Indices, indices);


		DGameSceneSetter(std::function<void()>, Update, update);

		Scene(const char *name)
		    : Name(name)
		{
		}
#undef DGameSceneSetter
	};
} // namespace DGame