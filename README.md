# Polymorphic Programming

This is a work in progress to demonstrate how to create computer programs which may have different code but still produces the same output.

In this example I attempt to generate as many different code variations that output "hello world", but it's application could be used for many different end results.

The basis for this code is using genetic algorithms, starting with a population of randomly generated programs and then testing each program for the fitness to a problem (i.e. running the code)

For example, the simplest way to produce "hello world" may be;

```
#include <iostream>

int main()
{
    std::cout << "hello world!";

    return 0;
}
```

However, you could also write;

```
#include <iostream>

int main()
{
    char[] data = { 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!' };

    for(int i = 0; i < 12; ++i)
    {
        std::cout << data[i];
    }

    return 0;
}

```

And even more indirectly;

```
#include <iostream>

int main()
{
    int z = 12;
    int y = 2;

    char[] data = { 'h' };
    
    for(int i = 0; i < 1; ++i )
    {
        std::cout << data[i];
    }
    
    if(z / 2 == 6)
    {
        std::cout <<  "ello ";   
    }

    y = 6;

    if(y < z)
    {
        std::cout << "world!";
    }

    return 0;
}
```

From an output point of view, all these programs are equally valid, however illogical the code producing the output may be! (however in these examples, we are ignoring any compiler optimisations that may negate seemingly useless code).


# Algorithm

- A large array (or a population), containing randomly generated programs is created (see below for details)

- Each program is run, and then scored based on how close it's output to "hello world" is (a number between 0.0f and 1.0f)

- Two programs are picked, that have outputted the closest string to "hello world" and then randomly combined with each other, to create a new program

- The new program is then added to the population, and then re-run and re-scored

- This process is repeated until a program with a score of 1.0f is reached

# Randomly Generating Snytax Correct Code

There are many different ways to generate random code, you could technically generate a random set of characters and using genetic algorithms eventually expect it to eventually produce a valid program (1 million monkeys on a million typewriters, expecting them to produce Shakespere isn't out of the realms of possibility with modern parallel hardware, such as GPUs) however this could a very long time to find a solution, so was rejected in favour of an alternative algorithm.

For that alternative, consider a program as a series of blocks, those blocks may represent a function, an control-block inside an IF statement, or a loop.  Each block may use a set of variables and have it's own set of instructions, such as assigning a value of a variable to another, initialising a variable to a value, or output a variable to the console window.  With these set of base rules, we can start to build an algorithm that will always generate syntax correct code, that will run and compile no matter ho random a mess it would seem from an initial glance.

For example, the creation of a new "block" of code may look like;

```
int x = rand(0,3);
block b;

if(x == 0)
{
    b = new block("if");
    b.init();
} 
else if(x == 1)
{
    b = new block("loop");
    b.init();
}

class block
{
    string _type;

public:
    block(string type) { _type = type; }
    
    void init()
    {
        auto variables = generate_variables();
        generate_instructions(variables);
    }
};
```

Using genetic algorithms in this way might be considered redundant now(?) with the recent progress in fields of machine learning -- there may be potential here to have a customized variation of ChatGPT that performed this task more efficiently, simply asking it to generate all the same variations of the code it could think of, but where's the fun in that?

# Example Output

Below are some example outputs from the program, as you can see, it's not entirely obvious at first glance what this program does, and demonstrates the many many different ways one can write some code to do the same thing.

```
#include <iostream>

using namespace std;

int main() {
  {
    string b = "";
    int c = 0;
    cout << b;
    b = "he";
    cout << b;
    b = "o ";
    b = "ll";
    cout << b;
    b = "o ";
    cout << b;
    b = "rl";
    b = "wo";
    if (c <= c) {
      cout << b;
      b = "rl";
      cout << b;
    }
    if (c >= c) {
      b = "d!";
      cout << b;
      b = "o ";
      c = 36;
    }
  }
  return 0;
}
```

```
#include <iostream>

using namespace std;

int main() {
   {
     string b = "";
     int c = 0;
     string d = "";
     string e = "";
     d = "o ";
     cout << b;
     cout << e;
     cout << e;
     b = "he";
     e = "ll";
     cout << b;
     cout << e;
     cout << d;
     if (d > e) {
       d = "wo";
       b = e;
       d = "ll";
       b = "rl";
       d = b;
       d = e;
       e = "wo";
       cout << e;
       e = "d!";
       d = "wo";
     }
     if (c <= c) {
       cout << b;
       cout << e;
       e = "ll";
       d = "ll";
       b = "he";
       e = "wo";
     }
   }
   return 0;
 }
```

```
#include <iostream>

using namespace std;

int main() {
  {
    string b = "";
    int c = 0;
    string d = "";
    string e = "";
    d = "o ";
    cout << b;
    cout << e;
    cout << e;
    b = "he";
    e = "ll";
    cout << b;
    cout << e;
    cout << d;
    if (d > e) {
      d = "wo";
      b = e;
      d = "ll";
      b = "rl";
      d = b;
      d = e;
      e = "wo";
      cout << e;
      e = "d!";
      d = "wo";
    }
    if (c <= c) {
      cout << b;
      cout << e;
      e = "ll";
      d = "ll";
      b = "he";
      e = "wo";
    }
  }
  return 0;
}
```

```
#include <iostream>

using namespace std;
int main() {
  {
    string b = "";
    int c = 0;
    bool d = false;
    string e = "";
    string f = "";
    int g = 0;
    f = e;
    cout << f;
    e = f;
    g = c;
    f = "he";
    e = f;
    f = "ll";
    cout << e;
    e = f;
    f = e;
    cout << b;
    cout << f;
    if (c == c) {
      f = "ll";
      f = "rl";
      cout << b;
      b = "d!";
      b = "o ";
      cout << b;
      d = false;
      d = true;
      b = "wo";
      cout << b;
      c = 62;
    }
    if (g == g) {
      e = f;
      e = f;
      cout << f;
      e = f;
      g = 45;
      e = "d!";
      cout << e;
      e = f;
      e = f;
      e = f;
      g = 59;
    }
  }
  return 0;
}
```

```
#include <iostream>

using namespace std;
int main() {
  {
    string b = "";
    int c = 0;
    int d = 0;
    string e = "";
    e = "he";
    cout << e;
    e = "d!";
    e = "ll";
    cout << b;
    cout << e;
    for (d; d < 3; ++d) {
      e = b;
      for (c; c < 4; ++c) {
        cout << b;
        c = 2;
        b = "o ";
        cout << b;
        c = 43;
        e = "wo";
        if (c >= d) {
          cout << e;
          b = "he";
          b = "d!";
        }
        if (b == b) {
          b = "rl";
          b = "rl";
          cout << b;
          b = "he";
        }
      }
    }
    for (d; d < 8; ++d) {
      e = "d!";
      d = 57;
      cout << e;
      c = 14;
      e = "wo";
    }
  }
  return 0;
}
```

# Further Notes

Whilst running the program, due to it's random nature, it may take several restarts before a solution is reached (working on this problem).  It may get close to a solution, but may take a large amount of time to reach to the final solution.

Why did I call it a "polymorphic-program"?  using such genetic algorithm techniques can provide the basis for generation different code for programs that produce the same output, it may be possible to have a program self-replicate in such a way, similar to systems that exploit evolutionary processes.

# Todo

- Fix empty blocks { } output bug

- Fix runner, doesn't seem entirely capable of running a program correctly

- Fix top level FOR level with no variable declarations

- Fix Crossover Tests (breaks due to code change, added randomness)

- Fix code generating variables being assigned to themselves

- Fix code generating IF statements comparisons with variables and self

- During variable creation, randomly generator init. values

- Add warning that generation is taking too long, i.e. if the top three of ecah generation hasn't changed, over X generations, issue a warning

# Running

- Ensure project is open within the VSCode development container
- Hit F5

# Requirements

The VSCode development container plugin is installed;

https://code.visualstudio.com/docs/remote/containers

Docker must also be installed;

https://docs.docker.com/get-docker/

This application, however is configured with linux based containers, and will not work correctly on Windows without modification.
