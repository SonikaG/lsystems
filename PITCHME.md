# L-Systems


|  *Sonika Garg*  |   *Nelson Wapon Safo*  | *Taylor Kline* |
|:---------------:|:----------------------:|:---------------:|

---

# Alphabet (1 of 2)

|<span style="font-size:0.6em">Symbol</span>|<span style="font-size:0.6em">Description</span>|
|------|-----------|
|<span style="font-size:0.6em">F</span>|<span style="font-size:0.6em">~~Pay respects.~~ Move upwards.</span>|
|<span style="font-size:0.6em">+ OR -</span>|<span style="font-size:0.6em">Forward / backward in Z-axis.</span>|
|<span style="font-size:0.6em">\* OR /</span>|<span style="font-size:0.6em">Forward / backward in X-axis.</span>|
|<span style="font-size:0.6em">[ OR ]</span>|<span style="font-size:0.6em">Push / pop.</span>|

+++

# Alphabet (2 of 2)

|<span style="font-size:0.6em">Symbol</span>|<span style="font-size:0.6em">Description</span>|
|------|-----------|
|<span style="font-size:0.6em">0-9</span>|<span style="font-size:0.6em">Variables.</span>|
|<span style="font-size:0.6em">T</span>|<span style="font-size:0.6em">Terminal.</span>|
|<span style="font-size:0.6em">(d)</span>|<span style="font-size:0.6em">Parameterize preceding symbol with *d*</span>|

---
# Translating axioms into code

* Start with Project 5

* Create strings for creating trees with more depth

* Rotate trees in 3D
 
* Read turtle geometry commands and translate into code

* Generate tree branch "cylinders" onto the screen

---

# Results

Live demo

```
"F(.25)[-(30)F1]F[/(30)F2]F[*(15)F1]F[+(30)F2]F0"
"F(.5)[-(15)F1]F[/(30)FT]F[*(30)F1]F[+(15)FT]F1"
"F(1.0)[-(30)FT]F[/(15)F2]F[*(30)FT]F[+(30)F2]F2"
```

---

# Future plans

* Reading starting axioms and rules from files

* Parametrize actual axioms instead of turtle geometry commands:
L0[x, θ] = F[x] -[θ] F[x] +[θ] F[1]

* Reducing branch size as depth increases

* Prettier trees