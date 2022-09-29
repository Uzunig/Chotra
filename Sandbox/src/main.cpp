#include <iostream>
#include <memory>

#include "Chotra/application.h"

class Sandbox : public Chotra::Application {
    
    int frame = 0;

    virtual void OnUpdate(float deltaTime) override {
       

    }

};

int main() {

    std::cout << "Hello from Chotra Engine Editor!" << std::endl;
	
    auto sandbox = std::make_unique<Sandbox>();
    int return_code = sandbox->Start();

	std::cin.get();

    return return_code;

}
