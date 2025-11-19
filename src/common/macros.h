#pragma once

#define DELETE_COPY_CONSTRUCTOR(Type) Type(const Type&) = delete;

#define DELETE_COPY_ASSIGN(Type) Type& operator=(const Type&) = delete;

#define DELETE_MOVE_CONSTRUCTOR(Type) Type(Type&&) noexcept = delete;

#define DELETE_MOVE_ASSIGN(Type) Type& operator=(Type&&) noexcept = delete;

#define MAKE_UNCOPIABLE(Type)     \
    DELETE_COPY_CONSTRUCTOR(Type) \
    DELETE_COPY_ASSIGN(Type)

#define MAKE_UNMOVABLE(Type)      \
    DELETE_MOVE_CONSTRUCTOR(Type) \
    DELETE_MOVE_ASSIGN(Type)

#define MAKE_FIXED(Type)  \
    MAKE_UNCOPIABLE(Type) \
    MAKE_UNMOVABLE(Type)
