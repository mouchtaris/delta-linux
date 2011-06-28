cp ../libdmsl.so .
javac DMSLJavaTest.java
java -Djava.library.path=. DMSLJavaTest
