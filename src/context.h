#pragma once

namespace Typhoon::Reflection {

class TypeDB;
class Allocator;
class ScopedAllocator;

struct Context {
	TypeDB*          typeDB;
	Allocator*       allocator;
	ScopedAllocator* scopedAllocator;
};

} // namespace Typhoon::Reflection
