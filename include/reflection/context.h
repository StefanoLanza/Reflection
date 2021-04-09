#pragma once

namespace Typhoon {

class Allocator;
class LinearAllocator;
class ScopedAllocator;

} // namespace Typhoon

namespace Typhoon::Reflection {

class TypeDB;

struct Context {
	TypeDB*          typeDB;
	Allocator*       allocator;
	LinearAllocator* pagedAllocator;
	ScopedAllocator* scopedAllocator;
};

} // namespace Typhoon::Reflection
