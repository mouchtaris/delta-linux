//////////////////////////////////////////////////////////////////////////////////
// Singleton pattern as a metaprogram
//

&function Singleton(name, members) {
	return <<
		function ~name {
			if (std::isundefined(static instance))
				instance = [ ~members ];
			return instance;
		}
	>>;
}

//////////////////////////////////////////////////////////////////////////////////
// Example using the Singleton pattern to create a memory manager
//
!(Singleton(
	<<MemoryManager>>,
	<<
		method Initialize { std::print("Initialize\n"); },
		method Cleanup { std::print("Cleanup\n"); },
		method Allocate (size) { std::print("Allocating ", size, " bytes\n"); }
	>>
));

MemoryManager().Initialize();
MemoryManager().Allocate(10);
MemoryManager().Cleanup();
