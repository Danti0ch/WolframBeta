# WolframBeta

### Requirements
- pdflatex

## Description
The entered expression is converted into a binary tree taking into account the priority of operations using a recursive descent algorithm. Then a recursive simplification of the expression is carried out (obvious mathematical expressions like exponentiation of a constant or multiplication of a number by itself are simplified). The program allows you to take derivatives of any degree and output them, together with a simplified expression, to a LaTeX file, which is subsequently assembled into a pdf file.

## Usage
``` $ mkdir logs``` <br/>
``` $ cd build``` <br/>
``` $ make``` <br/>
``` $ ./a.out``` <br/>

## Example
For example, the output for the expression sin(x * x)^ln(3) will be as follows:
![[sample.png]]
