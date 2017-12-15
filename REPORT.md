# Project background

## L-Systems introduction 

L-systems are a system of symbols that describe patterns in a plant. It was created by a botanist and meant to mimic various ways plants differentiate. To create an l-system, you essentially create a string parser. The string parser then uses a map to understand what each letter or symbol means conceptially. An examplemapping and the one we used is provided below. L-systems need axioms, variables, terminals and rules to function. An axiom is a starting string. Variables are symbols you can replace and terminals are symbols you cannot. Rules are simply what you replace variables with. The strings and the symbols used have to accomodate the various effects you wish to achieve. For us, this meant having symbols that could accomodate moving in the z-direction so that we could create 3-D trees.We also needed a symbol for parameterization. After a parser is created, the last step is to interpret the symbols graphically. 

## Alphabet 

We support the following alphabet:

|Symbol|Description|
|------|-----------|
|F|Move upwards.|
|+ OR -|Forward / backward in Z-axis.|
|\* OR /|Forward / backward in X-axis.|
|[ OR ]|Push / pop.|
|0-9|Variables.|
|T|Terminal.|
|(d)|Parameterize preceding symbol with *d*|

The alphabet was taken from the slides. We did add a few new symbols. 

To read the alphabet, we needed to create our parser. We took the input string put it into a vector of chars. Then, when we need to intepret each letter to graphically render our trees, we simply iterate through this vector. 

To create strings at various depths (further explanation below), we simply replace the symbol with its rule. For example if the axiom is F1+2. It would stored like this: [F, 1, +, 2] and then at depth 1 we would replace 1 and 2 with their respective rules. At depth 2, we would repeat this process again, therefore recursively increasing the length of the vector.

It is these various depth vectors that we use so that we only have to do a single pass of the vector. 
 
## Parameterization method

To parametrize trees, we interpret a word in the alphabet with a number in the function. For instance:

- 'F(x)' moves along the Y-axis by a factor of x while the symbol 'F' without the parameter moves up by a default value

The alphabet letters for movement and axis rotation can all take in decimal values to parameterized. The decimal values for axis rotation gives an angle to rotate in degrees. The code interprets each symbol parameterized or non-parameterized.

# What We Accomplished

## Reading in strings to render trees

Our codebase produces and stores the recurred a starting axiom after 1,2,3,4....n recursion depths. All of the strings are stored to change the depth of a tree on the screen fast. Having the raw strings for how to draw a tree at a recursion depth lets us read all our alphabet symbols consecutively.

Those symbols are read with a recursion function that manipulates a starting position, ending position and axis system to render each branch. The rotation symbols rotate the axis system about the Z-axis or X-axis of that axis system, F offsets the end position from the start position and '[' and ']' are handled by recurring through the function mentioned by keeping the necessary parameters to draw cylinders intact.

Cylinders are drawn for the branch similar to bones in Project 4 by using the start and end position as translations and then the axis system as our rotation axis.

## Parser that reads the rules file

The rules are read in from the file specified by `argv[1]`. These rules are then provided to the `String_Axioms` constructor to expand the string for depths 1-6 which are then used to generate the positions as described in the section above.

## Parameterized trees

* Coniferous

* Bush

* Oak

## Tree Planting

We use the mouse ray projection developed in Project 4 to have a ray from the mouse to the screen. Then we intersect it with the floor by solving the equation: `- Qy = (P + td)y= kFloor;` where `Q` is the ray equation and we solve for the y coordinate of the ray being at the same level as the floor.

We obtain that value when an user clicks the floor to render a new tree with the start position being at that floor intersection.

## Keyboard based depth / complexity

Pressing keys 1-6 will change the level of recursive depth, rendering more complex trees as the depth increases.

# Artifacts

## Bush, non-parameterized, depth 4

![Bush](screenshots/bush_nonparameterized.png)

## Bush, depth 2

![Bush Depth 2](screenshots/bush_2.png)

## Bush, depth 4

![Bush Depth 4](screenshots/bush_4.png)

## Bush, depth 6 (zoomed out)

![Bush Depth 6](screenshots/bush_6.png)

## Oak, depth 5 (zoomed out)

![Oak Depth 5](screenshots/oak.png)

## Pine, depth 3 (the best depth for pine)

![Pine Depth 3](screenshots/pine.png)

# References

["Graphical Applications of L-Systems" paper](http://algorithmicbotany.org/papers/graphical.gi86.pdf)

[Procedural Spline-Skeletons for Organic Structures and Adaptive Architecture slides](https://www.google.com/amp/slideplayer.com/amp/9955345/)

[L-System slides from class](https://www.cs.utexas.edu/~theshark/courses/cs354/lectures/cs354-22.pdf)
