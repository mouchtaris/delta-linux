/*&function SharedObject() {
	return [
		@member1 : "data",
		@member2 : "data",
		method f() { std::print("f"); },
		method g() { std::print("g"); }
	];
}*/
&function SharedObject() { return [ method m() { std::print(123); } ]; }
//&obj = SharedObject();
//&obj.f();