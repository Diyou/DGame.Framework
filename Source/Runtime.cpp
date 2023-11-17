#include <cstdlib>

#include "../Examples/Triangle.h"
#include "Renderer.h"

class Runtime
{
	friend int main(int, char **);

	int EXIT_CODE = EXIT_FAILURE;

	DGame::Renderer renderer;

	Runtime(int argc, char *argv[])
	    : renderer(std::make_shared<DGame::Scene>(DGame::Examples::Triangle))
	{
		renderer.Start();
		while (renderer.IsRendering)
		{
			renderer.Yield();
		}
	};
};

int main(int argc, char *argv[])
{
	Runtime _(argc, argv);
	return _.EXIT_CODE;
}