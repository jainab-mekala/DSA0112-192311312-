# Q7 – Diagnose, Redesign and Validate (report skeleton)

> Fill every section with YOUR OWN numbers from running `q7_original_vs_redesign.cpp`
> (and your Q4 benchmark). Write section D BEFORE you run the comparison.

Chosen program: Q4 – SavingsAccount mini-statement (shifting array).  
(You may choose another program from Q1–Q6 if your own evidence shows a bigger limitation.)

## A. Diagnose the limitation (2 marks)
- Where does the design slow down? (Which K / which operation? Quote your Q4 benchmark numbers.)
- Which characteristic of the original design causes it? (Hint: what happens to the array every time it is full?)

## B. Design objective (2 marks) – must be measurable
Example form: "Make adding a transaction independent of K, so that the time per transaction for K = 500 is within __ % of the time for K = 10, with the same mini-statement output."

## C. Revised design (4 marks)
- What changed in the class? (new private member `start`, modulo indexing, `record()` rewritten)
- C++ feature used: (private array inside class, modulo operator, member function defined in class / outside)
- Extra memory: (one extra int)
- New trade-off: (miniStatement() must translate logical position → physical index; code is slightly harder to read)
- Class diagram / pseudocode: (draw or write it here)

## D. Prediction BEFORE implementing/running (1 mark)
- Predicted improvement for K = 10, 50, 100, 500 and the measure (ms or ns per operation): ______

## E. Implementation and validation (2 marks)
Paste the table printed by the program / q7_results.csv, and the PASS lines of the correctness test. Include the compile command and optimisation level used (-O0 and/or -O2).

| K | shift (ms) | circular (ms) | speed-up |
|---|-----------|---------------|----------|
|   |           |               |          |

## F. Defend the final design (2 marks)
- Was the objective achieved? (use the numbers)
- New costs (extra index, modulo, readability)
- Where is the original still preferable? (e.g. K very small, such as 10, where the difference may be negligible; simpler code)
