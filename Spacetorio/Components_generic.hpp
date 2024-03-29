#ifndef COMPONENTS_GENERIC_H_
#define COMPONENTS_GENERIC_H_

#include <array>
#include <iostream>

struct GenericComponent{
    //Array of ptrs to other components
    std::array<GenericComponent*, 10> impls = {nullptr};
    size_t implN = 0;

    GenericComponent() = default;
    GenericComponent(GenericComponent* impl) : impls({impl}), implN(1) {}

    void addImpl(GenericComponent* impl){
        for (int i=0; i<10; i++){
            if (impls[i] == nullptr){
                impls[i] = impl;
                implN++;
                return;
            }
        }
        std::cout << "WARNING: Didn't added impl to generic class, too many registered!" << std::endl;
    }

};

struct MonoGenericComponent{
    MonoGenericComponent* impl = nullptr;

    MonoGenericComponent() = default;
    MonoGenericComponent(MonoGenericComponent* impl) : impl(impl) {}
};


#endif // COMPONENTS_GENERIC_H_
