Lambda Recipes
==============

Code used to build some examples of talk

Topics
------

- Introduction to Lambdas
- Lambdas in STL as predicate
- Lambdas in STL as deleter
- Return a const default value
- Lambdas as while condition
- Lambdas+RAII -> ScopeGuard
- Lambdas as Macro
- Lambdas as message passing system


Build
------

Example:
  clang++ -std=c++1y -Wall scope_guard.cpp -o scope_guard
or
  g++-4.9 -std=c++1y -Wall scope_guard.cpp -o scope_guard



Biblio
------

1. Lambdas, Lambdas Everywhere
http://herbsutter.com/2010/10/07/c-and-beyond-session-lambdas-lambdas-everywhere/

2. Mix RAII&lambdas for deferred execuUon (Marco Arena) 
http://marcoarena.wordpress.com/2012/08/27/mix­‐raii-­and-lambdas­‐for-­deferred-­execuUon/

3. Fun with Lambdas: C++14 Style (part 1)
http://cpptruths.blogspot.it/2014/03/fun-with-lambdas-c14-style-part-1.html

4. Fun with Lambdas: C++14 Style (part 2)
http://cpptruths.blogspot.it/2014/03/fun-with-lambdas-c14-style-part-2.html

5. C++11 Rocks (Alex Korban)