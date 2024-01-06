#include <iostream>
#include <memory>

#include "application.h"

int main() {
	
    auto application = std::make_unique<Chotra::Application>();
    application->Run();

    return 0;
}
