#pragma once

namespace Typhoon {

class Allocator;
class ArenaAllocator;
class ScopedAllocator;

} // namespace Typhoon

namespace Typhoon::Reflection {

class TypeDB;

struct Context {
	TypeDB*          typeDB;
	Allocator*       allocator;
	ArenaAllocator* pagedAllocator;
	ScopedAllocator* scopedAllocator;
};

} // namespace Typhoon::Reflection
