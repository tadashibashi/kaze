#include <iostream>
#include <nova/macros/enum.h>

NOVA_FLAGS_LEAN(AbilityFlag, CanFly, CanSwim, CanJump);

struct MyStruct
{
    NOVA_DEFINE_FLAGS(MyFlags, A, B, C);
};

NOVA_DEFINE_FLAGOPS(MyStruct::MyFlags);

int main()
{
    AbilityFlag abilities = AbilityFlag::CanFly | AbilityFlag::CanSwim;
    abilities |= AbilityFlag::CanJump;
    abilities.reset();

    auto flags = MyStruct::MyFlags::A | MyStruct::MyFlags::B;

    return 0;
}
