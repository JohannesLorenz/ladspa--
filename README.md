# Greetings

Welcome! Thank you for considering the README file.

** CONTENTS **

  1. What is ladspa++?
  2. Why using ladspa++?
  3. License
  4. Documentation
  5. Installation
  6. Examples
  7. Contact

# 1 What is ladspa++?

ladspa++ is a C++ wrapper for ladspa. More exactly, you write your plugin in
C++, and ladspa++ "generates" C code that ladspa can read.

ladspa++ is fully ladspa compatible. However, in the future, it will allow
additional features. Nevertheless, it will still be ladspa compatible.

Note: ladspa++ is a single header file. It is not a library, you will not find
a libladspa++.

# 2 Why using ladspa++?

ladpsa++ brings many advantages to ladspa:
  
  - compatibility to ladspa
  - new and more features. further ideas:
    * allow UI elements such as function plots
    * allow keyboard input
    * descriptions of knobs
    * different types of UI elements than knobs
    * ...
  - early error detection with compile time error messages
  - possible runtime speedups:
    * correct use of the const keyword
    * ...
  - code is easy to write:
    * collection of common port types and strings
    * code is smaller
    * ...
      
# 3 License

Please see the LICENSE.txt file.

# 4 Documentation

You can
  * type `make doc' to create Doxygen code docs
  * contact me: see `9 Contact'.

# 5 Installation

Type the following.

```sh
mkdir build
cd build
cmake ..
make
make install
```

# 6 Examples

See the examples folder.

# 7 Contact

Feel free to give feedback. My e-mail address is shown if you execute this in
a shell:

```sh
printf "\x6a\x6f\x68\x61\x6e\x6e\x65\x73\x40\x6c\x6f\
\x72\x65\x6e\x7a\x2d\x70\x72\x69\x76\x61\x74\x2e\x6e\x65\x74\x0a"
```


