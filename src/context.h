#pragma once

namespace Typhoon {

class Allocator;
class ScopedAllocator;

} // namespace Typhoon

namespace Typhoon::Reflection {

class TypeDB;

struct Context {
	TypeDB*          typeDB;
	Allocator*       allocator;
	ScopedAllocator* scopedAllocator;
};

} // namespace Typhoon::Reflection
