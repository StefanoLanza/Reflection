#pragma once

namespace Typhoon {

class HeapAllocator;
class ArenaAllocator;
class ScopedAllocator;

} // namespace Typhoon

namespace Typhoon::Reflection {

class TypeDB;

struct Context {
	TypeDB*          typeDB;
	HeapAllocator*   heapAllocator;
	ArenaAllocator*  arenaAllocator;
	ScopedAllocator* scopedAllocator;
};

} // namespace Typhoon::Reflection
