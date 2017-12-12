# Project Background

## What are L-Systems (Sonika, I can do this part)


## What was our alphabet (Sonika, I can do this part)


## How did we parameterize (Nelson)

To parametrize trees, we interpret a word in the alphabet with a number in the function. For instance:

- 'F(x)' moves along the Y-axis by a factor of x while the symbol 'F' without the parameter moves up by a default value

The alphabet letters for movement and axis rotation can all take in decimal values to parameterized. The decimal values for axis rotation gives an angle to rotate in degrees. The code interprets each symbol parameterized or non-parameterized.

# What We Accomplished

## Reading in strings to render trees (Nelson)

Our codebase produces and stores the recurred a starting axiom after 1,2,3,4....n recursion depths. All of the strings are stored to change the depth of a tree on the screen fast. Having the raw strings for how to draw a tree at a recursion depth lets us read all our alphabet symbols consecutively.

Those symbols are read with a recursion function that manipulates a starting position, ending position and axis system to render each branch. The rotation symbols rotate the axis system about the Z-axis or X-axis of that axis system, F offsets the end position from the start position and '[' and ']' are handled by recurring through the function mentioned by keeping the necessary parameters to draw cylinders intact.

Cylinders are drawn for the branch similar to bones in Project 4 by using the start and end position as translations and then the axis system as our rotation axis.

## Parser that reads the rules and axioms from file (Taylor)

## Can produce parameterized trees:

* Coniferous

* Bush

* Pine

## Tree Planting

We use the mouse ray projection developed in Project 4 to have a ray from the mouse to the screen. Then we intersect it with the floor by solving the equation: `- Qy = (P + td)y= kFloor;` where `Q` is the ray equation and we solve for the y coordinate of the ray being at the same level as the floor.

We obtain that value when an user clicks the floor to render a new tree with the start position being at that floor intersection.

## Can increase their “depth”/complexity with mouse controls

# Artifacts

SCREEN SHOTS GO HERE (Taylor)

# References

TODO: Clean up with markdown (Taylor)

http://algorithmicbotany.org/papers/graphical.gi86.pdf

https://www.google.com/amp/slideplayer.com/amp/9955345/

L-System slides from class