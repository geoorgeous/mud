#ifndef STRINGIFY_HPP
#define STRINGIFY_HPP

#define MUD__Stringify(X) #X
#define MUD__StringifyStringify(X) MUD__Stringify(X)

#endif