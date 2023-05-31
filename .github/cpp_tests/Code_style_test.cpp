#include "util_structures.h"

void code_test()
{    
    static_assert (std::is_constructible_v <SharedPtr<First>, Second*>);
    static_assert (!std::is_constructible_v <SharedPtr<Second>, First*>);

    static_assert (std::is_constructible_v <SharedPtr<First>, SharedPtr<Second>>);
    static_assert (!std::is_constructible_v <SharedPtr<Second>, SharedPtr<First>>);

    static_assert (std::is_assignable_v <SharedPtr<First>, SharedPtr<Second>>);
    static_assert (!std::is_assignable_v <SharedPtr<Second>, SharedPtr<First>>);

    static_assert (std::is_constructible_v <WeakPtr<First>, SharedPtr<Second>>);
    static_assert (!std::is_constructible_v <WeakPtr<Second>, SharedPtr<First>>);

    static_assert (std::is_constructible_v <WeakPtr<First>, WeakPtr<Second>>);
    static_assert (!std::is_constructible_v <WeakPtr<Second>, WeakPtr<First>>);

    static_assert (std::is_assignable_v <WeakPtr<First>, WeakPtr<Second>>);
    static_assert (!std::is_assignable_v <WeakPtr<Second>, WeakPtr<First>>);
}