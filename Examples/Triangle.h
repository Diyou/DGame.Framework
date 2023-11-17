#pragma once
#include "../Source/Scene.h"

namespace DGame::Examples
{
    static Scene Triangle = Scene{"Triangle"}
    .Vertices({
		-0.8f, -0.8f, 0.0f, 0.0f, 1.0f, // BL
		 0.8f, -0.8f, 0.0f, 1.0f, 0.0f, // BR
		-0.0f,  0.8f, 1.0f, 0.0f, 0.0f, // top
	})
    .Indices({
		0, 1, 2,
		0
	})
    .Update([]()->void{

    })
    ;
}