// Example regarding the use of standard library algorithms.
// A. Maragoudakis, February 2011.

using std;
const nl = "\n";

///////////////////////////////////////////////////
// Using standard library containers and iterators

l = list_new(1, 3, "hello", 3, true, "world", 0);
end_it = listiter_new().setend(l);
it = find(3, l);				// Returns an iterator to first occurance of an element with value 3
it = find(3, it.fwd(), end_it);	// Starts searching from the previous element until the end.
it_list = find_all(3, l);		// Returns a list with 2 iterators. One in each element

apply(							// Applies the function to every element of the container
	function(i){
		if (typeof(local x = i.getval()) == std::TYPEOF_NUMBER and x % 2 == 1)
			print(x, nl);
	},
	l
);								// Prints all odd numbers.

it = remove(true, l);			// Removes the element 'true' and returns an iterator to "world"
it = remove(0, it, end_it);		// Removes the element '0' and returns an iterator to the end.
remove_all(3, l);				// Removes all elements with value 3.

///////////////////////////////////////////////////

t = [
	method (x, y)	{return self[2] + x + y;},
	"Hello world!",
	1,
	function (x, y) {return x + y;}
];

end_it = tableiter_new();
end_it.setend(t);

it = find(10, t);				//If no elements match the search element, then find returns end iterator

if (it.equal(end_it))
	print("Did not find any element with value 10\n");

it = find_if					//Returns an iterator to the first element the return value of
(								//the lambda function is true
	lambda(i) {
		typeof(i.getval()) == std::TYPEOF_PROGRAMFUNC
	},							//Check if it is a program function
	t
);

print(it.getval()(3,9), nl);	//Prints 12

it_list = find_all_if			//Return a list of iterators to all the functions inside table t
(
	lambda(i) {
		typeof(i.getval()) == std::TYPEOF_METHODFUNC
	},							//Check if it is a method
	t
);

function table_eraser(t, iterator) {
	t[iterator.getindex()] = nil;
}								//Provide an external eraser function, because the table has no erase method

remove_all_if(
	lambda {true},
	t, it, table_eraser
);								//Remove everything from the container, until the iterator it

///////////////////////////////////////////////////
// User defined container and iterator

function NewStringIterator {
	const STRING_END = -1;
	return [
		method setbegin (strIterable) 
			{ @i = 0, @c = strIterable; },
		
		method setend (strIterable) 
			{ @i = STRING_END, @c = strIterable; },
		
		method equal (strIterator) 
			{ return @c == strIterator.c and @i == strIterator.i; },
			
		method @operator==(strIterator) //If cannot find equal method, then operator== is called.
			{ return @c == strIterator.c and @i == strIterator.i; },
			
		method copy {
			local iter = NewStringIterator();
			iter.i = @i;
			iter.c = @c;
			return iter;
		},

		method getcontainer 
			{ return @c; },
			
		method getcounter 
			{ return @i; },
		
		method getval {
			assert @i < std::strlen(@c.get());
			return std::strchar(@c.get(), @i);
		},
		
		method fwd {
			local len = std::strlen(@c.get());
			assert @i < len;

			if (++@i == len)
				@i = STRING_END;
			return self;
		},
		
		{ std::ALGORITHMS_SUPERCLASS : std::ALGORITHMS_ITERATOR }
	];
}

/////////////////////////////

function StringIterable (str) {
	
	return [
		@iterator: NewStringIterator,

		method get 	{ return str; },
		method @ 	{ return str; },
		
		method erase (iter){
			assert local oldLen = std::strlen(str) or true;
			if (iter.i == 0)
				str = std::strslice(str, iter.i + 1, 0);
			else 
			if (iter.i == std::strlen(str) - 1)
				str = std::strslice(str, 0, iter.i - 1);
			else 
			if (iter.i == 1)
				str = std::strchar(str, 0) + std::strslice(str, iter.i + 1, 0);
			else
				str = std::strslice(str, 0, iter.i - 1, iter.i + 1, 0);

			assert oldLen == std::strlen(str) + 1;
		},
		
		{ std::ALGORITHMS_SUPERCLASS : std::ALGORITHMS_CONTAINER }
	];
}

function external_erase(container, arg) 
	{ container.erase(arg); }

///////////////////////////////////////////////////
// Using user defined containers and iterators

strIterable = StringIterable("vvvvaaaddddbbbbbbjjjjjj");
end_it = strIterable.iterator();
end_it.setend(strIterable);
it = find("j", strIterable);		// Returns an iterator to the first character 'j'
it = find("f", it, end_it);			// Search in a range [it,end_it) and return first occurance of 'f'
it = find_if(lambda(i){i.getval()=="a"}, it, end_it);	// For every iterator calls the lambda function
it_list = find_all("h", strIterable);	// Returns a list with every occurance of 'h'

apply(
	function(i){ if(i.i % 2 == 0) print(i.getval()); },
	strIterable
);									//Prints the 1st, 3rd, 5th etc. characters.
print("\n");
it = remove("v", strIterable);		//Deletes first occurance of 'v'
it = remove("d", it, end_it);		//Deletes first occurance of 'd' in [it,end_it)
it = remove("j", strIterable, end_it, external_erase);	//Instead of method erase, calls function external_erase
std::remove_all("v",strIterable);
print(strIterable, "\n");
