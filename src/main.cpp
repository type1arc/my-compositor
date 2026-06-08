#include "compositor.hpp"

#include <cstdlib>
#include <iostream>

int main(void)
{
        try {
                CompositorServer server;
                server.run();
        } catch (const std::exception &e) {
                std::cerr << "Error: " << e.what() << std::endl;
                return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
}
