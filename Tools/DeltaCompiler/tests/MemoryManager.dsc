&function GenerateMemoryManagerAsFunction() {
	return [
		@deps : <<using std;>>,
		@defs : <<
					function MemoryManager() {
						if (std::isundefined(static mm))
							mm = [
								method Initialize () { print("MemoryManager initialization\n"); },
								method Cleanup () { print("MemoryManager cleanup\n"); },
								method Allocate (n) { print("MemoryManager allocating ", n, " bytes\n"); return [@bytes : n]; },
								method Deallocate (var) { print("MemoryManager deallocating '", var, "'\n"); }
							];
						return mm;
					}
				>>,
		@init : <<MemoryManager().Initialize()>>,
		@cleanup : <<MemoryManager().Cleanup()>>,
		method alloc (size) { return <<MemoryManager().Allocate(~size)>>; },
		method dealloc (var) { return <<MemoryManager().Deallocate(~var)>>; }
	];
}

&function GenerateMemoryManagerAsGlobalData() {
	return [
		@deps : <<using std;>>,
		@defs : <<	mm = [
						method Initialize () { print("MemoryManager initialization\n"); },
						method Cleanup () { print("MemoryManager cleanup\n"); },
						method Allocate (n) { print("MemoryManager allocating ", n, " bytes\n"); return [@bytes : n]; },
						method Deallocate (var) { print("MemoryManager deallocating '", var, "'\n"); }
					];
				>>,
		@init : <<mm.Initialize()>>,
		@cleanup : <<mm.Cleanup()>>,
		method alloc (size) { return <<mm.Allocate(~size)>>; },
		method dealloc (var) { return <<mm.Deallocate(~var)>>; }
	];
}

&mmImpls = [
	@func : GenerateMemoryManagerAsFunction,
	@global : GenerateMemoryManagerAsGlobalData
];
&mm = nil;

&{
	local fp = std::fileopen("memory_manager_specs.txt", "rt");
	local option = fp.read("string");
	fp.close();
	mm = mmImpls[option]();
}

!(mm.deps);
//other program deps
!(mm.defs);
//other program defs

!(mm.init);

x = !(mm.alloc(<<10>>));
print(x, "\n");
!(mm.dealloc(<<x>>));

!(mm.cleanup);